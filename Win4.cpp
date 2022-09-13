/*
 * Win4.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides GnuDoListKeys function for the
 * screen module
 */

#include <stdlib.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"
#include "gnukbd.h"
#include "gnumisc.h"
#include "gnuwin.h"


extern "C"
{

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/* This fn does the default keyboard processing for a list window
 * If an item in the window is selected, the keys are selection oriented.
 * If no item in the window is selected, the keys are display oriented.
 * A typical difference between selection and display oriented schemes
 * is that in display oriented, up and down arrows will cause a scroll
 * but in selection oriented they cause a selection change and maybe a 
 * scroll.
 *
 */

BOOL GnuDoListKeys (PGW pgw, int cExtKey)
	{
	INT  iSelDir;
	INT  iNewItem, iCurrent, iMaxItem, iMaxScroll;
	INT  iNewScroll, iPage;
	BOOL bView;

	if (!pgw || !pgw->iItemCount)
		return FALSE;

	bView    = !!(pgw->iSelection == -1);
	iCurrent = (!bView ? pgw->iSelection : pgw->iScrollPos);
	iSelDir  = 0;
	iMaxItem = (bView ? ScrollMax (pgw) : pgw->iItemCount - 1);
	iMaxScroll = ScrollMax (pgw);
	iPage    = 0;

	switch (cExtKey)
		{
		case K_UP:                             /*------ Up Arrow    ------*/
			if (iCurrent == 0)
				return GNUBeep (0);
			iSelDir =  - 1;
			break;

		case K_DOWN:                           /*------ Dn Arrow    ------*/
			if (iCurrent == iMaxItem)
				return GNUBeep (0);
			iSelDir =  1;
			break;

		case K_HOME:                           /*------ Home        ------*/
			iSelDir = - iCurrent;
			break;

		case K_END:                            /*------ End         ------*/
			iSelDir = (INT)iMaxItem - iCurrent;
			break;

		case K_CHOME:                          /*------ Ctl-Home    ------*/
		case K_PGUP:                           /*------ Pg-Up       ------*/
			if (iCurrent == 0)
				return GNUBeep (0);
			iSelDir = 0 - (INT)DisplayItems(pgw);
			iPage = (cExtKey == 0x149 ? 1 : 0);
			break;

		case K_CEND:                           /*------ Ctl-End     ------*/
		case K_PGDN:                           /*------ Pg-Dn       ------*/
			if (iCurrent == iMaxItem)                             
				return GNUBeep (0);
			iSelDir = DisplayItems(pgw);
			iPage = (cExtKey == 0x151 ? 2 : 0);
			break;

		case K_CUP:                            /*------Ctl-Up Arrow ------*/
			if (bView)
				return GNUBeep (0);
			iSelDir = pgw->iScrollPos - iCurrent;
			break;

		case K_CDOWN:                          /*------Ctl-Dn Arrow ------*/
			if (bView)                    
				return GNUBeep (0);
			iSelDir = pgw->iScrollPos - iCurrent + DisplayItems(pgw)- 1;
			break;

		default:
			return FALSE;
		}
	if (!iSelDir)
		return TRUE;
	else
		{
		iNewItem = max (0, min ((INT)iMaxItem, iCurrent+iSelDir));
		if (bView)
			GnuScrollTo (pgw, iNewItem);

		// PgUp and PgDn get special handling because
		// the user expects the screen to scroll 1 screen
		// worth of items, not miminum scrolling.
		else if (iPage)
			{
			if (iPage == 1)
				iNewScroll = pgw->iScrollPos - min (pgw->iScrollPos, DisplayItems(pgw));
			else
				iNewScroll = min (iMaxScroll, pgw->iScrollPos + DisplayItems(pgw));
			GnuScrollTo (pgw, iNewScroll);
			GnuSelectLine (pgw, iNewItem, TRUE);
			}
		else 
			GnuSelectLine (pgw, iNewItem, TRUE);
		}
	return TRUE;
	}

}