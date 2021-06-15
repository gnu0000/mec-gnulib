/*
 * Scr0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides base functions for the
 * screen module
 * 
 * 
 **************************************************************************** 
 * 
 * PMET  ScrInitMetrics    (void)
 * PMET  ScrGetMetrics     (void)
 * COORD ScrGetScreenSize  (void)
 * BOOL  ScrSetScreenSize  (COORD dwNewSize)
 * BOOL  ScrRestoreScreen  (void)
 * BOOL  ScrIsEgaVga       (void)
 * PMET  scrCheckGetMetrics(void)
 * 
 **************************************************************************** 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"

#define DEFDACREGS1 "\x00\x00\x00\x00\x00\x2a\x00\x2a\x00\x00\x2a\x2a\x2a\x00\x00\x2a\x00\x2a\x2a\x2a\x00\x2a\x2a\x2a"
#define DEFDACREGS2 "\x15\x15\x15\x15\x15\x3f\x15\x3f\x15\x15\x3f\x3f\x3f\x15\x15\x3f\x15\x3f\x3f\x3f\x15\x3f\x3f\x3f"

static MET SCREENMETRICS = 
				  {0x07,              // wOrigAtt
					{25, 80},          // dwOrigSize
					{25, 80},          // dwSize
					1,0,0};            // bColorDisplay,bDACMod,bInit;


/*************************************************************************/
/*                                                                       */
/* External fn's                                                         */
/*                                                                       */
/*************************************************************************/

/*
 * Under DOS  this returns true if we are running with  EGA or VGA
 * Under OS/2 this doesn't always work
 */
BOOL ScrIsEgaVga(void)
	{
	return TRUE;       // how do I do this in NT ?
	}


/*
 * Returns the number of rows/cols on screen
 */
COORD ScrGetScreenSize (void)
	{
	CONSOLE_SCREEN_BUFFER_INFO scrInfo;
	
	GetConsoleScreenBufferInfo(GetStdHandle (STD_OUTPUT_HANDLE), &scrInfo);
	return scrInfo.dwSize;
	}



/*
 *
 */
BOOL ScrSetScreenSize (COORD dwNewSize)
	{
	COORD dwCurrentSize;

	dwCurrentSize = ScrGetScreenSize ();

	if (dwCurrentSize.Y == dwNewSize.Y && dwCurrentSize.X == dwNewSize.X)
		return FALSE;

	if (SetConsoleScreenBufferSize(GetStdHandle (STD_OUTPUT_HANDLE), dwNewSize))
		{
		PMET pmet;

		pmet = scrCheckGetMetrics ();
		pmet->dwSize = dwNewSize;
		return TRUE;
		}
	return FALSE;
	}


/*
 *
 *
 */
BOOL ScrRestoreScreen (void)
	{
	PMET pmet;
	PSZ  psz;
	INT  i;

	pmet = ScrGetMetrics ();

	if (pmet->dwSize.Y != pmet->dwOrigSize.Y ||
		 pmet->dwSize.X != pmet->dwOrigSize.X)
		ScrSetScreenSize (pmet->dwOrigSize);

	if (pmet->bDACMod)
		{
		for (psz=DEFDACREGS1, i=0; i<8; i++, psz+=3)
			ScrSetDACReg (i, psz[0], psz[1], psz[2]);
		for (psz=DEFDACREGS2, i=0; i<8; i++, psz+=3)
			ScrSetDACReg (i+0x38, psz[0], psz[1], psz[2]);
		}
	return TRUE;
	}


/*
 * this returns the metric structure.
 * ScrInitMetrics must be called first
 */
PMET ScrGetMetrics (void)
	{
	PMET pmet;

	pmet = &SCREENMETRICS;

	return pmet;
	}

/*
 *
 */
PMET scrCheckGetMetrics (void)
	{
	PMET pmet = ScrGetMetrics();

	if (!pmet->bInit)
		pmet = ScrInitMetrics();
	return pmet;   
	}


/*
 * This should be the first function called in this module.  This
 * initializes the screen metric information needed to restore the
 * cursor and screen ...
 *
 */
PMET ScrInitMetrics (void)
	{
	PMET pmet;

	pmet = ScrGetMetrics ();
	pmet->cOrigAtt       = (BYTE)ScrReadCell (MKCOORD (1,1)).Attributes;
	pmet->dwOrigSize     = ScrGetScreenSize ();
	pmet->dwSize         = pmet->dwOrigSize;
	pmet->bColorDisplay  = TRUE;
	pmet->bDACMod        = FALSE;
	pmet->bInit          = TRUE;

	return pmet;
	}             

