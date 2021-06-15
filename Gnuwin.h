/*
 * GniWin.h
 *
 * (C) 1992-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file is part of the GNULIB module
 *
 * This file provides the windowing functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 * 
 * GNU Windows Functions
 * ---------------------
 *
 * GNU Window Creation/Destriction
 * --------------------------------------------------
 * PGW  GnuCreateWin (iYSize, iXSize, pPaintProc)
 * PGW  GnuCreateWin2 (dwPos, dwSize, PAINTPROC pPaintProc);
 * PGW  GnuDestroyWin (pgw)
 * VOID GnuPaintAtCreate (bDraw)
 *
 * GNU Window Painting functions
 * --------------------------------------------------
 * INT  GnuPaint (pgw,iRow,iCol,iJust,wAtt,pszText)
 * INT  GnuPaintStr (pgw, iRow, iCol, iJust, wAtt, psz, ...)
 * INT  GnuPaint2 (pgw, iRow, iCol, iJust, wAtt, PSZ pszText, iMaxLen)
 * INT  GnuPaintBig (pgw, iYPos,iXPos,iYSize,iXSize,iJust,wAtt,PSZ pszText)
 * INT  GnuPaintNChar (pgw, iRow,iCol,iJust,wAtt,cChar,iCount)
 * VOID GnuPaintBorder (pgw)
 * VOID GnuClearWin (pgw, wAtt, bIncludeBorder)
 *
 * GNU Window Settings
 * --------------------------------------------------
 * BOOL  GnuMoveWin (pgw, iYPos, iXPos)
 * COORD GnuGetCursor (pgw);
 * VOID  GnuMoveCursor (pgw, dwPos);
 * VOID  GnuPaintBorder (pgw)
 * INT   GnuSetBorderChars (pgw, pszBC)
 * INT   GnuDrawBox (dwPos, dwSize, pszBorder, cAtt);
 * INT   GnuPaintLen (PSZ psz)
 *
 * BYTE  GnuGetAtt (pgw, wAtt)
 * INT   GnuGetColor (pgw, iType)
 * VOID  GnuSetColor (pgw, iType, iVal)
 * BYTE  GnuGetSimpleMap (iType)
 * VOID  GnuSetSimpleMap (iType, cVal)
 *
 * GNU Window List specific functions
 * --------------------------------------------------
 * BOOL GnuPaintWin (pgw, iLine)
 * BOOL GnuMakeVisible (pgw, iLine)
 * BOOL GnuScrollTo  (pgw, iLine)
 * VOID GnuScrollWin (pgw, iLines, bUp)
 * BOOL GnuSelectLine (pgw, iLine, bOnScreen)
 * BOOL GnuDoListKeys (pgw, cExtKey)
 *
 * GNU Window Utility Functions
 * --------------------------------------------------
 * BOOL GnuSelectColorsWindow (pgwCh)
 * BOOL GnuModifyColorsWindow ()
 * INT  GnuFileWindow (pszFile, pszMatch, pszHeader, pszWorkBuff)
 * INT  GnuHexEdit (pszBuff, iOffset, iLines)
 * INT  GnuMsg (pszHeader, psz1, psz2)
 * INT  GnuMsgBox (pszHeader, pszFooter, pszChars, pszText)
 * INT  GnuMsgBox2 (pszHeader, pszFooter, pszChars, iYSize,iXSize, iJust, wAtt, pszText)
 * INT  GnuComboBox (ppsz, iY, iX, iYSize, iStartSel)
 * PSZ  GnuEditComboBox (pszDat, ppsz, iY, iX, iYSize, piStartSel);
 * VOID WinShimmer (pgw, cMask, cAttrib, iMax)
 * INT  WinUsedColorList (pgw, iColorsToCheck)
 *
 * GNU Window Internal functions (minimal param checking)
 * --------------------------------------------------
 * PCHAR_INFO gnuGetScreenDat (PGW pgw);
 * BOOL       gnuPutScreenDat (PGW pgw, BOOL bIncludeShadowArea);
 * VOID       gnuFreeScreenDat (PGW pgw);
 * VOID       gnuSwapScreenDat (PGW pgw, BOOL bIncludeShadowArea);
 * COORD gniWinToScreen (PGW pgw, COORD dwPos);
 * COORD gnuScreenToWin (PGW pgw, COORD dwPos);
 * COORD gnuClientWinToScreen (PGW pgw, COORD dwPos);
 * COORD gnuScreenToClientWin (PGW pgw, COORD dwPos);
 * VOID  gnuShowScrollPos (pgw, iNewLine, bShow)
 * VOID  gnuDrawShadow (pgw)
 * INT   gnuDefPaintProc (pgw, iIndex, iLine)
 * BOOL  gnuFixScroll (pgw)
 * PSZ   gnuGetBorderChars (VOID)
 * UCHAR _gnuNibbleMap (pgw, iVal)
 * PSZ   _gnuGetMap (bScreen) 
 * UCHAR _gnuSimpleMap (pgw, iVal)
 *
 *
 * GNU Window Macros
 * --------------------------------------------------
 * INT  DisplayItems(pgw)
 * INT  ScrollMax(pgw)
 * BOOL InWindow(pgw,iLine)
 * INT  RightOf(pgw)
 * INT  BottomOf(pgw)
 * INT  LeftOf(pgw)
 * INT  TopOf(pgw)
 * 
 ****************************************************************************
 *
 * Using GNU Windows
 * -----------------
 *
 *  The GnuCreateWin and GnuCreateWin2 functions are used to create static
 *  and list windows (NOT Respectively).  Modification of the window is
 *  done by changing the actual pgw structure.
 *   To make a window a list window:
 *
 *      1> set pgw->iItemCount to a number other than 0
 *
 *      2> set pgw->iItemSize to the size of the item in lines.
 *
 *      3> set pgw->pUser1 to a ppsz containing the list data
 *
 *      4> optionally provide a user paint fn in the 3rd param of
 *         the GnuCreateWin function. The default fn. treats pUser1
 *         as a PPSZ with iItemCount strings, and paints the strings
 *         left justified.
 *
 *      5> call GnuPaintWin (pgw, 0xFFFF)  (to repaint entire window)
 *
 *      6> you will probable then want to call GnuDoListKeys for
 *         each extended (2-byte) key you get from the user, and it
 *         will scroll the window for you.
 *
 *  If you want the interface to be selection oriented rather than view
 *  oriented (IE UpArrow move the selection up 1 rather than scroll and
 *  no selection) you will also want to:
 *
 *      7> set pgw->iSelection to a valid row number (use 0).
 *
 *
 *
 *  EXAMPLE #1:
 *    This example creates a small message window to tell the user to
 *    wait. When the work is finished, the window disappears, restoring
 *    the window to its previous state.
 *
 *
 *    ScrInitMetrics ();  //-- dont forget to call this first --//
 *    .
 *    .
 *    
 *    VOID DoWork (VOID)
 *       {
 *       PGW pgw;
 *
 *       pgw = GnuCreateWin (6, 20, NULL);
 *       GnuPaint (pgwWorking, 1, 0, 3, 0, "Working ...");
 *       GnuPaint (pgwWorking, 2, 0, 3, 0, "Please Wait");
 *       DoLotsOfPainfullWork ();
 *       GnuDestroyWin (pgwWorking);
 *       }
 *
 *
 *  EXAMPLE #2:
 *    This example shows a list of data in a window 20 x 60.  The array
 *    of strings and the count of strings is gottem by a fictitious fn
 *    called GetPPSZData.
 *
 *
 *    VOID DisplayDataInListWindow (VOID)
 *       {
 *       PGW     pgw;
 *
 *       //====== CREATE WINDOW ======//
 *       pgw = GnuCreateWin (20, 60, NULL);
 *       pgw->pszHeader = "[Data Log]";
 *       pgw->pszFooter = "[use ,,PgUp,PgDn to view, press <ESC> to exit]";
 *       pgw->pUser1         = GetPPSZData (&iNumItems);
 *       pgw->iItemCount     = iNumItems;
 *       pgw->bShowScrollPos = TRUE;
 *       GnuPaintBorder (pgw);
 *    // pgw->iSelection = 0;      //--- enable this to allow selections ---//
 *       GnuPaintWin (pgw, 0xFFFF);
 *
 *       //====== HANDLE KEYBOARD INPUT ======//
 *       while (TRUE)
 *          {
 *          //(c = getch ();
 *          //if (c == 0x00 || c == 0xE0)    //--- Extended Key ---//
 *          //  c = 0x100 + getch ();
 *          c = KeyGet (TRUE);               //--- from GnuKbd ---//
 *
 *          if (c == 0x1B)                   //-- Look for ESC ---//
 *             break;
 *
 *          if (!GnuDoListKeys (pgw, c)) //-- Auto handle scrolling --//
 *             GNUBeep (0);
 *          }
 *       GnuDestroyWin (pgw);      //--- This does NOT free pUser1 ---//
 *       }
 */


#if !defined (GNUWIN_INCLUDED)
#define GNUWIN_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GniType.h>
#endif



//extern "C"
//{

/****************************************************************************
 **                                                                        **
 *                        Structures and Types                              *
 *                         defines and macros                               *
 **                                                                        **
 ****************************************************************************/

#define EV_BORDER        0x01     /* 0000000000000001 */
#define EV_TOPBORDER     0x03     /* 0000000000000011 */
#define EV_BOTTOMBORDER  0x05     /* 0000000000000101 */
#define EV_LEFTBORDER    0x09     /* 0000000000001001 */
#define EV_RIGHTBORDER   0x11     /* 0000000000010001 */
#define EV_ANYBORDER     0x1F     /* 0000000000011111 */
#define EV_INCX          0x20     /* 0000000000100000 */
#define EV_INCY          0x40     /* 0000000001000000 */


/*
 * This typedef is for the paint proc for list windows
 * If you use list windows, and you want to override the default
 * paint proc, This is what the proc it must look like
 * The PaintProc fn pointer is part of the GW structure, defined below
 */
typedef INT  (*PAINTPROC) (struct _GW *pgw, INT  iIndex, INT  iWindowLine);


/*
 * This is the GniWindow structure.
 * You get a handle (pointer) to this structure when you create a window
 * and you pass this handle to all the window functions.
 * You can also use this handle as a structure ptr to directly set values
 *
 * The top of this header shows an example of how to use this stuff.
 *
 */
typedef struct _GW
   {
   COORD dwPos;             // position of the window
   COORD dwSize;            // window size
   COORD dwClientPos;       // client position
   COORD dwClientSize;      // client size

   BYTE  bBorder;           // True = draw border
   BYTE  bShadow;           // True = draw shadow
   PCHAR_INFO pcScreenDat;  // buffer holding data under window
   PSZ   pszHeader;         // text on top border
   PSZ   pszFooter;         // text on bottom border
   PSZ   pszCMap;           // Window color map.
   PSZ   pszBorder;         // border chars. use GnuSetBorder to change
   PVOID pUser1;            // user data, default place of listwin data
   PVOID pUser2;            // user data
   PVOID pUser3;            // user data, (used internally by grid module!)

   /*--- ListWin Metrics ---*/
   INT       iScrollPos;    // Current Scroll Position (0 based)
   INT       iSelection;    // Current Item Selection
   INT       iItemCount;    // # of items in list
   INT       iItemSize;     // # of lines per item
   PAINTPROC pPaintProc;    // proc to paint item in list
   BOOL      bShowScrollPos;// if true draws relative scroll pos thingie
   } GW;
typedef GW *PGW;


/*
 * Structure specific macros for GW's
 * Use there rather than the pgw directly
 */
#define DisplayItems(pgw)   (CYSize(pgw)/pgw->iItemSize)
#define ScrollMax(pgw)      (max (0, (int)pgw->iItemCount - (int)DisplayItems (pgw)))
#define InWindow(pgw,iLine) ((iLine) >= pgw->iScrollPos && (iLine) < pgw->iScrollPos + DisplayItems (pgw))

#define LeftOf(pgw)         (pgw->dwPos.X)
#define TopOf(pgw)          (pgw->dwPos.Y)
#define RightOf(pgw)        (pgw->dwPos.X + pgw->dwSize.X - 1)
#define BottomOf(pgw)       (pgw->dwPos.Y + pgw->dwSize.Y - 1)
#define XSize(pgw)          (pgw->dwSize.X)
#define YSize(pgw)          (pgw->dwSize.Y)

#define CLeftOf(pgw)        (pgw->dwClientPos.X)
#define CTopOf(pgw)         (pgw->dwClientPos.Y)
#define CRightOf(pgw)       (pgw->dwClientPos.X + pgw->dwClientSize.X - 1)
#define CBottomOf(pgw)      (pgw->dwClientPos.Y + pgw->dwClientSize.Y - 1)
#define CXSize(pgw)         (pgw->dwClientSize.X)
#define CYSize(pgw)         (pgw->dwClientSize.Y)

#define WinRect(pgw)        MKRECT (LeftOf(pgw),TopOf(pgw),RightOf(pgw),BottomOf(pgw))
#define ClientRect(pgw)     MKRECT (CLeftOf(pgw),CTopOf(pgw),CRightOf(pgw),CBottomOf(pgw))

/****************************************************************************
 **                                                                        **
 *                         Gnu Window Functions                             *
 **                                                                        **
 ****************************************************************************
 *
 *  General notes:
 *    > Gnu functions with attribute parameters:
 *        If the fn param looks like this: "WORD wAtt" then the 
 *        actual attribute used is based on this table:
 *
 *                wAtt     Forrground      Background
 *                -----------------------------------
 *                   0 - normal client FG  normal BG
 *                   1 - bright client FG  normal BG
 *                   2 - normal client FG  bright BG
 *                   3 - bright client FG  bright BG
 *                   4 - normal border FG  normal BG
 *                   5 - bright border FG  normal BG
 *               other - high 8 bits used as attribute
 *
 *        If you want the actual color, use GnuGetAtt to convert
 *        the above wAtt to the color value.  See below for 
 *        how to change the color values for wAtt = 0 to 5.
 *
 *    > Most fn's that have a PGW parameter will work if you pass
 *      a NULL for its pgw.  But the action performed will be screen
 *      oriented or global oriented rather than window oriented.
 *
 *    > List windows are just like static windows except that
 *      pgw->iItemCount > 0
 *
 ****************************************************************************
 */

/******************** Creation / Destruction ********************************/

/*
 * Creates a window with the given size, centered in the screen. 
 * GnuCreateWin2 lets you position the window.
 *
 * The window can be treated as either a static or a list window.
 * If using as a list, you can provide a ptr to a paint function
 * in pGnuPaintProc, or leave NULL to use default paint fn.
 * This fn returns a handle to the window.  Pass this handle to 
 * Gnu functions to paint and manipulate the window.
 */
EXP PGW GnuCreateWin (INT iYSize, INT iXSize, PAINTPROC pGnuPaintProc);


/*
 * Same as above but creates at a specific place on the screen
 * Windows are not created if they don't fit entirely on the screen.
 */
EXP PGW GnuCreateWin2 (COORD dwPos, COORD dwSize, PAINTPROC pPaintProc);



/*
 * frees associated memory, and restores previous contents of mem
 * Restores underlying screen data unless pgw->pcScreenDat has
 * been freed and set top NULL
 */
EXP PGW GnuDestroyWin (PGW pgw);


/*
 *  This BOOL determines if a window will be automatically painted
 *  at creation time. If set to false, you must call
 *     GnuClearWin (pgw, 0, TRUE)
 *  when you want to display the window.  The primary use for this
 *  is to allow you to set some structure values (ie b3Dch and b3Datt)
 *  before the window is displayed. Note that the window is there, it
 *  just hasn't been painted yet.
 */
EXP VOID GnuPaintAtCreate (BOOL bDraw);


/************************** Painting ****************************************/


/*
 * These are the fn's used to paint in a window (or screen if pgw=NULL).
 * The params are:
 *
 * pgw ------ The window, or the screen if null
 * iRow ----- Starting Row
 * iCol ----- Starting Col
 * iJust ---- Justification:
 *                   0 - left justify at iCol
 *                   1 - right justify at iCol
 *                   2 - ctr justify at iCol
 *                   3 - horiz center on win, ignore iCol
 *                   4 - vert center on win, ignore iRow
 *                   5 - center on win, ignore iRow & iCol
 *
 * wAtt  ---- Paint Attribute:   (from GnuGetAtt)
 *                   0 - normal client FG  normal BG
 *                   1 - bright client FG  normal BG
 *                   2 - normal client FG  bright BG
 *                   3 - bright client FG  bright BG
 *                   4 - normal border FG  normal BG
 *                   5 - bright border FG  normal BG
 *               other - high 8 bits used as attribute
 *
 * pszText - text to paint
 *
 * This function returns the count of characters written.
 *
 * The text may have sequences of the form \\@nn, where nn is a hex number
 * specifying an attribute. for example "Press \\@01Enter\\@00 to continue"
 * will make the word Enter print using the Hilite color.  To use a specific
 * color attribure, use \\~nn
 *
 */
EXP INT GnuPaint (PGW pgw, INT iRow, INT iCol, INT iJust, WORD wAtt, PSZ pszText);


/*
 * A printf like version of GnuPaint
 *
 */
EXP INT GnuPaintStr (PGW  pgw,      // local window ?
                     INT  iRow,     // 0 based row
                     INT  iCol,     // 0 based col
                     INT  iJust,    // 0=lt 1=rt 2=ctr-Col 4=ctr-row 5=center-win
                     WORD wAtt,     // see GnuGetAtt table
                     PSZ  psz, ...);

/*
 * A clipped version of GnuPaint
 * iMaxLen is the max # of chars that will print
 */
EXP INT GnuPaint2(PGW pgw,
                 INT  iRow,
                 INT  iCol,
                 INT  iJust,
                 WORD wAtt,
                 PSZ  pszText,
                 INT  iMaxLen);


/*
 * multiline GnuPaint, word breaks text formats within box params
 * \n in strings are handled correctly
 *
 * This fn differs from the other Paint functions in 2 ways:
 *  1> The return value is the number of lines written
 *  2> if pgw=0xFFFF this function does no painting (for getting line count)
 */
EXP INT GnuPaintBig (PGW pgw,
                     INT  iYPos,
                     INT  iXPos,
                     INT  iYSize,
                     INT  iXSize,
                     INT  iJust,
                     WORD wAtt,
                     PSZ  pszText);


/*
 * repeats CHAR
 */
EXP INT GnuPaintNChar (PGW  pgw,
                       INT  iRow,
                       INT  iCol,
                       INT  iJust,
                       WORD wAtt,
                       CHAR cChar,
                       INT  iCount);


/*
 * clears the window area (or screen if pgw=NULL), with the given
 * attribute.  If bIncludeBorder=TRUE a border is painted too.
 */
EXP VOID GnuClearWin (PGW pgw, WORD wAtt, BOOL bIncludeBorder);


/*
 * paints the border around the window (or screen if pgw=NULL)
 */
EXP VOID GnuPaintBorder (PGW pgw);


/************************** Settings ****************************************/


/*
 * moves the window to the new location
 * returns TRUE if successful
 */
EXP BOOL GnuMoveWin (PGW pgw, INT iYPos, INT iXPos);



/*
 * maps an attribute value to a attribute byte
 *
 * wAtt        values
 * ----------------------------------------------------------
 * 0-F       - Simple Map
 * 10-FF     - Nibble map BG / FG
 * 01xx-FFxx - Direct Palette Val or Nibble map each part
 *             x=0 for direct, 1 for nibble map for each part
 *
 *
 * VALUES 0-F:
 *
 *  iType (0-F)
 *  ---------------------------------------
 *  0       Normal Client FG   Normal BG
 *  1       Bright Client FG   Normal BG
 *  2       Normal Client FG   Bright BG
 *  3       Bright Client FG   Bright BG
 *  4       Error FG           Normal BG
 *  5       Error FG           Bright BG
 *  6       Normal Border FG   Normal BG
 *  7       Bright Border FG   Normal BG
 *  8...        user color mappings ...
 *  f       Shadow FG          Shadow BG
 *
 * VALUES 10-FF:
 *
 * 0xAB   where: A is Background map color
 *               B is Foreground map color
 *
 *        0  Normal Client FG       7  Shadow BG
 *        1  Bright Client FG       8  Error  FG
 *        2  Normal Border FG       9  User Color
 *        3  Bright Border FG       A  User Color
 *        4  Normal BG              .  .
 *        5  Bright BG              .  .
 *        6  Shadow FG              F  User Color
 *
 * VALUES 0xABab  where: A is background palette color or map
 *                       B is background palette color or map
 *                       a 0 if A if pal, 1 of A is mapped
 *                       b 0 if B if pal, 1 of B is mapped
 *
 * example attribute values:
 *
 *   1     - Bright Client FG on Normal BG colors
 *   2     - Normal Client FG on Bright BG
 *  0x50   - Normal Client FG on Bright BG
 *  0x48   - user color #8 FG on Normal BG
 *  0x1E00 - bright Yellow FG on blue BG
 *  0x4E10 - bright Yellow FG on Normal BG
 */
EXP BYTE GnuGetAtt (PGW pgw, WORD wAtt);


/*
 * Defines the primary color mapping
 *
 * if pgw != NULL,   the specific window's color scheme is modified
 * if pgw is NULL,   the screen color scheme is modified
 * if pgw is -1,     the global color scheme is modified
 * the global color scheme is used as the default when creating new windows
 *
 * iType specifies which color component to modify
 * iColor specifies what the new color should be for the given component (0-F)
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
EXP VOID GnuSetColor (PGW pgw, INT iType, INT iVal);

EXP INT  GnuGetColor (PGW pgw, INT iType);


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
EXP VOID GnuSetSimpleMap (INT iType, BYTE cVal);


EXP BYTE GnuGetSimpleMap (INT iType);


/*
 * returns full global or screen colormap
 */
EXP PSZ _gnuGetMap (BOOL bScreen);


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
EXP UCHAR _gnuNibbleMap (PGW pgw, INT iVal);


/*
 * simple map to attribute byte mapping
 *
 * input : iVal 0-F  
 * output: fg/bg attribute byte
 */
EXP UCHAR _gnuSimpleMap (PGW pgw, INT iVal);

EXP VOID GnuSetMonoMode (BOOL bMono);


///*
// * GnuSetColor
// * This function is used to set colors
// * if pgw is NULL, the global color scheme is modified
// * if pgw != NULL, the specific windows color scheme is modified
// * the global color scheme is used as the default when creating new windows
// *
// * iType specifies which color component to modify
// * iColor specifies what the new color should be for the given component
// *
// * iType also has several special values to perform special functions
// *
// *
// *  iType  definition
// *  -----------------------------
// *  0      Normal BG
// *  1      Bright BG
// *  2      Normal Client FG
// *  3      Bright Client FG
// *  4      Normal Border FG
// *  5      Bright Border FG
// *  6      Message FG
// *  7      Error FG
// *  8      Shadow FG
// *  9      Shadow BG
// *
// *  0x11   UserColor1
// *  .      .
// *  .      .
// *  0x1F   UserColor15
// *
// *  99     Monochrome (0=off, or HiBYTE!=0, LowNibble=FG att on brt bg)
// *
// *  100    reset pgw colors to be same as global color set
// *  200    reset global color set to be same as pgw
// *  300    reset pgw or global set to be system startup color defaults
// *  400    reset pgw or global set to be system startup mono  defaults
// *  500    reset pgw or global set to be system startup defaults (colr or mono)
// *  0xFFFF Reset colors to defaults:
// *             if pgw <> NULL, works like iType=100
// *             if pgw == NULL, works like iType=500
// *
// */
//INT  GnuSetColor (PGW pgw, INT  iType, INT  iColor);
//
//INT  GnuGetColor (PGW pgw, INT  iType);
//
//
//
///*
// *  wAttIdx  FG color           BG color
// *  -------------------------------------------
// *  0        Normal Client FG   Normal BG
// *  1        Bright Client FG   Normal BG
// *  2        Normal Client FG   Bright BG
// *  3        Bright Client FG   Bright BG
// *  4        Normal Border      Normal BG
// *  5        Bright Border      Normal BG
// *  6        Message FG         Normal BG
// *  7        Message FG         Bright BG
// *  8        Error FG           Normal BG
// *  9        Error FG           Bright BG
// *
// *  15 (F)   Shadow FG          Shadow BG
// *
// *  0xXY     Y User FG          X User BG
// *           (Y=1 - F)          (X=1 - F)
// *
// */
//INT  GnuGetAtt (PGW pgw, INT  wAttIDX);



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
EXP INT GnuSetBorderChars (PGW pgw, PSZ pszBC);


/*
 * This paints a box with the default box chars
 * This is used internally to draw the borders
 * use NULL for pszBDRCHR unless you know what your doing
 */
EXP INT GnuDrawBox (COORD dwPos, COORD dwSize, PSZ pszBorder, BYTE cAtt);


/*
 * Screen relative if pgw=NULL
 * 0 based Top Left origin
 * PMET stores max extents
 * This is really a Scr fn with a Gnu param option
 */
EXP VOID GnuMoveCursor (PGW pgw, COORD dwPos);


/*
 * Returns the current position of the cursor
 * 0 based Top Left origin
 * Screen relative if pgw is NULL
 */
EXP COORD GnuGetCursor (PGW pgw);


/************************ List Specific *************************************/


/*
 * calls the pGnuPaintProc to paint iLine. iLine is line index
 * if list, or window line if static.
 * paints all items (and entire window) if iLine = 0xFFFF
 * returns TRUE if item is visible
 */
EXP BOOL GnuPaintWin (PGW pgw, INT iLine);


/*
 * for list windows only.
 * scrolls if necessary to ensure that iLine is in view port.
 * returns TRUE if it had to scroll
 */
EXP BOOL GnuMakeVisible (PGW pgw, INT iLine);


/*
 * if a list window, this scrolls so that iLine is top line.
 * if a static window, no effect.
 */
EXP BOOL GnuScrollTo  (PGW pgw, INT iLine);


/*
 * scrolls iLines up or down,
 * any window type
 * if list window, changes iScrollPos.
 */
EXP VOID GnuScrollWin (PGW pgw, INT iLines, BOOL bUp);


/*
 * selects iLine. repaints new and old selection.
 * if bOnScreen = TRUE, uses GnuMakeVisible.
 * use iLine = 0xFFFF to deselect all
 */
EXP BOOL GnuSelectLine (PGW pgw, INT iLine, BOOL bOnScreen);


/****************************************************************************
 **                                                                        **
 *                           Gnu Utilities                                  *
 **                                                                        **
 ****************************************************************************/

/*
 * default processing of the movement keys
 * for listwindows
 * returns TRUE if the key was processed.
 */
EXP BOOL GnuDoListKeys (PGW pgw, int cExtKey);


///*
// * This fn creates a window to change the color mapping
// * of the window pgw or the default colors if
// * pgw is NULL
// */
//BOOL GnuSelectColorsWindow (PGW pgw);

EXP BOOL GnuSelectColorsWindow2 (PPSZ ppszLabels, PINT piColors);


/*
 * This fn creates a window to modify the colors
 * by directly modifying the VGA color registers
 * Returns TRUE if the colors were changed
 * Returns FALSE if the user aborts the winfow
 */
EXP BOOL GnuModifyColorsWindow (VOID);


/*
 * This fn creates a window where the user selects a file
 * pszMatch contains the wildcard string to load from and
 *  may have multiple filespecs separated by semi-colons.
 * pszHeader is the string to display at the top of the window.
 * pszWorkBuff is a working buffer that must be at least
 *  4 * max entries bytes (entries are drives + dirs + files).
 * On return pszFile will contain the filename and any relative path
 *  needed to load the file.
 * This fn returns 0 if the user aborted the window.
 */
EXP INT GnuFileWindow (PSZ pszFile, PSZ pszMatch, PSZ pszHeader, PSZ pszWorkBuff);


/*
 * This is a simple message box that displays 2 lines of text,
 * a header, and tells the user to press esc to continue.
 */
EXP INT GnuMsg (PSZ pszHeader, PSZ psz1, PSZ psz2);


/*
 * This is another message box
 * This can display a paragraph of data
 * It will size itself accordingly
 * You tell this box which chars to accept
 * The CHAR entered is returned
 */
EXP INT GnuMsgBox (PSZ pszHeader, PSZ pszFooter, PSZ pszChars, PSZ pszText);


/*
 * like above with iJust and wAtt added to preserve freshness
 */
EXP INT GnuMsgBox2 (PSZ  pszHeader,
                PSZ  pszFooter,
                PSZ  pszChars,
                INT  iYSize, // 0 = auto calculate
                INT  iXSize, // 0 = auto calculate
                INT  iJust,
                WORD wAtt,
                PSZ  pszText);


/*
 * pops up a combobox
 * user selects a choice from a list
 *  ppsz is the array of string choices (last entry must be NULL)
 *  iY,iX is the cell position, call with NULL for free combo
 *  iYSize is the size of the combo client, 0 = as large as needed/possible
 *  iStartSel is the initial selected line -1 = no start selection
 * return is 0 based index of selected row, or -1 if user hits esc
 */
EXP INT GnuComboBox (PPSZ ppsz, INT iY, INT iX, INT iYSize, INT iStartSel);


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
EXP PSZ GnuEditComboBox (PSZ pszDat, PPSZ ppsz, 
                     INT iY, INT iX,
                     INT iYSize, PINT piStartSel);


/*
 * Pops up a window and allows Hex Editing
 * of the data in pszBuff
 */
EXP INT GnuHexEdit (PSZ pszBuff, INT iOffset, INT iLines);


/*
 * This fn causes a gnu window to have a shimmer effect.
 * pgw is the window to work on
 * wAttrib is the color to use as the hilite
 * iMax is the max number of calls before it repeats
 */
EXP VOID WinShimmer (PGW pgw, BYTE cMask, BYTE cAttrib, INT iMax);


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
EXP WORD WinUsedColorList (PGW pgw, INT iColorsToCheck);

/****************************************************************************
 **                                                                        **
 *                           Gnu Internal fns                               *
 **                                                                        **
 ****************************************************************************/

/*
 * The following fns are mostly for internal use
 * If you use these and give bad params, you deserve to crash and burn.
 */
EXP INT  gnuDefPaintProc (PGW pgw, INT iIndex, INT iLine);

EXP PCHAR_INFO gnuGetScreenDat (PGW pgw);
EXP BOOL       gnuPutScreenDat (PGW pgw, BOOL bIncludeShadowArea);
EXP VOID       gnuFreeScreenDat (PGW pgw);
EXP VOID       gnuSwapScreenDat (PGW pgw, BOOL bIncludeShadowArea);

EXP COORD gniWinToScreen (PGW pgw, COORD dwPos);
EXP COORD gnuScreenToWin (PGW pgw, COORD dwPos);
EXP COORD gnuClientWinToScreen (PGW pgw, COORD dwPos);
EXP COORD gnuScreenToClientWin (PGW pgw, COORD dwPos);

EXP VOID  gnuShowScrollPos (PGW pgw, INT iNewLine, BOOL bShow);
EXP VOID  gnuDrawShadow (PGW pgw);
EXP BOOL  gnuFixScroll (PGW pgw);
EXP PSZ   gnuGetBorderChars (VOID);


//} // extern C

#endif  // GNUWIN_INCLUDED


