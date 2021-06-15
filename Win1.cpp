/*
 * Win1.c
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
#include <ctype.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"
#include "gnuwin.h"


#define  MAXLINE 256


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

INT GnuPaintLen (PSZ psz)
	{
	INT i;

	for (i=0; psz && *psz; i++)
		{
		if (*psz == '\\' && psz[1] == '@')
			{
			psz +=2;
			psz = (*psz ? psz+1 : psz);
			psz = (*psz ? psz+1 : psz);
			}
		else
			psz++;
		}
	return i;
	}



/*
 * This paints text at a given position using specified
 * justification and attribute. if pgw is not null, text is relative
 * to the client part of the window. If Null, relative to screen.
 * 
 * wAtt may be 0-3 for the pre-defined colors, or the high byte can be an
 * attribute value.
 *
 * iJust = 3 cannot be used if pgw is null, unless of course you want a
 * breakpoint for CodeView. (yuk yuk)
 *
 * returns length of string written
 */
INT GnuPaint (PGW  pgw,      // local window ?
				  INT  iRow,     // 0 based row
				  INT  iCol,     // 0 based col
				  INT  iJust,    // 0=lt 1=rt 2=ctr-Col 4=ctr-row 5=center-win
				  WORD wAtt,     // see GnuGetAtt table
				  PSZ  pszText)  // \0 term string
	{
	PMET  pmet;
	COORD dwPos;
	PSZ   pszLook, pszPaint;
	INT   iLen, iSubLen;
	INT   iActualLen, iShift;
	BYTE  cAtt;
	CHAR  sz[MAXLINE];

	if (!pszText || !*pszText)
		return 0;

	pmet = scrCheckGetMetrics ();

	iLen       = strlen (pszText);
	iActualLen = GnuPaintLen(pszText);

	if (iJust == 3 || iJust == 5)
		iCol = (pgw ? CXSize(pgw)/2 : pmet->dwSize.X / 2);
	if (iJust == 4 || iJust == 5)
		iRow = (pgw ? CYSize(pgw)/2 : pmet->dwSize.Y / 2);

	dwPos = gnuClientWinToScreen (pgw, MKCOORD (iCol, iRow));
	switch (iJust)
		{       
		case 4:
		case 0:                                               break;
		case 1:  dwPos.X = max (0, dwPos.X - iActualLen);     break;
		default: dwPos.X = max (0, dwPos.X - iActualLen / 2); break;
		}

	strncpy (sz, pszText, MAXLINE-1);
	pszPaint = pszLook = sz;
	while (pszLook = strchr (pszLook, '\\'))
		{
		iSubLen = pszLook - pszPaint;
		pszLook++;
		if (*pszLook != '@' && *pszLook !='~')
			continue;
		iShift = (*pszLook == '@' ? 0 : 8);
		cAtt = GnuGetAtt (pgw, wAtt);
		ScrWriteStrAtt (pszPaint, cAtt, dwPos, iSubLen);

		++pszLook;
		wAtt  = (toupper(*pszLook) - ((*pszLook > '9') ? '7' : '0')) << 4;
		++pszLook;
		wAtt |= (toupper(*pszLook) - ((*pszLook > '9') ? '7' : '0'));
		wAtt <<= iShift;
		pszPaint = ++pszLook;
		dwPos.X += iSubLen;
		}

	iSubLen = iLen - (pszPaint - sz);
	cAtt = GnuGetAtt (pgw, wAtt);
	ScrWriteStrAtt (pszPaint, cAtt, dwPos, iSubLen);
	return iLen;
	}


/*
 * This will paint text and clip it if its bigger than iMaxLen
 * returns length of string written
 *
 */
INT GnuPaint2 (PGW  pgw,     
					INT  iRow,    
					INT  iCol,    
					INT  iJust,   
					WORD wAtt,    
					PSZ  pszText, 
					INT  iMaxLen)
	{
	CHAR szTmp [256];
	INT iLen;

	if (pszText)
		{
		iLen = min (iMaxLen, (INT)strlen (pszText));
		strncpy (szTmp, pszText, iLen);
		szTmp[iLen] = '\0';
		}
	return GnuPaint (pgw, iRow, iCol, iJust, wAtt, (pszText ? szTmp : NULL));
	}


/*====================================================================*/
/*=                                                                  =*/
/*=                    text painting functions                       =*/ 
/*=                                                                  =*/
/*====================================================================*/


/*
 * This will paint a block of text in a box specified. the text is
 * word wrapped.
 *
 * This fn differs from the other Paint functions in 2 ways:
 *    1> The return value is the number of lines written
 *    2> if pgw=-1 this function does no painting
 */
INT GnuPaintBig (PGW  pgw,    
					  INT  iYPos,  INT iXPos,
					  INT  iYSize, INT iXSize,
					  INT  iJust,  
					  WORD wAtt,   
					  PSZ  pszText)
	{
	PSZ pszStart;
	INT iIdx, i, iLines, iLineSize, iXPaintPos;

	if (!pszText || !*pszText)
		return 0;

	/*--- make special adj so just isnt always relative to left edge ---*/
	iXPaintPos = iXPos + (iJust==1 ? iXSize : (iJust==2 ? iXSize/2 : 0));

	pszStart = pszText;

	for (iLines=0; iLines<iYSize && pszStart && *pszStart; iLines++)
		{
		iLineSize = iIdx = 0; 

		/*--- get max string piece: index and actual length ---*/
		while (pszStart[iIdx])
			{
			if (pszStart[iIdx] == '\n')
				break;
			if (iLineSize >= iXSize)
				break;

			if (pszStart[iIdx] == '\\' && pszStart[iIdx+1] == '@')
				iIdx+=3;
			else
				iLineSize++;
			iIdx++;
			}

		if (iLineSize >= iXSize      &&
			 pszStart[iIdx]   != '\n' && 
			 pszStart[iIdx]   != ' '  &&
			 pszStart[iIdx-1] != ' ')
			 {
			 for (i=iIdx-1; i; i--)
				if (pszStart[i] == ' ')
					break;
			 iIdx = (i ? i+1 : iIdx);
			 }

		if (pgw != (PGW)-1)
			GnuPaint2 (pgw, iYPos+iLines, iXPaintPos, iJust, wAtt, 
						  pszStart, iIdx);

		if (pszStart[iIdx] == '\n' || pszStart[iIdx] == ' ')
			iIdx++;

		pszStart += iIdx;
		}
	return iLines;
	}      




/*
 * paints cChar with wAtt iCount times at uCol, uRow, relative
 * to pgw using iJust. Got That?
 * returns # CHAR written
 */
INT GnuPaintNChar (PGW  pgw,   
						 INT  iRow,  
						 INT  iCol,  
						 INT  iJust, 
						 WORD wAtt,  
						 CHAR cChar,
						 INT  iCount)
	{
	CHAR szTmp [256];

	iCount = min (255, iCount);
	szTmp[iCount] = '\0';

	while (iCount--)
		szTmp[iCount] = cChar;
	return GnuPaint (pgw, iRow, iCol, iJust, wAtt, szTmp);
	}



