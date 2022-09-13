/*
 * Msc2.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides miscelaneous functions
 * 
 * 
 ****************************************************************************
 * 
 * BOOL IsFlag (USHORT fFlags, WORD fFlag)
 * BOOL SetFlag (PWORD pfFlags, WORD fFlag, BOOL bSet)
 * void FlagList (WORD fFlags, BOOL bInvert, PBYTE pbFlagList)
 * 
 ****************************************************************************
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnumisc.h"


extern "C"
{


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/* This function tests a flag value
 * returns 1 or 0
 */
BOOL IsFlag (WORD wFlags, int iFlag)
	{
	return ((wFlags & (1 << iFlag)) ? 1 : 0);
	}


BOOL IsFlagD (DWORD dwFlags, int iFlag)
	{
	return ((dwFlags & (1 << iFlag)) ? 1 : 0);
	}



/* 
 * This function sets a flag value
 * it returns bSet, or FALSE if bad fFlag range
 */
BOOL SetFlag (WORD* pwFlags, int iFlag, BOOL bSet)
	{
	if (iFlag == -1)
		return !!(*pwFlags = (USHORT)-1);

	if (iFlag > 15)
		return 0;

	if (bSet == 2)                        /*--- invert bit ---*/
		*pwFlags ^= (1 << iFlag);
	else if (!bSet)                       /*--- clear bit ---*/
		*pwFlags &= ~(1 << iFlag);
	else /*if (bSet == 1)*/               /*--- set bit ---*/
		*pwFlags |= (1 << iFlag);
	return bSet;
	}


BOOL SetFlagD (DWORD* pdwFlags, int iFlag, int iSet)
	{
	if (iFlag == -1)
		return !!(*pdwFlags = (USHORT)-1);

	if (iFlag > 31)
		return 0;

	if (iSet == 2)                        /*--- invert bit ---*/
		*pdwFlags ^= (1 << iFlag);
	else if (!iSet)                       /*--- clear bit ---*/
		*pdwFlags &= ~(1 << iFlag);
	else /*if (bSet == 1)*/               /*--- set bit ---*/
		*pdwFlags |= (1 << iFlag);

	return IsFlagD (*pdwFlags, iFlag);
	}





/*
 * makes an array of flag numbers from a flag bit field
 * pbFlagList must point to an existing array containing at least 16 bytes
 * the list is 0xFF terminated (unless full)
 * if bInvert is FALSE, there will be an array entry for each set bit
 * if bInvert is TRUE, there will be an array entry for each clear bit
 */
void FlagList (WORD wFlags, BOOL bInvert, BYTE* pbFlagList)
	{
	INT i, j;

	memset (pbFlagList, 0xFF, 16);
	for (i=j=0; i<16; i++)
		if (!!bInvert == !IsFlag (wFlags, i))
			pbFlagList[j++] = i;
	}

void FlagListD (DWORD dwFlags, BOOL bInvert, BYTE* pbFlagList)
	{
	INT i, j;

	memset (pbFlagList, 0xFF, 32);
	for (i=j=0; i<32; i++)
		if (!!bInvert == !IsFlagD (dwFlags, i))
			pbFlagList[j++] = i;
	}
}