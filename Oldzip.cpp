/*
 * Zip1.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides compression functions
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnufile.h"
#include "gnumem.h"
#include "gnuzip.h"
#include "zip.h"

extern "C"
{


#define MASK  0xA0000001

typedef struct
	{
	PSZ  pszBuff;
	FILE *fp;
	INT  iIndex;
	INT  iSize;
	} FLOW;
typedef FLOW *PFLOW;


static UPDATEFN pfnUPDATE[4] = {NULL, NULL, NULL, NULL};

static BOOL  bGENREADCRC  = FALSE; // external flag to create src CRC
static BOOL  bGENWRITECRC = FALSE; // external flag to create dest CRC
static ULONG ulREADCRC    = 0;     // external running src CRC
static ULONG ulWRITECRC   = 0;     // external running dest CRC

static FLOW SRC;                   // internal handle to source data location
static FLOW DEST;                  // internal handle to dest data location

static PSZ  pszWORKBUFF = NULL;    // internal ZIP BUFFER
static BOOL bNULLTERM   = 0;       // internal Null Term Flag
static INT  iERROR      = 0;       // internal Error State

static INT  iWINDOWSIZE = 4096;
static INT  iMODE       = CMP_BINARY;


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

ULONG CRC_32 (ULONG ulCRC, CHAR c)
	{
	INT i;

	for (i=0; i<8; i++)
		{
		if((ulCRC ^ c) & 1)
			ulCRC = (ulCRC>>1) ^ MASK;
		else
			ulCRC>>=1;
		c>>=1;
		}
	return ulCRC;
	}

													
ULONG CRC_BUFF (ULONG ulCRC, PSZ psz, INT iLen)
	{
	INT i;

	for (i=0; i<iLen; i++)
		ulCRC = CRC_32 (ulCRC, *psz++);
	return ulCRC;
	}



/*------ Buffer to zipper ------*/
INT ReadBuff (PSZ pszBuff, PUINT puSize, PVOID pParam)
	{
	INT i;

	if (!bNULLTERM)
		i = min ((INT)*puSize, SRC.iSize - SRC.iIndex);
	else
		for (i=0; i < (INT)*puSize && SRC.pszBuff[i + SRC.iIndex]; i++);
	memcpy (pszBuff, SRC.pszBuff + SRC.iIndex, i);

	if (bGENREADCRC)
		ulREADCRC = CRC_BUFF (ulREADCRC, pszBuff, i);

	SRC.iIndex += i;

	if (pfnUPDATE[0])
		(pfnUPDATE[0]) (i, SRC.iIndex, SRC.iSize);

	return i;
	}


/*------ zipper to Buffer ------*/
void WriteBuff (PSZ pszBuff, PUINT puSize, PVOID pParam)
	{
	INT i;

	i = min ((INT)*puSize, DEST.iSize - DEST.iIndex);
	if ((INT)*puSize > DEST.iSize - DEST.iIndex)
		iERROR = ZIP_BUFF_TO_SMALL;

	if (DEST.pszBuff)
		memcpy (DEST.pszBuff + DEST.iIndex, pszBuff, i);

	if (bGENWRITECRC)
		ulWRITECRC = CRC_BUFF (ulWRITECRC, pszBuff, i);

	DEST.iIndex += i;

	if (pfnUPDATE[1])
		(pfnUPDATE[1]) (i, DEST.iIndex, DEST.iSize);
	}


/*------ file to zipper ------*/
INT zipReadFile (PSZ pszBuff, PUINT puSize, PVOID pParam)
	{
	INT i;
	size_t uRead;

	i = min ((INT)*puSize, SRC.iSize - SRC.iIndex);
	uRead = fread(pszBuff, 1, i, SRC.fp);

	if (bGENREADCRC)
		ulREADCRC = CRC_BUFF (ulREADCRC, pszBuff, i);

	SRC.iIndex += uRead;

	if (pfnUPDATE[2])
		(pfnUPDATE[2]) (uRead, SRC.iIndex, SRC.iSize);

	return (INT) uRead;
	}


/*------ zipper to file ------*/
void zipWriteFile (PSZ pszBuff, PUINT puSize, PVOID pParam)
	{
	INT iWritten;

	if (DEST.fp)
		iWritten = fwrite(pszBuff, 1, *puSize, DEST.fp);

	if (iWritten < (INT)*puSize)
		iERROR = ZIP_CANT_WRITE;

	if (bGENWRITECRC)
		ulWRITECRC = CRC_BUFF (ulWRITECRC, pszBuff, *puSize);

	DEST.iIndex += *puSize;

	if (pfnUPDATE[3])
		(pfnUPDATE[3]) (*puSize, DEST.iIndex, DEST.iSize);
	}



void Cmp2SetUpdateFn (INT iType, UPDATEFN pfn)
	{
	if (iType < 4)
		pfnUPDATE[iType] = pfn;
	}


void InitFlow (PFLOW pfl, PSZ pszBuff, FILE *fp, INT iIndex, INT iSize)
	{
	pfl->pszBuff= pszBuff;
	pfl->fp     = fp;
	pfl->iIndex = iIndex;
	pfl->iSize  = iSize;
	}



/*
 * Buffer ---> IMPLODE ---> Buffer
 *
 * Implodes data from a buffer to a buffer
 * if uDataSize=0, source buffer is assumed to be null term.
 * if uCBuffSize=0, no bounds checking is performed for pszImploded
 * on return pUSize contains the size of the Imploded data
 * piSize may be null.
 */
INT Cmp2szIsz (PSZ   pszImploded, // Dest Imploded data buff
					INT   iCBuffSize,  // Dest buff Size
					PINT  piSize,      // Dest Imploded data size
					PSZ   pszExploded, // Src Exploded data buff
					INT   iDataSize)   // Src Exploded data size
	{
	InitFlow (&SRC,  pszExploded, NULL,   0, iDataSize);
	InitFlow (&DEST, pszImploded, NULL,   2, (iCBuffSize ? iCBuffSize : -1));

	bNULLTERM = !iDataSize;
	iERROR    = 0;

	if (piSize)   *piSize = 0;
	if (!pszExploded || !pszImploded) return ZIP_BUFF_TO_SMALL;

	switch (implode((PFReadBuf) ReadBuff, (PFWriteBuf) WriteBuff, pszWORKBUFF, NULL, (PUINT)&iMODE, (PUINT)&iWINDOWSIZE))
		{
		case CMP_INVALID_DICTSIZE: return ZIP_INVALID_DATA1;
		case CMP_INVALID_MODE:     return ZIP_INVALID_DATA2;
		}
// (UINT)(DEST.pszBuff) = DEST.iIndex; //I dont know what i was thinking. CLF
	*piSize = DEST.iIndex;
	return iERROR;
	}



/*
 * Buffer ---> EXPLODE ---> Buffer
 *
 * Explodes data from a buffer to a buffer
 * iBuffSize is the size of the Exploded buffer (0=no check)
 * on return piSize contains the size of the Exploded data
 * piSize may be null.
 */
INT Cmp2szEsz (PSZ  pszExploded, // Dest Exploded data buff
					INT  iBuffSize,   // Dest buff size
					PINT piSize,      // Dest Exploded data size
					PSZ  pszImploded) // Src  Imploded data buffer
	{
	InitFlow (&SRC,  pszImploded, NULL,   2, *((PUINT)pszImploded));
	InitFlow (&DEST, pszExploded, NULL,   0, (iBuffSize ? iBuffSize : -1));

	bNULLTERM = 0;
	iERROR    = 0;

	if (piSize)   *piSize = 0;
	if (!pszExploded || !pszImploded) return ZIP_BUFF_TO_SMALL;

	switch (explode((PFReadBuf) ReadBuff, (PFWriteBuf) WriteBuff, pszWORKBUFF, NULL))
		{
		case CMP_INVALID_DICTSIZE: return ZIP_INVALID_DATA1;
		case CMP_INVALID_MODE:     return ZIP_INVALID_DATA2;
		case CMP_BAD_DATA:         return ZIP_INVALID_DATA3;
		case CMP_ABORT:            return ZIP_ABORT;
		}

	if (DEST.iIndex < DEST.iSize)
		pszExploded [DEST.iIndex] = '\0';

	if (piSize)   *piSize = DEST.iIndex;
	return iERROR;
	}



/*
 * Buffer ---> IMPLODE ---> File
 *
 * Implodes data from a buffer to a file
 * iDataSize is the size of the buffer to write.
 * if iDataSize=0, the buffer is considered to be null terminated
 * the buffer must still however be smaller than 64k
 * piSize may be null.
 */
INT Cmp2szIfp (FILE *fp,       // Dest file for Imploded data
					PINT piSize,    // Dest Imploded data size
					PSZ  pszBuff,   // Src data buffer
					INT  iDataSize) // Src data size
	{
	fpos_t fPos, fPos2;

	InitFlow (&SRC,  pszBuff, NULL, 0, iDataSize);
	InitFlow (&DEST, NULL,    fp,   2, 0);

	bNULLTERM = !iDataSize;
	iERROR    = 0;

	if (piSize)   *piSize = 0;
	if (!pszBuff) return ZIP_BUFF_TO_SMALL;

	fgetpos (fp, &fPos);
	FilWriteUShort (fp, (USHORT)DEST.iIndex);  /*--- save position to write datasize ---*/
	switch (implode ((PFReadBuf) ReadBuff, (PFWriteBuf) zipWriteFile, pszWORKBUFF, NULL, (PUINT)&iMODE, (PUINT)&iWINDOWSIZE))
		{
		case CMP_INVALID_DICTSIZE: return ZIP_INVALID_DATA1;
		case CMP_INVALID_MODE:     return ZIP_INVALID_DATA2;
		}
	fgetpos (fp, &fPos2);
	fsetpos (fp, &fPos);

	FilWriteUShort (fp, (USHORT)DEST.iIndex);  /*--- save position to write datasize ---*/
	fsetpos (fp, &fPos2);

	if (piSize)  *piSize = DEST.iIndex;
	return iERROR;
	}



/*
 * File ---> EXPLODE ---> Buffer
 *
 * Explodes data from file to buffer.
 * if iBuffSize is 0, buffer overflow is not checked.
 * on return piSize contains the size of the data returned.
 * piSize may be null.
 */
INT Cmp2fpEsz (PSZ  pszBuff,   // Dest buffer
					INT  iBuffSize, // Dest buffer size
					PINT piSize,    // Dest Exploded data size
					FILE *fp)       // Src file with Imploded data
	{
	INT iSize;

	iSize = FilReadUShort (fp);
	InitFlow (&SRC,  NULL,    fp,   2, iSize);
	InitFlow (&DEST, pszBuff, NULL, 0, (iBuffSize ? iBuffSize : -1));
	if (piSize)   *piSize = 0;

	bNULLTERM = FALSE;
	iERROR    = 0;

	if (piSize)   *piSize = 0;
	if (!pszBuff) return ZIP_BUFF_TO_SMALL;

	switch (explode((PFReadBuf) zipReadFile, (PFWriteBuf) WriteBuff, pszWORKBUFF, NULL))
		{
		case CMP_INVALID_DICTSIZE: return ZIP_INVALID_DATA1;
		case CMP_INVALID_MODE:     return ZIP_INVALID_DATA2;
		case CMP_BAD_DATA:         return ZIP_INVALID_DATA3;
		case CMP_ABORT:            return ZIP_ABORT;
		}
	if (DEST.iIndex < DEST.iSize)
		pszBuff [DEST.iIndex] = '\0';

	if (piSize)   *piSize = DEST.iIndex;
	return iERROR;
	}


/*
 * File ---> IMPLODE ---> File
 *
 * Implodes data from a file to a file
 * piIn  is the size of the data read in  (may be null)
 * piOut is the size of the data written  (may be null)
 * iDataSize is the size of the data to read in, 0 means read up to 50k or so.
 */
INT Cmp2fpIfp (FILE *fpOut,    // Dest file for imploded data
					PINT piOut,     // Dest size of imploded data
					FILE *fpIn,     // Src file of data to implode
					INT  iDataSize, // Src size of data to implode
					PINT piIn)      // Src size of data imploded
	{
	fpos_t fPos, fPos2;

	InitFlow (&SRC,  NULL, fpIn,  0, (iDataSize ? iDataSize : MAXSAFEBLOCKSIZE));
	InitFlow (&DEST, NULL, fpOut, 2, 0);
	if (piOut)   *piOut = 0;
	if (piIn)    *piIn  = 0;

	bNULLTERM = FALSE;
	iERROR    = 0;

	fgetpos (fpOut, &fPos);
	FilWriteUShort (fpOut, (USHORT)DEST.iIndex);  /*--- save position to write datasize ---*/
	int ret=implode ((PFReadBuf) zipReadFile, (PFWriteBuf) zipWriteFile, pszWORKBUFF, NULL, (PUINT)&iMODE, (PUINT)&iWINDOWSIZE);
	switch (ret)
		{
		case CMP_INVALID_DICTSIZE: return ZIP_INVALID_DATA1;
		case CMP_INVALID_MODE:     return ZIP_INVALID_DATA2;
		}
	fgetpos (fpOut, &fPos2);
	fsetpos (fpOut, &fPos);

	FilWriteUShort (fpOut, (USHORT)DEST.iIndex);   /*--- save Imploded datasize ---*/
	fsetpos (fpOut, &fPos2);

	if (piOut)  *piOut = DEST.iIndex;
	if (piIn)   *piIn  = SRC.iIndex;
	return iERROR;
	}



/*
 * File ---> EXPLODE ---> File
 *
 * Explodes data from a file to a file
 * piIn  is the size of the data read in  (may be null)
 * piOut is the size of the data written  (may be null)
 */
INT Cmp2fpEfp (FILE *fpOut, // Dest file for exploded data
					PINT piOut,  // Dest size of exploded data
					FILE *fpIn,  // Src file with imploded data
					PINT piIn)   // Src size of data expanded
	{
	INT iSize;

	iSize = FilReadUShort (fpIn);
	InitFlow (&SRC,  NULL, fpIn,  2, iSize);
	InitFlow (&DEST, NULL, fpOut, 0, 0);
	if (piOut)   *piOut = 0;
	if (piIn)    *piIn  = 0;

	bNULLTERM = FALSE;
	iERROR    = 0;


	if (piOut)   *piOut = 0;
	if (piIn)    *piIn  = 0;


	switch (explode((PFReadBuf) zipReadFile, (PFWriteBuf) zipWriteFile, pszWORKBUFF, NULL))
		{
		case CMP_INVALID_DICTSIZE: return ZIP_INVALID_DATA1;
		case CMP_INVALID_MODE:     return ZIP_INVALID_DATA2;
		case CMP_BAD_DATA:         return ZIP_INVALID_DATA3;
		case CMP_ABORT:            return ZIP_ABORT;
		}
	if (piOut)   *piOut = DEST.iIndex;
	if (piIn)    *piIn  = iSize;

	return iERROR;
	}


PSZ Cmp2GetCurrBuff (void)
	{
	return pszWORKBUFF;
	}


INT Cmp2Init (PSZ pszBuff, INT iWinSize, INT iMode)
	{
	switch (iWinSize)
		{
		case 1: iWINDOWSIZE = 1024; break;
		case 2: iWINDOWSIZE = 2048; break;
		case 3: iWINDOWSIZE = 4096; break;
		}
	switch (iMode)
		{
		case 1: iMODE = CMP_BINARY; break;
		case 2: iMODE = CMP_ASCII;  break;
		}

	if (pszBuff && pszBuff != (PSZ)-1)
		pszWORKBUFF = pszBuff;
	else if (!pszWORKBUFF) 
		pszWORKBUFF = (PSZ) malloc(2*36312U);
	return 0;
	}



/*
 * Enables/Disables CRC tracking for Reading/Writing
 *
 * bRead = TRUE  enable/disable Read CRC
 * bRead = FALSE enable/disable Write CRC
 */
BOOL Cmp2EnableCRC (BOOL bEnable, BOOL bRead)
	{
	if (bRead)
		bGENREADCRC  = !!bEnable;
	else
		bGENWRITECRC = !!bEnable;

	return bEnable;
	}


BOOL Cmp2CRCEnabled (BOOL bRead)
	{
	return (bRead ? bGENREADCRC : bGENWRITECRC);
	}
	



/*
 * Sets CRC value for Reading/Writing
 *
 * bRead = TRUE  set Read CRC
 * bRead = FALSE set Write CRC
 */
ULONG Cmp2SetCRC  (ULONG ulVal, BOOL bRead)
	{
	if (bRead)
		return (ulREADCRC  = ulVal);
	else
		return (ulWRITECRC = ulVal);
	}

/*
 * Gets CRC value for Reading/Writing
 *
 * bRead = TRUE  get Read CRC
 * bRead = FALSE get Write CRC
 */
ULONG Cmp2GetCRC  (BOOL bRead)
	{
	if (bRead)
		return ulREADCRC;
	else
		return ulWRITECRC;
	}



/****************************************************************/
/*********************  Module Test  ****************************/
/****************************************************************/
//
//#define SSIZE 1024
//
//int main (int argc, CHAR *argv[])
//   {
//   PSZ    psz;
//   INT iSize;
//   FILE   *fp;
//
//   InitCompressModule (TRUE, 3, 1);
//
//   /*-- test szIfp and fpEsz --*/
//   fp  = fopen ("test.bin", "wb");
//   psz = "This is a test to see if this is a test";
//   szIfp (fp, &iSize, psz, 0);
//   printf ("%s\nstring, %d bytes, wrote in %d bytes\n", psz, strlen (psz), iSize);
//
//   psz = "This is a string with mane eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee's";
//   szIfp (fp, &iSize, psz, 0);
//   printf ("%s\nstring, %d bytes, wrote in %d bytes\n", psz, strlen (psz), iSize);
//
//   psz = "HHHHHHHHEEEEEEEEELLLLLLLLLLLLLLLLOOOOOOOOOO";
//   szIfp (fp, &iSize, psz, 0);
//   printf ("%s\nstring, %d bytes, wrote in %d bytes\n", psz, strlen (psz), iSize);
//   fclose (fp);
//
//   psz = malloc (1024);
//
//   fp  = fopen ("test.bin", "rb");
//   fpEsz (psz, 0, &iSize, fp);
//   printf ("%s\nstring, %d bytes, read in %d bytes\n", psz, strlen (psz), iSize);
//   fpEsz (psz, 0, &iSize, fp);
//   printf ("%s\nstring, %d bytes, read in %d bytes\n", psz, strlen (psz), iSize);
//   fpEsz (psz, 0, &iSize, fp);
//   printf ("%s\nstring, %d bytes, read in %d bytes\n", psz, strlen (psz), iSize);
//   fclose (fp);
//
//   return 0;
//   }
//

}
