/*
 * Win6.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides Gnu Set Color Window Dlg Box function for the
 * screen module
 */

#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"
#include "gnukbd.h"
#include "gnumisc.h"
#include "gnuwin.h"


//0   "Screen Background",
//1   "Screen Selection",
//2   "Screen Normal Text",
//3   "Screen Hilite Text",
//4   "Screen Border",
//5   "Window Background",
//6   "Window Selection",
//7   "Window Normal Text",
//8   "Window Hilite Text",
//9   "Window Border",


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

static WORD k (PINT pu, INT uB, INT uF)
	{
	return (pu[uB] << 12) + (pu[uF] << 8);
	}


void ShowPicture (PGW pgwParent, PINT pu)
	{
	INT i, iTop, iLeft;

	iTop  = 2, iLeft = 42;

	GnuDrawBox (MKCOORD (LeftOf(pgwParent)+iLeft, TopOf(pgwParent)+iTop),
					MKCOORD (16, 11), 
					NULL, (BYTE)(k(pu,0,4) >> 8));

	for (i=0; i<9; i++)
		{
		GnuPaint (pgwParent, iTop+i, iLeft,   0, k(pu,0,2), "Normal  ");
		GnuPaint (pgwParent, iTop+i, iLeft+8, 0, k(pu,0,3), "Hilite");
		}
	for (i=4; i<6; i++)
		{
		GnuPaint (pgwParent, iTop+i, iLeft,   0, k(pu,1,2), "Normal  ");
		GnuPaint (pgwParent, iTop+i, iLeft+8, 0, k(pu,1,3), "Hilite");
		}

	GnuDrawBox (MKCOORD (LeftOf(pgwParent)+iLeft+4, TopOf(pgwParent)+iTop+2),
					MKCOORD (9, 7), 
					NULL, (BYTE)(k(pu,5,9) >> 8));

	for (i=0; i<5; i++)
		{
		GnuPaint (pgwParent, iTop+2+i, iLeft+4, 0, k(pu,5,7), "*** ");
		GnuPaint (pgwParent, iTop+2+i, iLeft+8, 0, k(pu,5,8), "***");
		}
	GnuPaint (pgwParent, iTop+5, iLeft+4, 0, k(pu,6,7), "*** ");
	GnuPaint (pgwParent, iTop+5, iLeft+8, 0, k(pu,6,6), "***");
	}




/*
 *
 * BOOL GnuSetColorsWindow2 (pgwCh)
 *
 */
BOOL GnuSelectColorsWindow2 (PPSZ ppszLabels, PINT piColors)
	{
	static INT iEntry = 0;
	INT   i, iEntries, iColor;
	int   iEnt, iCol;
	PGW   pgw;

	if (!ScrIsEgaVga ())
		return !GNUBeep (0);

	ScrPushCursor (FALSE);
	pgw = GnuCreateWin (15, 60, NULL);
	pgw->pszHeader = "[Select Colors]";
	pgw->pszFooter = "[Press <Enter> to accept, <Esc> to exit window]";
	GnuPaintBorder (pgw);
	GnuPaint (pgw, 2,  3, 0, 0, "Setting:");
	GnuPaint (pgw, 4,  3, 0, 0, "  Color:");
	GnuPaint (pgw, 8,  3, 0, 0, "<\x18,\x19> - Select Setting");
	GnuPaint (pgw, 10, 3, 0, 0, "<\x1a,\x1b> - Change color");

	for (iEntries=0; ppszLabels[iEntries]; iEntries++)
		;

	iColor = piColors[iEntry];

	for (i=0; i<16; i++)
		GnuPaint (pgw, 4,  12+i, 0, (WORD)(((i<<4)|15)<<8), "*");

	iColor = piColors[iEntry];
	GnuPaint (pgw, 2, 12, 0, 1, ppszLabels[iEntry]);
	ShowPicture (pgw, piColors);
	GnuPaint (pgw, 5, 13 + iColor, 1, 1, "");

	while (TRUE)
		{
		iEnt = iCol = 0;
		switch (KeyChoose ("\x0D\x1b\x09", "\x48\x50\x4d\x4b\x0f"))
			{
			case K_TAB:                       // tab
			case K_DOWN:  iEnt = 1;  break;   // dn

			case K_STAB:                      // sh-tab
			case K_UP:    iEnt = -1; break;   // up

			case K_RIGHT: iCol = 1;  break;   // rt

			case K_LEFT:  iCol = -1; break;   // lt

			case K_ESC:                       // esc
				GnuDestroyWin (pgw);
				ScrPopCursor ();
				return FALSE;

			default:                         //ret
				GnuDestroyWin (pgw);
				ScrPopCursor ();
				return TRUE;
			}
		GnuPaint (pgw, 5, 13 + iColor, 1, 1, " ");
		if (iEnt)
			{
			iEntry = (iEnt == 1 ? (iEntry + 1) % iEntries : (iEntry ? iEntry - 1 : iEntries - 1));
			iColor = piColors[iEntry];
			GnuPaintNChar (pgw, 2, 12, 0, 0, ' ', 24);
			GnuPaint (pgw, 2, 12, 0, 1, ppszLabels[iEntry]);
			}
		if (iCol)
			{
			iColor = (iCol == 1 ? (iColor + 1) % 16 : (iColor ? iColor - 1 : 15));
			piColors[iEntry] = iColor;
			ShowPicture (pgw, piColors);
			}
		GnuPaint (pgw, 5, 13 + iColor, 1, 1, "");
		}
	}

