/*
 * GnuStr.h
 *
 * (C) 1992-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file provides string handling functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * PSZ  StrClip (pszStr, pszClipList)      // Trailing
 * PSZ  StrStrip (pszStr, pszStripList)    // Leading
 * PSZ  StrExtract (pszStr, pszExtractList)
 * UINT StrGetWord (*ppsz, pszWord, pszSkip, pszDelim, bCSV, bEatDelim)
 * UINT StrGetIdent (*ppsz, pszWord, bAllowNum, bEatDelim)
 * UINT StrEatWord (*ppszStr, pszTarget)
 * UINT StrEatChar (*ppsz, pszEdibles, pszWhitespace)
 * PSZ  StrSkipBy (psz, pszSkipList)
 * PSZ  StrSkipTo (psz, pszToList)
 * BOOL StrBlankLine (pszLine)
 * PSZ  strnchr (psz, c, iLen)
 * PSZ  StrReplace (ppszDest, pszSrc)
 * PSZ StrBuild (PSZ pszDest, int iParams, PSZ pszFormat, ...)
 *
 * BOOL StrMatches (pszName, pszWildCard, bCase)
 * PSZ  StrMyCat (pszDest, pszSrc, ...)
 *
 * PSZ  StrPadLeft0 (sz, iSize)
 *
 * PSZ *StrMakePPSZ (pszStr, pszDelim, bCSV, bTrim, piCols)
 * UINT StrPPSZCut (ppsz, pszLine)
 * PSZ  StrMakeCSVField (pszDest, pszSrc)
 * PSZ  StrUnCSV (pszDest, pszSrc)
 * BOOL StrGetCSVField (szFrom, psz, iIndex)
 * PSZ  StrAddQuotes (psz)
 * PSZ  StrCookLine (pszDest, pszSrc, fOptions)
 * PSZ  Stristr (psz1, psz2)
 * BOOL StrTrue (PSZ psz)
 *
 ****************************************************************************
 */

#if !defined (GNUSTR_INCLUDED)
#define GNUSTR_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif


//extern "C"
//{

/*
 * these defines are used with StrCookLine
 *
 */
#define STR_NEWLINES 1
#define STR_TILDAESC 2


/*
 * Removes trailing chars from str
 */
EXP PSZ StrClip (PSZ pszStr, PSZ pszClipList);


/*
 * Removes leading chars from str
 * Moves memory, not ptr;
 */
EXP PSZ StrStrip (PSZ pszStr, PSZ pszStripList);


/*
 * Removes chars from anywhere in string
 */
EXP PSZ StrExtract (PSZ pszStr, PSZ pszExtractList);


/*
 * Skips leading characters in pszSkipList (spaces and tabs if NULL)
 * Eats a word/field from the front of a string and returns that word and 
 *  the rest of the string.
 * if pszWord is NULL, the word is discarded.
 * if bCSV is TRUE, the word is un-CSVified.
 * if bCSV is TRUE, wou will probable want ",\n" for pszDelimeters
 * if bEatDelim = TRUE, the returned string starts one CHAR past
 *  the delimeter.
 * The return value is the delimeter CHAR, or 0xFFFF on error
 */
EXP INT StrGetWord (PSZ  *ppsz,
                    PSZ  pszWord,
                    PSZ  pszSkipList,
                    PSZ  pszDelimiters,
                    BOOL bCSV,
                    BOOL bEatDelim);


/*
 * This fn extracts an identifier from the beginning of the string
 * This fn will skip any leading whitespace
 * This fn will then read the identifier
 * If !bAllowNum, the identifier must start with a letter or _
 * The delimiter is returned, if bEatDelim=TRUE then the delim
 * is skipped past.
 *
 */
EXP INT StrGetIdent (PSZ *ppsz,
                     PSZ  pszWord,
                     BOOL bAllowNum,
                     BOOL bEatDelim);
   

/*
 * 0 = match
 * 1 = no match
 * 2 = end of string
 */
EXP INT StrEatWord (PSZ *ppszStr, PSZ pszTarget);



/*
 * This fn is used to make sure a certain character is at the head
 * of a string.  This CHAR (and any leading whitespace) is removed
 * from the front of the string, and the CHAR eaten is returned.
 * 0 is returned on error or if ist CHAR is not on eaten list.
 */
EXP INT StrEatChar (PSZ *ppsz, PSZ pszEdibles, PSZ pszWhitespace);




/*
 * returns ptr into psz
 * skips to first non pszSkipList CHAR
 * returns NULL if end of string reached
 */
EXP PSZ StrSkipBy (PSZ psz, PSZ pszSkipList);


/*
 * returns ptr into psz
 * skips to first pszSkipList CHAR
 * returns NULL if end of string reached
 */
EXP PSZ StrSkipTo (PSZ psz, PSZ pszToList);


/*
 * returns true if string contains only white space
 */
EXP BOOL StrBlankLine (PSZ pszLine);


/*
 * This is basically a strchr
 * with a string length
 *
 */
EXP PSZ strnchr (PSZ psz, int c, INT iLen);


/*
 * pszWildCard is match spec, and may contain * and ?
 * wildcards may be separated with ;
 * pszName is string to test
 * if bCase=TRUE comparison is case sensitive
 * returns TRUE if match is successful
 */
EXP BOOL StrMatches (PSZ pszName, PSZ pszWildCard, BOOL bCase);


/*
 * shifts string in sz right
 * and pads on left with 0's
 */
EXP PSZ StrPadLeft0 (PSZ sz, INT iSize);


/* 
 * Makes a PPSZ from a CSV line
 * This PPSZ is malloced and returned
 * piCols returns the number of cols in the PPSZ
 * if bTrim is TRUE, leading and trailing whitespace are removed
 *   AFTER converting the column from a CSV field
 * If pszCSVLine is NULL, this fn returns NULL
 * If pszCSVLine is empty, this fn returns a single empty col.
 *
 */
EXP PSZ *StrMakePPSZ (PSZ pszStr, PSZ pszDelim, BOOL bCSV, BOOL bTrim, PINT piCols);


/*
 * CSVizes a string
 */
EXP PSZ StrMakeCSVField (PSZ pszDest, PSZ pszSrc);


/*
 * un CSVizes a string
 */
EXP PSZ StrUnCSV (PSZ pszDest, PSZ pszSrc);


/*
 * Adds quotes to the beginning and end of the string in place
 */
EXP PSZ StrAddQuotes (PSZ psz);

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
EXP PSZ StrCookLine (PSZ pszDest, PSZ pszSrc, INT iOptions);


/*
 * case insensitive version of strstr
 *
 */
EXP PSZ Stristr (PSZ psz1, PSZ psz2);

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/

/*
 * gets a CSV field (0 based)
 */
EXP BOOL StrGetCSVField (PSZ pszWord, PSZ pszLine, INT iIndex);

/*
 * Returns TRUE if the string is TRUE or Yes or 1 etc...
 *
 */
EXP BOOL StrTrue (PSZ psz);


/*
 * This fn concatenates n strings together
 * and returns that string
 */
EXP PSZ StrMyCat (PSZ pszDest, PSZ pszSrc, ...);


/*
 * frees *ppszDest if not null
 * mallocs copy of pszSrc if not null
 * and assigns to *ppszDest
 */
EXP PSZ StrReplace (PPSZ ppszDest, PSZ pszSrc);


/*
 * Build a string:
 * pszDest   = string dest buffer
 * iParams   = # of optional parameters passed to fn
 * pszformat = format string containing %0 %1 %2, etc..
 *
 * in the pszformat string, %0 is replaced with the ist optional param,
 * %1 with the next, etc....
 *
 *  Example:
 *  strBuild (szBuff, 4, "1=%1, 2=%2, 3=%3\n" , "AAA", "BBB", "CCC", "DDD");
 *    szBuff will be: "1=BBB, 2=CCC, 3=DDD\n"
 */
EXP PSZ StrBuild (PSZ pszDest, int iParams, PSZ pszFormat, ...);


//} // extern C

#endif  // GNUSTR_INCLUDED
