/*
 * Win2.c
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
 * This repaints the window border.  This will be needed if you set or
 * change the header or footer string.
 */
void GnuPaintBorder (PGW pgw)
	{
	WORD wAtt; // Needed because GnuPaint gets NULL pgw

	if (!pgw) // entire screen;
		{
		PMET pmet = scrCheckGetMetrics ();
		GnuDrawBox (MKCOORD (0, 0), pmet->dwSize,
						gnuGetBorderChars(), GnuGetAtt (pgw, 6));
		return;
		}
	if (!pgw->bBorder)
		return;

	GnuDrawBox (pgw->dwPos, pgw->dwSize, pgw->pszBorder, GnuGetAtt (pgw, 6));

	wAtt = GnuGetAtt (pgw, 6) << 8;

	if (pgw->pszHeader)
		GnuPaint (NULL, TopOf(pgw),     LeftOf(pgw) + XSize(pgw)/2, 2, wAtt, pgw->pszHeader);
	if (pgw->pszFooter)
		GnuPaint (NULL, BottomOf (pgw), LeftOf(pgw) + XSize(pgw)/2, 2, wAtt, pgw->pszFooter);
	}







														 
/*
 * This function sets the characters used for drawing window borders
 *  and boxes. There is one global set of box characters.  In addition,
 *  each window may have its own box CHAR set.  The global set is used by 
 *  all windows which haven't had thier borders explicitly set.
 * The character string pszBC should be 9 bytes long and contain in order:
 *
 *       TopL TopR BotL BotR HorizTop HorizBtm VertLeft VertRt ScrollMark.
 *
 * The default pszBC is "É»È¼ÍÍºº×"
 *
 * If pgw is NULL then the function will change the global set.  
 * If pgw is NULL and pszBC is NULL the global CHAR set is reset to 
 *  the default.
 * If only pszBC is NULL the window will use the global CHAR set.
 *
 * Got that?
 */
INT GnuSetBorderChars (PGW pgw, PSZ pszBC)
	{
	if (pszBC && strlen (pszBC) != 9)
		return 1;

	if (!pgw)
		{
		strcpy (gnuGetBorderChars (), (pszBC ? pszBC : "É»È¼ÍÍºº×"));
		return 0;
		}
	if (pgw->pszBorder)
		free (pgw->pszBorder);
	pgw->pszBorder = strdup ((pszBC ? pszBC : gnuGetBorderChars ()));
	return 0;
	}



/*
 * Jus wat it say.
 *
 * Attribute param is the actual attribute byte
 * use GnuGetAtt to convert from wAtt
 */
INT GnuDrawBox (COORD dwPos, COORD dwSize, PSZ pszBorder, BYTE cAtt)
	{
	INT  i, iBottom, iRight;
	  
	iRight  = dwPos.X + dwSize.X - 1;
	iBottom = dwPos.Y + dwSize.Y - 1;

	if (!pszBorder || strlen (pszBorder) < 9)
		pszBorder = gnuGetBorderChars ();

	ScrWriteNCell (MKCELL(pszBorder[0], cAtt), dwPos, 1);
	ScrWriteNCell (MKCELL(pszBorder[4], cAtt), MKCOORD(dwPos.X+1, dwPos.Y), dwSize.X-2);
	ScrWriteNCell (MKCELL(pszBorder[1], cAtt), MKCOORD(iRight   , dwPos.Y), 1);

	for (i=dwPos.Y+1; i< iBottom; i++)
		{
		ScrWriteNCell (MKCELL(pszBorder[6], cAtt), MKCOORD(dwPos.X, i), 1);
		ScrWriteNCell (MKCELL(pszBorder[7], cAtt), MKCOORD(iRight,  i), 1);
		}
	ScrWriteNCell (MKCELL(pszBorder[2], cAtt), MKCOORD(dwPos.X  , iBottom), 1);
	ScrWriteNCell (MKCELL(pszBorder[5], cAtt), MKCOORD(dwPos.X+1, iBottom), dwSize.X-2);
	ScrWriteNCell (MKCELL(pszBorder[3], cAtt), MKCOORD(iRight   , iBottom), 1);
	return i;
	}


