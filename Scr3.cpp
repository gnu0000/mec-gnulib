/*
 * Scr3.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * Base Screen Reads/Writes
 *
 *
 ****************************************************************************
 *
 * CHAR_INFO  ScrReadCell    (dwPos)
 * PCHAR_INFO ScrReadStripe  (pCell, dwPos, iLen)
 * PCHAR_INFO ScrReadBlock   (pCell, dwBuffSize, rReadRect)
 * PSZ        ScrReadStr     (pszStr, dwPos, iLen)
 * void       ScrWriteCell   (cell, dwPos)
 * void       ScrWriteStripe (pCell, dwPos, iLen)
 * void       ScrWriteBlock  (pCell, dwBuffSize, rWriteRect)
 * INT        ScrWriteStr    (pszStr, dwPos, iLen)
 * INT        ScrWriteStrAtt (pszStr, cAtt, dwPos, iLen)
 * INT        ScrWriteNChar  (c, dwPos, iLen)
 * INT        ScrWriteNAtt   (cAtt, dwPos, iLen)
 * INT        ScrWriteNCell  (cell, dwPos, iLen)
 * void       ScrClear       (rect, ciFill)
 *
 ****************************************************************************
 *
 */

#include <stdlib.h>
#include <windows.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"

extern "C"
{


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/

COORD MKCOORD (INT X, INT Y)
	{
	COORD c = {X,Y};
	return c;
	}

SMALL_RECT MKRECT (INT Left, INT Top, INT Right, INT Bottom)
	{
	SMALL_RECT r = {Left, Top, Right, Bottom};
	return r;
	}

CHAR_INFO MKCELL (CHAR c, BYTE cAtt)
	{
	CHAR_INFO cell;

	cell.Char.AsciiChar = c;
	cell.Attributes     = cAtt;
	return cell;
	}


SMALL_RECT AdjustRect (SMALL_RECT rect, INT Left, INT Top, INT Right, INT Bottom)
	{
	rect.Left   += Left  ;
	rect.Top    += Top   ;
	rect.Right  += Right ;
	rect.Bottom += Bottom;
	return rect;
	}

/************************************************************************/

CHAR_INFO ScrReadCell (COORD dwPos)
	{
	CHAR_INFO  ci;
	SMALL_RECT rect;

	setr (rect, dwPos.Y, dwPos.X, dwPos.Y, dwPos.X);
	ReadConsoleOutput (GetStdHandle (STD_OUTPUT_HANDLE), 
							  &ci, MKCOORD(1,1), MKCOORD(0,0), &rect);
	return ci;
	}

PCHAR_INFO ScrReadStripe (PCHAR_INFO pCell, COORD dwPos, INT iLen)
	{
	SMALL_RECT rect;

	setr (rect, dwPos.Y, dwPos.X, dwPos.Y, dwPos.X+iLen-1);
	ReadConsoleOutput (GetStdHandle (STD_OUTPUT_HANDLE), 
							 pCell, MKCOORD(1,iLen), MKCOORD(0,0), &rect);
	return pCell;
	}

PCHAR_INFO ScrReadBlock (PCHAR_INFO pCell, COORD dwBuffSize, SMALL_RECT rReadRect)
	{
	ReadConsoleOutput (GetStdHandle (STD_OUTPUT_HANDLE), 
							 pCell, dwBuffSize, MKCOORD(0,0), &rReadRect);
	return pCell;
	}

PSZ ScrReadStr (PSZ pszStr, COORD dwPos, INT iLen)
	{
	ReadConsoleOutputCharacter (GetStdHandle (STD_OUTPUT_HANDLE), 
										 pszStr, iLen, dwPos, (LPDWORD) &iLen);
	return pszStr;
	}

/***************************************************************************/

void ScrWriteCell (CHAR_INFO cell, COORD dwPos)
	{
	SMALL_RECT rect;

	setr (rect, dwPos.Y, dwPos.X, dwPos.Y, dwPos.X);
	WriteConsoleOutput (GetStdHandle (STD_OUTPUT_HANDLE), 
							  &cell, MKCOORD(1,1), MKCOORD(0,0), &rect);
	}

void ScrWriteStripe (PCHAR_INFO pCell, COORD dwPos, INT iLen)
	{
	SMALL_RECT rect;

	setr (rect, dwPos.Y, dwPos.X, dwPos.Y, dwPos.X+iLen-1);
	WriteConsoleOutput (GetStdHandle (STD_OUTPUT_HANDLE), 
							  pCell, MKCOORD(1,iLen), MKCOORD(0,0), &rect);
	}

void ScrWriteBlock (PCHAR_INFO pCell, COORD dwBuffSize, SMALL_RECT rWriteRect)
	{
	WriteConsoleOutput (GetStdHandle (STD_OUTPUT_HANDLE), 
							  pCell, dwBuffSize, MKCOORD(0,0), &rWriteRect);
	}

INT ScrWriteStr (PSZ pszStr, COORD dwPos, INT iLen)
	{
	pszStr = (pszStr ? pszStr : "");
	iLen = (iLen == -1 ? strlen (pszStr) : iLen);

	WriteConsoleOutputCharacter (GetStdHandle (STD_OUTPUT_HANDLE),
										  pszStr, iLen, dwPos, (LPDWORD) &iLen);
	return iLen;
	}

INT ScrWriteStrAtt (PSZ pszStr, BYTE cAtt, COORD dwPos, INT iLen)
	{
	iLen = ScrWriteStr (pszStr, dwPos, iLen);
	return ScrWriteNAtt (cAtt, dwPos, iLen);
	}

INT ScrWriteNChar (CHAR c, COORD dwPos, INT iLen)
	{
	FillConsoleOutputCharacter (GetStdHandle (STD_OUTPUT_HANDLE),
										 c, iLen, dwPos, (LPDWORD) &iLen);
	return iLen;
	}

INT ScrWriteNAtt (BYTE cAtt, COORD dwPos, INT iLen)
	{
	FillConsoleOutputAttribute (GetStdHandle (STD_OUTPUT_HANDLE),
										 cAtt, iLen, dwPos, (LPDWORD) &iLen);
	return iLen;
	}

INT ScrWriteNCell (CHAR_INFO cell, COORD dwPos, INT iLen)
	{
	ScrWriteNChar (cell.Char.AsciiChar, dwPos, iLen);
	return ScrWriteNAtt ((BYTE)cell.Attributes, dwPos, iLen);
	}

void ScrClear (SMALL_RECT rect, CHAR_INFO ciFill)
	{
	INT iLine;

	scrAdjustRect (&rect);

	for (iLine = rect.Top; iLine <= rect.Bottom; iLine++)
		ScrWriteNCell (ciFill, MKCOORD (rect.Left, iLine), 
							rect.Right-rect.Left+1);
	}


//ScrReadBlk     -> ScrReadCell, *ScrReadStripe, scrReadBlock
//ScrReadStr     -> ScrReadStr
//ScrWriteBlk    -> ScrWriteCell, *ScrWriteStripe
//ScrWriteStr    -> ScrWriteStr
//ScrWriteStrAtt -> ScrWriteStrAtt
//ScrWriteNCell  -> ScrWriteNChar, ScrWriteNAtt
}