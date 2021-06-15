/*
 * Res1.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides various resource reading functions
 * It is used in conjunction with my GNURES program
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <share.h>
#include <string.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuzip.h"
#include "gnures.h"
#include "res.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * This fn returns a pointer to the resource data.
 * If an error occurs, call ResGetErr to get error string
 *
 * [I] pszResFile ... Filename containing the resource.
 *                     Typically,  it will be the name of the Exe file.
 *                     A ".EXE" will be appended if a "." is not part 
 *                     of the filename.
 * [I] pszRes ....... Name of the Resource to load.
 *                     This param may also be a cast of an integer index.
 *                     This parameter may be NULL, which will match the
 *                     first resource in the file.
 * [I] pBuff  ....... Buffer where the resource is loaded to.
 *                     This param may be NULL, in which case the resource
 *                     data is malloced and returned.
 * [I] uMaxLen ...... Max size of the pBuff.
 *                     If pBuff is NULL, this is the max size to malloc
 *                     This param may be 0, meaning no bounds check.
 * [O] piReadLen .... Pointer to the size of the data read. 
 *                     This param may be NULL
 *
 * RETURN VALUE ..... Pointer to the resource data. This is either pBuff
 *                     or a newly malloced buffer. 
 */
PVOID ResLoadData (PSZ pszResFile, PSZ pszRes, PVOID pBuff, INT iMaxLen, PINT piReadLen)
	{
	RES  res;
	PSZ  pszBuff;
	FILE *fpExe;
	INT  iBuffSize, iDataSize, iReadSize;

	if (piReadLen)
		*piReadLen = 0;
	if (pBuff)
		*(PSZ)pBuff = '\0';

	if (!(fpExe = ResGetPtr (pszResFile, pszRes, &res)))
		return NULL;

	iDataSize = (res.ulOrgLen > 65535L ? 64000U : (INT) res.ulOrgLen);
	iBuffSize = (iMaxLen ? min (iMaxLen, iDataSize) : iDataSize);

	if (!(pszBuff = (PSZ) (pBuff ? pBuff : malloc (iBuffSize+1))))
		return ResSetErr (3, fpExe);

	if (res.iCompression)
		Cmp0Read (fpExe, pszBuff, iBuffSize, &iReadSize);
	else
		iReadSize = fread (pszBuff, 1, iDataSize, fpExe);

	fclose (fpExe);

	if ((iReadSize < iMaxLen) || !iMaxLen)
		pszBuff[iReadSize] = '\0';

	if (piReadLen)
		*piReadLen = iReadSize;
	return pszBuff;
	}



/*
 * Extracts a resource to a file
 * fpDest is the file to write to
 * fpSrc is the pointer to the resource (ResGetPtr will do it)
 * pres may be NULL iff resource is compressed
 *
 * returns error code: 0 - ok
 *                     3 - mem error
 *                     7 - error extracting
 *
 */
INT ResExtract (FILE *fpDest, FILE *fpSrc, PRES pres)
	{
	PFNX  pfnXlateBuffer;
	INT   iSize = 1, iInSize, iOutSize;
	ULONG ulRead, ulWrote;
	PSZ   pszBuff;

	if (!fpDest || !fpSrc)
		return FALSE;

	if (!pres || pres->iCompression) // compressed resource
		{
		if (Cmp0ExplodeFile (fpDest, fpSrc, &ulWrote, NULL))
			return 7; // unzip error
		if (pres && (ulWrote != pres->ulOrgLen))
			return 7; // size error
		return 0; // ok
		}

	/*--- uncompressed resource ---*/
	if (!(pszBuff = (PSZ) malloc (1024)))
		return 3;

	for (ulRead = ulWrote = 0; iSize && ulRead < pres->ulResLen; )
		{
		iSize = (INT)min (1024L, (pres->ulResLen - ulRead));

		iInSize  = fread  (pszBuff, 1, iSize, fpSrc);

		if (pfnXlateBuffer = Cmp0GetXlateBuffer ())
			pfnXlateBuffer (pszBuff, iInSize);

		iOutSize = fwrite (pszBuff, 1, iInSize, fpDest);
		if (iInSize != iSize || iOutSize != iInSize)
			break;
		ulRead  += iInSize;
		ulWrote += iOutSize;
		}
	free (pszBuff);
	if (ulWrote != pres->ulOrgLen)
		return 7; // unzip error
	return 0;
	}

