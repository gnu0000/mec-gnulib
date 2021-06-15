/*
 * grd1.c
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
#include "gnumisc.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


INT pfnComboAtt (PGW pgw, PPOS ppos, INT iLabel)
	{
	PGRINFO  pgr;

	pgr = GrdGetPGR (pgw);

	if (iLabel)
		return (ppos->wAtt = 1);
	return (ppos->wAtt = (ppos->dwData.Y == pgw->iSelection && 
			(pgr->iSelCol == -1 || pgr->iSelCol == ppos->dwData.X) ? 2 : 0));
	}


/*
 * Grid combo box
 *
 * This fn will create a combo box containing a grid of data that may
 * be scrolled in both directions.  The selection is vertical only.
 * The window will contain labels.  The fn will automatically
 * size the columns to fit the largest element in each column.
 * The window can optionally size itself to fit what it can.
 * The window can optionally center itself in the Y and/or X direction
 * If the header or footer is not NULL there is a 1 char border on the
 * top and bottom of the grid window
 *
 * pDAt       is an array of PPSZ or structures with PSZ as 1st N elements
 * ppszLabels is a ppsz of label strings, also used to count columns
 * iYPos      starting position, set to 0 for default
 * iXPos      starting position, set to 0 for default
 * iYSize     starting size, set to 0 to fave fn calc size
 * iXSize     starting size, set to 0 to fave fn calc size
 * iStartY    starting Selection
 *
 * row oriented selection only
 *
 * return is selected row
 */
INT GrdComboBox (PVOID pDat,   PPSZ ppszLabels,
						INT  iYPos,  INT  iXPos, 
						INT  iYSize, INT  iXSize,
						INT  iStartY,
						PSZ  pszHeader, PSZ pszFooter)
	{
	PGRINFO pgr;
	PGW     pgw;
	PMET    pmet;
	INT     i, j, uRows, iCols, uLocalXSize, iColSize;
	PINT    piColSizes;
	PPSZ    *pppsz;
	INT     c;

	pmet = ScrGetMetrics ();
	pppsz = (PPSZ*) pDat;

	for (uRows=0; pppsz[uRows]; uRows++)
		;
	for (iCols=0; ppszLabels[iCols]; iCols++)
		;
//   iYSize = (iYSize ? iYSize : max (10, min (iYPos-pmet->dwSize.Y-2, uRows+4+ (pszHeader || pszFooter ? 2 : 0))));

	if (!iYSize) iYSize = max (10, min (iYPos-pmet->dwSize.Y-2, uRows+4+ (pszHeader || pszFooter ? 2 : 0)));

	piColSizes = (PINT) calloc (iCols, sizeof (INT));
	uLocalXSize = 0;
	for (i=0; i<iCols; i++)
		{
		iColSize = strlen (ppszLabels[i]);
		for (j=0; j<uRows; j++)
			iColSize = max (iColSize, (INT)strlen ((pppsz[j])[i]));

		piColSizes[i] = max (3, min (40, iColSize +2));
		uLocalXSize += piColSizes[i];
		}
//   iXSize = (iXSize ? iXSize : max (30, min (iXPos-pmet->dwSize.X-2, uLocalXSize + 4)));

	if (!iXSize) iXSize = max (30, min (iXPos-pmet->dwSize.X-2, uLocalXSize + 4));

	if (!iYPos && !iXPos)
		iYPos = iXPos = -1;  // center if not specified

	ScrPushCursor (FALSE);
	GnuPaintAtCreate (FALSE);
	pgw = GrdCreateWin2 (MKCOORD(iYPos, iXPos), MKCOORD(iYSize, iXSize), NULL);

	pgw->pszHeader = pszHeader;
	pgw->pszFooter = pszFooter;
	GrdSetupLabels (pgw, 1, 2, 0, ppszLabels);
	for (i=0; i<iCols; i++)
		GrdResizeCol (pgw, i, piColSizes[i]);
	MemFreeData (piColSizes);

	if (pszHeader || pszFooter)
		{
		pgw->dwClientPos.Y  += 1;
		pgw->dwClientSize.Y -= 2;
		}
	pgw->dwClientPos.X  += 1;
	pgw->dwClientSize.X -= 2;
	pgw->pUser1     = pDat;
	pgw->iItemCount = uRows;
	pgw->iSelection = iStartY;

	pgr = GrdGetPGR (pgw);
	pgr->pfnAtt    = pfnComboAtt;
	pgr->iColCount = iCols;
	pgr->iSelCol   = -1;
	pgr->wNormalAtt= 1;

	GnuClearWin (pgw, 0, TRUE);
	GnuPaintWin (pgw, -1);
	GnuPaintBorder (pgw);
	GnuPaintAtCreate (TRUE);
	GrdRefreshLabels (pgw);

	while (TRUE)
		{
		c = KeyGet (TRUE);
		if (c == K_ESC)
			break;
		else if (c == K_RET)
			break;
		else if (!GrdDoKeys (pgw, c))
			GNUBeep (0);
		}
	i = pgw->iSelection;
	GrdDestroyWin (pgw);
	ScrPopCursor ();
	return (c == K_ESC ? -1 : i);
	}


