/*
 * Msc6.c
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
 * INT DoGNUBeep (INT iHz, INT iDuration)
 * INT GNUBeep (INT iGNUBeep)
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

extern "C"
{


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


INT DoGNUBeep (INT iHz, INT iDuration)
	{
#if defined (VER_NT)

	Beep (iHz, iDuration);

#else // 16 or 32 bit DOS 

	INT iDiv, iVal;

	/*--- set Frequency ---*/
	iDiv = (INT) (1193180L/(LONG)iHz);
	outp (0x43, 0xB6);
	outp (0x42, iDiv &  0xFF);
	outp (0x42, iDiv >> 8);

	/*--- speaker on ---*/
	iVal = inp (0x61) | 03;
	outp (0x61, iVal);

	/*--- wait ---*/
//   DoSleep (iDuration);
	FineSleep (iDuration);

	/*--- speaker off ---*/
	iVal = inp (0x61) & ~03;
	outp (0x61, iVal);

#endif

	return 0;
	}


//
// GNUBeeps: 0 - 5
//
INT GNUBeep (INT iGNUBeep)
	{
	switch (iGNUBeep)
		{
		case 0:  DoGNUBeep (123,   45); break;
		case 1:  DoGNUBeep (365,   45); break;
		case 2:  DoGNUBeep (500,   45); break;
		case 3:  DoGNUBeep (700,   45); break;
		case 4:  DoGNUBeep (900,   45); break;
		case 5:  DoGNUBeep (50,    45); break;
		default: DoGNUBeep (iGNUBeep, 45); break;
		}
	return 1;
	}
}