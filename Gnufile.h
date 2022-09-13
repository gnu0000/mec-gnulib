/*
 * GnuFile.h
 *
 * (C) 1992-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file provides file handling functions
 *
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * INT   FilReadLine (*fpIn, pszStr, pszComments, iMaxLen);
 * INT   FilReadTo   (*fpIn, pszStr, pszDelim, iMaxLen, bEatDelim);
 * INT   FilReadWhile(*fpIn, pszStr, pszChars, iMaxLen, bEatDelim);
 * INT   FilReadWord (*fpIn, pszWord, pszSkip, pszDelim, iMaxLen, bEatDelim);
 *
 * BYTE   FilReadByte (*fp);
 * SHORT  FilReadShort (*fp);
 * USHORT FilReadUShort (*fp);
 * ULONG  FilReadULong  (*fp);
 * PSZ    FilReadStr   (*fp, psz);
 * BOOL   FilWriteByte (*fp, c);
 * BOOL   FilWriteShort (*fpOut, i);
 * BOOL   FilWriteUShort (*fpOut, u);
 * BOOL   FilWriteLong  (*fpOut,  l);
 * BOOL   FilWriteULong  (*fpOut, ul);
 * BOOL   FilWriteStr   (*fpOut, psz);
 *
 * INT   FilPushPos (*fp);
 * INT   FilPopPos  (*fp, bSet);
 * ULONG FilPeekPos (*fp);
 * BOOL  FilSwapPos (*fp, bSet);
 *
 * ULONG FilGetLine (void);
 * ULONG FilSetLine (ulGlobalLine);
 *       
 * INT   FilPeek (*fp);
 * INT   f_getc (*fp);
 * INT   f_ungetc (c, *fp);
 * INT   Filfputstr (psz, *fp);
 *
 * PSZ   FilGet4DosDesc (pszFile, pszDesc);
 * BOOL  FilPut4DosDesc (pszFile, pszDesc);
 * INT   FilReadCSVField (*fp, pszStr, uMaxLen, bLastItem);
 *
 ****************************************************************************
 */


#if !defined (GNUFILE_INCLUDED)
#define GNUFILE_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

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
EXP INT FilReadLine (FILE *fpIn,
                     PSZ  pszStr,
                     PSZ  pszComments,
                     INT  iMaxLen);


/*
 * -> Returns count of chars read into pszStr
 * -> Returns -1 if fpIn comes in with EOF
 * -> Reads until it gets up to one of pszDelim or EOF
 * -> if iMaxLen to small, the remaining chars are discarded
 * -> similar to readline.
 * -> If bEatDelim != 0 the delimiting CHAR is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszStr may be NULL, in which case this function performs
 *     as a skipto function
 */
EXP INT FilReadTo (FILE *fpIn,
                   PSZ  pszStr,
                   PSZ  pszDelim,
                   INT  iMaxLen,
                   BOOL bEatDelim);


/*
 * -> Returns count of chars read into pszStr
 * -> Returns -1 if fpIn comes in with EOF
 * -> Reads while CHAR is one of pszChars or EOF
 * -> if iMaxLen to small, the remaining chars are discarded
 * -> similar to readline.
 * -> If bEatDelim != 0 the delimiting CHAR is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszStr may be NULL, in which case this function performs
 *     as a skipwhile function
 */
EXP INT FilReadWhile (FILE *fpIn,
                      PSZ  pszStr,
                      PSZ  pszChars,
                      INT  iMaxLen,
                      INT  bEatDelim);
   
   
/*
 * 0> returns -1 if it reads EOF before it gets a word.
 * 1> Skips pszSkip chars (normally spaces and tabs and maybe newlines)
 * 2> Reads word up to iMaxLen chars
 * 3> Skips remaining chars if iMaxLen Reached
 * 4> If bEatDelim != 0 the delimiting CHAR is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszWord may be null
 */
EXP INT FilReadWord (FILE *fpIn,
                     PSZ  pszWord,
                     PSZ  pszSkip,
                     PSZ  pszDelim,
                     INT  iMaxLen,
                     INT  bEatDelim);


/*
 * -> pushes the file position for FILE
 * -> a separate stack is kept for each FILE
 *     so pushing different FILEs wil not cause a conflict
 * -> returns the depth of the stack for FILE
 */
EXP INT FilPushPos (FILE *fp);


/*
 * -> pops the file position for FILE
 * -> id uSet != 0 the FILE position is set to the last push position
 *     otherwise the position is popped and discarded.
 * -> returns the depth of the stack for FILE or -1 if there is
 *     no stack for FILE
 */
EXP INT FilPopPos (FILE *fp, BOOL bSet);


/* -> peeks file position for FILE
 * -> returns 0 on error
 */
EXP ULONG FilPeekPos (FILE *fp);


/*
 * Swaps the current file pos with the position on the stack
 * if bSet = TRUE, the file ptr is moved to the old stack position
 */
EXP BOOL FilSwapPos (FILE *fp, BOOL bSet);


/*
 * read 4dos description of file
 * Uses fully qualified file names
 *
 */
EXP PSZ FilGet4DosDesc (PSZ pszFile, PSZ pszDesc);


/*
 * Writes 4dos description
 * Uses fully qualified file names
 * returns TRUE if successful
 */
EXP BOOL FilPut4DosDesc (PSZ pszFile, PSZ pszDesc);


/*
 * This looks at the next CHAR without actually reading it
 */
EXP INT FilPeek (FILE *fp);


/*
 * This function reads in a csv fiels from the stream
 * if fp comes in EOF 1 is returned.
 * supporting quotes are stripped
 * LastItem determines if fiels should be terminated by a
 * comma or an \n | \EOF.  If the delimeter is of the incorrect
 * type, 2 is returned.
 * pszStr may be null, in which case the field is discarded.
 *
 * return values:
 *   0 - ok
 *   1 - eof on fp
 *   2 - incorrect terminator
 *
 */
EXP INT FilReadCSVField (FILE *fp, 
                         PSZ  pszStr, 
                         INT  iMaxLen, 
                         BOOL bLastItem);


/*
 * This module keeps track of \n's in a file when reading via the f_getc fn.
 * The user can get and set the count of \n's with these
 * 2 functions
 *
 */
EXP ULONG FilSetLine (ULONG ulGlobalLine);
EXP ULONG FilGetLine (void);


/*
 * Just like thier c lib counterparts except that they update
 * the GLOBALLINE variable on a \n
 */
EXP INT f_getc (FILE *fp);
EXP INT f_ungetc (INT c, FILE *fp);


/*
 * This is like fputs except a null is ok
 * A \0 is written at the end
 */
EXP INT Filfputstr (PSZ psz, FILE *fp);



/*
 * simple read/write functions
 *
 */
EXP BOOL FilWriteByte (FILE *fp, BYTE c);
EXP BOOL FilWriteShort (FILE *fp, SHORT i);
EXP BOOL FilWriteUShort (FILE *fp, USHORT u);
EXP BOOL FilWriteLong (FILE *fp, LONG l);
EXP BOOL FilWriteULong (FILE *fp, ULONG ul);
EXP BOOL FilWriteStr (FILE *fp, PSZ p);

EXP BYTE   FilReadByte (FILE *fp);
EXP SHORT  FilReadShort (FILE *fp);
EXP USHORT FilReadUShort (FILE *fp);
EXP LONG   FilReadLong (FILE *fp);
EXP ULONG  FilReadULong (FILE *fp);
EXP PSZ    FilReadStr (FILE *fp, PSZ psz);


#ifdef __cplusplus
}
#endif

#endif  // GNUFILE_INCLUDED

