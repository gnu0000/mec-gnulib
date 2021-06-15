/*
 * Win8.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides the GnuHexEdit function for the
 * screen module
 */

#include <stdio.h>
#include <stdlib.h>
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

/*--- reads mem and writes to screen ---*/
static void MemToScreen  (PGW pgw, PSZ pszBuff, INT iLines)
	{
	INT  x,y;
	CHAR szTmp[4];

	for (y=0; y<iLines; y++)
		for (x=0; x<16; x++)
			{
			szTmp[0] = HexChar (pszBuff [y * 16 + x] >> 4);
			szTmp[1] = HexChar (pszBuff [y * 16 + x] & 0x0F);
			szTmp[2] = '\0';
			GnuPaint (pgw, y+3, 7+x*3+x/4, 0, 0, szTmp);
			}
	}



/*--- reads screen and writes to mem ---*/
static void ScreenToMem (PGW pgw, PSZ pszBuff, INT iLines)
	{
	INT  x,y, iScrY, iScrX, iLen;
	CHAR szTmp[4];

	for (y=0; y<iLines; y++)
		{
		iScrY = TopOf (pgw) + 4 + y;
		for (x=0; x<16; x++)
			{
			iScrX = LeftOf (pgw) + 8 + x*3 + x/4;
			iLen = 2;
			ScrReadStr (szTmp, MKCOORD (iScrX, iScrY), iLen);
			ScrWriteStr ("..", MKCOORD (iScrX, iScrY), 2);

			pszBuff [y * 16 + x]  = (CHAR)((szTmp[0] - ((szTmp[0] > '9') ? '7' : '0')) << 4);
			pszBuff [y * 16 + x] += szTmp[1] - ((szTmp[1] > '9') ? '7' : '0');
			}
		}
	}




INT GnuHexEdit (PSZ pszBuff, INT iOffset, INT iLines)
	{
	PGW  pgw;
	INT  c, y, x, iScrX, iScrY;
	int  ix, iy;
	BOOL bLoop, bChanged;
	CHAR szTmp[16];

	pgw = GnuCreateWin (iLines+6, 60, NULL);
	pgw->pszHeader = "[Edit Offset]";
	pgw->pszFooter = "[Press <Enter> to accept <Esc> to exit]";
	GnuPaintBorder (pgw);

	/*--- Paint the window ---*/
	GnuPaint      (pgw, 1, 5, 0, 0, "00 01 02 03  04 05 06 07  08 09 0A 0B  0C 0D 0E 0F");
	GnuPaintNChar (pgw, 2, 1, 0, 0, '-', 16 * 3 + 6);
	for (y=0; y<iLines; y++)
		{
		sprintf (szTmp, "%4.4X", iOffset + y * 16);
		GnuPaint      (pgw, 3+y, 1, 0, 0, szTmp);
		}

	MemToScreen (pgw, pszBuff+iOffset, iLines);

	/*--- Edit The Window ---*/
	ScrPushCursor (TRUE);

	ix = iy = 0;
	x = y = 0;
	bLoop = TRUE;
	while (bLoop)
		{
		x += ix;
		if (x == 32)     x = 0,  iy = 1;
		if (x == -1) x = 31, iy = -1;
		y += iy;
		if (y == iLines)      y = 0;
		if (y == -1)  y = iLines-1;
		ix = iy = 0;
		iScrX = 7 + x + x/2 + x/8;
		iScrY = y + 3;

		GnuMoveCursor (pgw, MKCOORD (iScrX, iScrY));

		switch (c = KeyGet (TRUE))
			{
			case K_LEFT:   ix = -1; break;        /*--- Left  Arrow ---*/
			case K_RIGHT:  ix =  1; break;        /*--- Right Arrow ---*/
			case K_UP:     iy = -1; break;        /*--- Up    Arrow ---*/
			case K_DOWN:   iy =  1; break;        /*--- Down  Arrow ---*/
			case K_RET:    bLoop=FALSE; break;    /*--- Enter       ---*/
			case K_ESC:                           /*--- Escape      ---*/
				ScrPopCursor ();
				GnuDestroyWin (pgw);
				return 0;

			default:
				if (!strchr ("0123456789ABCDEF", c))
					GNUBeep (0);
				else
					{
					szTmp[0] = (UCHAR) c;
					szTmp[1] = '\0';
					GnuPaint (pgw, iScrY, iScrX, 0, 1, szTmp);
					ix = 1;
					bChanged = TRUE;
					}
			}
		}
	ScrPopCursor ();
	ScreenToMem (pgw, pszBuff+iOffset, iLines);
	GnuDestroyWin (pgw);
	return 1;
	}

