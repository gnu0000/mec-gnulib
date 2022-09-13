/*
 * Res4.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides resource manipulation functions
 *
 */

#include <io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnudir.h"
#include "gnuzip.h"
#include "gnures.h"
#include "res.h"

extern "C"
{

#define uWORKBUFFSIZE 4096


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

INT resCopyFile (FILE   *fpBase,    // base file to add to at proper fppos
					  PSZ    pszFile,    // file to add
					  INT    bBin,       // open in bin mode?
					  INT    iComp,      // compress file?
					  PULONG pulOrgLen,  // original file len
					  PULONG pulResLen)  // resource data len
	{
	FILE *fpData;
	INT  iDestLen, iSrcLen;
	PSZ  pszWorkBuff;

	if (!(fpData = fopen (pszFile, (bBin ? "rb" : "rt"))))
		return ResSetErrN (8, NULL); // error condition

	if (iComp)
		Cmp0ImplodeFile (fpBase, fpData, FALSE, pulResLen, pulOrgLen);
	else
		{
		pszWorkBuff = (PSZ) malloc (uWORKBUFFSIZE);
		*pulOrgLen = *pulResLen = 0;
		while (!feof (fpData))
			{
			iSrcLen  = fread (pszWorkBuff, 1, uWORKBUFFSIZE, fpBase);
			iDestLen = fwrite (pszWorkBuff, 1, iSrcLen, fpData);
			*pulResLen += (ULONG)iDestLen;
			*pulOrgLen += (ULONG)iSrcLen;
			}
		free (pszWorkBuff);
		}
	fclose (fpData);
	return 0;
	}



/*
 * Reads resource list from a file
 * mallocs and returns the list.
 * if no resources, an empty list (1 term entry) is returned
 */
PRES ResReadRecs (FILE *fpIn)
	{
	PRES  pres;
	ULONG ulMark;
	LONG  lOffset;
	INT   iAppends;

	for (lOffset=-6; lOffset > -200; lOffset--)
		{
		fseek (fpIn, lOffset, SEEK_END);
		fread (&ulMark, sizeof (ULONG), 1, fpIn);
		if (ulMark == ADDEXEMARK)
			break;
		}
	if (ulMark != ADDEXEMARK)
		{
		/*--- no files appended ---*/
		pres = (PRES) malloc (sizeof (RES));
		pres[0].szName[0] = '\0';
		return pres;
		}
	else
		{
		iAppends = 0;
		fread (&iAppends, sizeof (USHORT), 1, fpIn);
		fseek (fpIn, lOffset - iAppends * sizeof (RES), SEEK_END);
		if (!(pres = (PRES) malloc (sizeof (RES) * (iAppends +1))))
			return (PRES) ResSetErr (3, NULL);
		pres[iAppends].szName[0] = '\0';
		fread (pres, sizeof (RES), iAppends, fpIn);
		return pres;
		}
	}




/*
 * Adds a resource to the Base File fpBase
 * pres is updated
 * if bBin is FALSE, uBuffLen can be 0
 * Error code is returned
 * 
 */
INT ResAddBuffer (PRES  *ppres,      // resource table (which is updated)
						FILE  *fpBase,     // file containing resources (upd)
						PSZ   pszName,     // new resource name
						PSZ   pszBuff,     // new resource data
						INT   iBuffLen,    // data size (0=null term)
						INT   iComp,       // compress data
						BOOL  bBin,        // mark as bin?  (just stored)
						ULONG ulSpecial)   // special val?  (just stored)
	{
	PRES  pres;
	ULONG ulEndPos, ulOrgLen, ulResLen, ulMark;
	INT   iAppends, iDestLen, iErr;

	pres = *ppres;

	if (!pres || !fpBase || !pres->szName[0])
		return ResSetErrN (9, NULL);

	/*--- count the number of resource recs ---*/
	for (iAppends=0; pres && pres[iAppends].szName[0]; iAppends++)
		;

	if (iAppends)
		ulEndPos = pres[iAppends-1].ulStart + pres[iAppends-1].ulResLen;
	else
		ulEndPos = filelength (fileno (fpBase));

	fseek (fpBase, ulEndPos, SEEK_SET);

	if (pszBuff == (PSZ)-1)      /*--- Data from a file ---*/
		{
		if (iErr = resCopyFile (fpBase, pszName, bBin, iComp, &ulOrgLen, &ulResLen))
			return iErr;
		}
	if (!pszBuff)  /*--- No Data           ---*/
		iBuffLen = iDestLen = 0;

	else if (iComp)             /*--- Compressed Data   ---*/
		Cmp0Write (fpBase, pszBuff, iBuffLen, TRUE, &iDestLen);

	else                        /*--- uncompressed data ---*/
		{
		/*--- if len not provided, calc length ---*/
		iBuffLen = (iBuffLen ? iBuffLen : strlen (pszBuff));
		iDestLen = fwrite (pszBuff, 1, iBuffLen, fpBase);
		fputc ('\0', fpBase);
		}
	ulOrgLen = iBuffLen;
	ulResLen = iDestLen;

	/*--- make space for new rec ---*/
	iAppends++;
	*ppres = pres = (PRES) realloc (pres, sizeof (RES) * (iAppends + 1));
	pres[iAppends].szName[0] = '\0';

	/*--- write new rec ---*/
	memset (pres[iAppends-1].szName, 0, 16);
	DirStripPath (pres[iAppends-1].szName, pszName, TRUE);

	pres[iAppends-1].ulStart      = ulEndPos;
	pres[iAppends-1].ulOrgLen     = ulOrgLen;
	pres[iAppends-1].ulResLen     = ulResLen;
	pres[iAppends-1].iCompression = iComp;
	pres[iAppends-1].bBin         = bBin;
	pres[iAppends-1].ulSpecial    = ulSpecial;

	fwrite (pres, sizeof (RES), iAppends, fpBase);

	/*--- write terminating data ---*/
	ulMark = ADDEXEMARK;
	fwrite (&ulMark,   sizeof (ULONG),  1, fpBase);
	fwrite (&iAppends, sizeof (USHORT), 1, fpBase);

	return ResGetErr (NULL);
	}




/*
 * Adds a resource to the Base File fpBase
 * pres is updated
 * Like ResAddBuffer, except works with a file
 * Error code is returned
 *
 */
INT ResAddFile (PRES *ppres, FILE *fpBase, PSZ pszInFile, INT iComp, BOOL bBin, ULONG ulSpecial)
	{
	return ResAddBuffer (ppres, fpBase, pszInFile, (PSZ)-1,
								0, iComp, bBin, ulSpecial);
	}




/*
 * Duplicates a resource from fpBase to fpDest
 * Error code is returned
 * pres must point to the full pres
 *
 */
INT ResDupe (PRES pres, FILE *fpBase, FILE *fpDest) 
	{
	PSZ   pszWorkBuff;
	ULONG ulEndPos;
	INT   iInSize;

	if (!pres || !fpBase || !pres->szName[0])
		return ResSetErrN (9, NULL);

	ulEndPos = filelength (fileno (fpDest));
	fseek (fpDest, ulEndPos, SEEK_SET);
	fseek (fpBase, pres->ulStart, SEEK_SET);

	pszWorkBuff = (PSZ) malloc (uWORKBUFFSIZE);
	while (!feof (fpBase))
		{
		iInSize = fread (pszWorkBuff, 1, uWORKBUFFSIZE, fpBase);
		fwrite (pszWorkBuff, 1, iInSize, fpDest);
		}
	free (pszWorkBuff);
	return 0;
	}



///*
// * Extracts a resource to a file
// * pres is the resource RES to extract
// * pszDestFile is the file to create, it may be null, in which case
// * the resource name is used as the filename
// */
//INT ResExtract (PRES pres, FILE *fpBase, PSZ pszDestFile)
//   {
//   ULONG  ulSrcRead, ulTotalWrite;
//   INT uOutSize, iInSize, uReadSize;
//   PSZ    pszDest, pszWorkBuff;
//   FILE   *fpOut;
//
//   if (!pres || !fpBase || !pres->szName[0])
//      return ResSetErrN (9, NULL);
//
//   pszDest = (pszDestFile ? pszDestFile : pres->szName);
//
//   if (!(fpOut = fopen (pszDest, (pres->bBin ? "wb" : "wt"))))
//      return ResSetErrN (1, NULL);
//
//   ulSrcRead = ulTotalWrite = 0;
//   fseek (fpBase, pres->ulStart, SEEK_SET);
//
//   if (pres->iCompression)
//      Cmp0ExplodeFile (fpOut, fpBase, &ulTotalWrite, &ulSrcRead);
//   else
//      {
//      pszWorkBuff = malloc (uWORKBUFFSIZE);
//      while (ulSrcRead < pres->ulResLen)
//         {
//         uReadSize = (INT) min (pres->ulResLen, (ULONG)uWORKBUFFSIZE);
//         iInSize   = fread (pszWorkBuff, 1, uReadSize, fpBase);
//         uOutSize  = fwrite (pszWorkBuff, 1, iInSize, fpOut);
//         ulSrcRead    += iInSize;
//         ulTotalWrite += uOutSize;
//         }
//      free (pszWorkBuff);
//      }
//   fclose (fpOut);
//
//   if (ulTotalWrite != pres->ulOrgLen)
//      return ResSetErrN (7, NULL);
//   return 0;
//   }

/*
 * Removes all resources from fpBase
 * pres is freed
 * Error code is returned
 *
 *
 */
INT ResStrip (PRES pres, FILE *fpBase)
	{
	if (!pres || !pres[0].szName[0])
		return 0;
	chsize (fileno (fpBase), pres[0].ulStart);

	free (pres);
	pres = (PRES) malloc (sizeof (RES));
	pres[0].szName[0] = '\0';
	return 0;
	}

}