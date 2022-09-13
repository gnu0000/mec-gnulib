/*
 * Fil3.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides a 4Dos description functions
 */

#include <stdio.h>
#include <io.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnufile.h"


extern "C"
{

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * If line does not match, returns NULL
 * If line does match, returns 1st CHAR after space.
 *
 */
PSZ LineMatches (PSZ pszName, INT iNameLen,
					  PSZ pszLine, INT iLineLen)
	{
	if (iLineLen <= iNameLen || pszLine[iNameLen] != ' ')
		return NULL;
	if (strnicmp (pszName, pszLine, iNameLen))
		return NULL;
	return pszLine + iNameLen+1;
	}


/*
 * read 4dos description of file
 * Uses fully qualified file names
 *
 */
PSZ FilGet4DosDesc (PSZ pszFile, PSZ pszBuff)
	{
	FILE *fpDesc;
	INT  iNameLen, iLineLen;
	PSZ  p, p2;
	CHAR szLine [256];
	CHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
	CHAR szName [_MAX_FNAME + _MAX_EXT + 1], szExt[_MAX_EXT];

	*pszBuff = '\0';

	_splitpath (pszFile, szDrive, szDir, szName, szExt);

	strcat (szName, szExt);

	/*--- read description from 4dos file ---*/
	sprintf (szLine, "%s%sDESCRIPT.ION", szDrive, szDir);

	if (!(fpDesc = fopen (szLine, "rt")))
		return NULL;

	iNameLen = strlen (szName);

	while ((iLineLen = FilReadLine (fpDesc, szLine, "", sizeof szLine)) != -1)
		{
		if (p = LineMatches (szName, iNameLen, szLine, iLineLen))
			{
			for (p2 = pszBuff; *p && *p !='\04' && *p !='\n';)
				*p2++ = *p++;
			*p2 = '\0';
			fclose (fpDesc);
			return pszBuff;
			}
		}
	fclose (fpDesc);
	return NULL;
	}




/*
 * Writes 4dos description
 * Uses fully qualified file names
 * returns TRUE if successful
 */
BOOL FilPut4DosDesc (PSZ pszFile, PSZ pszDesc)
	{
	FILE *fpIn, *fpOut;
	INT  iLineLen, iNameLen;
	CHAR szInFile[256], szOutFile[256], szLine[256];
	CHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
	CHAR szName[_MAX_FNAME + _MAX_EXT + 1], szExt[_MAX_EXT];

	if (!pszDesc || !*pszDesc || !pszFile || !*pszFile)
		return FALSE;

	_splitpath (pszFile, szDrive, szDir, szName, szExt);
	strcat (szName, szExt);
	iNameLen = strlen (szName);

	sprintf (szOutFile, "%s%sDESCRIPT.GNU", szDrive, szDir);
	if (!(fpOut = fopen (szOutFile, "wt")))
		return FALSE;

	/*--- read description from 4dos file ---*/
	sprintf (szInFile, "%s%sDESCRIPT.ION", szDrive, szDir);
	if (fpIn = fopen (szInFile, "rt"))
		{
		while ((iLineLen = FilReadLine (fpIn, szLine, "", sizeof szLine)) != -1)
			{
			if (!LineMatches (szName, iNameLen, szLine, iLineLen))
				fprintf (fpOut, "%s\n", szLine);
			}
		fclose (fpIn);
		unlink (szInFile);
		}
// fprintf (fpOut, "%s %s\04\n", szName, pszDesc); // according to docs
	fprintf (fpOut, "%s %s\n", szName, pszDesc);
	fclose (fpOut);

	rename (szOutFile, szInFile);

#if defined (VER_NT)
	return SetFileAttributes (szInFile, _A_HIDDEN);
#else
	_dos_setfileattr (szInFile, _A_HIDDEN);
#endif

	return TRUE;
	}

}