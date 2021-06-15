/*
 * Win3.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides Gnu List Window functions for the
 * screen module
 */

#include <stdlib.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"
#include "gnuwin.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * for list windows only.
 * scrolls if necessary to ensure that iLine is in view port.
 * returns TRUE if it had to scroll.
 */
BOOL GnuMakeVisible (PGW pgw, INT iLine)
	{
	INT iDisplayItems;

	if (!pgw || !pgw->pPaintProc || iLine > pgw->iItemCount) return FALSE;
	if (InWindow (pgw,iLine)) return FALSE;

	iDisplayItems = DisplayItems (pgw);

	if (iLine < pgw->iScrollPos)
		GnuScrollWin (pgw, pgw->iScrollPos - iLine, FALSE);
	else
		GnuScrollWin (pgw, iLine - pgw->iScrollPos - iDisplayItems + 1, TRUE);
	return TRUE;
	}



/*
 * Selects iLine. repaints new and old selection.
 * if bOnScreen = TRUE, uses GnuMakeVisible.
 * use iLine = -1 to deselect all
 */
BOOL GnuSelectLine (PGW pgw, INT iLine, BOOL bOnScreen)
	{
	INT iOldSel;

	if (!pgw || !pgw->pPaintProc) return FALSE;
	iOldSel = pgw->iSelection;
	pgw->iSelection = iLine;
	GnuPaintWin (pgw, iOldSel);
	if (iLine == (INT)-1) return FALSE;
	if (bOnScreen)
		if (!GnuMakeVisible (pgw, iLine))
			GnuPaintWin (pgw, iLine);
	return TRUE;
	}


/*
 * Calls the pGnuPaintProc to paint iLine. iLine is line index
 * if list, or window line if static.
 * paints all items (and entire window) if iLine = -1
 * returns TRUE if item is visible
 */
BOOL GnuPaintWin (PGW pgw, INT iLine)
	{
	INT iDisplayItems, i;

	if (!pgw || !pgw->pPaintProc) return FALSE;

	iDisplayItems = DisplayItems(pgw);

	if (iLine == -1 || iLine == -2)  /*--- paint entire window ---*/
		{
		if (iLine == -1)
			GnuClearWin (pgw, 0, FALSE);
		for (i=0; i < iDisplayItems && i + pgw->iScrollPos < pgw->iItemCount; i++)
			pgw->pPaintProc (pgw, i + pgw->iScrollPos, i * pgw->iItemSize);
		if (pgw->bShowScrollPos)
			gnuShowScrollPos (pgw, pgw->iScrollPos, TRUE);
		return TRUE;
		}

	if (iLine >= pgw->iItemCount) return FALSE;
	if (pgw->iScrollPos > iLine) return FALSE;
	if (pgw->iScrollPos + iDisplayItems -1 < iLine) return FALSE;
	pgw->pPaintProc (pgw, iLine, (iLine - pgw->iScrollPos) * pgw->iItemSize);
	return TRUE;
	}


/*
 * if a list window, this scrolls so that iLine is top line.
 * if a static window, no effect.
 */
BOOL GnuScrollTo  (PGW pgw, INT iLine)
	{
	if (!pgw || !pgw->pPaintProc || iLine == pgw->iScrollPos) return FALSE;
	if (iLine > pgw->iScrollPos)
		GnuScrollWin (pgw, iLine - pgw->iScrollPos, TRUE);
	else
		GnuScrollWin (pgw, pgw->iScrollPos - iLine, FALSE);
	return TRUE;
	}



/*
 * scrolls iLines up or down,
 * any window type
 * if list window, changes iScrollPos.
 * calls PaintItem  once for each item space brought 
 * into view area
 */
void GnuScrollWin (PGW pgw, INT iLines, BOOL bUp)
	{
	INT iOldScrollPos, iScrollMax, iDisplayItems, i;
	CHAR_INFO cell;

	if (!pgw || !iLines) return;

	cell = MKCELL (' ', GnuGetAtt (pgw, 0));

	iOldScrollPos = pgw->iScrollPos;
	iScrollMax    = ScrollMax (pgw);
	iDisplayItems = DisplayItems (pgw);

	if (bUp)
		{
		if (pgw->pPaintProc)
			{
			iLines = min (iScrollMax, pgw->iScrollPos + iLines) - iOldScrollPos;
			pgw->iScrollPos = iOldScrollPos + iLines;
			}
		ScrScroll (ClientRect(pgw), MKCOORD (0, -iLines * pgw->iItemSize), cell);
		
		/*--- paint uncovered space ---*/
		for (i = min (iLines, iDisplayItems); i; i--)
			GnuPaintWin (pgw, pgw->iScrollPos + iDisplayItems - i);
		}
	else
		{
		if (pgw->pPaintProc)
			{
			iLines = min (iOldScrollPos, iLines);
			pgw->iScrollPos = iOldScrollPos - iLines;
			}
		ScrScroll (ClientRect(pgw), MKCOORD (0, iLines * pgw->iItemSize), cell);

		/*--- paint uncovered space ---*/
		for (i = min (iLines, iDisplayItems); i; i--)
			GnuPaintWin (pgw, pgw->iScrollPos + i - 1);
		}
	if (pgw->bShowScrollPos)
		{
		gnuShowScrollPos (pgw, iOldScrollPos,   FALSE);
		gnuShowScrollPos (pgw, pgw->iScrollPos, TRUE);
		}
	}



/*
 * If the number of elements in the list suddenly decreases
 * and you are scrolled more than is possible, this fixes
 * the problem
 *
 * if bShowScrollPos=TRUE
 * call gnuShowScrollPos (pgw, pgw->iScrollPos, FALSE)
 * before resetting the data and valling this proc
 *
 */
BOOL gnuFixScroll (PGW pgw)
	{
	INT  iNewScrollPos;
	BOOL bTmp;

	if (!pgw->iScrollPos || pgw->iScrollPos + CYSize(pgw) <= pgw->iItemCount)
		return FALSE;

	iNewScrollPos = (pgw->iItemCount > CYSize(pgw) ? pgw->iItemCount - CYSize(pgw) : 0);
	bTmp = pgw->bShowScrollPos;
	pgw->bShowScrollPos = FALSE;
	GnuScrollTo  (pgw, iNewScrollPos);
	pgw->bShowScrollPos = bTmp;

	if (pgw->bShowScrollPos)
		gnuShowScrollPos (pgw, pgw->iScrollPos, TRUE);

	return TRUE;
	}

