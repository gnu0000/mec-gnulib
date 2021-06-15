/*
 * Msc5.c
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
 * ULONG InitFineSleep (ulMils)
 * void  FineSleep     (ulMils)
 * void  DoSleep       (ulTime)
 * 
 ****************************************************************************
 * 
 */

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnumisc.h"

static ULONG ulCOUNTSPERSEC = 0;


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * Initializes the fine grained delay fn FineSleep
 * the param ulLen is the ammount of time to use to init the fn
 * in milliseconds.  The larger the value the more accurate the delay
 *
 * ulLen  variance
 * ------------------
 *    20  20.66%
 *    50   9.63%
 *   100   3.61%
 *   200   5.16%
 *   500   2.29%
 *  1000   1.42%
 *  1500   1.06%
 *  3000    .76%
 */
ULONG InitFineSleep (ULONG ulMils)
	{
	ULONG ul1, ul2, ulCount, ulDest; 

	for (ul1=ul2=clock (); ul1 == ul2; ul1=clock ())
		;
	ulDest = ul1 + ulMils;
	for (ulCount = 0; ulDest >= (ul2 = (ULONG)clock ()); ulCount++)
		;
	ulCOUNTSPERSEC = (ulCount * CLOCKS_PER_SEC + (ul2 - ul1) / 2) / (ul2 - ul1);
	return ulCOUNTSPERSEC;
	}


/*
 * This is a better sleep fn
 * delay ulMils is in milliseconds.
 */
void FineSleep (ULONG ulMils)
	{
	ULONG ulDest, ulStart, ulCounts;

	if (!ulCOUNTSPERSEC) // we need to initialize timer var
		{
		InitFineSleep (ulMils);
		return;
		}
	ulDest = ulCOUNTSPERSEC * ulMils / 1000;
	ulStart = clock ();
	for (ulCounts = 0; ulCounts < ulDest; ulCounts++)
		clock (); // to keep compatible loop time with the init fn
	}


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


#if defined (__WATCOMC__)
#pragma optimize ("egl", off)
extern USHORT far *free_slice (void);
#pragma aux free_slice = \
		"mov ax, 1680h "   \
		"int 2fh       "   
#endif


void DoSleep (ULONG ulTime)
	{
#if defined (VER_DOS16M)

	ULONG lStart;

	lStart = clock ();
	while (lStart+ulTime >= (ULONG)clock ())
		{
		_asm mov ax, 1680h    // If under windows, give up time slice
		_asm int 2fh          // Otherwise, return immediately
		}

#elif defined (__WATCOMC__)
	ULONG lStart;

	lStart = clock ();
	ulTime /= 10;
	while (lStart+ulTime >= (ULONG)clock ())
		{
//      free_slice ();
		}
#endif

	}
#pragma optimize ("", on)
