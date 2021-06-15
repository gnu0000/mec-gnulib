/*
 * Win5.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides Gnu Window Message Box functions for the
 * screen module
 */

#include <stdlib.h>
#include <string.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"
#include "gnukbd.h"
#include "gnuwin.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * This is a simple message box that displays 2 lines of text,
 * a header, and tells the user to press esc to continue.
 */
INT GnuMsg (PSZ pszHeader, PSZ psz1, PSZ psz2)
	{
	PGW  pgw;
	PMET pmet;
	INT  iXSize;

	pmet = scrCheckGetMetrics ();
	iXSize = min (pmet->dwSize.X-2U, max (30U, max (strlen (psz1), strlen (psz2))+6U));
	KeyClearBuff ();
	ScrPushCursor (FALSE);
	pgw = GnuCreateWin (6, iXSize, NULL);
	pgw->pszHeader = pszHeader;
	pgw->pszFooter = "[Press <Esc> to exit window]";
	GnuPaintBorder (pgw);
	GnuPaint (pgw, 1, 0, 3, 1, psz1);
	GnuPaint (pgw, 2, 0, 3, 1, psz2);
	KeyChoose ("\x1B", "");
	GnuDestroyWin (pgw);
	ScrPopCursor ();
	return 0;
	}




INT GnuMsgBox2 (PSZ  pszHeader, 
					 PSZ  pszFooter, 
					 PSZ  pszChars, 
					 INT  iYSize,
					 INT  iXSize,
					 INT  iJust, 
					 WORD wAtt, 
					 PSZ  pszText)
	{
	PGW  pgw;
	PMET pmet;
	INT  iLen, iRet;

	pmet = scrCheckGetMetrics ();
	iLen = strlen (pszText);

	if (!iXSize)
		iXSize = 40+(!!(iLen>150))*10+ (!!(iLen>300))*10+ (!!(iLen>400))*10;

	if (!iYSize)
		iYSize = GnuPaintBig ((PGW)-1,1,1,pmet->dwSize.Y-4,iXSize,0,0,pszText);

	iXSize = min (pmet->dwSize.X-6, iXSize);
	iYSize = min (pmet->dwSize.Y-4, iYSize);

	KeyClearBuff ();
	ScrPushCursor (FALSE);
	pgw = GnuCreateWin (iYSize+4, iXSize+6, NULL);
	pgw->pszHeader = pszHeader;
	pgw->pszFooter = pszFooter;
	GnuPaintBorder (pgw);

	GnuPaintBig (pgw, 1, 2, iYSize, iXSize, iJust, wAtt, pszText);

	iRet = KeyChoose (pszChars, "");
	GnuDestroyWin (pgw);
	ScrPopCursor ();
	return iRet;
	}



INT GnuMsgBox (PSZ pszHeader, PSZ pszFooter, PSZ pszChars, PSZ pszText)
	{
	return GnuMsgBox2 (pszHeader, pszFooter, pszChars, 0, 0, 0, 0,  pszText);
	}


