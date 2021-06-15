/*
 * Win0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides base Gnu Window functions for the
 * screen module
 */

#include <stdlib.h>
#include <string.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"
#include "gnuwin.h"


#define MAXLINE 256
#define BORDERCHRS  "É»È¼ÍÍºº×"

static  BOOL PAINTONCREATE  = TRUE;
static  CHAR szBORDERCHARS[10]    = BORDERCHRS;


/***************************************************************************/
/*                                                                         */
/* Internal Functions                                                      */
/*                                                                         */
/***************************************************************************/

/*
 * malloc buff, save screen data at window area.
 */
PCHAR_INFO gnuGetScreenDat (PGW pgw)
	{
	PCHAR_INFO  pcBuff;
	COORD       dwSize;

//   pmet = scrCheckGetMetrics ();
//   rect.Right  = min (rect.Right,  pmet->dwSize.X-1);
//   rect.Bottom = min (rect.Bottom, pmet->dwSize.Y-1);
//
//
	dwSize = MKCOORD (XSize(pgw)+1, YSize(pgw)+1); // add border area
	pcBuff = (PCHAR_INFO) calloc (dwSize.X * dwSize.Y, sizeof (CHAR_INFO));

	ScrReadBlock (pcBuff, dwSize, AdjustRect (WinRect(pgw), 0, 0, 1, 1));
	return pcBuff;
	}



/*
 * returns true if it displays anything 
 * displays screen data
 */
BOOL gnuPutScreenDat (PGW pgw, BOOL bIncludeShadowArea)
	{
	COORD       dwSize;
	SMALL_RECT  rect;
	PMET        pmet;

	if (!pgw || !pgw->pcScreenDat)
		return FALSE;

	dwSize = MKCOORD (XSize(pgw)+1, YSize(pgw)+1); // add border area

	rect = WinRect(pgw);
	if (bIncludeShadowArea) 
		rect = AdjustRect (rect, 0, 0, 1, 1);

	pmet = scrCheckGetMetrics ();
	rect.Right  = min (rect.Right,  pmet->dwSize.X-1);
	rect.Bottom = min (rect.Bottom, pmet->dwSize.Y-1);

	ScrWriteBlock (pgw->pcScreenDat, dwSize, rect);
	return TRUE;
	}


void gnuFreeScreenDat (PGW pgw)
	{
	if (!pgw)
		return;
	if (pgw->pcScreenDat)
		free (pgw->pcScreenDat);
	pgw->pcScreenDat = NULL;
	}


/*
 * if writing scrn dat and saving window, set bIncludeShadowArea=TRUE
 * if writing window and saving scrn dat, set bIncludeShadowArea=FALSE
 */
void gnuSwapScreenDat (PGW pgw, BOOL bIncludeShadowArea)
	{
	PCHAR_INFO pcTmp;

	if (!pgw) return;
	pcTmp = gnuGetScreenDat (pgw);
	if (gnuPutScreenDat (pgw, bIncludeShadowArea))
		gnuFreeScreenDat (pgw);
	pgw->pcScreenDat = pcTmp;
	}


/*
 *
 */
COORD gnuWinToScreen (PGW pgw, COORD dwPos)
	{
	if (!pgw) return dwPos;

	dwPos.X += LeftOf(pgw);
	dwPos.Y += TopOf (pgw);
	dwPos.X = max (0, dwPos.X);
	dwPos.Y = max (0, dwPos.Y);
	return dwPos;
	}


/*
 *
 */
COORD gnuScreenToWin (PGW pgw, COORD dwPos)
	{
	if (!pgw) return dwPos;

	dwPos.X -= LeftOf(pgw);
	dwPos.Y -= TopOf (pgw);
	return dwPos;
	}


/*
 *
 */
COORD gnuClientWinToScreen (PGW pgw, COORD dwPos)
	{
	if (!pgw) return dwPos;

	dwPos.X += CLeftOf(pgw);
	dwPos.Y += CTopOf (pgw);
	dwPos.X = max (0, dwPos.X);
	dwPos.Y = max (0, dwPos.Y);
	return dwPos;
	}


/*
 *
 */
COORD gnuScreenToClientWin (PGW pgw, COORD dwPos)
	{
	if (!pgw) return dwPos;

	dwPos.X -= CLeftOf(pgw);
	dwPos.Y -= CTopOf (pgw);
	return dwPos;
	}



/*
 *
 */
void gnuShowScrollPos (PGW pgw, INT iNewLine, BOOL bShow)
	{
	INT iNewPos;

	if (!pgw || !ScrollMax(pgw) || !pgw->bBorder)
		return;

	if (!ScrollMax(pgw))
		iNewPos = CYSize(pgw)-1;
	else
		iNewPos = (INT) ((LONG)(CYSize(pgw)-1) * 
						(LONG)iNewLine / (LONG)ScrollMax(pgw));

	ScrWriteStr (pgw->pszBorder + (bShow ? 8 : 7),
					 MKCOORD (RightOf (pgw), CTopOf(pgw)+iNewPos),
					 1);
	}


/*
 * Write Shadow Area around Border
 */
void gnuDrawShadow (PGW pgw)
	{
	BYTE cAtt;
	INT  x, y;

	if (!pgw || !pgw->bShadow) return;

	cAtt = GnuGetAtt (pgw, 0x0F);

	/*--- Paint Bar under window ---*/
	ScrWriteNAtt (cAtt, MKCOORD (LeftOf(pgw)+1, BottomOf(pgw)+1), XSize(pgw));

	x = RightOf(pgw)+1;
	for (y=TopOf(pgw); y<=BottomOf(pgw); y++)
		ScrWriteNAtt (cAtt, MKCOORD (x, y+1), 1);
	}   


/*
 *
 */
INT gnuDefPaintProc (PGW pgw, INT iIndex, INT iLine)
	{
	PSZ  *ppszStr;
	INT  iLen;
	WORD wAtt;

	if (!pgw->pUser1 || iIndex >= pgw->iItemCount)
		return 0;

	wAtt = (pgw->iSelection == iIndex ? 2 : 0);
	ppszStr = (PSZ*) pgw->pUser1;
	iLen = GnuPaint2 (pgw, iLine, 0, 0, wAtt, 
							ppszStr[iIndex], CXSize(pgw));

	/*--- blank off rest of line ---*/
	if ((int)iLen < CXSize(pgw))
		GnuPaintNChar (pgw, iLine, iLen, 0, wAtt, ' ', CXSize(pgw) - iLen);
	return 1;
	}


PSZ gnuGetBorderChars (void)
	{
	return szBORDERCHARS;
	}

/*====================================================================*/
/*=                                                                  =*/
/*=                 gnu window functions (external)                  =*/ 
/*=                                                                  =*/
/*====================================================================*/



/*
 * creates a window with the given size. the pointer is to a proc to
 * paint each line if a list window, or NULL for a static window
 *
 * There are special values you can give for uYSize, and uXSize:
 * if 1000 <= uYSize <= 1100, the value over 1000 is subtracted from the
 * screen size, and that value is used for uYSize.  The same goes for uXSize.
 * For example: uXSize=1008 is equivalent to uXSize=72
 *
 * Size left as separate params for historical reasons
 */
PGW GnuCreateWin (INT iYSize,
						INT iXSize,
						PAINTPROC pPaintProc)
	{
	return GnuCreateWin2 (MKCOORD (-1, -1), MKCOORD(iXSize, iYSize), pPaintProc);
	}



/* 
 * same as above but creates at a specific place
 * if dwPos.Y == -1, Window is vertically centered
 * if dwPos.X == -1, Window is horizontally centered
 * if dwSize.Y > 1000, Window size is relative to screen size - (uYSize-1000)
 * if dwSize.X > 1000, Window size is relative to screen size - (uXSize-1000)
 */
PGW GnuCreateWin2 (COORD dwPos, COORD dwSize, PAINTPROC pPaintProc)
	{
	PGW  pgw;
	PMET pmet;

	pmet = scrCheckGetMetrics ();
	pgw  = (PGW) malloc (sizeof (GW));

	if (dwSize.Y >= 1000 && dwSize.Y <= 1000 + pmet->dwSize.Y)
		dwSize.Y = 1000 + pmet->dwSize.Y - dwSize.Y;
	if (dwSize.X >= 1000 && dwSize.X <= 1000 + pmet->dwSize.X)
		dwSize.X = 1000 + pmet->dwSize.X - dwSize.X;

	if (dwPos.X == -1)
		dwPos.X = (pmet->dwSize.X - dwSize.X) / 2;
	if (dwPos.Y == -1)
		dwPos.Y = (pmet->dwSize.Y - dwSize.Y) / 2;

	pgw->dwSize.X    = min (pmet->dwSize.X, dwSize.X);
	pgw->dwSize.Y    = min (pmet->dwSize.Y, dwSize.Y);
	pgw->dwPos.X     = min (dwPos.X, pmet->dwSize.X - dwSize.X);
	pgw->dwPos.Y     = min (dwPos.Y, pmet->dwSize.Y - dwSize.Y);
	pgw->dwClientSize= MKCOORD (pgw->dwSize.X - 2, pgw->dwSize.Y - 2);
	pgw->dwClientPos = MKCOORD (pgw->dwPos.X + 1, pgw->dwPos.Y + 1);

	pgw->bBorder     = TRUE;
	pgw->bShadow     = TRUE;
	pgw->pcScreenDat = gnuGetScreenDat (pgw);
	pgw->pszHeader   = NULL;
	pgw->pszFooter   = NULL;
	pgw->pszCMap     = strndup(_gnuGetMap (0), 9);
	pgw->pszBorder   = strdup(szBORDERCHARS);
	pgw->pUser1      = NULL;
	pgw->pUser2      = NULL;
	pgw->pUser3      = NULL;

	pgw->iScrollPos  = 0;
	pgw->iSelection  = (INT)-1;
	pgw->iItemCount  = 0;
	pgw->iItemSize   = 1;

	pgw->pPaintProc  = (pPaintProc ? pPaintProc : gnuDefPaintProc);

	pgw->bShowScrollPos = FALSE;
	if (PAINTONCREATE)
		GnuClearWin (pgw, 0, TRUE);
	return pgw;
	}




/* 
 * frees associated memory, and restores previous contents of mem
 */
PGW GnuDestroyWin (PGW pgw)
	{
	if (!pgw) return NULL;
	gnuPutScreenDat (pgw, TRUE);
	gnuFreeScreenDat (pgw);
	free (pgw->pszCMap);
	free (pgw->pszBorder);
	free (pgw);
	return NULL;
	}





/* clears the window area, and repaints the border
 * if bIncludeBorder
 */
void GnuClearWin (PGW pgw, WORD wAtt, BOOL bIncludeBorder)
	{
	CHAR_INFO cell;

	cell = MKCELL (' ', GnuGetAtt (pgw, wAtt));

	if (!pgw) /*--- Entire Screen ---*/
		{   
		ScrClear (MKRECT (0,0,-1,-1), cell);
		if (bIncludeBorder)
			GnuPaintBorder (NULL);
		return;
		}

	if (!bIncludeBorder) /*--- Clear Client Area ---*/
		{
		ScrClear (ClientRect(pgw), cell);
		return;
		}

	/*--- Clear Window Area ---*/
	ScrClear (WinRect(pgw), cell);

	if (pgw->bBorder)
		GnuPaintBorder (pgw);

	if (pgw->bShadow)
		gnuDrawShadow (pgw);

	if (pgw->bShowScrollPos)
		gnuShowScrollPos (pgw, pgw->iScrollPos, TRUE);
	}



/*
 * moves the cursor
 */
void GnuMoveCursor (PGW pgw, COORD dwPos)
	{
	ScrSetCursorPos (gnuClientWinToScreen (pgw, dwPos));
	}


/*
 * gets the cursor
 *
 */
COORD GnuGetCursor (PGW pgw)
	{
	return gnuScreenToClientWin (pgw, ScrGetCursorPos ());
	}




void GnuPaintAtCreate (BOOL bDraw)
	{
	PAINTONCREATE = bDraw;
	}


/*
 * moves the window to the new location
 * returns TRUE if successful
 */
BOOL GnuMoveWin (PGW pgw, INT iYPos, INT iXPos)
	{
	PMET pmet;

	pmet = scrCheckGetMetrics ();
	if (!pgw || iXPos >= pmet->dwSize.X - XSize(pgw) ||
					iYPos >= pmet->dwSize.Y - YSize(pgw))
		return FALSE;

	gnuSwapScreenDat (pgw, TRUE);
	CXSize(pgw) += iXPos - LeftOf (pgw);
	CYSize(pgw) += iYPos - TopOf (pgw);
	LeftOf(pgw) = iXPos;
	TopOf(pgw)  = iYPos;
	gnuSwapScreenDat (pgw, FALSE);
	if (pgw->bShadow)
		gnuDrawShadow (pgw);
	return TRUE;
	}


