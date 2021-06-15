/*
 * Winf.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * Editable combobox
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
#include "gnukbd.h"
#include "gnumisc.h"


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/

///*
// * pops up a combobox
// * user selects a choice from a list or enters new value
// *  ppsz is the array of string choices (last entry must be NULL)
// *  iY,iX is the cell position, call with NULL for free combo
// *  iYSize is the size of the combo client, 0 = as large as needed/possible
// *  iStartSel is the initial selected line -1 = no start selection
// *
// *  return is string or NULL if user hits escape
// */
//PSZ GnuEditComboBox (PSZ pszDat, PPSZ ppsz, 
//                           INT iY, INT iX, INT iYSize)
//   {
//   PMET pmet;
//   PGW  pgw;
//   INT i, uMaxItemsOnScreen, c, iEntries;
//   INT iXSize, iYPos, iXPos; 
//   INT  iStartSel;
//   CHAR szTmp[256];
//
//   pmet = ScrGetMetrics ();
//   uMaxItemsOnScreen = pmet->dwSize.Y-2;
//
//   for (iEntries=0; ppsz && ppsz[iEntries]; iEntries++)
//      ;
//
//   strncpy (szTmp, pszDat, sizeof (szTmp));
//
//   iStartSel = (*pszDat ? -1 : 0);
//   for (i=0; ppsz && ppsz[i]; i++)
//      if (!strcmp (pszDat, ppsz[i]))
//         {
//         iStartSel = i;
//         break;
//         }
//
//   /*--- determine X Size ---*/
//   for (i=iXSize=0; i<iEntries; i++)
//      iXSize = max (iXSize, strlen (ppsz[i]));
//   iXSize = min (iXSize, 70);
//   iXSize += 3; // cvt client area size to window size
//
//   /*--- if size not spec'd determine a good Y size for window ---*/
//   if (!iYSize) 
//      iYSize = (iEntries > uMaxItemsOnScreen ? 12 : iEntries);
//   iYSize += 2; // cvt client area size to window size
//
//   /*--- determine a good place for the window ---*/
//   iXPos = iYPos = (INT)-1;
//   /*--- determine Y position ---*/
//   if (!iY || (iY+1 == pmet->dwSize.Y)) // start at edge
//      iYPos = (iY ? pmet->dwSize.Y-iYSize: 0);
//   else if (iY + iYSize - 2 < pmet->dwSize.Y)
//      iYPos = iY - 1;
//   else if (iY + 2 >= iYSize)
//      iYPos = iY + 2 - iYSize;
//   else
//      iYPos = pmet->dwSize.Y-iYSize;
//
//   /*--- determine X position ---*/
//   if (!iX)
//      iXPos = 0;
//   else if (iX + iXSize - 2 < pmet->dwSize.X)
//      iXPos = iX - 1;
//   else
//      iXPos = pmet->dwSize.X - iX;
//
//   ScrPushCursor (FALSE);
//   GnuPaintAtCreate (FALSE);
//   pgw = GnuCreateWin2 (iYPos, iXPos, iYSize, iXSize, NULL);
//   pgw->bShadow        = FALSE;
//   pgw->pUser1         = ppsz;
//   pgw->iItemCount     = iEntries;
//   pgw->bShowScrollPos = TRUE;
//   pgw->iSelection     = iStartSel;
//
//   GnuClearWin (pgw, 0, TRUE);
//   CTopOf(pgw) += 2;
//   CYSize(pgw)-= 2;
//
//   GnuPaintAtCreate (TRUE);
//   GnuPaintWin (pgw, (INT)-1);
//   GnuPaintBorder (pgw);
//
//   GnuPaint (pgw, -1, -1, 0, 4, "Ç");
//   GnuPaint (pgw, -1, CXSize(pgw), 0, 4, "¶");
//   GnuPaintNChar (pgw, -1, 0, 0, 4, 'Ä', CXSize(pgw));
//   GnuPaint2 (pgw, -2, 0, 0, 1, szTmp, CXSize(pgw));
//   GnuSelectLine (pgw, iStartSel, TRUE);
//
//   KeyEditCellMode (NULL, "\x50", 2);
//   while (TRUE)
//      {
//      if (pgw->iSelection == (INT)-1) // in edit control
//         {
//         c = KeyEditCell (szTmp, CTopOf(pgw)-2, CLeftOf(pgw), CXSize (pgw), 1);
//
//         if (c == K_DOWN && pgw->iItemCount)
//            GnuSelectLine (pgw, 0, TRUE);
//
//         if (!c || c == K_ESC || c == K_RET)
//            break;
//         }
//      else
//         {
//         c = KeyGet (TRUE);
//         if (!c || c == K_ESC || c == K_RET)
//            break;
//
//         if (!pgw->iSelection && (c == K_UP) /* up */) // enter edit box
//            GnuSelectLine (pgw, (INT)-1, FALSE);
//
//         else if (!GnuDoListKeys (pgw, c))
//            GNUBeep (0);
//         }
//      }
//   KeyEditCellMode (NULL, NULL, 2);
//   ScrPopCursor ();
//   i = pgw->iSelection;
//   GnuDestroyWin (pgw);
//
//   if (c == K_ESC)
//      return NULL;
//
//   strcpy (pszDat, (i == (INT)-1 ? szTmp : ppsz[i]));
//   return pszDat;
//   }


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * pops up a combobox
 * user selects a choice from a list or enters new value
 *  ppsz is the array of string choices (last entry must be NULL)
 *  iY,iX is the cell position, call with NULL for free combo
 *  iYSize is the size of the combo client, 0 = as large as needed/possible
 *  iStartSel is the initial selected line -1 = no start selection
 *
 *  return is string or NULL if user hits escape
 */
PSZ GnuEditComboBox (PSZ pszDat, PPSZ ppsz, 
							INT iY, INT iX, 
							INT iYSize, PINT piStartSel)
	{
	PMET pmet;
	PGW  pgw;
	INT  i, iMaxItemsOnScreen, c, iEntries;
	INT  iXSize, iYPos, iXPos; 
	CHAR szTmp[256];

	pmet = ScrGetMetrics ();
	iMaxItemsOnScreen = pmet->dwSize.Y-2;

	for (iEntries=0; ppsz && ppsz[iEntries]; iEntries++)
		;
	if (!iEntries)
		return NULL;

	strncpy (szTmp, pszDat, sizeof (szTmp));

	/*--- determine X Size ---*/
	for (i=iXSize=0; i<iEntries; i++)
		iXSize = max (iXSize, (INT)strlen (ppsz[i]));
	iXSize = min (iXSize, 70);
	iXSize += 3; // cvt client area size to window size

	/*--- if size not spec'd determine a good Y size for window ---*/
	if (!iYSize) 
		iYSize = (iEntries > iMaxItemsOnScreen ? 12 : iEntries);
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
		iXPos = pmet->dwSize.X - iX;

	ScrPushCursor (FALSE);
	GnuPaintAtCreate (FALSE);
	pgw = GnuCreateWin2 (MKCOORD (iXPos, iYPos), MKCOORD (iXSize, iYSize), NULL);
	pgw->bShadow        = FALSE;
	pgw->pUser1         = ppsz;
	pgw->iItemCount     = iEntries;
	pgw->bShowScrollPos = TRUE;
	pgw->iSelection     = min (iEntries-1, (INT)max (0, (INT)*piStartSel));

	GnuClearWin (pgw, 0, TRUE);
	CTopOf(pgw) += 2;
	CYSize(pgw)-= 2;

	GnuPaintAtCreate (TRUE);
	GnuPaintWin (pgw, (INT)-1);
	GnuPaintBorder (pgw);

	GnuPaint (pgw, -1, -1, 0, 4, "Ç");
	GnuPaint (pgw, -1, CXSize(pgw), 0, 4, "¶");
	GnuPaintNChar (pgw, -1, 0, 0, 4, 'Ä', CXSize(pgw));
	GnuPaint2 (pgw, -2, 0, 0, 1, szTmp, CXSize(pgw));
	GnuSelectLine (pgw, *piStartSel, TRUE);

	KeyEditCellMode (NULL, "\x50", 2);
	while (TRUE)
		{
		if (pgw->iSelection == (INT)-1) // in edit control
			{
			c = KeyEditCell (szTmp, MKCOORD (CLeftOf(pgw), CTopOf(pgw)-2), CXSize (pgw), 1);

			if (c == K_DOWN && pgw->iItemCount)
				GnuSelectLine (pgw, 0, TRUE);

			if (!c || c == K_ESC || c == K_RET)
				break;
			}
		else
			{
			c = KeyGet (TRUE);
			if (!c || c == K_ESC || c == K_RET)
				break;

			if (!pgw->iSelection && (c == K_UP) /* up */) // enter edit box
				GnuSelectLine (pgw, (INT)-1, FALSE);

			else if (!GnuDoListKeys (pgw, c))
				GNUBeep (0);
			}
		}
	KeyEditCellMode (NULL, NULL, 2);
	ScrPopCursor ();
	*piStartSel = pgw->iSelection;
	GnuDestroyWin (pgw);

	if (c == K_ESC)
		return NULL;

	strcpy (pszDat, (*piStartSel == (INT)-1 ? szTmp : ppsz[*piStartSel]));
	return pszDat;
	}
