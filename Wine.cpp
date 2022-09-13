/*
 * WinE.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * Gratuitous functions
 * dont work very well since NT console port
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

extern "C"
{

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


static void PaintStripe (CHAR_INFO ciStripe[30][2], INT x,
								 INT iXLt, INT iXRt, INT iYTop, INT iYBot)
	{
	INT iYPos, iXPos, iPaintLen;

	for (iYPos=iYTop; iYPos<=iYBot; iYPos++)
		{
		iXPos = x - (iYPos-iYTop); // x-(iYBot-iYPos)  for other diag 

		iPaintLen = max (0, min (2, iXRt - iXPos + 1)) * 2;

		if (iXPos >= iXLt && iPaintLen)
			ScrWriteStripe (ciStripe[iYPos-iYTop], MKCOORD (iXPos, iYPos), 2);
		}
	}



/*
 * This fn causes a gnu window to have a shimmer effect.
 * pgw is the window to work on
 * uAttrib is the color to use as the hilite
 * uMax is the max number of calls before it repeats
 */
void WinShimmer (PGW pgw, BYTE cMask, BYTE cAttrib, INT iMaxCt)
	{
	static INT x = 0;
	INT    y, iYBot, iXRt, iMaxX, ydx;
	CHAR_INFO newCI[30][2], old[30][2];

	x = (x > iMaxCt ? 0 : x + 1);

	if (!iMaxCt || !pgw)
		return;

	iYBot = TopOf(pgw) + YSize(pgw)-1;
	iXRt  = LeftOf(pgw) + XSize(pgw)-1;
	iMaxX = iXRt + (iYBot - TopOf(pgw));

	if (x < (INT)LeftOf(pgw) || x > iMaxX+1)
		return;

	PaintStripe (old, x-1, LeftOf(pgw), iXRt, TopOf(pgw), iYBot);
	for (y=TopOf(pgw); y<=iYBot; y++)
		{
		ydx = y-TopOf(pgw);

		ScrReadStripe (old[ydx], MKCOORD (x - (y-TopOf(pgw)), y), 2); // x-(iYBot-y) for other diag

//      ScrReadBlk (old[ydx], 6, y, x - (y-TopOf(pgw))); 
//      new[ydx][0] =  old[ydx][0];
//      new[ydx][1] = (old[ydx][1] & cMask) | cAttrib;
//      new[ydx][2] =  old[ydx][2];
//      new[ydx][3] = (old[ydx][3] & cMask) | cAttrib;

		newCI[ydx][0] = old[ydx][0];
		newCI[ydx][1] = old[ydx][1];
		newCI[ydx][0].Attributes = (old[ydx][0].Attributes & cMask) | cAttrib;
		newCI[ydx][1].Attributes = (old[ydx][1].Attributes & cMask) | cAttrib;
		}
	PaintStripe (newCI, x, LeftOf(pgw), iXRt, TopOf(pgw), iYBot);
	}



/*
 * returns a INT containing bit flags
 * each used palette entry will have its bit set to 1
 * use PGW=NULL   to look at the screen colors
 * use PGW=-1     to look at the default colors
 * use PGW=window to look at a windows colors
 *
 * iColorsToCheck determines how many of the colors to look at
 * see GnuSetColor to see which colors.  Usually setting
 * iColorsToCheck to 5 or to 9 is enough
 *
 */
WORD WinUsedColorList (PGW pgw, INT iColorsToCheck)
	{
	WORD i, iColorList = 0;

	for (i=0; i<iColorsToCheck; i++)
		SetFlag (&iColorList, GnuGetColor (pgw, i), TRUE);
	return iColorList;
	}

}
