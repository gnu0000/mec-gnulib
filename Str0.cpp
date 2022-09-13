/*
 * Str0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides various file and string functions
 * that i wish were part of the standard library set.
 *
 *
 ****************************************************************************
 *
 *  PSZ  StrClip (pszStr, pszClipList)      // Trailing
 *  PSZ  StrStrip (pszStr, pszStripList)    // Leading
 *  PSZ  StrExtract (pszStr, pszExtractList)
 *  INT  StrGetWord (*ppsz, pszWord, pszDelimeters, bHasQuotes, bEatDelim)
 *  INT  StrEatWord (*ppszStr, pszTarget)
 *  PSZ  StrSkipBy (psz, pszSkipList)
 *  PSZ  StrSkipTo (psz, pszToList)
 *  BOOL StrBlankLine (pszLine)
 *  PSZ  strnchr (PSZ psz, int c, INT iLen)
 *
 *  BOOL StrMatches (pszName, pszWildCard, bCase)
 *
 *  PSZ  StrMakeCSVField (pszDest, pszSrc)
 *  PSZ  StrUnCSV (pszDest, pszSrc)
 *  PSZ  StrAddQuotes (psz)
 *  PSZ  StrPadLeft0 (sz, uSize)
 *  PSZ  StrMakePPSZ (pszStr, pszDelim, bCSV, bTrim, puCols)
 *  PSZ  StrCookLine (PSZ pszDest, PSZ pszSrc, BOOL bNewLines)
 *
 ****************************************************************************
 *
 */

#include "stdafx.h"
#include "gnumisc.h"
#include "gnustr.h"


extern "C"
{

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


/*
 * returns ptr into psz
 * skips to first non pszSkipList CHAR
 * returns NULL if end of string reached
 */
PSZ StrSkipBy (PSZ psz, PSZ pszSkipList)
	{
	PSZ p;

	if (!psz || !pszSkipList) 
		return NULL;

	for (p = psz; *p && strchr (pszSkipList, *p); p++)
		;
	return (*p ? p : NULL);
	}



/*
 * returns ptr into psz
 * skips to first pszSkipList CHAR
 * returns NULL if end of string reached
 */
PSZ StrSkipTo (PSZ psz, PSZ pszToList)
	{
	PSZ p;

	if (!psz || !pszToList) 
		return NULL;

	for (p = psz; *p && !strchr (pszToList, *p); p++)
		;
	return (*p ? p : NULL);
	}



/*
 * Removes trailing chars from str
 */
PSZ StrClip (PSZ pszStr, PSZ pszClipList)
	{
	int i;

	if (!pszStr || !pszClipList) 
		return pszStr;

	for (i=strlen(pszStr); i && strchr (pszClipList, pszStr[i-1]); i--)
		pszStr[i-1] = '\0';

//	i = strlen (pszStr);
//	while (i >= 0 && strchr (pszClipList, pszStr[i-1]) != NULL)
//		pszStr[--i] = '\0';
	return pszStr;
	}



/*
 * Removes leading chars from str
 * Moves memory, not ptr;
 */
PSZ StrStrip (PSZ pszStr, PSZ pszStripList)
	{
	INT iLen, i = 0;

	if (!pszStr || !pszStripList) 
		return pszStr;

	if (!(iLen = strlen (pszStr)))
		return pszStr;
	while (i < iLen && strchr (pszStripList, pszStr[i]) != NULL)
		i++;
	if (i)
		memmove (pszStr, pszStr + i, iLen - i + 1);
	return pszStr;
	}



/*
 * returns true if string contains only white space
 */
BOOL StrBlankLine (PSZ psz)
	{
	for (; psz && (*psz == ' ' || *psz == '\t'); psz++)
		;

	return (!psz || !*psz);
	}



/*
 * This is basically a strchr
 * with a string length
 */
PSZ strnchr (PSZ psz, int c, INT iLen)
	{
	if (!psz)
		return NULL;

	for (; iLen; psz++, iLen--)
		if (*psz == (UCHAR) c)
			return psz;
	return NULL;
	}


/*
 * shifts string in sz right
 * and pads on left with 0's
 */
PSZ StrPadLeft0 (PSZ psz, INT iSize)
	{
	INT i, iLen;

	if (!psz)
		return psz;

	if ((iLen = strlen (psz)) >= iSize)
		return psz;

	memmove (psz + iSize - iLen, psz, iLen);
	for (i=0; i< iSize - iLen; i++)
		psz[i] = '0';
	psz[iLen] = '\0';
	return psz;
	}


/*************************************************************************/



static BOOL Match (PSZ pszName, PSZ pszWildCard, BOOL bCase)
	{
	switch (*pszWildCard)
		{
		case '\0':
			return !*pszName;

		case '?':
			return *pszName && Match (pszName+1, pszWildCard+1, bCase);

		case '*':
			if (!pszWildCard[1])
				return TRUE;
			while (*pszName)
				{
				if (Match (pszName, pszWildCard+1, bCase))
					return TRUE;
				pszName++;
				}
			return Match (pszName, pszWildCard+1, bCase);

		default:
			if (!*pszName)
				return FALSE;

			if (bCase)
				return (*pszName == *pszWildCard) &&
						 Match (pszName+1, pszWildCard+1, bCase);
			return (toupper(*pszName) == toupper(*pszWildCard)) &&
						 Match (pszName+1, pszWildCard+1, bCase);
		}
	}



/*
 * pszWildCard is match spec, and may contain * and ?
 * wildcards may be separated with ;
 * pszName is string to test
 * if bCase=TRUE comparison is case sensitive
 * returns TRUE if match is successful
 */
BOOL StrMatches (PSZ pszName, PSZ pszWildCard, BOOL bCase)
	{
	CHAR szTmp [80];

	while (StrGetWord (&pszWildCard, szTmp, NULL, ";", FALSE, TRUE) != -1)
		{
		if (Match (pszName, szTmp, bCase))
			return TRUE;
		}
	return FALSE;
	}


/*************************************************************************/

/*
 * Skips leading spaces and tabs.
 * Eats a word/field from the front of a string and returns that word and 
 *  the rest of the string.
 * if pszWord is NULL, the word is discarded.
 * if bCSV is TRUE, the word is un-CSVified.
 * if bCSV is TRUE, wou will probable want ",\n" for pszDelimeters
 * if bEatDelim = TRUE, the returned string starts one CHAR past
 *  the delimeter.
 */
INT StrGetWord (PSZ  *ppsz,
					 PSZ  pszWord,
					 PSZ  pszSkipList,
					 PSZ  pszDelimiters,
					 BOOL bCSV,
					 BOOL bEatDelim)
	{
	BOOL bInQuotes;
	INT  c;

	if (pszWord)
		*pszWord = '\0';
	if (!ppsz || !*ppsz || !**ppsz)
		return -1;
	
	/*--- Skip past leading whitespace ---*/
	if (!pszSkipList)
		pszSkipList = (strchr(pszDelimiters, '\t') ? " " : " \t");
	while (**ppsz && strchr (pszSkipList, **ppsz))
		(*ppsz)++;

	for (bInQuotes = FALSE; c = **ppsz; (*ppsz)++)
		{
		/*--- End of the field? ---*/
		if (!bInQuotes && pszDelimiters && strchr (pszDelimiters, c))
			break;

		if (bCSV && c == '"')
			{
			if (bInQuotes && *(*ppsz+1) == '"')
				{
				(*ppsz)++;
				}
			else
				{
				bInQuotes = !bInQuotes;
				continue;
				}
			}
		if (pszWord)
			*pszWord++ = (CHAR)c;
		}
	if (pszWord)
		*pszWord = '\0';

	if (bEatDelim)
		return (int) (**ppsz ? *((*ppsz)++): 0);
	else
		return (**ppsz);
	}



/*
 * 0 = match
 * 1 = no match
 * 2 = end of string
 */
INT StrEatWord (PSZ *ppszStr, PSZ pszTarget)
	{
	CHAR szWord [512];

	if ((StrGetWord (ppszStr, szWord, NULL, " ,\t\n", 0, 1)) == -1)
		return 2;
	return !!stricmp (pszTarget, szWord);
	}



/*
 * This fn is used to make sure a certain character is at the head
 * of a string.  This CHAR (and any leading whitespace) is removed
 * from the front of the string, and the CHAR eaten is returned.
 * 0 is returned on error or if ist CHAR is not on eaten list.
 */
INT StrEatChar (PSZ *ppsz, PSZ pszEdibles, PSZ pszWhitespace)
	{
	PSZ p;

	if (!pszWhitespace)
		pszWhitespace = (PSZ)" \t";

	if (!StrSkipBy (*ppsz, pszWhitespace))
		return 0;

	if (!(p = strchr (pszEdibles, **ppsz)))
		return 0;
	(*ppsz)++;
	return *p;
	}


/*
 * Removes chars from anywhere in string
 *
 */
PSZ StrExtract (PSZ pszStr, PSZ pszExtractList)
	{
	PSZ pszPtrSrc, pszPtrDest;

	if (!pszStr || !pszExtractList)
		return pszStr;

	pszPtrSrc = pszPtrDest = pszStr;
	while (*pszPtrSrc != '\0')
		{
		if (strchr (pszExtractList, *pszPtrSrc) == NULL)
			*pszPtrDest++ = *pszPtrSrc;
		pszPtrSrc++;
		}
	*pszPtrDest = '\0';
	return pszStr;
	}



/*************************************************************************/


/*
 * CSVizes a string
 * if pszDest is NULL, dest is malloced
 */
PSZ StrMakeCSVField (PSZ pszDest, PSZ pszSrc)
	{
	CHAR szWord [256];
	BOOL bDup;
	PSZ  psz;

	psz  = (pszDest ? pszDest : szWord);
	bDup = (pszDest ? FALSE   : TRUE);
	*psz = '\0';

	if (!pszSrc)
		return pszDest;

	if (strchr (pszSrc, '"'))
		{
		*psz++ = '"';
		while (*pszSrc)
			{
			*psz++ = *pszSrc;
			if (*pszSrc++ == '"')
				*psz++ = '"';
			}
		*psz++ = '"';
		*psz   = '\0';
		}
	else if (strchr (pszSrc, ','))
		{
		*psz++ = '"';
		strcpy (psz, pszSrc);
		psz += strlen(pszSrc);
		*psz++ = '"';
		*psz = '\0';
		}
	else
		strcpy (psz, pszSrc);
	return (bDup ? strdup (szWord) : pszDest);
	}


/*
 *    un CSVizes a string
 * Assumes a single CSV field
 * removes extra quotes
 * clips data after closing quote if quoted
 * if pszDest is NULL, dest is malloced
 */
PSZ StrUnCSV (PSZ pszDest, PSZ pszSrc)
	{
	CHAR szWord [256];
	BOOL bDup;
	PSZ  psz;
	PSZ  p;

	psz  = (pszDest ? pszDest : szWord);
	bDup = (pszDest ? FALSE   : TRUE);
	*psz = '\0';
	p    = pszSrc;

	if (!p)
		return pszDest;

	while (strchr (" \t", *p))
		p++;

	if (*p != '"')
		return (bDup ? strdup (pszSrc) : strcpy (psz, pszSrc));

	p++;
	while (TRUE)
		{
		if ((*p == '"') &&  (*(p+1) != '"'))
			break;
		if (*p == '"')
			p++;
		*psz++ = *p++;;
		}
	*psz = '\0';
	return (bDup ? strdup (szWord) : pszDest);
	}



/*
 * Adds quotes to the beginning and end of the string in place
 */
PSZ StrAddQuotes (PSZ psz)
	{
	INT iLen;

	if (!psz)
		return psz;

	iLen = strlen (psz);
	memmove (psz+1, psz, iLen);
	psz[0] = '"';
	psz[iLen+1] = '"';
	psz[iLen+2] = '\0';
	return psz;
	}


///* 
// * Makes a PPSZ from a CSV line
// * This PPSZ is malloced and returned
// * piCols returns the number of cols in the PPSZ
// * if bTrim is TRUE, leading and trailing whitespace are removed
// *   AFTER converting the column from a CSV field
// * If pszCSVLine is NULL, this fn returns NULL
// * If pszCSVLine is empty, this fn returns a single empty col.
// *
// */
//
//PSZ *StrMakePPSZ (PSZ   pszStr, 
//						PSZ   pszDelim, 
//						BOOL  bCSV, 
//						BOOL  bTrim, 
//						PINT  piCols)
//	{
//	PSZ   *ppsz = NULL;
//	CHAR  szWord [2048];
//	INT  i, iDelim, iPrevDelim;
//
//	if (piCols)
//		*piCols = 0;
//	if (!pszStr)
//		return NULL;
//
//	i=0;
//	while (TRUE)
//		{
//		iDelim = StrGetWord (&pszStr, szWord, NULL, pszDelim, bCSV, TRUE);
//		if (iDelim == -1 && iPrevDelim != ',')
//			break;
//			
//		ppsz = (PSZ*) realloc (ppsz, (i+2) * sizeof (PSZ));
//		if (bTrim)
//			{
//			PSZ pszTrimList = (strchr(pszDelim, '\t') ? " " : " \t");
//			StrStrip (StrClip (szWord, pszTrimList), pszTrimList);
//			}
//		ppsz[i]   = strdup (szWord);
//		ppsz[i+1] = NULL;
//		i++;
//
//		if (iDelim == -1)
//			break;
//		iPrevDelim = iDelim;
//		}
//	if (piCols)
//		*piCols = i;
//	return ppsz;
//	}

#define PPSZ_GROW_SIZE 8

PSZ *StrMakePPSZ (PSZ   pszStr, 
						PSZ   pszDelim, 
						BOOL  bCSV, 
						BOOL  bTrim, 
						PINT  piCols)
	{
	PSZ* ppsz = NULL;
	CHAR szWord [2048];
	int  i, iDelim, iPrevDelim, iSize;

	if (piCols)
		*piCols = 0;
	if (!pszStr)
		return NULL;

	i=0;
	while (TRUE)
		{
		iDelim = StrGetWord (&pszStr, szWord, NULL, pszDelim, bCSV, TRUE);
		if (iDelim == -1 && iPrevDelim != ',')
			break;

		//ppsz = (PSZ*) realloc (ppsz, (i+2) * sizeof (PSZ));
		if (!ppsz)
			{
			iSize = PPSZ_GROW_SIZE;
			ppsz = (PSZ*)calloc (iSize, sizeof (PSZ));
			}
		else if (iSize <= i+1)
			{
			iSize += PPSZ_GROW_SIZE;
			ppsz = (PSZ*)realloc (ppsz, iSize * sizeof (PSZ));
			}
			
		if (bTrim)
			{
			PSZ pszTrimList = (strchr(pszDelim, '\t') ? " " : " \t");
			StrStrip (StrClip (szWord, pszTrimList), pszTrimList);
			}
		ppsz[i]   = strdup (szWord);
		ppsz[i+1] = NULL;
		i++;

		if (iDelim == -1)
			break;
		iPrevDelim = iDelim;
		}
	if (piCols)
		*piCols = i;
	return ppsz;
	}




/*
 * gets a CSV field (0 based)
 */
BOOL StrGetCSVField (PSZ pszWord, PSZ pszLine, INT iIndex)
	{
	if (pszWord) *pszWord = '\0';

	for (; iIndex; iIndex--)
		StrGetWord (&pszLine, NULL, NULL, ",\n", TRUE, TRUE);

	return (StrGetWord (&pszLine, pszWord, NULL, ",\n", TRUE, TRUE) != -1);
	}


/*
 * 
 * This fn converts a line of text to another line of text
 * converting special sequences.
 *
 * fOptions defines which options to enable. Combine with |
 * the curent options are:
 *       STR_NEWLINES ... append \n at EOL unless str ends with \ 
 *       STR_TILDAESC ... cvt ~ to esc CHAR
 * 
 * conversions always recognized:
 *  \n ........ convert to new line
 *  \f ........ convert to form feed
 *  \t ........ convert to tab
 *  \b ........ convert to backspace
 *  \r ........ convert to ?
 *  \xHH ...... convert to CHAR with HH hex val
 *  \CHAR ..... convert to CHAR
 *  \ ......... if at end of line, means dont append \n (see STR_NEWLINES)
 * 
 */
PSZ StrCookLine (PSZ pszDest, PSZ pszSrc, INT iOptions)
	{
	INT  c, uS, uD;
	BOOL bNewLines, bTildaEsc;

	bNewLines = !!(iOptions & STR_NEWLINES);
	bTildaEsc = !!(iOptions & STR_TILDAESC);

	for (uS=uD=0; c = pszSrc[uS]; uS++)
		{
		if (c == '\\')
			{
			switch (c = pszSrc[++uS])
				{
				case 'n' : c = '\n';  break;
				case 'f' : c = '\f';  break;
				case 't' : c = '\t';  break;
				case 'b' : c = '\b';  break;
				case 'r' : c = '\r';  break;
				case 'x' : c = HexValPsz (pszSrc+uS+1); uS+=2; break;
				case '\0': pszDest[uD] = '\0'; return pszDest;
				}
			}
		else if (c == '~' && bTildaEsc)
			{
			c = 0x1b;
			}
		pszDest[uD++] = (CHAR) c;
		}
	if (bNewLines)
		pszDest[uD++] = '\n';
	pszDest[uD]   = '\0';
	return pszDest;
	}




/*
 * case insensitive version of strstr
 *
 */
PSZ Stristr (PSZ psz1, PSZ psz2)
	{
	INT i, iLen1, iLen2;

	if (!psz1 || !psz2 || !*psz1 || !*psz2)
		return NULL;

	iLen1 = strlen (psz1);
	iLen2 = strlen (psz2);

	if (iLen2 > iLen1)
		return NULL;

	for (i=0; i+iLen2 <= iLen1; i++)
		if (!stricmp (psz1+i, psz2))
			return psz1+i;
	return NULL;
	}

}

