/*
 * WinA.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides Gnu Set Color Window Dlg Box function for the
 * screen module
 */

#include <stdio.h>
#include <string.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"
#include "gnukbd.h"
#include "gnumisc.h"
#include "gnuwin.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

static void PaintBlock (PGW pgw, INT iAtt)
	{
	INT i;

	for (i=0; i<3; i++)
		ScrWriteStrAtt("      ", (BYTE)(iAtt<<4), MKCOORD (LeftOf(pgw)+56, TopOf(pgw)+4+i), 6);
	}


static void PaintSample (PGW pgw, INT iFg, INT iHi, INT iBg)
	{
	INT  i;
	CHAR sz[32];
	BYTE cAtt;

	cAtt = (iBg << 4) | iFg;

	GnuDrawBox (MKCOORD (LeftOf(pgw)+41, TopOf(pgw)+10), 
					MKCOORD (26, 10), NULL, cAtt);
	
	for (i=0; i<8; i++)
		ScrWriteStrAtt("                         ", cAtt, MKCOORD (LeftOf(pgw)+42, TopOf(pgw)+11+i), 24);
	ScrWriteStrAtt("[Sample Area]",  cAtt, MKCOORD (LeftOf(pgw)+47, TopOf(pgw)+10), 13);
	sprintf (sz, "Background Color: %2.2d", iBg);
	ScrWriteStrAtt(sz,               cAtt, MKCOORD (LeftOf(pgw)+44, TopOf(pgw)+13), 20);
	sprintf (sz, "ForeGround Color: %2.2d", iFg);
	ScrWriteStrAtt(sz,               cAtt, MKCOORD (LeftOf(pgw)+44, TopOf(pgw)+15), 20);
	sprintf (sz, "Hilite Color    : %2.2d", iHi);
	ScrWriteStrAtt(sz,               cAtt, MKCOORD (LeftOf(pgw)+44, TopOf(pgw)+17), 20);
	}


BOOL GnuModifyColorsWindow (void)
	{
	CHAR sz1[128], szLine[128];
	PGW  pgw;
	INT  i, j, c, uRGB[3], uBKUP[16][3];
	INT  iTriad = 0, iBg=1, iFg=14, iHi=15;
	static INT iPalIdx = 0;

	if (!ScrIsEgaVga ())
		return FALSE;

	ScrPushCursor (FALSE);

	for (i=0; i<16; i++) // store colors in case we abort
		ScrGetDACReg (ScrGetPALReg (i), &uBKUP[i][0], &uBKUP[i][1], &uBKUP[i][2]);

	GnuPaintAtCreate (FALSE);
	pgw = GnuCreateWin (24, 70, NULL);
	pgw->bShadow = FALSE;
	GnuSetColor (pgw, 4, 0);
	GnuPaintAtCreate (TRUE);
	GnuClearWin (pgw, 0, TRUE);

	pgw->pszHeader = "[Modify Color Registers]";
	pgw->pszFooter = "[Press <Enter> to accept, <Esc> to exit window]";
	GnuPaintBorder (pgw);

	GnuPaint (pgw, 3, 41, 0, 0, "Red");
	GnuPaint (pgw, 4, 41, 0, 0, "Green");
	GnuPaint (pgw, 5, 41, 0, 0, "Blue");

	GnuPaint (pgw, 2, 5, 0, 0, "0 1 2 3 4 5 6 7 8 9 a b c d e f");
	for (i=0; i<16; i++)
		{
		sprintf (szLine, "%1x ", i);
		for (j=0; j<16; j++)
			{
			sprintf (sz1, "\\~%2.2x**", (i<<4) | j);
			strcat (szLine, sz1);
			}
		GnuPaint (pgw, 3+i, 3, 0, 0, szLine);
		}
	ScrWriteStrAtt("  ", 0, MKCOORD (LeftOf(pgw)+6, TopOf(pgw)+4), 2);

// GnuPaint (pgw, 20, 5,  0, 1, "<TAB>,<Shift-TAB>-select color");
	GnuPaint (pgw, 20, 5,  0, 1, "<SPACE>-select color");
	GnuPaint (pgw, 7,  41, 0, 1, "<\x18,\x19,\x1a,\x1b>-modify color");
	GnuPaint (pgw, 20, 41, 0, 1, "<B><F><H>-set sample area");

	GnuPaintStr (pgw, 3+iTriad, 40, 0, 1, "\x1A"); 
	GnuPaintStr (pgw, 3+iPalIdx, 2, 0, 1, "\x1A");
	GnuPaintStr (pgw, 1, 5+iPalIdx*2, 0, 1, "");
	PaintSample (pgw, iFg, iHi, iBg);
	PaintBlock  (pgw, iPalIdx);

	ScrGetDACReg (ScrGetPALReg (iPalIdx), &uRGB[0], &uRGB[1], &uRGB[2]);
	for (i=0; i<3; i++)
		GnuPaintStr (pgw, 3+i, 51, 0, 0, "%2.2d", uRGB[i]);
	
	while (TRUE)
		{
		c = KeyChoose ("\x0D\x1b\x09 FBH=", "\x48\x50\x4d\x4b\x0f");

		if (c == K_RET) // return - ok/keep
			break;

		if (c == K_ESC) // abort
			{
			for (i=0; i<16; i++)
				ScrSetDACReg (ScrGetPALReg (i), uBKUP[i][0], uBKUP[i][1], uBKUP[i][2]);
			break;
			}

		/*--- selecr R/G/B DAC color component ---*/
		if (c == K_UP) // up - r/g/b select
			iTriad = (iTriad ? iTriad-1 : 2);
		if (c == K_DOWN) // dn - r/g/b select
			iTriad = (iTriad+1) % 3;
		if (c == K_UP || c == K_DOWN)
			for (i=0; i<3; i++)
				GnuPaintStr (pgw, 3+i, 40, 0, 1, "%c", (i==iTriad ? '\x1A' : ' '));

		/*--- modify color component ---*/
		if (c == K_RIGHT) // rt - current color incerment
			uRGB[iTriad] = (uRGB[iTriad] + 1) % 64;
		if (c == K_LEFT) // lt - current color decrement
			uRGB[iTriad] = (uRGB[iTriad] ? uRGB[iTriad]-1 : 63);
		if (c == '=')
			for (i=0; i<3; i++)
				uRGB[i] = uRGB[iTriad];
		if (c == K_RIGHT || c == K_LEFT || c == '=')
			ScrSetDACReg (ScrGetPALReg (iPalIdx), uRGB[0], uRGB[1], uRGB[2]);

		/*--- increment color attribute choice ---*/
		if (c == K_TAB || c == ' ')  // tab    - next color
			iPalIdx = (iPalIdx + 1) % 16;
		if (c == K_STAB) // sh-tab - prev color
			iPalIdx = (iPalIdx ? iPalIdx - 1 : 15);
		if (c == K_TAB || c == K_STAB || c == ' ')
			{
			for (i=0; i<16; i++)
				{
				GnuPaintStr (pgw, 3+i, 2,   0, 1, "%c", (i==iPalIdx ? '\x1A' : ' '));
				GnuPaintStr (pgw, 1, 5+i*2, 0, 1, "%c", (i==iPalIdx ? ''    : ' '));
				}
			ScrGetDACReg (ScrGetPALReg (iPalIdx), &uRGB[0], &uRGB[1], &uRGB[2]);
			PaintBlock (pgw, iPalIdx);
			}

		/*--- paint sample area ---*/
		if (c == 'F')
			iFg = iPalIdx;
		if (c == 'H')
			iHi = iPalIdx;
		if (c == 'B')
			iBg = iPalIdx;
		if (c == 'F' || c == 'H' || c == 'B')
			PaintSample (pgw, iFg, iHi, iBg);

		/*--- update r/g/b numbers ---*/
		for (i=0; i<3; i++)
			GnuPaintStr (pgw, 3+i, 51, 0, 0, "%2.2d", uRGB[i]);
		}
	GnuDestroyWin (pgw);
	ScrPopCursor ();
	return (c == K_RET);
	}


