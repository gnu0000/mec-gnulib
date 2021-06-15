/*
 * GnuReg.h
 *
 * (C) 1996 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file provides regular expression handling functions
 *
 * Note: The difference between the "Match" and the "Find" functions
 * is that when "Match"ing, the entire source string must match the
 * regular expression and when "Find"ing the target may be anywhere 
 * in the source string
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * BOOL RegMatch (pszSrc, pszRegex)
 * BOOL RegMatchReplace (pszDest, pszSrc, pszRegex, pszResult)
 * BOOL RegFind (pszSrc, pszRegex, piStart, piEnd)
 * BOOL RegFindReplace (pszDest, pszSrc, pszRegex, pszResult)
 *
 * void RegCaseSensitive (bCase)
 * PSZ  RegIsError ()
 *
 * -------------------------------------------------
 *
 * PRX  _regParseRegex (pszRegex)
 * PRX  _regFreePRX (prx)
 *
 * BOOL _regMatch (pszSrc, prx)
 * BOOL _regMatchReplace (pszDest, pszSrc, prx, pszResult)
 * BOOL _regFind (pszSrc, prx, piStart, piEnd)
 * BOOL _regFindReplace (pszDest, pszSrc, prx, pszResult)
 * BOOL _regFindReplace2 (pszDest, pszSrc, prx, pszResult, piStart, piEnd)
 *
 ****************************************************************************
 */

#if !defined (GNUREG_INCLUDED)
#define GNUREG_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif


extern "C"
{

/*
 * structure which holds the post parsed regular expression
 */
typedef struct _rx
   {
   CHAR       cType;
   INT        iStart;
   INT        iIndex;
   CHAR       cChar;
   PSZ        pszStr;
   BOOL       bSuper;
   struct _rx *left;
   struct _rx *right;
   struct _rx *next;
   } RX;
typedef RX *PRX;


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * set regular expression searches to be case sensitive or case insensitive
 */
EXP void RegCaseSensitive (BOOL bCase);

/*
 * Call this fn after calling one of the Reg search/match fns or after
 * calling _regParseRegex to see if there was a parse error.
 *
 * returns NULL if there was no error
 */
EXP PSZ RegIsError (void);

/*
 * Match a string.  A match means the entire source string 
 * must match the Regular expression
 *
 * pszSrc .... The source string to operate on
 * pszRegex .. The regular expression to search with
 *
 * Returns TRUE if the replacement was successful
 */
EXP BOOL RegMatch (PSZ pszSrc, PSZ pszRegex);

/*
 * Match/Replace a string.  A match means the entire source string 
 * must match the Regular expression
 *
 * pszDest ... The result string
 * pszSrc .... The source string to operate on
 * pszRegex .. The regular expression to search with
 * pszResult . The replacement text
 *
 * Returns TRUE if the replacement was successful
 */
EXP BOOL RegMatchReplace (PSZ pszDest, PSZ pszSrc, PSZ pszRegex, PSZ pszResult);

/*
 * Find a string within a string
 *
 * pszSrc .... The source string to operate on
 * pszRegex .. The regular expression to search with
 * piStart ... The starting match position
 * piEnd ..... The ending match position +1 !!!!!!!!!!!
 *
 * Returns TRUE if the find was successful
 */
EXP BOOL RegFind (PSZ pszSrc, PSZ pszRegex, PINT piStart, PINT piEnd);

/*
 * Find/Replace a string within a string
 *
 * pszDest ... The result string
 * pszSrc .... The source string to operate on
 * pszRegex .. The regular expression to search with
 * pszResult . The replacement text
 *
 * Returns TRUE if the replacement was successful
 */
EXP BOOL RegFindReplace (PSZ pszDest, PSZ pszSrc, PSZ pszRegex, PSZ pszResult);

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * Parses a regular expression into an internal form
 */
EXP PRX _regParseRegex (PSZ pszRegex);

/*
 * Frees a parsed regular expression
 */
EXP PRX _regFreePRX (PRX prxHead);

/*
 * like RegMatch, but works on an already parsed expression
 */
EXP BOOL _regMatch (PSZ pszSrc, PRX prx);

/*
 * like RegMatchReplace, but works on an already parsed expression
 */
EXP BOOL _regMatchReplace (PSZ pszDest, PSZ pszSrc, PRX prx, PSZ pszResult);

/*
 * like RegFind, but works on an already parsed expression
 */
EXP BOOL _regFind (PSZ pszSrc, PRX prx, PINT piStart, PINT piEnd);

/*
 * like RegFindReplace, but works on an already parsed expression
 */
EXP BOOL _regFindReplace (PSZ pszDest, PSZ pszSrc, PRX prx, PSZ pszResult);

/*
 * like _regFindReplace, but also returns replace poritions
 */
EXP BOOL _regFindReplace2 (PSZ pszDest, PSZ pszSrc, PRX prx, PSZ pszResult,
                           PINT piStart, PINT piEnd);

} // extern C

#endif  // GNUREG_INCLUDED

