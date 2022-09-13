/*
 * WinB.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides color attribute mapping functions
 *
 */

#include <stdlib.h>
#include <string.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"
#include "gnuwin.h"

extern "C"
{


#define szDEFCMAP "\xFE\xFF\x51\x07\xAC\xCB\x98\xFE"

#define NUMATTS 9

static BYTE szGLOBALCMAP[NUMATTS] = szDEFCMAP;
static BYTE szSCREENCMAP[NUMATTS] = szDEFCMAP;

static BYTE pcSIMPLEMAP[17] = {0x40,0x41,0x50,0x51,0x48,0x58,0x42,0x43,
										 0x49,0x4A,0x59,0x5A,0x4B,0x5B,0x4C,0x76,0x76};
  
static BYTE puMONOMAP  [17] = {0x70,0xF0,0x07,0x07,0xF0,0x07,0xF0,0xF0,
										 0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0x00};

static BOOL bMONOMODE = FALSE;


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * returns full global or screen colormap
 */
PSZ _gnuGetMap (BOOL bScreen)
	{
	return (PSZ) (bScreen ? szSCREENCMAP : szGLOBALCMAP);
	}


/*
 * This fn provides a low level mapping between user palette values
 * and true color values.
 *
 * input : iVal 0-F  
 * output: palette value 0-F
 *
 * Each window can have its own nibble map
 *
 * convention:
 *
 *  iVal definition
 *  ---------------------
 *  0    Normal Client FG
 *  1    Bright Client FG
 *  2    Normal Border FG
 *  3    Bright Border FG
 *  4    Normal BG
 *  5    Bright BG
 *  6    Shadow FG
 *  7    Shadow BG
 *  8    Error  FG
 */
UCHAR _gnuNibbleMap (PGW pgw, INT iVal)
	{
	PSZ pszCMap;

	pszCMap = (pgw ? (pgw==(PGW)-1 ? (PSZ) szGLOBALCMAP : (PSZ) pgw->pszCMap): (PSZ) szSCREENCMAP);

	iVal &= 0x0F;
	return ((pszCMap[iVal/2] >> (iVal%2)*4) & 0x0F);
	}


/*
 * Defines the primary color mapping
 *
 * if pgw != NULL,   the specific window's color scheme is modified
 * if pgw is NULL,   the screen color scheme is modified
 * if pgw is -1,     the global color scheme is modified
 * the global color scheme is used as the default when creating new windows
 *
 * uType specifies which color component to modify
 * uColor specifies what the new color should be for the given component (0-F)
 *
 *  iType definition
 *  ---------------------
 *  0    Normal Client FG
 *  1    Bright Client FG
 *  2    Normal Border FG
 *  3    Bright Border FG
 *  4    Normal BG
 *  5    Bright BG
 *  6    Shadow FG
 *  7    Shadow BG
 *  8    Error  FG
 *  9    User Color
 *  A    User Color
 *  .    .
 *  .    .
 *  F    User Color
 *
 *  100  set pgw map to default map (pgw map can be window,screen,global set)
 *  200  set pgw map to global  map ...
 *  300  set pgw map to screen  map
 *  400  set global map to pgw  map 
 *  500  set screen map to pgw  map
 *
 *
 *  0xFFFF Reset colors to defaults:
 *             Window or screen to global set (pgw=window || !pgw)
 *             Global set to default set (pgw=-1)
 */
void GnuSetColor (PGW pgw, INT iType, INT iVal)
	{
	PSZ pszCMap;

	pszCMap = (pgw ? (pgw==(PGW)-1 ? (PSZ) szGLOBALCMAP : (PSZ) pgw->pszCMap): (PSZ) szSCREENCMAP);

	if (iType == 100) memcpy (pszCMap, szDEFCMAP   , NUMATTS);
	if (iType == 200) memcpy (pszCMap, szGLOBALCMAP, NUMATTS);
	if (iType == 300) memcpy (pszCMap, szSCREENCMAP, NUMATTS);
	if (iType == 400) memcpy (szGLOBALCMAP, pszCMap, NUMATTS);
	if (iType == 500) memcpy (szSCREENCMAP, pszCMap, NUMATTS);

	if (iType < 0x10)
		pszCMap[iType/2] = (pszCMap[iType/2] & (iType%2 ? 0x0F : 0xF0)) |
								 ((iVal & 0x0F) << (iType%2)*4);
	return;
	}


INT GnuGetColor (PGW pgw, INT iType)
	{
	PSZ pszCMap;

	pszCMap = (pgw ? (pgw==(PGW)-1 ? (PSZ) szGLOBALCMAP : (PSZ) pgw->pszCMap): (PSZ) szSCREENCMAP);

	return ((pszCMap[iType/2] & (iType%2 ? 0xF0 : 0x0F)) >> ((iType%2)*4));
	}


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


/*
 * simple map to attribute byte mapping
 *
 * input : iVal 0-F  
 * output: fg/bg attribute byte
 */
BYTE _gnuSimpleMap (PGW pgw, INT iVal)
	{
	BYTE cNMap;

	if (bMONOMODE)
		return (UCHAR) puMONOMAP [iVal & 0x0F];

	cNMap = pcSIMPLEMAP [iVal & 0x0F];
	return (_gnuNibbleMap (pgw, cNMap >> 4) << 4 | 
			  _gnuNibbleMap (pgw, cNMap & 0x0F));
	}


/*
 * Maps the simple map indexes to the primary map.
 *
 * see comments for GnuGetAtt to see how attributes are mapped
 *
 *
 *  iType (0-F) FG color (0x0_)    BG color (0x_0
 *  ---------------------------------------
 *  0           Normal Client FG   Normal BG
 *  1           Bright Client FG   Normal BG
 *  2           Normal Client FG   Bright BG
 *  3           Bright Client FG   Bright BG
 *  4           Error FG           Normal BG
 *  5           Error FG           Bright BG
 *  6           Normal Border FG   Normal BG
 *  7           Bright Border FG   Normal BG
 *  8               user color mapping 
 *    ...           user color mapping
 *  f           Shadow FG          Shadow BG
 */
void GnuSetSimpleMap (INT iType, BYTE cVal)
	{
	pcSIMPLEMAP [iType & 0x0F] = cVal;
	}

/*
 * simple map to primary mapping (0-F)
 *
 */
BYTE GnuGetSimpleMap (INT iType)
	{
	return pcSIMPLEMAP [iType & 0x0F];
	}

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * maps an attribute value to a attribute byte
 *
 * iAtt        values
 * ----------------------------------------------------------
 * 0-F       - Simple Map
 * 10-FF     - Nibble map BG / FG
 * 01xx-FFxx - Direct Palette Val or Nibble map each part
 *             x=0 for direct, 1 for nibble map for each part
 */
BYTE GnuGetAtt (PGW pgw, WORD wAtt)
	{
	BYTE cBG, cFG, cRet;

	if (wAtt < 0x10)
		cRet =  _gnuSimpleMap (pgw, wAtt);
	else if (wAtt < 0x100)
		cRet = (_gnuNibbleMap (pgw, wAtt >> 4) << 4 | 
				  _gnuNibbleMap (pgw, wAtt & 0x0F));
	else
		{
		cBG = (wAtt & 0xF0 ? _gnuNibbleMap (pgw, wAtt >> 12) : (wAtt >> 12)& 0x0F);
		cFG = (wAtt & 0x0F ? _gnuNibbleMap (pgw, wAtt >> 8)  : (wAtt >> 8) & 0x0F);
		cRet =  cBG << 4 | cFG;
		}
	if (bMONOMODE)
		{
		cBG = (cRet & 0xF0) >> 4;
		cFG = (cRet & 0x0F);
		if (cFG && cFG < 8) cFG = 0x07;
		if (cFG && cFG > 7) cFG = 0x0F;
		if (cBG > 1) cBG = 7, cFG = 0;
		if (cBG == 1) cBG = 0;

		cRet =  cBG << 4 | cFG;
		}
	return cRet;
	}



void GnuSetMonoMode (BOOL bMono)
	{
	bMONOMODE = bMono;
	}
}