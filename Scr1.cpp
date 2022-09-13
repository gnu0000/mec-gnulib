/*
 * Scr1.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * Base Screen/Cursor manipulation
 * 
 * 
 **************************************************************************** 
 * 
 * void  ScrScroll (SMALL_RECT rect, COORD dwOffset, CHAR_INFO ciFill)
 * COORD ScrGetCursorPos (void)
 * void  ScrSetCursorPos (COORD dwPos)
 * void  ScrShowCursor (BOOL bShow)
 * BOOL  ScrCursorVisible (void)
 * void  ScrGetDACReg (INT iReg, PINT piRed, PINT piGrn, PINT piBlu)
 * void  ScrSetDACReg (INT iReg, INT iRed, INT iGrn, INT iBlu)
 * INT  ScrGetPALReg (INT iPALIdx)
 * void  ScrSetPALReg (INT iPALIdx, INT iReg)
 * void  ScrStoreColors (void)
 * void  ScrRestoreColors (void)
 * void  scrAdjustRect (PSMALL_RECT prect)
 *
 **************************************************************************** 
 * 
 */

#include <stdlib.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"

extern "C"
{

#if defined (__WATCOMC__)
#include <graph.h>
#endif


#pragma optimize ("egl", off)

#define  MAXLINE 256

BYTE bSTOREDCOLORS [48];


/*******************************************************************/
/*                                                                 */
/* Screen Functions                                                */
/*                                                                 */
/*******************************************************************/

/*
 * converts -1 extents into screen extents
 */
void scrAdjustRect (PSMALL_RECT prect)
	{
	PMET pmet;

	pmet = scrCheckGetMetrics ();
	if (prect->Right  == -1) prect->Right  = pmet->dwSize.X-1;
	if (prect->Bottom == -1) prect->Bottom = pmet->dwSize.Y-1;
	}


/*
 * Scroll an area of the screen in any direction
 * if rect Right or Bottom is -1, assume Right/Bottom edge of screen
 *
 * dwOffset: +X <- Right  -X <- Left
 *           +Y <- Down   -Y <- Up
 */
void ScrScroll (SMALL_RECT rect, COORD dwOffset, CHAR_INFO ciFill)
	{
	scrAdjustRect (&rect);
	dwOffset.X += rect.Left;
	dwOffset.Y += rect.Top;
	ScrollConsoleScreenBuffer (GetStdHandle (STD_OUTPUT_HANDLE), 
										&rect, &rect, dwOffset, &ciFill);
	}



/*
 *
 */
void ScrGetDACReg (INT iReg, PINT piRed, PINT piGrn, PINT piBlu)
	{
#if defined (VER_DOS)
	PMET pmet = scrCheckGetMetrics ();

	if (!pmet->bColorDisplay)
		return;
	Int10 (0x1015, iReg & 0xFF, 0, 0);
	*piRed = Int10GetReg (3) >> 8;
	*piGrn = Int10GetReg (2) >> 8;
	*piBlu = Int10GetReg (2) & 0xFF;
#endif
	}


/*
 *
 */
void ScrSetDACReg (INT iReg, INT iRed, INT iGrn, INT iBlu)
	{
#if defined (VER_DOS)
	PMET pmet = scrCheckGetMetrics ();
	BYTE cRed, cGrn, cBlu;

	if (!pmet->bColorDisplay)
		return;
	pmet->bDACMod = TRUE;

	cRed  = (BYTE)(iRed & 0x3F);
	cGrn  = (BYTE)(iGrn & 0x3F);
	cBlu  = (BYTE)(iBlu & 0x3F);
	Int10 (0x1010, iReg & 0xFF, cGrn <<8 | cBlu, cRed << 8);
#endif
	}



/*
 *
 */
INT ScrGetPALReg (INT iPALIdx)
	{
#if defined (VER_DOS)
	PMET pmet = scrCheckGetMetrics ();

	if (!pmet->bColorDisplay)
		return 0;
	Int10 (0x1007, 0xFF00 | iPALIdx, 0, 0);
	return Int10GetReg (1) >> 8;
#else
	return 0;
#endif
	}


/*
 *
 */
void ScrSetPALReg (INT iPALIdx, INT iReg)
	{
#if defined (VER_DOS)
	PMET pmet = scrCheckGetMetrics ();

	if (!pmet->bColorDisplay)
		return;

	pmet->bDACMod = 3;

	Int10 (0x1000, (iReg < 8) | (iPALIdx & 0xFF), 0, 0);
#endif
	}


/*
 *
 */
void ScrStoreColors (void)
	{
	INT i, uR, uG, uB;

	for (i=0; i<16; i++)
		{
		ScrGetDACReg (ScrGetPALReg (i), &uR, &uG, &uB);
		bSTOREDCOLORS[i*3 + 0] = (BYTE) uR;
		bSTOREDCOLORS[i*3 + 1] = (BYTE) uG;
		bSTOREDCOLORS[i*3 + 2] = (BYTE) uB;
		}
	}

/*
 *
 */
void ScrRestoreColors (void)
	{
	INT i, uR, uG, uB;

	for (i=0; i<16; i++)
		{
		uR = (INT) bSTOREDCOLORS[i*3 + 0];
		uG = (INT) bSTOREDCOLORS[i*3 + 1];
		uB = (INT) bSTOREDCOLORS[i*3 + 2];
		ScrSetDACReg (ScrGetPALReg (i), uR, uG, uB);
		}
	}


/*******************************************************************/
/*                                                                 */
/* Cursor Functions                                                */
/*                                                                 */
/*******************************************************************/


/*
 *
 */
COORD ScrGetCursorPos (void)
	{
	CONSOLE_SCREEN_BUFFER_INFO scrInfo;

	GetConsoleScreenBufferInfo(GetStdHandle (STD_OUTPUT_HANDLE), &scrInfo);
	return scrInfo.dwCursorPosition;
	}



/*
 *
 */
void ScrSetCursorPos (COORD dwPos)
	{
	SetConsoleCursorPosition (GetStdHandle (STD_OUTPUT_HANDLE), dwPos);
	}



/*
 * shows/hides the cursor
 *
 */
void ScrShowCursor (BOOL bShow)
	{
	CONSOLE_CURSOR_INFO CursorInfo;

	CursorInfo.bVisible = bShow;
	CursorInfo.dwSize   = (bShow ? 25 : 1);
	SetConsoleCursorInfo(GetStdHandle (STD_OUTPUT_HANDLE), &CursorInfo);
	}


/*
 * Returns TRUE if cursor is visible
 *
 */
BOOL ScrCursorVisible (void)
	{
	CONSOLE_CURSOR_INFO CursorInfo;

	GetConsoleCursorInfo(GetStdHandle (STD_OUTPUT_HANDLE), &CursorInfo);
	return CursorInfo.bVisible;
	}
}