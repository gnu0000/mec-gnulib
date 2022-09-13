/*
 * grd0.c
 *
 * Craig Fitzgerald  1996
 *
 * Part of the GnuLib library
 *
 * 
 * This module creates grid windows.
 * This module rides on top of the base functionality of Gnu Windows.
 * Gnu Windows provide for line oriented vertical scrolling.
 * This module adds horizontal scrolling, cell selection, and key handling.
 * This is a minimal implementation and is not very efficient.  For example
 * when the user moves the selection up or down 1 line, the 2 full lines
 * are repainted rather than just the 2 cells.  This is because the code
 * here relies on gnuwin functionality wherever possible.
 *
 ****************************************************************************
 *
 * PGW GrdCreateWin (iYSize, iXSize, pfnPaint)
 * PGW GrdCreateWin2 (uYPos, iXPos, iYSize, iXSize, pfnPaintCell)
 * PGW GrdDestroyWin (pgw)
 * INT GrdResizeCol (pgw, iCol, iColSize)
 * PGRINFO GrdGetPGR (pgw)
 *
 * INT GrdDoKeys (pgw, c)
 * BOOL GrdSelectCell (pgw, iNewY, iNewX, bOnScreen)
 * void GrdPaintCell (pgw, uY, uX, bMakeVisible)
 * void GrdGetPos (pgw, ppos)
 * INT GrdMakeVisible (pgw, uY, uX)
 * INT GrdRefreshLabels (pgw)
 *
 * INT pfnDefaultPaintCell (pgw, ppos, bLabel)
 * INT GrdPaintColumns (pgw, iStart, iEnd)
 * INT GrdPaintLabels  (pgw, iStart, iEnd)
 *
 ****************************************************************************
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuwin.h"
#include "gnuscr.h"
#include "gnugrid.h"
#include "gnukbd.h"


extern "C"
{


#define GRD_MAXCOLS  256

#define CXSSize(pgw)   (CXSize(pgw)  - ((PGRINFO)pgw->pUser3)->iLeftEdge)
#define CXSPos(pgw)    (CLeftOf(pgw) + ((PGRINFO)pgw->pUser3)->iLeftEdge)


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


/*
 *
 *
 */
PGRINFO GrdGetPGR (PGW pgw)
	{
	return (PGRINFO) (pgw ? pgw->pUser3 : NULL);
	}


INT ColXWin (PGW pgw, INT iCol)
	{
	PGRINFO pgr;
	INT     i, iXWin;

	if (!(pgr = GrdGetPGR (pgw)))
		return 0;

	if (iCol < pgr->iFrozen)
		{
		for (iXWin = i = 0; i < iCol; i++)
			iXWin += pgr->pcSize[i];
		}
	else if (pgr->iScrollCol > iCol)
		{
		return -100;
		}
	else
		{
		for (iXWin = pgr->iLeftEdge, i = pgr->iScrollCol; i < iCol; i++)
			iXWin += pgr->pcSize[i];
		}
	return iXWin;
	}




static INT grdMaxScroll (PGW pgw)
	{
	PGRINFO pgr;
	INT     i, iXSize;

	if (!(pgr = GrdGetPGR (pgw)))
		return 0;

	/*--- determine max scroll col ---*/
	iXSize = pgr->iLeftEdge;
	for (i=pgr->iColCount; i && (iXSize <= CXSize(pgw)); i--)
		iXSize += pgr->pcSize[i-1];

	return min (i+1, pgr->iColCount - 1);
	}


static INT grdCalcMetrics (PGW pgw)
	{
	PGRINFO pgr;
	INT    i;

	if (!(pgr = GrdGetPGR (pgw)))
		return 0;

	/*--- calculate 1st col that is part of scroll area ---*/
	for (pgr->iLeftEdge=i=0; i<pgr->iFrozen &&  pgr->iLeftEdge <= CXSize(pgw); i++)
		pgr->iLeftEdge += pgr->pcSize[i];
	pgr->iLeftEdge = min (pgr->iLeftEdge, CXSize (pgw));

	return pgr->iLeftEdge;
	}


INT pfnDefaultAtt (PGW pgw, PPOS ppos, INT uLabel)
	{
	PGRINFO pgr;

	if (!(pgr = GrdGetPGR (pgw)))
		return 0;

	if (uLabel)
		return (ppos->wAtt = pgr->wLabelAtt);

	if (ppos->dwData.Y == pgw->iSelection && 
		 (pgr->iSelCol == -1 || pgr->iSelCol == ppos->dwData.X))
		 return ppos->wAtt = pgr->wSelectAtt;
	return ppos->wAtt = pgr->wNormalAtt;
	}

/*
 *
 *
 *
 *
 */
PSZ pfnDefaultText (PGW pgw, PPOS ppos, INT uLabel)
	{
	PGRINFO pgr;
	PSZ     *ppsz, **pppsz;

	if (!(pgr = GrdGetPGR (pgw)))
		return NULL;

	if (uLabel == 1)
		return (ppos->dwData.X >= pgr->iColCount ? NULL : pgr->ppszLabels[ppos->dwData.X]);
	if (uLabel == 2)
		return (PSZ)-1;
	if (uLabel)
		return NULL;

	if ((pppsz = (PPSZ *) pgw->pUser1)  &&  // ptr to ptr array
		 (ppsz  = pppsz[ppos->dwData.Y] ))     // ptr to ppsz or struct
		return ppsz[ppos->dwData.X];           // ptr to string element

	return NULL;
	}


/*
 * This fn is needed to overcome a problem with the GnuPaint2 API.
 * When the field justification is 1 (right justify) the xpos refers
 * to the right edge of the string. If the field also needs to be 
 * clipped, there is no way to specify the field because clipping
 * is always done from the left edge of the field and in this case
 * only the right edge is defined.  We know the right edge if we
 * also have the field length, but it is not a param to GnuPaint2
 */
INT CellPaint (PGW pgw, PPOS ppos, INT iDeltaY, PSZ pszText)
	{
	INT iDeltaX = 0;

	if (ppos->iJust == 1 || ppos->iJust == 2)
		{
		iDeltaX = ppos->iClip/ppos->iJust;
		if (ppos->iClip < ppos->iLen)
			{
			CHAR szBuff[128];
			INT iLen;

			iLen = min (127, max (0, strlen (pszText) - (ppos->iLen - ppos->iClip)));
			strncpy (szBuff, pszText, iLen);
			szBuff[iLen] = '\0';

			return GnuPaint2 (pgw, ppos->dwWin.Y+iDeltaY, ppos->dwWin.X+iDeltaX,
									ppos->iJust, ppos->wAtt, szBuff,
									(ppos->iClip>1 ? ppos->iClip -1 : 1));
			}
		}
	return GnuPaint2 (pgw, ppos->dwWin.Y+iDeltaY, ppos->dwWin.X+iDeltaX,
									ppos->iJust, ppos->wAtt, pszText,
									(ppos->iClip>1 ? ppos->iClip -1 : 1));
	}


/*
 * This is the default paint procedure for a grid
 * If the paint fn is passed as NULL during the create call, this is used.
 *
 * This paint proc assumes:
 * the pgw->uLineCount and pgr->iColCount are valid
 *
 * the pUser1 points to an array of pointers
 * These pointers point to either a PPSZ or a structure with  
 * PSZ's as its first iCol elements
 *
 * if you want this fn to paint labels, you should first:
 * 1> resize client area to make room at to for the labels
 * 2> set pgr->ppszLabels to a PPSZ array of label strings
 */
INT pfnDefaultPaintCell (PGW pgw, PPOS ppos, BOOL bLabel)
	{
	PGRINFO pgr;
	INT     i;
	PSZ     psz;

	if (!(pgr = GrdGetPGR (pgw)))
		return 1;

	if (bLabel)
		{
		for (i=0; i < pgr->iLabelSize; i++)
			{
			if (pgr->pfnAtt)
				pgr->pfnAtt (pgw, ppos, i+1);
			psz = (pgr->pfnText ? pgr->pfnText (pgw, ppos, i+1) : NULL);
			GnuPaintNChar (pgw, pgr->iLabelPos + i, 
								ppos->dwWin.X, 0, pgr->wLabelAtt, ' ', ppos->iClip);

			if (psz == (PSZ)-1)
				GnuPaintNChar (pgw, pgr->iLabelPos+1, ppos->dwWin.X, 0, pgr->wLabelAtt, 'Ä', ppos->iClip);
			else
				CellPaint (pgw, ppos, i, psz);
			}
		return 1;
		}
	if (pgr->pfnAtt)
		pgr->pfnAtt (pgw, ppos, 0);

	/*--- If outside range, cleanup screen area ---*/
	if (ppos->dwData.Y >= pgw->iItemCount || ppos->dwData.X >= pgr->iColCount)
		{
		GnuPaintNChar (pgw, ppos->dwWin.Y, ppos->dwWin.X, 0, ppos->wAtt, ' ', ppos->iClip);
		return 1;
		}

	psz = NULL;
	if ((ppos->dwData.Y < pgw->iItemCount) &&  // Y in range
		 (ppos->dwData.X < pgr->iColCount ) &&  // X in range
		 (pgr->pfnText))

	if (pgr->pfnText)
		psz = pgr->pfnText (pgw, ppos, 0);

// uSize = (ppos->iClip>1 ? ppos->iClip -1 : 1);
	GnuPaintNChar (pgw, ppos->dwWin.Y, ppos->dwWin.X, 0, ppos->wAtt, ' ', ppos->iClip);
	if (psz && *psz)
		CellPaint (pgw, ppos, 0, psz);
	return 1;
	}






///*
// * This is the default paint procedure for a grid
// * If the paint fn is passed as NULL during the create call, this is used.
// *
// * This paint proc assumes:
// * the pgw->uLineCount and pgr->iColCount are valid
// *
// * the pUser1 points to an array of pointers
// * These pointers point to either a PPSZ or a structure with  
// * PSZ's as its first iCol elements
// *
// * if you want this fn to paint labels, you should first:
// * 1> resize client area to make room at to for the labels
// * 2> set pgr->ppszLabels to a PPSZ array of label strings
// */
//INT pfnDefaultPaintCell (PGW pgw, PPOS ppos, BOOL bLabel)
//   {
//   PGRINFO pgr;
//   INT     uAtt, uSize;
//   PSZ     psz, *ppsz, **pppsz;
//
//   if (!(pgr = GrdGetPGR (pgw)))
//      return 1;
//
//   if (bLabel)
//      {
//      if (!pgr->ppszLabels || !pgr->iLabelSize)
//         return 0; // default to no labels
//      psz = (ppos->dwData.X >= pgr->iColCount ? NULL : pgr->ppszLabels[ppos->dwData.X]);
//      GnuPaintNChar (pgw, pgr->iLabelPos, ppos->dwWin.X, 0, pgr->wLabelAtt, ' ', ppos->iClip);
//      GnuPaint2 (pgw, pgr->iLabelPos, ppos->dwWin.X, 0, pgr->wLabelAtt, psz, ppos->iClip);
//
//      if (pgr->iLabelSize == 2)
//         GnuPaintNChar (pgw, pgr->iLabelPos+1, ppos->dwWin.X, 0, pgr->wLabelAtt, 'Ä', ppos->iClip);
//      return 1;
//      }
//
//   uAtt = (ppos->dwData.Y == pgw->iSelection && 
//         (pgr->iSelCol == -1 || pgr->iSelCol == ppos->dwData.X) ? 2 : 0);
//
//   /*--- If outside range, cleanup screen area ---*/
//   if (ppos->dwData.Y >= pgw->iItemCount || ppos->dwData.X >= pgr->iColCount)
//      {
//      GnuPaintNChar (pgw, ppos->dwWin.Y, ppos->dwWin.X, 0, uAtt, ' ', ppos->iClip);
//      return 1;
//      }
//
//   psz = NULL;
//   if ((ppos->dwData.Y < pgw->iItemCount) &&  // Y in range
//       (ppos->dwData.X < pgr->iColCount ) &&  // X in range
//       (pppsz = (PPSZ *) pgw->pUser1  ) &&  // ptr to ptr array
//       (ppsz  = pppsz[ppos->dwData.Y]   ))    // ptr to ppsz or struct
//      psz = ppsz[ppos->dwData.X];             // ptr to string element
//
//   uSize = (ppos->iClip>1 ? ppos->iClip -1 : 1);
//   GnuPaintNChar (pgw, ppos->dwWin.Y, ppos->dwWin.X, 0, uAtt, ' ', ppos->iClip);
//   if (psz && *psz)
//      GnuPaint2 (pgw, ppos->dwWin.Y, ppos->dwWin.X, 0, uAtt, psz, uSize);
//   return 1;
//   }


/************************************************************************/

/*
 * this has to handle frozen labels
 *
 */
INT GrdPaintLabels (PGW pgw, INT iStart, INT iEnd)
	{
	PGRINFO pgr;
	POS     pos;
	INT     i, iCol, iColSize, iXWin;

	if (!(pgr = GrdGetPGR (pgw)) || (iStart > iEnd))
		return 0;

	iStart = max (0, iStart);
	iEnd   = max (0, min (iEnd, CXSize(pgw)-1));

	for (i=0; i<pgr->iColCount; i++)
		{
		iCol = (i < pgr->iFrozen ? i : i - pgr->iFrozen + pgr->iScrollCol);

		iXWin = ColXWin (pgw, iCol);
		iColSize = (iCol < pgr->iColCount ? pgr->pcSize[iCol] : CXSize(pgw)-iXWin);
		if (iXWin < 0 || iXWin + iColSize <= iStart)
			continue;
		if (iXWin > iEnd)
			break;

		pos.dwWin.Y  = pgr->iLabelPos;              // invarient only of user
		pos.dwData.Y = -1;                    // fn doesnt change it
		pos.wAtt     = pgr->wLabelAtt;              //
		pos.iJust    = 0;                           //
		pos.dwData.X = iCol;
		pos.dwWin.X  = iXWin;
		pos.iLen     = iColSize;
		pos.iClip    = min (pos.iLen,  CXSize(pgw) - iXWin);
		pgr->pfnPaint (pgw, &pos, TRUE);
		}
	return 0;
	}


/*
 *
 *
 */
INT GrdRefreshLabels (PGW pgw)
	{
	return GrdPaintLabels (pgw, 0, CXSize(pgw)-1); // paint em all
	}


/*
 *
 *
 */
INT GrdHorizMove (PGW pgw, INT iCols)
	{
	PGRINFO    pgr;
	SMALL_RECT rect;
	CHAR_INFO  cell;
	INT        i, iDelta;

	if (!(pgr = GrdGetPGR (pgw)) || !pgw->iItemCount)
		return 0;

	if (iCols < 0) 
		iCols = max (iCols, pgr->iFrozen - pgr->iScrollCol);
	if (!iCols) 
		return 0;

	cell = MKCELL (' ', 0);

	if (iCols > 0) // scroll left
		{
		/*--- calc scroll ammt in chars ---*/
		for (iDelta=i=0; i<iCols; i++)
			iDelta += pgr->pcSize[pgr->iScrollCol+i];

		if (iDelta < CXSSize(pgw))
			{
			/*--- scroll client area ---*/
			rect = AdjustRect (ClientRect(pgw), pgr->iLeftEdge, 0, 0, 0);
			ScrScroll (rect, MKCOORD (-iDelta, 0), cell);

			/*--- scroll label area ---*/
			if (pgr->iLabelSize)
				{
				rect = MKRECT (CXSPos(pgw), LTopOf(pgw), CRightOf(pgw), LBottomOf(pgw));
				ScrScroll (rect, MKCOORD (-iDelta, 0), cell);
				}
			}
		pgr->iScrollCol += iCols;
		GrdPaintColumns (pgw, CXSize(pgw)-iDelta, CXSize(pgw)-1);
		GrdPaintLabels  (pgw, CXSize(pgw)-iDelta, CXSize(pgw)-1);
		}
	else // (iCols < 0) scroll right
		{
		/*--- calc scroll ammt in chars ---*/
		for (iDelta=i=0; i > iCols; i--)
			iDelta += pgr->pcSize[pgr->iScrollCol+i-1];

		if (iDelta < CXSSize(pgw))
			{
			/*--- scroll client area ---*/
			rect = AdjustRect (ClientRect(pgw), pgr->iLeftEdge, 0, 0, 0);
			ScrScroll (rect, MKCOORD (iDelta, 0), cell);

			/*--- scroll label area ---*/
			if (pgr->iLabelSize)
				{
				rect = MKRECT (CXSPos(pgw), LTopOf(pgw), CRightOf(pgw), LBottomOf(pgw));
				ScrScroll (rect, MKCOORD (iDelta, 0), cell);
				}
			}
		pgr->iScrollCol += iCols;
		GrdPaintColumns (pgw, pgr->iLeftEdge, pgr->iLeftEdge + iDelta);
		GrdPaintLabels  (pgw, pgr->iLeftEdge, pgr->iLeftEdge + iDelta);
		}
	return 0;
	}


INT GrdScrollTo (PGW pgw, INT iCol)
	{
	PGRINFO pgr;

	if (!(pgr = GrdGetPGR (pgw)) || !pgw->iItemCount)
		return 0;

	return GrdHorizMove (pgw, iCol - pgr->iScrollCol);
	}


/*
 * makes a cell visible
 *
 */
INT GrdMakeVisible (PGW pgw, INT uY, INT iX)
	{
	PGRINFO pgr;
	INT    i, iXWin;

	if (!(pgr = GrdGetPGR (pgw)) || !pgw->iItemCount)
		return FALSE;

	GnuMakeVisible (pgw, uY);  // gnulib handles vert scroll

	if (iX == pgr->iScrollCol || iX < pgr->iFrozen)
		return 0;

	if (iX < pgr->iScrollCol && pgr->iScrollCol > pgr->iFrozen)
		return GrdHorizMove (pgw, max (iX, pgr->iFrozen) - pgr->iScrollCol);

	for (iXWin=pgr->iLeftEdge, i=iX+1; i > pgr->iScrollCol; i--)
		{
		iXWin += pgr->pcSize[i-1];
		if (iXWin > CXSize(pgw))
			return GrdHorizMove (pgw, min (i, iX) - pgr->iScrollCol); //GNU
		}
	return 0;
	}


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


/*
 * gets the current data / screen coordinates
 *
 */
void GrdGetPos (PGW pgw, PPOS ppos)
	{
	PGRINFO pgr;
	INT    iXWin;

	if (!(pgr = GrdGetPGR (pgw)) || !pgw->iItemCount)
		return;

	ppos->dwData.Y = pgw->iSelection;
	ppos->dwData.X = pgr->iSelCol;
	ppos->dwWin.Y  = (ppos->dwData.Y - pgw->iScrollPos) * pgw->iItemSize;
	ppos->iLen   = pgr->pcSize[ppos->dwData.X];

	iXWin = ColXWin (pgw, pgr->iSelCol);
	ppos->dwWin.X  = iXWin;
	ppos->iClip  = min (ppos->iLen,  CXSize(pgw) - iXWin);
	}


/*
 * this is the fn called from the GnuWin module to paint a line
 * so here we paint each col in the line that is visible
 */
INT pfnPaintGrid (PGW pgw, INT uYData, INT uYWin)
	{
	PGRINFO pgr;
	INT    i, iCol;
	INT     iXWin;
	POS     pos;

	if (!(pgr = GrdGetPGR (pgw)))
		return 1;
	
	for (i=0; i<pgr->iColCount; i++)
		{
		iCol = (i < pgr->iFrozen ? i : pgr->iScrollCol + i - pgr->iFrozen);
		iXWin = ColXWin (pgw, iCol);

		pos.dwData.Y = uYData;          // invarient only if user fn 
		pos.dwWin.Y  = uYWin;           // doesn change these!
		pos.wAtt     = pgr->wNormalAtt; //
		pos.iJust    = 0;               //

		pos.dwData.X = iCol;
		pos.dwWin.X  = iXWin;
		pos.iLen   = pgr->pcSize[iCol];
		pos.iClip  = min (pos.iLen,  CXSize(pgw) - iXWin);

		pgr->pfnPaint (pgw, &pos, FALSE);

		if (iXWin + pos.iLen >= CXSize(pgw))
			break;
		}
	iXWin += pos.iLen;
	if (iXWin < CXSize(pgw))
		GnuPaintNChar (pgw, uYWin, iXWin, 0, 0, ' ', CXSize(pgw) - iXWin);

	return 1;
	}


/*
 * Call to paint a cell
 * if bOnScreen is TRUE then the screen is scrolled to make it visible
 */
void GrdPaintCell (PGW pgw, INT uY, INT iX, BOOL bMakeVisible)
	{
	PGRINFO pgr;
	POS     pos;
	INT    iXWin;

	if (!(pgr = GrdGetPGR (pgw)) || !pgw->iItemCount)
		return;

	if (bMakeVisible)
		GrdMakeVisible (pgw, uY, iX);

	iXWin = ColXWin (pgw, iX);

	pos.dwData.Y = uY;
	pos.dwData.X = iX;
	pos.dwWin.Y  = (uY - pgw->iScrollPos) * pgw->iItemSize;
	pos.dwWin.X  = iXWin;
	pos.iLen   = pgr->pcSize[iX];
	pos.iClip  = min (pos.iLen,  CXSize(pgw) - iXWin);

	pgr->pfnPaint (pgw, &pos, FALSE);
	}


/*
 * Call to select a cell
 * if bOnScreen is TRUE then the screen is scrolled to make it visible
 */
BOOL GrdSelectCell (PGW pgw, INT iNewY, INT iNewX, BOOL bOnScreen)
	{
	PGRINFO pgr;
	INT    uOldY, uOldX;

	if (!(pgr = GrdGetPGR (pgw)) || !pgw->iItemCount)
		return 0;

	pgr = (PGRINFO) pgw->pUser3;

	uOldX = pgr->iSelCol;
	uOldY = pgw->iSelection;

	pgr->iSelCol    = iNewX;
	pgw->iSelection = iNewY;

	GrdPaintCell (pgw, uOldY, uOldX, FALSE);
	GrdPaintCell (pgw, iNewY, iNewX, bOnScreen);
	return TRUE;
	}


/*
 * assume: never asked to paint a frozen column
 *
 *
 */
INT GrdPaintColumns (PGW pgw, INT iStart, INT iEnd)
	{
	PGRINFO pgr;
	POS     pos;
	INT    iXWin, uYWin, iCol, iColSize;

	if (!(pgr = GrdGetPGR (pgw)) || !pgw->iItemCount)
		return 0;

	iStart = max (pgr->iLeftEdge, iStart);
	iEnd   = max (0, min (iEnd, CXSize(pgw)-1));

	if (iStart > iEnd)
		return 0;

	for (iCol=pgr->iScrollCol, iXWin=pgr->iLeftEdge; iXWin < iEnd; iCol++)
		{
		iColSize = (iCol < pgr->iColCount ? pgr->pcSize[iCol] : CXSize(pgw)-iXWin+1);

		if (iXWin + iColSize > iStart) //CHANGED FROM >=
			{
			pos.dwData.X = iCol;
			pos.dwWin.X  = iXWin;
			pos.iLen   = iColSize;
			pos.iClip  = min (pos.iLen,  CXSize(pgw) - iXWin);

			for (uYWin = 0; uYWin < CYSize(pgw); uYWin++)
				{
				pos.dwData.Y = (pgw->iScrollPos + uYWin) * pgw->iItemSize;
				pos.dwWin.Y  = uYWin;
				pgr->pfnPaint (pgw, &pos, FALSE);
				}
			}
		iXWin += iColSize;
		}
	return 0;
	}

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


//INT GrdDoKeys (PGW pgw, INT c)
//   {
//   PGRINFO pgr;
//   INT     iDelta;
//   INT    uNewCol;
//
//   if (!(pgr = GrdGetPGR (pgw)) || !pgw->iItemCount)
//      return 0;
//
//   switch (c)
//      {
//      case 0x14b:                         /*------ left  arrow    ------*/
//         iDelta = -1;
//         break;
//
//      case 0x14d:                         /*------ right arrow    ------*/
//         iDelta = 1;
//         break;
//
//      case 0x174:                         /*------ ^right arrow   ------*/
//         iDelta = pgr->iColCount;
//         break;
//
//      case 0x173:                         /*------ ^Left arrow    ------*/
//         iDelta = - pgr->iColCount;
//         break;
//
//      default:
//         return GnuDoListKeys (pgw, c);;
//      }
//   uNewCol = max (0, min (pgr->iSelCol + iDelta, pgr->iColCount-1));
//   if (uNewCol == pgr->iSelCol)
//      return FALSE;
//
//   GrdSelectCell (pgw, pgw->iSelection, uNewCol, TRUE);
//   return TRUE;
//   }


/*
 * Default key Handling for grid windows.
 *
 * if pgr->iSelCol == -1 then selection is line oriented
 *   and <- -> keys always scroll
 *
 * if pgr->iSelCol == a col then selection is cell oriented
 *   and arrows move the cell selection
 *
 * This behavior is analagous to GnuDoListKeys
 */
INT GrdDoKeys (PGW pgw, INT c)
	{
	PGRINFO pgr;
	INT     iStart, iNew, iMin, iMax, iDelta;
	BOOL    bLine;

	if (!(pgr = GrdGetPGR (pgw)) || !pgw->iItemCount)
		return 0;

	bLine = (pgr->iSelCol == -1);

	iStart = (bLine ? pgr->iScrollCol : pgr->iSelCol);
	iMin   = (bLine ? pgr->iFrozen    : 0);
	iMax   = (bLine ? grdMaxScroll (pgw) : pgr->iColCount-1);

	switch (c)
		{
		case K_LEFT:                         /*------ left  arrow    ------*/
			iDelta = -1;
			break;

		case K_RIGHT:                        /*------ right arrow    ------*/
			iDelta = 1;
			break;

		case K_CRIGHT:                       /*------ ^right arrow   ------*/
			iDelta = iMax - iStart;
			break;

		case K_CLEFT:                        /*------ ^Left arrow    ------*/
			iDelta = iMin - iStart;
			break;

		default:
			return GnuDoListKeys (pgw, c);;
		}
	iNew = max (iMin, min (iMax, iStart + iDelta));
	if (iNew == iStart)
		return FALSE;

	if (bLine)
		GrdScrollTo (pgw, iNew);
	else
		GrdSelectCell (pgw, pgw->iSelection, iNew, TRUE);
	return TRUE;
	}

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * Resizes a grid column
 * Call this once for each column you want to resize
 * The default size is 16 chars
 * You must repaint the window after this
 */
INT GrdResizeCol (PGW pgw, INT iCol, INT iColSize)
	{
	PGRINFO pgr;

	if (!(pgr = GrdGetPGR (pgw)))
		return 0;

	pgr->pcSize[iCol] = (CHAR) iColSize;
	return grdCalcMetrics (pgw);
	}


/*
 * Call this fn to freeze the 1st iCols columns
 * You must repaint the window after this
 */
INT GrdFreezeCol (PGW pgw, INT iCols)
	{
	PGRINFO pgr;

	if (!(pgr = GrdGetPGR (pgw)))
		return 0;

	pgr->iFrozen    = min (iCols, pgr->iColCount);
	pgr->iScrollCol = pgr->iFrozen;
	grdCalcMetrics (pgw);
	return pgr->iFrozen;
	}



/*
 * Sets up labels for painting by the default paint proc
 *
 */
void GrdSetupLabels (PGW pgw, INT iPos, INT uSize, WORD wAtt, PPSZ ppszLabels)
	{
	PGRINFO pgr;

	if (!(pgr = GrdGetPGR (pgw)))
		return;

	pgr->iLabelSize     = uSize;
	pgr->wLabelAtt      = wAtt; 
	pgr->ppszLabels     = ppszLabels;
	pgw->dwClientPos.Y  = TopOf(pgw) + max (1, iPos + uSize);
	pgw->dwClientSize.Y = BottomOf(pgw) - CTopOf(pgw);
	pgr->iLabelPos      = -uSize;
	if (pgw->bBorder && TopOf(pgw) && (TopOf(pgw) == LTopOf(pgw)))
		pgr->iLabelPos -= 1;
	}



/*
 * Destroys a grid window
 *
 */
PGW GrdDestroyWin (PGW pgw)
	{
	PGRINFO pgr;

	if (!pgw)
		return FALSE;

	if (pgr = GrdGetPGR (pgw))
		{
		if (pgr->pcSize) free (pgr->pcSize);
		if (pgr->pcID  ) free (pgr->pcID  );
		free (pgr);
		}
	return GnuDestroyWin (pgw);
	}


/*
 *
 *
 */
static PGW grdFinishCreate (PGW pgw, PAINTCELL pfnPaint)
	{
	PGRINFO pgr;
	INT    i;

	pgw->iSelection     = 0;
	pgw->bShowScrollPos = TRUE;

	pgr = (PGRINFO) malloc (sizeof (GRINFO));
	pgr->iColCount = 0;
	pgr->iSelCol   = 0;
	pgr->iScrollCol= 0;
	pgr->iLabelPos = 1;
	pgr->iLabelSize= 0;
	pgr->iFrozen   = 0;
	pgr->iLeftEdge = 0;
	pgr->ppszLabels= NULL;
	pgr->pfnPaint  = (pfnPaint ? pfnPaint : pfnDefaultPaintCell);
	pgr->pfnText   = pfnDefaultText;
	pgr->pfnAtt    = pfnDefaultAtt;

	pgw->pUser3    = (PVOID) pgr;

	pgr->wLabelAtt  = 4;  // default attribute for label line
	pgr->wNormalAtt = 0;  // default attribute for cells
	pgr->wSelectAtt = 2;  // default attribute for selected cells

	pgr->pcSize = (PBYTE) calloc (GRD_MAXCOLS, sizeof (BYTE));
	pgr->pcID   = (PBYTE) calloc (GRD_MAXCOLS, sizeof (BYTE));

	for (i=0; i<GRD_MAXCOLS; i++)
		pgr->pcSize[i] = 16; // default column size
		
	GrdResizeCol (pgw, 0, 16);
	return pgw;
	}


/*
 * Creates a grid window
 * This is like GrdCreateWin but allows you to specify
 * the window position
 */
PGW GrdCreateWin2 (COORD dwPos, COORD dwSize, PAINTCELL pfnPaint)
	{
	PGW pgw;

	pgw = GnuCreateWin2 (dwPos, dwSize, pfnPaintGrid);
	return grdFinishCreate (pgw, pfnPaint);
	}


/*
 * Creates a grid window
 * The Gnulib window libraries only handle the rows
 * so we add the framework to handle columns
 */
PGW GrdCreateWin (INT iYSize, INT iXSize, PAINTCELL pfnPaint)
	{
	PGW pgw;

	pgw = GnuCreateWin (iYSize, iXSize, pfnPaintGrid);
	return grdFinishCreate (pgw, pfnPaint);
	}

}

