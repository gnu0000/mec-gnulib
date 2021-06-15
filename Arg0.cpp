/*
 * Arg0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This module provides the base functions for
 * the Arg module
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gnutype.h"
#include "gnumem.h"
#include "arg.h"
#include "gnuarg.h"

#define STRLEN    512
#define SMSTRLEN  128
#define BIGSTRLEN 4096
#define VALLEN    128

static BOOL   bInit        = FALSE;
static PARG   parg         = NULL;
static PINST  pinstFree    = NULL;
static INT    iGlobalIndex = 0;
static PSZ    pszARG0      = NULL;

INT    iArgErr = 0;
CHAR   szErrBuf[SMSTRLEN + 80];

static CHAR *Errors[] = 
					  {"No Error",                                     // 0
						"Unknown Parameter, or, argument required for", // 1
						"Parameter in bad form",                        // 2
						"ArgBuildBlk must be called first",             // 3
						"EOL Encountered before Param name read",       // 4
						"Value expected but not found",                 // 5
						"Requested Parameter not defined",              // 6
						"Ambiguous Parameter",                          // 7
						"Bad Parameter Value Type Flag for",            // 8
						"Requested Parameter not Entered",              // 9
						"Index too large",                              // 10
						NULL};


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 *
 *
 */
BOOL argGetInfo (PARG *ppargG, PINST *ppinstG)
	{
	if (ppargG)
		*ppargG  = parg;
	if (ppinstG)
		*ppinstG = pinstFree;
	return (bInit);
	}


/*
 *
 */
PSZ ArgGetErr (void)
	{
	return szErrBuf;
	}


/*
 *
 */
INT ArgIsErr (void)
	{
	return iArgErr;
	}


/*
 *
 */
INT argSetErr (INT iErr, PSZ pszErr)
	{
	CHAR szTmp [SMSTRLEN];

	iArgErr = iErr;

	/*--- error str (bmdline) could be BIG! ---*/
	*szTmp = '\0';
	if (pszErr)
		strncpy (szTmp, pszErr, SMSTRLEN-2);
	szTmp[SMSTRLEN-1] = '\0';
		
	sprintf (szErrBuf, "%s : \"%s\"", Errors[iArgErr], szTmp);
	return iErr;
	}



/*
 * Eats leading chars from the beginning of ppsz
 * returns 1st char in string
 */
static CHAR Eat (PPSZ ppsz, PSZ pszEatList)
	{
	while (**ppsz && strchr (pszEatList, **ppsz))
		(*ppsz)++;
	return **ppsz;
	}



/*
 * Eats leading whitespace from the beginning of ppsz
 * then expects to find and skip the char c
 * returns false if char is not c
 */
static BOOL EatChar (PPSZ ppsz, CHAR c)
	{
	BOOL   bReturn;

	Eat (ppsz, " \t\n");
	if (bReturn = ((CHAR) **ppsz == (CHAR) c))
		(*ppsz)++;
	return bReturn;
	}



/*
 * builds the argument list from 
 * a metadata string
 */
static INT BuildArgInfo (PSZ pszArgStr)
	{
	INT  i,iArgs = 0;
	CHAR c, szParam[VALLEN];

	parg = (PARG) malloc (sizeof (ARG));
	parg->pszParam = NULL;

	while (1)
		{
		if (!Eat (&pszArgStr, " \t\n"))
			break;

		iArgs++;
		parg = (PARG) realloc (parg, sizeof (ARG) * (iArgs+1));
		parg[iArgs - 1].pinst = NULL;
		parg[iArgs - 1].iCount = 0;

		if (parg[iArgs-1].bMinimal = (*pszArgStr == '*'))
			pszArgStr++;

		if (parg[iArgs-1].bNoCase = (*pszArgStr == '^'))
			pszArgStr++;

		i = 0;
		if (!(szParam[i++] = *pszArgStr++))
			return argSetErr (4, pszArgStr);

		while (*pszArgStr && !strchr ("@$=:?-%#& \t\n", *pszArgStr))
			szParam[i++] = *pszArgStr++;
		szParam[i] = '\0';

		parg[iArgs-1].pszParam = strdup (szParam);

		c = parg[iArgs-1].cValType = (CHAR)(*pszArgStr ? *pszArgStr: ' ');

		if (strchr ("\n\t\r\b", c))
			parg[iArgs-1].cValType = ' ';

		if (!*pszArgStr++)
			break;
		}
	parg[iArgs].pszParam = NULL;
	return 0; //mdh
	}


/*
 * under dos, if you want whitespace within a single arg you must enclose
 * the arg in double quotes.  Then the C argument processing code
 * strips the quotes.  This fn puts the quotes back in if needed
 */
static PSZ DecodeArg (PSZ pszBuff, PSZ pszArg)
	{
	PSZ  pszA, pszB;
	CHAR c;

	/*--- no imbedded whitespace in arg ---*/
	if (!strchr (pszArg, ' ') && !strchr (pszArg, '\t'))
		return pszArg;

	pszA = pszArg;
	pszB = pszBuff;
	c = Eat (&pszA, " \t\n");
	if (c != '/' && c != '-')
		{
		sprintf (pszBuff, "\"%s\"", pszArg);
		return pszBuff; // Free param, quote entire thing
		}

	*pszB++ = *pszA++;       // xfer stitch char
	while (*pszA && !strchr ("@$=:?-%#& \t\n", *pszA))
		*pszB++ = *pszA++;    // xfer param name
	while (*pszA && strchr (" \t", *pszA))
		*pszB++ = *pszA++;    // xfer whitespace 
	if (strchr (" =:", *pszA))
		{
		*pszB++ = *pszA++;    // assign operator
		while (*pszA && strchr (" \t", *pszA))
			*pszB++ = *pszA++; // whitespace
		}
	*pszB++ = '"';           // open parens
	strcpy (pszB, pszA);
	strcat (pszB, "\"");     // close parens
	return pszBuff;
	}



/*
 * makes all cmd line params into a single string
 *
 */
static void Flatten (PPSZ argv, PSZ pszCmdLine)
	{
	CHAR szBuff [256];

	*pszCmdLine = '\0';

	pszARG0 = argv[0];
	for (argv++; *argv != NULL; argv++)
		{
		strcat (pszCmdLine, DecodeArg (szBuff, *argv));
		strcat (pszCmdLine, " ");
		}
	}



/*
 * adds a parameter instance into the the argument list
 *
 */
static void AddNewParam (INT i, PSZ pszVal)
	{
	parg[i].iCount++;
	parg[i].pinst = (PINST) realloc (parg[i].pinst, 
												 sizeof (INST) * parg[i].iCount);
	parg[i].pinst[parg[i].iCount-1].iIndex = iGlobalIndex - 1;
	parg[i].pinst[parg[i].iCount-1].pszVal = (pszVal ? strdup (pszVal) : NULL);
	}



/*
 * extracts a word from the front of ppsz and returns it in psz
 *
 */
static INT GetStringVal (PPSZ ppsz, PSZ psz, BOOL bIgnoreSwitch)
	{
	PSZ p = psz;

	*psz = '\0';
	Eat (ppsz, " \t\n");

	if (!bIgnoreSwitch && (**ppsz == '/' || **ppsz == '-'))
		return 5;

	if (**ppsz == '"')
		{
		(*ppsz)++;
		while (**ppsz && **ppsz != '"')
			*psz++ = *(*ppsz)++;
		(*ppsz)++;
		}
	else
		{
		while (**ppsz && !strchr (" \t\n", **ppsz))
			*psz++ = *(*ppsz)++;
		}
	*psz = '\0';

	return !!(*p == '\0') * 2;
	}



/*
 * This fn expects the index of the matching arg blk
 *                 the the cmd line
 *                 the ptr into the cmd line just after the matched param 
 *
 * returns 0 if it added ok
 *         2 if bad form.
 *         5 if string val is bad or non existant
 */
static INT AddMatchedParam (INT iIndex, PPSZ ppszCL, PSZ pszValPtr)
	{
	CHAR szTmp[STRLEN];
	INT  iError;

	switch (parg[iIndex].cValType)
		{
		case ' ':
			if (*pszValPtr != ' ')
				return 2;
			AddNewParam (iIndex, NULL);
			*ppszCL = pszValPtr;
			return 0;

		case '$':
			if (*pszValPtr != ' ')
				return 2;
			if (iError = GetStringVal (&pszValPtr, szTmp, 0))
				return iError;
			AddNewParam (iIndex, szTmp);
			*ppszCL = pszValPtr;
			return 0;

		case '=':
			if (!EatChar (&pszValPtr, '='))
				return 2;
			if (iError = GetStringVal (&pszValPtr, szTmp, 0))
				return iError;
			AddNewParam (iIndex, szTmp);
			*ppszCL = pszValPtr;
			return 0;

		case ':':
			if (!EatChar (&pszValPtr, ':'))
				return 2;
			if (iError = GetStringVal (&pszValPtr, szTmp, 0))
				return iError;
			AddNewParam (iIndex, szTmp);
			*ppszCL = pszValPtr;
			return 0;

		case '%':
			Eat (&pszValPtr, " \t\n:=");
			if (iError = GetStringVal (&pszValPtr, szTmp, 0))
				return iError;
			AddNewParam (iIndex, szTmp);
			*ppszCL = pszValPtr;
			return 0;

		case '#':
			Eat (&pszValPtr, " \t\n:=");
			if (iError = GetStringVal (&pszValPtr, szTmp, 1))
				return iError;
			AddNewParam (iIndex, szTmp);
			*ppszCL = pszValPtr;
			return 0;

		case '&':
			if (!EatChar (&pszValPtr, ':') && !EatChar (&pszValPtr, '='))
				return 2;
			if (iError = GetStringVal (&pszValPtr, szTmp, 1))
				return iError;
			AddNewParam (iIndex, szTmp);
			*ppszCL = pszValPtr;
			return 0;
			
//
// This works but
// -param val  really shouldn't work because it is too confusing
//
//      case '?':
//         Eat (&pszValPtr, " \t\n:=");
//         iError = GetStringVal (&pszValPtr, szTmp, 0);
//         AddNewParam (iIndex, (iError ? NULL : szTmp));
//         *ppszCL = pszValPtr;
//         if (iError && (*(pszValPtr -1) == ' ' || *(pszValPtr -1) == '\t'))
//            (*ppszCL)--;
//         return 0;

		case '?':
			{
			if (*pszValPtr == ' ')
				{
				/*--- no param val ---*/
				AddNewParam (iIndex, NULL);
				*ppszCL = pszValPtr;
				return 0;
				}
			if (!EatChar (&pszValPtr, ':') && !EatChar (&pszValPtr, '='))
				return 2;
			if (iError = GetStringVal (&pszValPtr, szTmp, 0))
				return iError;
			AddNewParam (iIndex, szTmp);
			*ppszCL = pszValPtr;
			return 0;
			}

		case '@':
			if (*pszValPtr == ' ')
				return 2;
			if (iError = GetStringVal (&pszValPtr, szTmp, 1))
				return iError;
			AddNewParam (iIndex, szTmp);
			*ppszCL = pszValPtr;
			return 0;

		case '-':
			AddNewParam (iIndex, NULL);
			*ppszCL = pszValPtr;
			return 0;
		}
	return 8;
	}



/*
 *
 */
static INT FindMinParam (PPSZ ppszCL)
	{
	INT iComp, i, iLen, iMatch, iCount = 0;

	for (iLen=0; (*ppszCL)[iLen]; iLen++)
		if (strchr (" :=", (*ppszCL)[iLen]))
			break;

	for (i=0; parg && parg[i].pszParam; i++)
		{
		if (parg[i].bNoCase)
			iComp = strnicmp (parg[i].pszParam, *ppszCL, iLen);
		else
			iComp = strncmp (parg[i].pszParam, *ppszCL, iLen);

		iCount += !iComp;
		if (!iComp) iMatch = i;
		}

	if (!iCount)
		return argSetErr (1, *ppszCL);             // no match

	if (iCount > 1)
		return argSetErr (7, *ppszCL);             // >1 match

	if (!parg[iMatch].bMinimal)
		return argSetErr (1, *ppszCL);             // match not bMin

	if ((parg[iMatch].cValType == '@') || (parg[iMatch].cValType == '-'))
		return argSetErr (1, *ppszCL);             // cannot match @ or - types

	return AddMatchedParam (iMatch, ppszCL, *ppszCL + iLen);
	}



/* 
 *
 */
static INT FindParam (PSZ *ppszCL, CHAR cType)
	{
	INT i, iLen, iRet, iComp;

	for (i=0; parg && parg[i].pszParam; i++)
		{
		if (cType != parg[i].cValType)
			continue;
		if ((iLen = (int)strlen (parg[i].pszParam)) > (int)strlen (*ppszCL))
			continue;
		if (parg[i].bNoCase)
			iComp = strnicmp (parg[i].pszParam, *ppszCL, iLen);
		else
			iComp = strncmp (parg[i].pszParam, *ppszCL, iLen);

		if (iComp) continue;

		iRet = AddMatchedParam (i, ppszCL, *ppszCL + iLen);
		if (!iRet || iRet > 2)
			return argSetErr (iRet, *ppszCL);
		}
	return argSetErr (1, *ppszCL);
	}



/*
 *
 */
static void AddFreeParam (PPSZ ppszCL)
	{
	CHAR  szStr[STRLEN];
	INT   i;

	GetStringVal (ppszCL, szStr, 1);
	for (i=0; pinstFree && pinstFree[i].pszVal; i++)
		;
	pinstFree = (PINST) realloc (pinstFree, sizeof (INST) * (i+2));
	pinstFree[i].pszVal   = strdup (szStr);
	pinstFree[i].iIndex   = iGlobalIndex - 1;
	pinstFree[i+1].pszVal = NULL;
	}



/*
 *
 */
INT argDigest (PSZ pszCL)
	{
	INT  i, iRet;
	PSZ  pszLook;

	while (1)
		{
		if (!Eat (&pszCL, " \t\n"))
			break;

		if (*pszCL != '-' && *pszCL != '/')
			{
			iGlobalIndex++;
			AddFreeParam (&pszCL);
			continue;
			}
		pszCL++;

		while (1)
			{
			if (!*pszCL || *pszCL == ' ')
				break;

			iGlobalIndex++;
			pszLook = " $%#?=:@-&";
			for (i=0; *pszLook != '\0'; pszLook++)
				{
				iRet = FindParam (&pszCL, *pszLook);
				if (!iRet || iRet > 2)
					break;
				}
			if (iRet == 1)
				iRet = FindMinParam (&pszCL);
			if (!iRet) continue;
			return argSetErr (iRet, pszCL);
			}
		}
	return argSetErr (0, NULL);
	}




/*
 *
 */
INT ArgBuildBlk (PSZ pszArgDef)
	{
	CHAR szCpy [STRLEN];
	PSZ  pszCpy = szCpy;


	if (!pszArgDef)
		return argSetErr (3, NULL);

	strcpy (szCpy, pszArgDef);
	if (*pszCpy == '"')
		{
		pszCpy++;
		szCpy[strlen (szCpy) -1] = '\0';
		}

	bInit    = TRUE;
	parg     = NULL;
	pinstFree = NULL;
	return BuildArgInfo (pszCpy);
	}


 
/*
 *
 */
INT ArgFillBlk (PSZ argv[])
	{
	CHAR  szCmdLine [BIGSTRLEN];

	if (!bInit)
		return argSetErr (3, NULL);
	Flatten (argv, szCmdLine);
	return argDigest (szCmdLine);
	}



/*******************************************************************/



/*
 *
 */
INT ArgIs (PSZ pszArg)
	{
	INT i, iComp;

	if (!bInit)
		{
		argSetErr (3, NULL);
		return 0;
		}

	if (!pszArg) /*--- free Args ---*/
		{
		for (i=0; pinstFree && pinstFree[i].pszVal; i++)
			;
		return i;
		}
	for (i=0; parg && parg[i].pszParam; i++)
		{
		if (parg[i].bNoCase)
			iComp = stricmp (parg[i].pszParam, pszArg);
		else
			iComp = strcmp (parg[i].pszParam, pszArg);

		if (!iComp)
			return parg[i].iCount;
		}
	argSetErr (6, pszArg);
	return 0;
	}



/*
 *
 */
PSZ ArgGet (PSZ pszArg, INT iIndex)
	{
	INT i, iComp;

	if (!bInit)
		{
		argSetErr (3, NULL);
		return NULL;
		}

	if (!pszArg) /*--- free Args ---*/
		{
		for (i=0; pinstFree && pinstFree[i].pszVal; i++)
			if ((i == iIndex) || (iIndex == -1 && !pinstFree[i+1].pszVal))
				return pinstFree[i].pszVal;
		argSetErr (6, pszArg);
		return NULL;
		}

	for (i=0; parg && parg[i].pszParam; i++)
		{
		if (parg[i].bNoCase)
			iComp = stricmp (parg[i].pszParam, pszArg);
		else
			iComp = strcmp (parg[i].pszParam, pszArg);
		if (iComp)
			continue;

		if (iIndex >= 0 && iIndex < parg[i].iCount)
			return parg[i].pinst[iIndex].pszVal;
		else if (iIndex == -1 && parg[i].iCount)
			return parg[i].pinst[parg[i].iCount-1].pszVal;

		argSetErr (9, pszArg);
		return NULL;
		}
	argSetErr (6, pszArg);
	return NULL;
	}


PSZ ArgGet0 (void)
	{
	return pszARG0;
	}


//cdecl main (int argc, CHAR *argv[])
//   {
//   INT iRet; 
//   PSZ psz, pArgStr;
//
//
//   pArgStr = getenv ("PARG");
//
//   printf ("%s\n\n", pArgStr);
//
//   if (iRet = ArgBuildBlk (pArgStr))
//      {
//      psz = ArgGetErr ();
//      printf ("Build Error : %s\n\n", psz);
//      }
//
//   if (iRet = ArgFillBlk (argv))
//      {
//      psz = ArgGetErr ();
//      printf ("Fill Arg Error : %s\n\n", psz);
//      }
//
//   Dump ();
//   return 0;
//   }


/*
 * Free memory allocated to parg
 */
void ArgFreeBlk()
{
	int i=0;
	for (i=0; parg && parg[i].pszParam; i++)
	{
		MemFreeData(parg[i].pinst);
		MemFreeData(parg[i].pszParam);
	}
	MemFreeData(parg);
	parg = NULL;

	for (i=0; pinstFree && pinstFree[i].pszVal; i++)
	{
		MemFreeData(pinstFree[i].pszVal);
	}
	MemFreeData(pinstFree);
	pinstFree = NULL;
}
