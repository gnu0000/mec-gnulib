/*
 * WinD.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * Combobox fn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <minmax.h>

#include <string.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuwin.h"
#include "gnuscr.h"
#include "gnukbd.h"
#include "gnumisc.h"


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/


/*
 * pops up a combobox
 * user selects a choice from a list
 *  ppsz is the array of string choices (last entry must be NULL)
 *  iY,iX is the cell position, call with NULL for free combo
 *  iYSize is the size of the combo client, 0 = as large as needed/possible
 *  iStartSel is the initial selected line -1 = no start selection
 * return is 0 based index of selected row, or -1 if user hits esc
 */
INT GnuComboBox (PPSZ ppsz, INT iY, INT iX, INT iYSize, INT iStartSel)
	{
	PMET pmet;
	PGW  pgw;
	INT i, iMaxItemsOnScreen, c, iEntries;
	INT iXSize, iYPos, iXPos, iRet;

	if (!ppsz || !*ppsz)
		return -1;

	pmet = ScrGetMetrics ();
	iMaxItemsOnScreen = pmet->dwSize.Y-2;

	for (iEntries=0; ppsz[iEntries]; iEntries++)
		;

	/*--- determine X Size ---*/
	for (i=iXSize=0; i<iEntries; i++)
		iXSize = max (iXSize, (INT)strlen (ppsz[i]));
	iXSize = min (iXSize, 70);
	iXSize += 3; // cvt client area size to window size

	/*--- if size not spec'd determine a good Y size for window ---*/
	if (!iYSize) 
		{
		iYSize = (iEntries > iMaxItemsOnScreen ? 10 : iEntries);
//
//
//      iYSize = min (iEntries, iMaxItemsOnScreen);
//      if ((iYSize<iEntries) && (iY + 10 < pmet->dwSize.Y))
//         iYSize = pmet->dwSize.Y - iY - 2;
		}
	iYSize += 2; // cvt client area size to window size

	/*--- determine a good place for the window ---*/
	iXPos = iYPos = (INT)-1;
	/*--- determine Y position ---*/
	if (!iY || (iY+1 == pmet->dwSize.Y)) // start at edge
		iYPos = (iY ? pmet->dwSize.Y-iYSize: 0);
	else if (iY + iYSize - 2 < pmet->dwSize.Y)
		iYPos = iY - 1;
	else if (iY + 2 >= iYSize)
		iYPos = iY + 2 - iYSize;
	else
		iYPos = pmet->dwSize.Y-iYSize;

	/*--- determine X position ---*/
	if (!iX)
		iXPos = 0;
	else if (iX + iXSize - 2 < pmet->dwSize.X)
		iXPos = iX - 1;
	else
		iXPos = pmet->dwSize.X - iXSize;

	ScrPushCursor (FALSE);
	GnuPaintAtCreate (FALSE);
	pgw = GnuCreateWin2 (MKCOORD (iXPos, iYPos), MKCOORD(iXSize, iYSize), NULL);
	pgw->bShadow        = FALSE;
	pgw->pUser1         = ppsz;
	pgw->iItemCount     = iEntries;
	pgw->bShowScrollPos = TRUE;
	pgw->iSelection     = (iStartSel >= 0 && iStartSel < (INT)iEntries ? iStartSel : 0);
	GnuPaintAtCreate (TRUE);

	GnuPaintWin (pgw, (INT)-1);
	GnuPaintBorder (pgw);
	while (TRUE)
		{
		c = KeyGet (TRUE);
		if (!c || c == K_ESC || c == K_RET)
			break;
		if (!GnuDoListKeys (pgw, c))
			GNUBeep (0);
		}
	iRet = (c == K_RET ? pgw->iSelection : -1);
	GnuDestroyWin (pgw);
	ScrPopCursor ();
	return iRet;
	}

