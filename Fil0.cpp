/*
 * Fil0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides basic file manipulation utilities
 *
 *
 ****************************************************************************
 *
 * INT f_getc (FILE *fp);
 * INT f_ungetc (INT c, FILE *fp);
 * INT Filfputstr (PSZ psz, FILE *fp);
 *
 * INT   FilReadLine (*fpIn, pszStr, pszComments, iMaxLen)
 * INT   FilReadTo   (*fpIn, pszStr, pszDelim, iMaxLen, bEatDelim)
 * INT   FilReadWhile(*fpIn, pszStr, pszChars, iMaxLen, bEatDelim)
 * INT   FilReadWord (*fpIn, pszWord, pszSkip, pszDelim, iMaxLen, bEatDelim)
 * void  FilWriteShort (*fpOut, u)
 * void  FilWriteLong  (*fpOut, ul)
 * void  FilWriteStr   (*fpOut, psz)
 * ULONG FilGetLine ()
 * ULONG FilSetLine (ulGlobalLine)
 *
 ****************************************************************************
 *
 */

#include <string.h>
#include <stdio.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnufile.h"

extern "C"
{


static ULONG GLOBALLINE = 1L;


/*
 * note that this is used by FilReadStr if param is NULL
 * therefore this fn is not always re-entrant
 */
static CHAR szWorkingFil0Buff [2048];


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


INT f_getc (FILE *fp)
	{
	INT c;

	c = fgetc (fp);
	GLOBALLINE += (c == '\n');
	return c;
	}


INT f_ungetc (INT c, FILE *fp)
	{
	GLOBALLINE -= (c == '\n');
	return ungetc (c, fp);
	}



/* this is like fputs except
 * a null is ok
 * a \0 is written at the end
 */
INT Filfputstr (PSZ psz, FILE *fp)
	{
	INT iRet;

	iRet = (psz ? fputs (psz, fp) : 0);
	fputc ('\0', fp);   
	return iRet;
	}


/* -> Returns count of chars read into pszStr
 * -> Returns -1 if fpIn comes in with EOF
 * -> Reads until it gets up to one of pszDelim or EOF
 * -> if iMaxLen to small, the remaining chars are discarded
 * -> similar to readline.
 * -> of pszStr comes in NULL all chars to delim are discarded
 * -> If bEatDelim != 0 the delimiting CHAR is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszStr may be NULL, in which case this function performs
 *     as a skipto function
 */
INT FilReadTo (FILE *fpIn,
					PSZ  pszStr,
					PSZ  pszDelim,
					INT  iMaxLen,
					BOOL bEatDelim)
	{
	INT c, i = 0;

	if (pszStr) *pszStr = '\0';
	while (!strchr (pszDelim, c = f_getc (fpIn)) && c != EOF)
		if ((i < iMaxLen) && pszStr) 
			pszStr[i++] = (CHAR)c;

	if (pszStr && (i -= (i && i == iMaxLen)))
		pszStr [i] = '\0';

	if (!bEatDelim) f_ungetc (c, fpIn);
	return i;
	}



/*
 * -> Reads a line from fpIn.
 * -> Returns -1 if EOF before string could be read.
 * -> Returns the length of the line read in.
 * -> pszComments is a string of characters that are considered
 *     to be comment delimeters if at the beginning of the line. If the
 *     line has a comment CHAR as its first non whitespace CHAR. That line
 *     is skipped and the next is read. if \n is included in pszComments
 *     then blank lines are considered comments too.
 * -> if the line is longer than iMaxLen, the rest of the line is thrown
 *     away.
 * -> newline itself is read and discarded
 */
INT FilReadLine (FILE *fpIn,
					  PSZ  pszStr,
					  PSZ  pszComments,
					  INT  iMaxLen)
	{
	INT i, c;

	if (pszStr)
		*pszStr = '\0';
	while (1)
		{
		i = 0;
		while ((c = f_getc (fpIn)) == (INT)' ' || c == (INT)'\t')
			{
			if (i < iMaxLen && pszStr)
				pszStr[i++] = (CHAR)c;
			}

		if (c == EOF)
			return -1;

		if (!pszComments || !strchr (pszComments, c))
			break;

		while ((c = f_getc (fpIn)) != '\n' && c != EOF)
			;
		}
	f_ungetc (c, fpIn);

	/*---This could be speeded up if this were done manually ---*/
	return i + FilReadTo (fpIn, pszStr + i, "\n", iMaxLen - i, 1);

	while ((c = f_getc (fpIn)) != '\n' && c != EOF)
			if (i < iMaxLen && pszStr) pszStr[i++] = (CHAR)c;

	if ((i -= (i && i == iMaxLen)) && pszStr)
		pszStr [i] = '\0';
	return i;

	}


/* -> Returns count of chars read into pszStr
 * -> Returns -1 if fpIn comes in with EOF
 * -> Reads while CHAR is one of pszChars or EOF
 * -> if iMaxLen to small, the remaining chars are discarded
 * -> similar to readline.
 * -> If bEatDelim != 0 the delimiting CHAR is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszStr may be NULL, in which case this function performs
 *     as a skipwhile function
 */
INT FilReadWhile (FILE *fpIn,
						PSZ  pszStr,
						PSZ  pszChars,
						INT  iMaxLen,
						INT  bEatDelim)
	{
	INT c, j, i = 0;

	if (pszStr)
		*pszStr = '\0';


	for (i=j=0; strchr (pszChars, c = f_getc (fpIn)) && c != EOF; j++)
		if (i < iMaxLen && pszStr)
			pszStr[i++] = (CHAR)c;

	if (c == EOF && !j)
		return -1;

	if (!bEatDelim)
		f_ungetc (c, fpIn);
	if (i -= (i && i == iMaxLen))
		pszStr [i] = '\0';
	return i;
	}




/* 0> returns -1 if it reads EOF before it gets a word.
 * 1> Skips pszSkip chars (normally spaces and tabs and maybe newlines)
 * 2> Reads word up to iMaxLen chars
 * 3> Skips remaining chars if iMaxLen Reached   
 * 4> If bEatDelim != 0 the delimiting CHAR is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszWord may be null
 */
INT FilReadWord (FILE *fpIn,
					  PSZ  pszWord,
					  PSZ  pszSkip,
					  PSZ  pszDelim,
					  INT  iMaxLen,
					  INT  bEatDelim)
	{
	if (FilReadWhile (fpIn, NULL, pszSkip, 32767, 0) == -1)
		return -1;
	return FilReadTo (fpIn, pszWord, pszDelim, iMaxLen, bEatDelim);
	}


PSZ FilReadStr (FILE *fp, PSZ psz)
	{
	PSZ p;
	INT c;

	p = (psz ? psz : szWorkingFil0Buff);

	while ((c = (CHAR)fgetc (fp)) && c != EOF)
		*p++ = (CHAR) c;
	*p = '\0';

	if (!psz)
		psz = strdup (szWorkingFil0Buff);

	return psz;
	}





ULONG FilGetLine (void)
	{
	return GLOBALLINE;
	}


ULONG FilSetLine (ULONG ulGlobalLine)
	{
	GLOBALLINE = ulGlobalLine;
	return GLOBALLINE;
	}

}


