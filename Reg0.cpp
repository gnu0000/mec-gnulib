/*
 * Reg0.c
 *
 * Craig Fitzgerald  1996
 *
 * Part of the GnuLib library
 *
 *
 * This file provides regular expression handling functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnureg.h"

extern "C"
{

PSZ  pszTOKENSTR;           // holds regex string while tokenizing
INT  iTOKENIDX;             // holds index into pszTOKENSTR while tokenizing
									 //
PSZ  pszGLOBALERR;          // global error string set while parsing
									 //
BOOL bMATCHMODE;            // vs find mode set internally
BOOL bCASESENSITIVE = TRUE; // vs non sensitive set via RegCaseSensitive


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

PVOID AddError (PSZ psz)
	{
	if (!pszGLOBALERR)
		pszGLOBALERR = psz;
	return NULL;
	}


/***************************************************************************/
/*                                                                         */
/* Tokenizer fns                                                           */
/*                                                                         */
/***************************************************************************/


static void InitTokens (PSZ pszRegex)
	{
	if (!pszRegex || !*pszRegex)
		AddError ("No Pattern Specified");
	iTOKENIDX   = 0;
	pszTOKENSTR  = pszRegex;
	pszGLOBALERR = NULL;
	}


/*
 * returns one of:
 *   <   >
 *   {   }
 *   [   ]
 *   +   -
 *   @   *
 *   ?   ~
 *   |   C  (any other char)
 *   0 end of stream
 */
static UCHAR GetToken (BOOL bEat)
	{
	UCHAR cTok;

	if (!(cTok = pszTOKENSTR [iTOKENIDX]))
		return 0;
	if (!strchr ("<>{}[]+@*?~|-", cTok))
		cTok = 'C';
	if (bEat)
		iTOKENIDX++;
	if (cTok == '\\')
		{
		if (!(cTok = pszTOKENSTR [iTOKENIDX]))
			return 0;
		if (bEat)
			iTOKENIDX++;
		}
	return cTok;
	}


/*
 * if current token is a 'C' this fn
 * will return the actual char
 */
static UCHAR GetTokenVal (BOOL bEat)
	{
	UCHAR cTok;

	if (!(cTok = pszTOKENSTR [iTOKENIDX]))
		return 0;
	if (bEat)
		iTOKENIDX++;
	if (cTok == '\\')
		{
		if (!(cTok = pszTOKENSTR [iTOKENIDX]))
			return 0;
		if (bEat)
			iTOKENIDX++;
		}
	return cTok;
	}


/*
 *
 */
static void EatToken (UCHAR cTok, PSZ pszErr)
	{
	if (cTok != GetToken (1))
		AddError (pszErr);
	}


/*
 * builds a string of consecutive literals
 * expands '-'s to be a range indicator
 *
 * Currently this is only used for ranges because
 * "fred|joe" is "fre{d|j}oe" not "{fred}|{joe}" so we
 * cannot group by words
 */
static PSZ GetTokenString (BOOL bRanges)
	{
	UCHAR cTok, cLast, cEnd, szList[512];
	INT   iSize = 0;

	cTok = GetToken (0);
	if (bRanges && cTok != 'C')
		return (PSZ) AddError ("Invalid Range");
	szList[iSize++] = cLast = GetTokenVal (1);

	while (TRUE)
		{
		cTok = GetToken (0);
		if (cTok == 'C')
			szList[iSize++] = cLast = GetTokenVal (1);
		else if (!bRanges && cTok == '-')
			szList[iSize++] = cLast = GetTokenVal (1);
		else if (bRanges && cTok == '-')
			{
			EatToken ('-', NULL);
			if ((cTok = GetToken (0)) != 'C')
				return (PSZ) AddError ("Invalid Range");
			if ((cEnd = GetTokenVal (1)) < cLast)
				return (PSZ) AddError ("Invalid Range");
			for (cTok=cLast+1; cTok<=cEnd; cTok++)
				szList[iSize++] = cTok;
			cLast = cEnd;
			}
		else
			break;
		}
	szList[iSize] = '\0';
	return strdup ((PSZ) szList);
	}


/***************************************************************************/
/*                                                                         */
/* Parser fns                                                              */
/*                                                                         */
/***************************************************************************/

static PRX ParseR1 (void);


static PRX NewOp (UCHAR cType)
	{
	PRX prx;

	prx = (PRX) calloc (1, sizeof (RX));
	prx->cType = cType;
	return prx;
	}


static PRX PointTo (PRX prxChain, PRX prxTo)
	{
	PRX prx;

	/*--- add (last child to parent ptr)fixup to make continuous path ---*/
	for (prx = prxChain; prx && prx->next; prx = prx->next)
		;
	if (!prx) 
		return prxTo;
	prx->next = prxTo;
	prx->bSuper = TRUE;
	return prxTo;
	}


static PRX AddParent (PRX prxKid, UCHAR cType)
	{
	PRX prx;
	
	prx = NewOp (cType);
	prx->left = prxKid;
	return PointTo (prxKid, prx);
	}


//static PRX AddTerminator (PRX prxOrg)
//   {
//   PRX prx, prx2;
//
//   for (prx=prxOrg; prx && prx->next; prx = prx->next)
//      ;
//   prx2 = NewOp ('>');
//   if (prx)
//      prx->next = prx2;
//   else
//      prx = prx2;
//   return prxOrg;
//   }


static BOOL ReadRange (PPSZ ppszRange)
	{
	UCHAR cTok;
	BOOL bInvert = FALSE;

	EatToken ('[', NULL);
	cTok = GetToken (0);
	if (cTok == '~')
		{
		EatToken ('~', NULL);
		bInvert = TRUE;
		}
	*ppszRange = GetTokenString (TRUE);
	EatToken (']', "Invalid [] closure");
	return bInvert;
	}


/*
 * R4 = '<' | '>' | '?' | '*'   |
 *      '[' ['~'] UCHARLIST1 ']' |
 *      '{' R1 '}'              |
 *      UCHAR2
 *
 * UCHARLIST1 = UCHAR1 { [ '-' UCHAR1 ] | UCHAR1}
 * UCHARLIST2 = UCHAR2 { UCHAR2 }
 * UCHAR1     = any char except {}[]<>?*|-  or  '\' ANYUCHAR
 * UCHAR2     = any char except {}[]<>?*|   or  '\' ANYUCHAR
 * ANYUCHAR   = all characters
 */
static PRX ParseR4 (void)
	{
	UCHAR cTok;
	PRX   prx;

	cTok = GetToken(0);

	if (strchr ("<>?*", cTok))
		return NewOp (GetToken(1));
	if (cTok == 'C')
		{
		prx = NewOp ('C');
		prx->cChar = GetTokenVal (TRUE);
		return prx;
		}
	if (cTok == '{')
		{
		EatToken ('{', NULL);
		prx = ParseR1 ();
		EatToken ('}', "Invalid {} closure");
		return AddParent (prx, 'S');
		}
	if (cTok == '[')
		{
		prx = NewOp ('O'); // 'or' string
		prx->cChar = (ReadRange (&prx->pszStr) ? '~' : ' ');
		return prx;
		}
	return (PRX) AddError ("Invalid Closure");
	}


/*
 * R3 = R4 ['+' | '@']
 */
static PRX ParseR3 (void)
	{
	PRX  prx;
	UCHAR cTok;

	prx = ParseR4 ();
	cTok = GetToken(0);
	if (cTok == '+' || cTok == '@')
		return AddParent (prx, GetToken (1));
	return prx;
	}


/*
 * R2 = R3 [ '|' R2]
 */
static PRX ParseR2 (void)
	{
	PRX prx;

	prx = ParseR3 ();
	if (GetToken(0) == '|')
		{
		prx = AddParent (prx, GetToken (1));
		prx->right = ParseR2 ();
		return PointTo (prx->right, prx);
		}
	return prx;
	}


/*
 * R1 = R2 {R2 ...}
 */
static PRX ParseR1 (void)
	{
	PRX prx1, prx2, prxHead;
	UCHAR cTok;

	prxHead = prx1 = ParseR2 ();

	while (TRUE)
		{
		cTok = GetToken(0);
		if (!cTok || cTok == '}')
			break;
		prx2 = ParseR2 ();
		prx1->next = prx2;
		prx1 = prx2;
		}
	return prxHead;
	}


/*
 * Parses a regular expression into an internal form
 */
PRX _regParseRegex (PSZ pszRegex)
	{
	InitTokens (pszRegex);
	return AddParent (ParseR1 (), 'S');
	}


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


static BOOL CharMatch (CHAR c1, CHAR c2)
	{
	if (bCASESENSITIVE)
		return c1 == c2;
	return (toupper(c1) == toupper (c2));
	}


static BOOL RangeMatch (CHAR c1, PSZ pszRange)
	{
	if (bCASESENSITIVE)
		return !!strchr (pszRange, c1);
	c1 = toupper (c1);
	for (; pszRange && *pszRange; pszRange++)
		if (c1 == toupper(*pszRange))
			return TRUE;
	return FALSE;
	}


static BOOL EvalRegex (PRX prx, PSZ pszStr, INT iIndex, BOOL bFromChild)
	{
	INT  i;
	BOOL bNextOnly, bInvert;

	if (!prx)
		{
		if (bMATCHMODE && pszStr[iIndex] && pszStr[iIndex] != '\n')
			return FALSE;
		return TRUE;
		}

	switch (prx->cType)
		{
		case 'C': //CHAR
			if (!CharMatch (pszStr[iIndex], prx->cChar))
				return FALSE;
			return EvalRegex (prx->next, pszStr, iIndex+1, prx->bSuper);

		case 'O': //RANGE
			bInvert = (prx->cChar == '~');
			if (RangeMatch (pszStr[iIndex], prx->pszStr) == bInvert)
				return FALSE;
			return EvalRegex (prx->next, pszStr, iIndex+1, prx->bSuper);

		case '<': //SOL
			if (iIndex && pszStr[iIndex-1] != '\n')
				return FALSE;
			return EvalRegex (prx->next, pszStr, iIndex, prx->bSuper);

		case '>': //EOL
			if (pszStr[iIndex] && pszStr[iIndex] != '\n')
				return FALSE;
			return EvalRegex (prx->next, pszStr, iIndex, prx->bSuper);

		case '*': // WILDCARD
			for (i=0; pszStr[iIndex+i]; i++)
				if (EvalRegex (prx->next, pszStr, iIndex+i, prx->bSuper))
					return TRUE;
			return EvalRegex (prx->next, pszStr, iIndex+i, prx->bSuper);

		case '?': // ANY1
			if (!pszStr[iIndex])
				return FALSE;
			return EvalRegex (prx->next, pszStr, iIndex+1, prx->bSuper);

		case 'S': // SCOPE
			if (!bFromChild)
				{
				prx->iStart = iIndex;
				return EvalRegex (prx->left, pszStr, iIndex, prx->bSuper);
				}
			else
				{
				prx->iIndex = iIndex; // end
				return EvalRegex (prx->next, pszStr, iIndex, prx->bSuper);
				}

		case '+': // 1ORMORE
			bNextOnly = (prx->iIndex == iIndex);
			prx->iIndex = iIndex;

			if (!bFromChild)
				return EvalRegex (prx->left, pszStr, iIndex, FALSE);

			prx->iIndex = iIndex;
			if (EvalRegex (prx->next, pszStr, iIndex, prx->bSuper))
				return TRUE;

			if (bNextOnly)
				return FALSE;

			prx->iIndex = iIndex;
			return EvalRegex (prx->left, pszStr, iIndex, FALSE);

		case '@': // 0ORMORE
			bNextOnly = (prx->iIndex == iIndex);
			prx->iIndex = iIndex;

			if (EvalRegex (prx->next, pszStr, iIndex, prx->bSuper))
				return TRUE;

			if (bFromChild && bNextOnly)
				return FALSE;

			prx->iIndex = iIndex;
			return EvalRegex (prx->left, pszStr, iIndex, FALSE);

		case '|': // OR
			if (bFromChild)
				return EvalRegex (prx->next, pszStr, iIndex, prx->bSuper);
			if (EvalRegex (prx->left,  pszStr, iIndex, FALSE))
				return TRUE;
			if (EvalRegex (prx->right, pszStr, iIndex, FALSE))
				return TRUE;
			return FALSE;

		default :
			AddError ("Unknown OP");
			return FALSE;
		}
	}


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


static PRX FindScope (PRX prxHead, PINT piIdx)
	{
	PRX  prx, prxRet;

	for (prx = prxHead; prx; prx = (prx->bSuper ? NULL : prx->next))
		{
		if (prx->cType == 'S')
			if (!(*piIdx)--)
				return prx;
		if (prxRet = FindScope (prx->left, piIdx))
			return prxRet;
		if (prxRet = FindScope (prx->right, piIdx))
			return prxRet;
		}
	return NULL;
	}


static BOOL StrScopeMatch (PSZ pszBuff, PSZ pszSrc, PRX prx, INT iIndex)
	{
	if (!(prx = FindScope (prx, &iIndex)))
		return FALSE;

	strncpy (pszBuff, pszSrc+prx->iStart, prx->iIndex - prx->iStart);
	pszBuff[prx->iIndex-prx->iStart] = '\0';
	return TRUE;
	}


static PSZ BuildReplaceString (PSZ pszDest, PSZ pszSrc, PSZ pszResult, PRX prx)
	{
	PSZ  psz;
	CHAR szBuff [256];

	for (psz = pszResult; psz && *psz; )
		{
		if (*psz == '\\' && psz[1] >= '0' && psz[1] <= '9')
			{
			if (StrScopeMatch (szBuff, pszSrc, prx, psz[1]-'0'+1))
				{
				strcpy (pszDest, szBuff);
				pszDest += strlen (szBuff);
				}
			psz += 2;
			}
		else
			*pszDest++ = *psz++;
		}
	*pszDest = '\0';
	return pszDest;
	}


/***************************************************************************/
/*                                                                         */
/* Exported Fns                                                            */
/*                                                                         */
/***************************************************************************/


/*
 * Frees a parsed regular expression
 */
PRX _regFreePRX (PRX prxHead)
	{
	PRX  prx, prxNext;

	if (!prxHead)
		return NULL;

	for (prx = prxHead; prx; prx = prxNext)
		{
		prxNext = (prx->bSuper ? NULL : prx->next);

		_regFreePRX (prxHead->left);
		_regFreePRX (prxHead->right);
		if (prx->pszStr)
			free (prx->pszStr);
		free (prx);
		}
	return NULL;
	}


/*
 * like RegMatch, but works on an already parsed expression
 */
BOOL _regMatch (PSZ pszSrc, PRX prx)
	{
	bMATCHMODE = TRUE;
	return EvalRegex (prx, pszSrc, 0, FALSE);
	}


/*
 * like RegMatchReplace, but works on an already parsed expression
 */
BOOL _regMatchReplace (PSZ pszDest, PSZ pszSrc, PRX prx, PSZ pszResult)
	{
	*pszDest = '\0';
	bMATCHMODE = TRUE;
	if (!EvalRegex (prx, pszSrc, 0, FALSE))
		return FALSE;
	BuildReplaceString (pszDest, pszSrc, pszResult, prx);
	return TRUE;
	}


/*
 * like RegFind, but works on an already parsed expression
 */
BOOL _regFind (PSZ pszSrc, PRX prx, PINT piStart, PINT piEnd)
	{
	INT i, iLen;

	bMATCHMODE = FALSE;
	iLen = strlen (pszSrc);
	for (i=0; i< iLen; i++)
		if (EvalRegex (prx, pszSrc, i, FALSE))
			{
			*piStart  = prx->iStart;
			*piEnd    = prx->iIndex;
			return TRUE;
			}
	return FALSE;
	}


/*
 * similar _regFindReplace, but also has piStart and piEnd
 * piStart and piEnd are both input and output params
 * if piStart and piEnd are NULL they are ignored
 * otherwise coming in they are the start and endpoints of the search
 * coming out they are the start and end positions of the replacement
 */
BOOL _regFindReplace2 (PSZ pszDest, PSZ pszSrc, PRX prx, PSZ pszResult,
							  PINT piStart, PINT piEnd)
	{
	INT i, iStart, iEnd, iLen;

	*pszDest = '\0';
	bMATCHMODE = FALSE;
	iLen = strlen (pszSrc);

	iStart = (piStart ? min (iLen, *piStart) : 0);
	iEnd   = (piEnd   ? min (iLen, *piEnd+1) : iLen);

	for (i=iStart; i< iEnd; i++)
		if (EvalRegex (prx, pszSrc, i, FALSE))
			{
			strncpy (pszDest, pszSrc, prx->iStart);
			pszDest += prx->iStart;
			BuildReplaceString (pszDest, pszSrc, pszResult, prx);
			if (piStart) *piStart = prx->iStart;
			if (piEnd  ) *piEnd   = prx->iStart + strlen (pszDest);
			strcat (pszDest, pszSrc+prx->iIndex);
			return TRUE;
			}
	if (piStart) *piStart = 0;
	if (piEnd  ) *piEnd   = 0;
	return FALSE;
	}

/*
 * like RegFindReplace, but works on an already parsed expression
 */
BOOL _regFindReplace (PSZ pszDest, PSZ pszSrc, PRX prx, PSZ pszResult)
	{
	return _regFindReplace2 (pszDest, pszSrc, prx, pszResult, NULL, NULL);
	}


/***************************************************************************/


/*
 * set regular expression searches to be case sensitive or case insensitive
 */
void RegCaseSensitive (BOOL bCase)
	{
	bCASESENSITIVE = bCase;
	}


/*
 * Call this fn after calling one of the Reg search/match fns or after
 * calling _regParseRegex to see if there was a parse error.
 *
 * returns NULL if there was no error
 */
PSZ RegIsError (void)
	{
	return pszGLOBALERR;
	}


/*
 * Match a string.  A match means the entire source string 
 * must match the Regular expression
 *
 * pszSrc .... The source string to operate on
 * pszRegex .. The regular expression to search with
 *
 * Returns TRUE if the replacement was successful
 */
BOOL RegMatch (PSZ pszSrc, PSZ pszRegex)
	{
	PRX  prx;
	BOOL bRet;

	prx = _regParseRegex (pszRegex);
	if (pszGLOBALERR)
		return FALSE;

	bRet = _regMatch (pszSrc, prx);
	_regFreePRX (prx);
	return bRet;
	} 


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
BOOL RegMatchReplace (PSZ pszDest, PSZ pszSrc, PSZ pszRegex, PSZ pszResult)
	{
	PRX prx;
	BOOL bRet;

	prx = _regParseRegex (pszRegex);
	if (pszGLOBALERR)
		return FALSE;
	bRet = _regMatchReplace (pszDest, pszSrc, prx, pszResult);
	_regFreePRX (prx);
	return bRet;
	} 


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
BOOL RegFind (PSZ pszSrc, PSZ pszRegex, PINT piStart, PINT piEnd)
	{
	PRX prx;
	BOOL bRet;

	prx = _regParseRegex (pszRegex);
	if (pszGLOBALERR)
		return FALSE;
	bRet = _regFind (pszSrc, prx, piStart, piEnd);
	_regFreePRX (prx);
	return bRet;
	}


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
BOOL RegFindReplace (PSZ pszDest, PSZ pszSrc, PSZ pszRegex, PSZ pszResult)
	{
	PRX  prx;
	BOOL bRet;

	prx = _regParseRegex (pszRegex);
	if (pszGLOBALERR)
		return FALSE;
	bRet = _regFindReplace (pszDest, pszSrc, prx, pszResult);
	_regFreePRX (prx);
	return bRet;
	} 

}