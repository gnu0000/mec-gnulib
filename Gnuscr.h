/*
 * GnuScr.h
 *
 * (C) 1992-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file is part of the EBS module
 *
 * This file provides base screen and cursor handling functions
 * and base screen read / write finctions.
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 * 
 * SCR Functions
 * -------------
 *
 * PMET  ScrInitMetrics   (VOID)
 * PMET  ScrGetMetrics    (VOID)
 * COORD ScrGetScreenSize (VOID)
 * BOOL  ScrSetScreenSize (COORD dwNewSize)
 * BOOL  ScrRestoreScreen (VOID)
 *
 * CHAR_INFO  ScrReadCell    (dwPos)
 * PCHAR_INFO ScrReadStripe  (pCell, dwPos, iLen)
 * PCHAR_INFO ScrReadBlock   (pCell, dwBuffSize, rReadRect)
 * PSZ        ScrReadStr     (pszStr, dwPos, iLen)
 * VOID       ScrWriteCell   (cell, dwPos)
 * VOID       ScrWriteStripe (pCell, dwPos, iLen)
 * VOID       ScrWriteBlock  (pCell, dwBuffSize, rWriteRect)
 * INT        ScrWriteStr    (pszStr, dwPos, iLen)
 * INT        ScrWriteStrAtt (pszStr, cAtt, dwPos, iLen)
 * INT        ScrWriteNChar  (c, dwPos, iLen)
 * INT        ScrWriteNAtt   (cAtt, dwPos, iLen)
 * INT        ScrWriteNCell  (cell, dwPos, iLen)
 * VOID       ScrClear       (rect, ciFill)
 *
 * COORD ScrGetCursorPos (VOID)
 * VOID  ScrSetCursorPos (COORD dwPos)
 * VOID  ScrShowCursor (BOOL bShow)
 * BOOL  ScrCursorVisible (VOID)
 *
 * VOID  ScrScroll (SMALL_RECT rect, COORD dwOffset, CHAR_INFO ciFill)
 *
 * VOID  ScrPushCursor (bShow)
 * VOID  ScrPopCursor (VOID)
 *
 *
 * VOID  ScrGetDACReg (INT iReg, PINT piRed, PINT piGrn, PINT piBlu)
 * VOID  ScrSetDACReg (INT iReg, INT iRed, INT iGrn, INT iBlu)
 * UINT  ScrGetPALReg (INT iPALIdx)
 * VOID  ScrSetPALReg (INT iPALIdx, INT iReg)
 * VOID  ScrStoreColors    (VOID)
 * VOID  ScrRestoreColors  (VOID)
 * UINT  ScrIncrementColor (iModColor, iStartColor)
 *
 * BOOL  ScrIsEgaVga        (VOID)
 * PMET  scrCheckGetMetrics (VOID)
 * VOID  scrAdjustRect      (PSMALL_RECT prect)
 *
 *
 ****************************************************************************
 */

#if !defined (GNUSCR_INCLUDED)
#define GNUSCR_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif


//extern "C"
//{

/****************************************************************************
 **                                                                        **
 *                        Structures and Types                              *
 *                         defines and macros                               *
 **                                                                        **
 ****************************************************************************/

/*
 * This fn stores metric info about the physical display area
 * It is created by the ScrInitMetrics fn.
 */
typedef struct
   {
   BYTE  cOrigAtt;       // original screen background attribute
   COORD dwOrigSize;     // original screen Size
   COORD dwSize;         // current screen Size
   BOOL  bColorDisplay;  //
   BOOL  bDACMod;        // 
   BOOL  bInit;          // Set TRUE after ScrInitMetrics
   } MET;
typedef MET *PMET;



/****************************************************************************
 **                                                                        **
 *                           Scr Functions                                  *
 **                                                                        **
 ****************************************************************************/

/*
 * Under DOS  this returns true if we are running with  EGA or VGA
 * Under OS/2 this doesn't always work
 */
EXP BOOL ScrIsEgaVga(VOID);

/*
 * Returns the number of rows/cols on screen
 */
EXP COORD ScrGetScreenSize (VOID);

/*
 *
 */
EXP BOOL ScrSetScreenSize (COORD dwNewSize);

/*
 *
 *
 */
EXP BOOL ScrRestoreScreen (VOID);

/*
 * this returns the metric structure.
 * ScrInitMetrics must be called first
 */
EXP PMET ScrGetMetrics (VOID);

/*
 *
 */
EXP PMET scrCheckGetMetrics (VOID);

/*
 * This should be the first function called in this module.  This
 * initializes the screen metric information needed to restore the
 * cursor and screen ...
 *
 */
EXP PMET ScrInitMetrics (VOID);

/*
 * converts -1 extents into screen extents
 */
EXP VOID scrAdjustRect (PSMALL_RECT prect);

/*
 * Scroll an area of the screen in any direction
 * if rect Right or Bottom is -1, assume Right/Bottom edge of screen
 *
 * dwOffset: +X <- Right  -X <- Left
 *           +Y <- Down   -Y <- Up
 */
EXP VOID ScrScroll (SMALL_RECT rect, COORD dwOffset, CHAR_INFO ciFill);

/*
 *
 */
EXP VOID ScrGetDACReg (INT iReg, PINT piRed, PINT piGrn, PINT piBlu);

/*
 *
 */
EXP VOID ScrSetDACReg (INT iReg, INT iRed, INT iGrn, INT iBlu);

/*
 *
 */
EXP INT ScrGetPALReg (INT iPALIdx);

/*
 *
 */
EXP VOID ScrSetPALReg (INT iPALIdx, INT iReg);

/*
 *
 */
EXP VOID ScrStoreColors (VOID);

/*
 *
 */
EXP VOID ScrRestoreColors (VOID);

/*
 *
 */
EXP COORD ScrGetCursorPos (VOID);

/*
 *
 */
EXP VOID ScrSetCursorPos (COORD dwPos);

/*
 * shows/hides the cursor
 *
 */
EXP VOID ScrShowCursor (BOOL bShow);

/*
 * Returns TRUE if cursor is visible
 *
 */
EXP BOOL ScrCursorVisible (VOID);

/*
 * Pushes the cursor pos/visibility onto the stack
 * also shows/hides the cursor
 */
EXP VOID ScrPushCursor (BOOL bShow);

/*
 * Pops the cursor pos/visibility from the stack
 *
 */
EXP VOID ScrPopCursor (VOID);

EXP COORD MKCOORD (INT X, INT Y);

EXP SMALL_RECT MKRECT (INT Left, INT Top, INT Right, INT Bottom);

EXP CHAR_INFO MKCELL (CHAR c, BYTE cAtt);

EXP SMALL_RECT AdjustRect (SMALL_RECT rect, INT Left, INT Top, INT Right, INT Bottom);

EXP CHAR_INFO ScrReadCell (COORD dwPos);

EXP PCHAR_INFO ScrReadStripe (PCHAR_INFO pCell, COORD dwPos, INT iLen);

EXP PCHAR_INFO ScrReadBlock (PCHAR_INFO pCell, COORD dwBuffSize, SMALL_RECT rReadRect);

EXP PSZ ScrReadStr (PSZ pszStr, COORD dwPos, INT iLen);

EXP VOID ScrWriteCell (CHAR_INFO cell, COORD dwPos);

EXP VOID ScrWriteStripe (PCHAR_INFO pCell, COORD dwPos, INT iLen);

EXP VOID ScrWriteBlock (PCHAR_INFO pCell, COORD dwBuffSize, SMALL_RECT rWriteRect);

EXP INT ScrWriteStr (PSZ pszStr, COORD dwPos, INT iLen);

EXP INT ScrWriteStrAtt (PSZ pszStr, BYTE cAtt, COORD dwPos, INT iLen);

EXP INT ScrWriteNChar (CHAR c, COORD dwPos, INT iLen);

EXP INT ScrWriteNAtt (BYTE cAtt, COORD dwPos, INT iLen);

EXP INT ScrWriteNCell (CHAR_INFO cell, COORD dwPos, INT iLen);

EXP VOID ScrClear (SMALL_RECT rect, CHAR_INFO ciFill);

/*
 * This fn changes the color associated with the uModColor palette index
 * legal values for uModColor and uStartColor are 0 thru 15 (the pal colors)
 * This fn is intended to be called several times
 * Call with uPal>15 to initialize.
 * The 1st 60 calls fade from the start color to the 1st rainbow color
 * Then each cubsequent call slowly fades to different colors.
 */
EXP INT ScrIncrementColor (INT iModColor, INT iStartColor);


//} // extern C

#endif  // GNUSCR_INCLUDED



