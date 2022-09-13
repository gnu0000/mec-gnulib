/*
 * Zip3.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides low-level compression functions
 *
 * Cmp0ReadBlock and Cmp0WriteBlock read and write a single compressed
 * block.  Cmp0Read and Cmp0Write read and write complete streams.
 * These functions do NOT work in conjunction with CmpRead and CmpWrite
 * The write functions simply dump the data as data blocks
 * The read functions read full blocks or the full data stream into a buffer
 * There is no buffering involved with these functions
 *
 *
 ****************************************************************************
 *
 * INT Cmp0ReadBlock  (fp, pszBuff, iMaxLen, piLen)
 * INT Cmp0WriteBlock (fp, pszBuff, iLen,    piLen)
 * INT Cmp0Read       (fp, pszBuff, iMaxLen, piLen)
 * INT Cmp0Write      (fp, pszBuff, iLen,    bTerm, piLen)
 * INT Cmp0ImplodeFile(fpDest, fpSrc, bTerm, pulDestLen, pulSrcLen)
 * INT Cmp0ExplodeFile(fpDest, fpSrc, pulDestLen, pulSrcLen)
 *
 * INT Cmp0WriteTerm  (FILE *fp)
 * INT Cmp0SetBlockSize (INT iSize)
 *
 * ULONG  Cmp0EnableCRC (BOOL bEnable)
 * ULONG  Cmp0SetCRC (ULONG ulCRC)
 * ULONG  Cmp0GetCRC (void)
 * ULONG  Cmp0GenerateCRC (fp)
 *
 ****************************************************************************
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuzip.h"
#include "zip.h"

extern "C"
{

static INT iDEFBLOCKSIZE = 4096;

typedef void (*STATUSPROC) (ULONG ul);

STATUSPROC pfnSTATUSFN = NULL;

static PFNX pfnXlateBuffer = NULL;

/*************************************************************************/
/*                                                                       */
/*                                                                       */
/*                                                                       */
/*************************************************************************/

void Cmp0SetXlateBuffer (PFNX pfnXlate)
	{
	pfnXlateBuffer = pfnXlate;
	}

PFNX Cmp0GetXlateBuffer (void)
	{
	return pfnXlateBuffer;
	}


INT Cmp0WriteTerm  (FILE *fp)
	{
	return (_cmpWriteTerm (fp) ? 0 : ZIP_CANT_WRITE);
	}


INT Cmp0SetBlockSize (INT iSize)
	{
	return iDEFBLOCKSIZE = iSize;
	}


/*************************************************************************/


/*
 * read a block
 * fp      - file to read from
 * pszBuff - destination buffer
 * iMaxLen - size of destination buffer
 * piLen   - size of data read in to buffer
 *
 * returns: Error Code (0 = OK)
 */
INT Cmp0ReadBlock (FILE *fp, PSZ pszBuff, INT iMaxLen, PINT piLen)
	{
	INT iRet, iUBlockLen;

	if (iRet = _cmpReadBlk (fp, pszBuff, iMaxLen, 0,
						  _cmpDefaultType (), _cmpDefaultWindow (), 
						  piLen, &iUBlockLen, NULL))
		return iRet;

	if (iUBlockLen > iMaxLen)
		return ZIP_BUFF_TO_SMALL; // partial block read

	return 0;
	}


/*
 * write a block.
 * fp      - file to write to
 * pszBuff - src buffer
 * iLen    - size to write
 * piLen   - size of data written (the block length)
 *
 * returns: Error Code (0 = OK)
 */
INT Cmp0WriteBlock (FILE *fp, PSZ pszBuff, INT iLen, PINT piLen)
	{
	return _cmpWrtBlk (fp, pszBuff, iLen, _cmpDefaultType (), 
							 _cmpDefaultWindow (), piLen);
	}


/*
 * Read a block stream into a string up to /including the terminator
 * fp      - file to read from
 * pszBuff - destination buffer
 * iMaxLen - size of destination buffer
 * piLen   - size of data read in to buffer
 *
 * returns: Error Code (0 = OK)
 */
INT Cmp0Read (FILE *fp, PSZ pszBuff, INT iMaxLen, PINT piLen)
	{
	INT iRet, iLen;

	for (*piLen=0; *piLen < iMaxLen; *piLen += iLen)
		{
		if (iRet = Cmp0ReadBlock (fp, pszBuff, iMaxLen - *piLen, &iLen))
			return (iRet == ZIP_END_OF_STREAM ? 0 : iRet);

		pszBuff += iLen;
		}
	return ZIP_BUFF_TO_SMALL;
	}


/*
 * Write a block stream. Does not write the terminator
 * fp      - file to write to
 * pszBuff - src buffer
 * iLen    - size to write
 * piLen   - size of data written (the compressed stream length)
 *
 * returns: Error Code (0 = OK)
 */
INT Cmp0Write (FILE *fp, PSZ pszBuff, INT iLen, BOOL bTerm, PINT piLen)
	{
	INT iRet, iWrote, iBlockSize, iCompLen;

	*piLen = 0;
	for (iWrote = 0; iBlockSize = min (iDEFBLOCKSIZE, iLen - iWrote); )
		{
		if (iRet = Cmp0WriteBlock (fp, pszBuff, iBlockSize, &iCompLen))
			return iRet;
		pszBuff += iBlockSize;
		*piLen  += iCompLen;
		}
	if (bTerm)
		{
		if (!_cmpWriteTerm (fp))
			return ZIP_CANT_WRITE;
		*piLen  += 2;;
		}

	return 0;
	}




/*************************************************************************/

void Cmp0SetStatusFn (STATUSPROC pfnStatusFn)
	{
	pfnSTATUSFN = pfnStatusFn;
	}

INT Cmp0ImplodeFile (FILE *fpDest, FILE *fpSrc, BOOL bTerm, PULONG pulDestLen, PULONG pulSrcLen)
	{
	PSZ pszBuff;
	INT iRead, iWrote, iRet;

	if (pulDestLen) *pulDestLen = 0;
	if (pulSrcLen ) *pulSrcLen  = 0;

	if (!( pszBuff = (PSZ) malloc (iDEFBLOCKSIZE)))
		return ZIP_MEMORY;

	while (!feof (fpSrc))
		{
		iRead = fread (pszBuff, 1, iDEFBLOCKSIZE, fpSrc);

		if (pfnXlateBuffer)
			pfnXlateBuffer (pszBuff, iRead);

		if (iRet = Cmp0WriteBlock (fpDest, pszBuff, iRead, &iWrote))
			{
			free (pszBuff);
			return iRet;
			}
		if (pulDestLen) *pulDestLen += (ULONG)iWrote;
		if (pulSrcLen ) *pulSrcLen  += (ULONG)iRead;

		if (pfnSTATUSFN && pulDestLen)
			pfnSTATUSFN (*pulDestLen);
		}
	free (pszBuff);
	if (bTerm)
		{
		Cmp0WriteTerm (fpDest);
		if (pulDestLen) *pulDestLen += 2;
		}
	return 0;
	}

/*
 * This will only read files with block sizes <= iDEFBLOCKSIZE
 * Otherwise, use CmpExplodeFile!
 */
INT Cmp0ExplodeFile (FILE *fpDest, FILE *fpSrc, PULONG pulDestLen, PULONG pulSrcLen)
	{
	PSZ pszBuff;
	INT iRet, iRead, iWrote;

	if (pulDestLen) *pulDestLen = 0;
	if (pulSrcLen ) *pulSrcLen  = 0;

	if (!(pszBuff = (PSZ) malloc (iDEFBLOCKSIZE)))
		return ZIP_MEMORY;

	while (TRUE)
		{
		if (iRet = Cmp0ReadBlock (fpSrc, pszBuff, iDEFBLOCKSIZE, &iRead))
			{
			free (pszBuff);
			return (iRet == ZIP_END_OF_STREAM ? 0 : iRet);
			}
		if (pfnXlateBuffer)
			pfnXlateBuffer (pszBuff, iRead);

		iWrote = fwrite (pszBuff, 1, iRead, fpDest);
		if (pulDestLen) *pulDestLen += (ULONG)iWrote;
		if (pulSrcLen ) *pulSrcLen  += (ULONG)iRead;

		if (pfnSTATUSFN && pulDestLen)
			pfnSTATUSFN (*pulDestLen);   // not a fn!!!!!!

		}
	}

/*************************************************************************/

}