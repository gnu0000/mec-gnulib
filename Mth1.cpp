/*
 * Mth1.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides the MthFmat (BIG to STR) function for Math
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnumath.h"
#include "mth.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

//char szMONEY_SYMBOL [16] = "$";
//
//void SetMoneySymbol (char* szSymbol)
//	{
//	
//	}


static PSZ AddCommas (PSZ psz)
	{
	int iLen = strlen (psz) + 5;
	int i = (strchr (psz, '.') ? strchr (psz, '.') - psz - 3 : strchr (psz, '\0') - psz - 3);
	int iMin;
   for (iMin=i; iMin>0; iMin--)
//    if (!isdigit (psz[iMin-1]))
      if (psz[iMin-1]<'0' ||  psz[iMin-1]>'9')
         break;

//printf ("i=%d  iMin=%d iLen=%d\n", i, iMin, iLen);

//	for (; i > (*psz == '(' ? 2 : (*psz == '$' || *psz == '-' ? 1 : 0)); i -= 3)
	for (; i > iMin; i -= 3)
		{
		memmove (psz + i + 1, psz + i, iLen);
		psz[i] = ',';
		}
	return psz;
	}


/*
 * This fn formats a number and returns it as a string
 * if iWhole=0, any sized number is printed, using e notation if necessary
 * if bMoney is true and number is neg.  Accounting format (parens) is used
 *
 */
PSZ MthFmat (PSZ  pszNum,  // Return String
				 BIG  bgNum,   // Number
				 INT  iWhole,  // # of digits before decimal
				 INT  iDec,    // # of digits after decimal
				 BOOL bCommas, // use commas ?
				 BOOL bMoney)  // use leading $ ?
	{
	BIG bgTooBig;

	pszNum[0] = '\0';

	if (!MthValid (bgNum))
		return pszNum;

	if (!iWhole && (fabs(bgNum) > 9e12 || fabs(bgNum) < -9e12))
		sprintf (pszNum, "%le", (double) bgNum);
	else if (!bMoney)
		sprintf (pszNum, "%1.*lf",  iDec, (double)bgNum);
	else if (bgNum >= 0)
		sprintf (pszNum, "$%1.*lf", iDec, (double)bgNum);
	else
		sprintf (pszNum, "($%1.*lf)", iDec, (double) fabs (bgNum));

	bgTooBig = pow ((double)10, iWhole);

	/*--- Number does not fit into string ---*/
	if (iWhole && (bgTooBig <= bgNum || -bgTooBig >= bgNum))
		{
		memset (pszNum, '*', iWhole + iDec + 1);
		pszNum [iWhole + iDec + 1] = '\0';
		pszNum [iWhole] = '.';
		}

	if (bCommas)
		AddCommas (pszNum);
	return pszNum;
	}


/*
 * This fn formats a number and returns it as a string
 * if iWhole=0, any sized number is printed, using e notation if necessary
 * if pszMoneySymbol is not null and number is neg.  Accounting format (parens) is used
 */
PSZ MthFmat (PSZ  pszNum,          // Return String
				 BIG  bgNum,           // Number
				 INT  iWhole,          // # of digits before decimal
				 INT  iDec,            // # of digits after decimal
				 BOOL bCommas,         // use commas ?
				 PSZ  pszMoneySymbol)  // use leading currency Symbol ?
	{
   BOOL bMoney = pszMoneySymbol && *pszMoneySymbol;
	pszNum[0] = '\0';

	if (!MthValid (bgNum))
		return pszNum;

	if (!iWhole && (fabs(bgNum) > 9e12 || fabs(bgNum) < -9e12))
		sprintf (pszNum, "%le", (double) bgNum);
	else if (!bMoney)
		sprintf (pszNum, "%1.*lf",  iDec, (double)bgNum);
	else if (bgNum >= 0)
		sprintf (pszNum, "%s%1.*lf", pszMoneySymbol, iDec, (double)bgNum);
	else
		sprintf (pszNum, "(%s%1.*lf)", pszMoneySymbol, iDec, (double) fabs (bgNum));

	BIG bgTooBig = pow ((double)10, iWhole);

	/*--- Number does not fit into string ---*/
	if (iWhole && (bgTooBig <= bgNum || -bgTooBig >= bgNum))
		{
		memset (pszNum, '*', iWhole + iDec + 1);
		pszNum [iWhole + iDec + 1] = '\0';
		pszNum [iWhole] = '.';
		}

	if (bCommas)
		AddCommas (pszNum);
	return pszNum;
	}


PSZ MthFmat2  (PSZ  pszNum,  // Return String
					BIG  bgNum,   // Number
					INT  iWhole,  // # of digits before decimal
					INT  iDec,    // # of digits after decimal
					BOOL bCommas) // use commas ?
	{
//	pszNum[0] = '\0';
//
//	if (!MthValid (bgNum))
//		return pszNum;
//
//	if (!iWhole && (fabs(bgNum) > 9e12 || fabs(bgNum) < -9e12))
//		sprintf (pszNum, "%le", (double) bgNum);
//
//	sprintf (pszNum, "%*.*lf", iWhole, iDec, (double)bgNum);
//	BIG bgTooBig = pow (10, iWhole);
//
//	/*--- Number does not fit into string ---*/
//	if (iWhole && (bgTooBig <= bgNum || -bgTooBig >= bgNum))
//		{
//		memset (pszNum, '*', iWhole + iDec + 1);
//		pszNum [iWhole + iDec + 1] = '\0';
//		pszNum [iWhole] = '.';
//		}
//	if (bCommas)
//		AddCommas (pszNum);
//	return pszNum;

   return MthFmat (pszNum, bgNum, iWhole, iDec, bCommas, FALSE);
	}
