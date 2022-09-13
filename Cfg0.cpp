/*
 * Cfg.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides various file and string functions
 * that i wish were part of the standard library set.
 *
 */

#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <stdio.h>
#include <ctype.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnufile.h"
#include "gnustr.h"
#include "gnucfg.h"

extern "C"
{

#define MAXSTRINGSIZE 16384

static INT iCFGERROR = 0;

static CHAR *pszCFGERRORS[] = 
							  {"No Error",           // 0
								"file not found",     // 1
								"section not found",  // 2
								"line not found",     // 3
								"unknown error"};     // 4


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

static BOOL _SectionLineMatch (PSZ pszLine, PSZ pszSect)
	{
	PSZ psz;
	INT iLen;

	iLen = strlen (pszSect);

	if (!(psz = StrSkipBy (pszLine, " \t")))
		return FALSE;
	if (*psz != '[')
		return FALSE;
	if (!(psz = StrSkipBy (psz+1, " \t")))
		return FALSE;
	if (strnicmp (psz, pszSect, iLen))
		return FALSE;
	if (!(psz = StrSkipBy (psz + iLen, " \t")))
		return FALSE;
	if (*psz != ']')
		return FALSE;
	return TRUE;
	}

				 


/* This fn returns data from a configuration file in the ini format
 * The pszSection should not have the [] characters.
 * If there is more than 1 line in the section, the sections are combined
 * with the '\n' separator.
 * It is assumed that any path information needed is already in pszCfgFile
 *
 * returns:
 *  0  - OK
 *  1  - unable to open input file
 *  2  - unable to find section
 */
INT CfgGetSection (PSZ pszCfgFile, PSZ pszSection, PSZ *ppszData, BOOL bSkipBlank)
	{
	FILE *fp;
	CHAR szData [MAXSTRINGSIZE];
	CHAR szLine [512];
	INT  iLen;

	*szData = '\0';
	iLen  = 0;
	if (!(fp = fopen (pszCfgFile, "rt")))
		return 1;

	while (FilReadLine (fp, szLine, ";", sizeof szLine) != -1)
		{
		if (!_SectionLineMatch (szLine, pszSection))
			continue;

		while (FilReadLine (fp, szLine, ";", sizeof szLine) != -1)
			{
			if (*szLine == '[')
				break;

			if (bSkipBlank && StrBlankLine (szLine))
				continue;

			if ((iLen += 1 + strlen (szLine)) > MAXSTRINGSIZE)
				continue;
		 
			strcat (szData, szLine);
			strcat (szData, "\n");
			}
//
// This removes the last \n
//
//      if (*szData)
//         szData[strlen (szData) -1] = '\0';
		*ppszData = strdup (szData);
		fclose (fp);
		return 0;
		}
	fclose (fp);
	return 2;
	}

										 
/*
 * current error value is returned
 * if pszErrStr!=NULL the error string is returned
 *
 * 0 no error
 * 1 file not found
 * 2 section not found
 * 3 line not found
 * 4
 * 5
 */
INT CfgGetError (PSZ *ppszErrStr)
	{
	if (ppszErrStr)
		*ppszErrStr = pszCFGERRORS [iCFGERROR];
	return iCFGERROR;
	}



PSZ CfgSetError (INT i)
	{
	iCFGERROR = i;
	return NULL;
	}



BOOL CfgEndOfSection (PSZ pszLine)
	{
//   PSZ psz;
//
//   if (!(psz = StrSkipBy (pszLine, " \t")))
//      return FALSE;
//   return (*psz == '[');
	return (*pszLine == '[');
	}



/*
 * Opens the config file and looks for the named section
 * if the section is found, the file ptr is returned
 * file ptr points to start of 1st line in section
 * If file or section not found, error is set and NULL
 * is returned. Error values and strings can be obtained
 * from the ArgGetErr function
 */
FILE * CfgFindSection (PSZ pszFile, PSZ pszSect)
	{
	FILE *fp;
	CHAR szData [512];
	INT  iLen;

	if (!(fp = fopen (pszFile, "rt")))
		return (FILE *)CfgSetError (1);

 	FilSetLine (0);

	iLen = strlen (pszSect);
	while (FilReadLine (fp, szData, ";", sizeof szData) != -1)
		{
		if (!_SectionLineMatch (szData, pszSect))
			continue;
		return fp;
		}
	fclose (fp);
	CfgSetError (2);
	return NULL;
	}


/*
 * pszFile - config file to read
 * pszSect - section to find
 * pszLine - line to find
 * pszRet  - return string, (or NULL to malloc)
 *
 * pszRet or malloced string is returned
 *
 * NULL is returned on error
 */

PSZ CfgGetLine (PSZ pszFile, PSZ pszSect, PSZ pszLine, PSZ pszRet)
	{
	FILE *fp;
	CHAR szData [512];
	INT  iLen;
	PSZ  psz;

	if (!(fp = CfgFindSection (pszFile, pszSect)))
		return NULL; // error already set

	iLen = strlen (pszLine);

	while (FilReadLine (fp, szData, ";", sizeof szData) != -1)
		{
		if (!(psz = StrSkipBy (szData, " \t")))
			continue;

		if (*psz == '[')
			{
			fclose (fp);
			return CfgSetError (3);
			}
		if (strnicmp (psz, pszLine, iLen))
			continue;
		if (!(psz = StrSkipBy (psz + iLen, " \t")))
			continue;

		if (*psz != '=')
			continue;

//      if (!(psz = StrSkipBy (psz+1, " \t")))
//         continue;

		/*--- allow explicit empty entries ---*/
		if (!(psz = StrSkipBy (psz+1, " \t")))
			psz = "";

		strcpy ((pszRet ? pszRet : szData), psz);
		StrClip((pszRet ? pszRet : szData), " ");  /*--remove trailing spc--*/

		fclose (fp);
		if (!pszRet)
			return strdup (szData);
		return pszRet;
		}
	fclose (fp);
	return NULL;
	}

/*
 * This fn is different in that it does not malloc!
 *
 */
INT CfgGetNextSection (FILE *fp, PSZ pszSection, PSZ pszData, INT iMaxLen)
	{
	CHAR  szLine [512];
	INT   iLen;
	PSZ   psz;
	ULONG ulPos;

	iLen  = 0;
	*pszData = '\0';
	while (FilReadLine (fp, szLine, ";", sizeof szLine) != -1)
		{
		if (*szLine != '[')
			continue;

		StrStrip (szLine, " \t[");
		if (!(psz = strchr (szLine, ']')))
			continue;
		*psz = '\0';
		StrClip (szLine, " \t");
		strcpy (pszSection, szLine);

		while (TRUE)
			{
			ulPos = ftell (fp);

			if (FilReadLine (fp, szLine, ";", sizeof szLine) == -1)
				break;

			if (*szLine == '[')
				{
				fseek (fp, ulPos, SEEK_SET);
				break;
				}
//       if (bSkipBlank && StrBlankLine (szLine))
//          continue;

			if ((iLen += 1 + strlen (szLine)) > iMaxLen)
				continue;

			strcat (pszData, szLine);
			strcat (pszData, "\n");
			}
		return 0;
		}
	return -1;
	}



/*
 * Reads the input stream looking for the next section
 * if the section is found, the file ptr is returned
 * If a section is not found, error is set and NULL
 * is returned. Error values and strings can be obtained
 * from the ArgGetErr function
 */
FILE * CfgFindNextSection (FILE *fp, PSZ pszSect)
	{
	CHAR szData [512];
	PSZ  p, psz;

	while (FilReadLine (fp, szData, ";", sizeof szData) != -1)
		{
		if (*szData != '[')
			continue;
		if (!(psz = StrSkipBy (szData+1, " \t")))
			continue;
		if (!(p = strchr (psz, ']')))
			continue;
		*p = '\0';
		strcpy (pszSect, psz);
		return fp;
		}
	fclose (fp);
	CfgSetError (2);
	return NULL;
	}




BOOL CfgGetNextEntry (FILE *fp, PSZ pszVar, PSZ pszVal)
	{
	CHAR szData [512];
	PSZ  psz, psz2;

	while (FilReadLine (fp, szData, ";", sizeof szData) != -1)
		{
		if (!(psz = StrSkipBy (szData, " \t")))
			continue;

		if (*psz == '[')   // this section is over
			return FALSE;

		if (!(psz2 = strchr (psz, '=')))
			continue;
		*psz2 = '\0';

		strcpy (pszVar, psz);
		strcpy (pszVal, psz2+1);
		StrStrip (StrClip (pszVar, " \t"), " \t");
		StrStrip (StrClip (pszVal, " \t"), " \t");
		return TRUE;
		}
	return FALSE;
	}      



/*
 *
 *
 *
 */
INT CfgSetLine (PSZ pszFile, PSZ pszSect, PSZ pszLine, PSZ pszVal)
	{
	FILE  *fpIn, *fpOut;
	CHAR  szData [512];
	CHAR  szTempFile [256];
	ULONG ulItemPos, ulSectionPos, ulDest;
	INT   iLen;
	PSZ   psz;
	BOOL  bDone;

	if (!(fpIn = CfgFindSection (pszFile, pszSect)))
		{
		if (iCFGERROR == 1) // file not found
			return 1;
		if (!pszVal)
			return 0;
		fpOut = fopen (pszFile, "at");
		fprintf (fpOut, "\n\n[%s]\n", pszSect);
		fprintf (fpOut, "%s=%s\n", pszLine, pszVal);
		fclose (fpOut);
		return 0;
		}
	ulItemPos    = 0;
	ulSectionPos = ftell (fpIn);      // line after section
	iLen         = strlen (pszLine);

	while (FilReadLine (fpIn, szData, ";", sizeof szData) != -1)
		{
		if (!(psz = StrSkipBy (szData, " \t")))
			continue;
		if (*psz == '[')
			break;
		if (strnicmp (psz, pszLine, iLen))
			continue;
		if (!(psz = StrSkipBy (psz + iLen, " \t")))
			continue;
		if (*psz != '=')
			continue;
		ulItemPos = ftell (fpIn);;
		break;
		}

	strcpy (szTempFile, pszFile);             // no temp path, use file pathspec
	if (psz = strrchr (szTempFile, '.'))      //
		*psz = '\0';                           //
	strcat (szTempFile, ".@@@");              // add temp extension

	if (!(fpOut = fopen (szTempFile, "wt")))
		{
		fclose (fpIn);
		return iCFGERROR = 1;
		}

	rewind (fpIn);
	ulDest = (ulItemPos ? ulItemPos : ulSectionPos);
	bDone = FALSE;

	while (FilReadLine (fpIn, szData, "", sizeof szData) != -1)
		{
		if (bDone || (ftell (fpIn) < (long)ulDest))
			{
			fprintf (fpOut, "%s\n", szData);
			continue;
			}
		if (!ulItemPos)                       // adding new entry
			fprintf (fpOut, "%s\n", szData);   // write section header
			
		if (pszVal)
			fprintf (fpOut, "%s=%s\n", pszLine, pszVal);
		bDone = TRUE;
		}
	fclose (fpIn);
	fclose (fpOut);
	unlink (pszFile);
	rename (szTempFile, pszFile);
	return 0;
	}

}