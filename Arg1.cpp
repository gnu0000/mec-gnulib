/*
 * Arg1.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides additonal functions
 * for the Arg module
 */

#include <stdio.h>
#include <string.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuarg.h"
#include "arg.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 *
 */
INT ArgGetIndex (PSZ pszArg, INT iIndex)
	{
	INT i, j, iComp;
	PARG   parg;
	PINST  pinstFree;

	if (!argGetInfo (&parg, &pinstFree))
		{
		argSetErr (3, NULL);
		return -1;
		}

	if (!pszArg) /*--- free Args ---*/
		{
		for (i=0; pinstFree && pinstFree[i].pszVal; i++)
			if (i == iIndex)
				return pinstFree[i].iIndex;
		argSetErr (6, pszArg);
		return -1;
		}

	for (i=0; parg && parg[i].pszParam; i++)
		{
		if (parg[i].bNoCase)
			iComp = stricmp (parg[i].pszParam, pszArg);
		else
			iComp = strcmp (parg[i].pszParam, pszArg);

		if (iComp)
			continue;

		for (j=0; j< parg[i].iCount; j++)
			if (j == iIndex)
				return parg[i].pinst[j].iIndex;
		argSetErr (9, pszArg);
		return -1;
		}
	argSetErr (6, pszArg);
	return -1;
	}



/*
 *
 */
INT ArgFillBlk2 (PSZ pszArgStr)
	{
	CHAR szCpy [STRLEN];
	PSZ  pszCpy;

	if (!argGetInfo (NULL, NULL))
		return argSetErr (3, NULL);
	if (!pszArgStr || !*pszArgStr)
		return 0;

	strcpy (szCpy, pszArgStr);
	pszCpy = szCpy;

	if (*pszCpy == '"')
		{
		pszCpy++;
		szCpy[strlen (szCpy) -1] = '\0';
		}
	strcat (szCpy, " ");
	return argDigest (pszCpy);
	}



/*
 *
 */
INT ArgEnum (PSZ pszArg, PSZ pszVal, INT iIndex)
	{
	INT i, j;
	PARG   parg;
	PINST  pinstFree;

	if (pszArg) pszArg[0] = '\0';
	if (pszVal) pszVal[0] = '\0';

	if (!argGetInfo (&parg, &pinstFree))
		return !argSetErr (3, NULL);

	for (i=0; pinstFree && pinstFree[i].pszVal; i++)
		if (pinstFree[i].iIndex == iIndex)
			{
			if (pszArg) strcpy (pszArg, pinstFree[i].pszVal);
			return 1;
			}

	for (i=0; parg && parg[i].pszParam; i++)
		for (j=0; j< parg[i].iCount; j++)
			if (parg[i].pinst[j].iIndex == iIndex)
				{
				if (pszArg) strcpy (pszArg, parg[i].pszParam);
				if (parg[i].pinst[j].pszVal && pszVal)
					strcpy (pszVal, parg[i].pinst[j].pszVal);
				return 2;
				}

	argSetErr (9, NULL);
	return FALSE;
	}



/*
 *
 */
void ArgDump (void)
	{
	INT   i,j;
	PARG  parg;
	PINST pinstFree;

	printf ("ARG Cmd Line Parse Utility v1.0     Craig Fitzgerald      %s  %s\n\n", __TIME__, __DATE__);

	if (!argGetInfo (&parg, &pinstFree))
		return;

	for (i=0; parg[i].pszParam; i++)
		{
		printf ("bMin: %d   bNoCase: %d   cType: %c   uCount: %d   Param: %s\n", 
				  parg[i].bMinimal, 
				  parg[i].bNoCase, 
				  parg[i].cValType, 
				  parg[i].iCount, 
				  parg[i].pszParam);

		for (j=0; j <parg[i].iCount && parg[i].pinst; j++)
			printf ("   Index: %d   Val : %s\n", 
					  parg[i].pinst[j].iIndex,  
					  parg[i].pinst[j].pszVal);
		printf ("\n");
		}
	for (i=0; pinstFree && pinstFree[i].pszVal; i++)
		{
		printf ("Free: Index: %d    Val: %s\n", 
				  pinstFree[i].iIndex, 
				  pinstFree[i].pszVal); 
		}

	}




void ArgClear (void)
	{
	INT   i, j;
	PARG  parg;
	PINST pinstFree;

	if (!argGetInfo (&parg, &pinstFree))
		return;

	for (i=0; parg[i].pszParam; i++)
		{
		if (parg[i].iCount)
			{
			for (j=0; j<parg[i].iCount; j++)
				parg[i].pinst[j].pszVal = (PSZ) MemFreeData (parg[i].pinst[j].pszVal);
			parg[i].pinst  = (PINST) MemFreeData (parg[i].pinst);
			parg[i].iCount = 0;
			}
		}
	for (i=0; pinstFree && pinstFree[i].pszVal; i++)
		{
		pinstFree[j].pszVal = (PSZ) MemFreeData (pinstFree[j].pszVal);
		}
	pinstFree = (PINST) MemFreeData (pinstFree);
	}
