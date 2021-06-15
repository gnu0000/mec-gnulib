/*
 *
 * GnuGrid.h
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
 * PGW  GrdCreateWin (iYSize, iXSize, pfnPaint)
 * PGW  GrdCreateWin2 (iYPos, iXPos, iYSize, iXSize, pfnPaint)
 * PGW  GrdDestroyWin (pgw)
 * PGRINFO GrdGetPGR (pgw)
 * INT  GrdResizeCol (pgw, iCol, iColSize)
 * INT  GrdFreezeCol (pgw, iCols)
 * INT  GrdDoKeys (pgw, c)
 * INT  GrdRefreshLabels (pgw)
 * BOOL GrdSelectCell (pgw, iNewY, iNewX, bOnScreen)
 * void GrdPaintCell (pgw, iY, iX, bMakeVisible)
 * void GrdGetPos (pgw, ppos)
 * INT  GrdMakeVisible (pgw, iY, iX)
 *
 * INT  _cdecl pfnDefaultPaintCell (pgw, ppos, bLabel)
 * INT  GrdPaintLabels (pgw, iStart, iEnd)
 * INT  GrdHorizMove (pgw, iCols)
 * INT  GrdPaintColumns (pgw, iStart, iEnd)
 *
 *
 * INT  GrdComboBox (pDat, ppszLabels, iYPos, iXPos, iYSize, iXSize, iStartY,
 *                   pszHeader, pszFooter)
 *
 *
 ****************************************************************************
 *
 * Using Grid Windows
 * ------------------
 *
 * You must include GnuWin.h first!
 *
 * This module rides on top of the base functionality of Gnu Windows.
 * Gnu Windows provide for line oriented vertical scrolling.
 * This module adds horizontal scrolling, cell selection, and key handling.
 * This is a minimal implementation and is not very efficient.  For example
 * when the user moves the selection up or down 1 line, the 2 full lines
 * are repainted rather than just the 2 cells.  This is because the code
 * here relies on gnuwin functionality wherever possible.
 * 
 * Example:
 * This example creates a grid window, displays sample data, and allows
 * the user to edit cells via a popup combo box.
 * 
 *  *
 *  * testgrid.c
 *  *
 * #include <stdio.h>
 * #include <stdlib.h>
 * #include <string.h>
 * #include <GnuType.h>
 * #include <GnuKbd.h>
 * #include <GnuScr.h>
 * #include <GnuWin.h>
 * #include <GnuGrid.h>
 * #include <GnuMisc.h>
 * 
 * PPSZ *pppszDATA;
 * 
 * PSZ pszCHOICES [] =
 *             {"Field",        "Test",       "String",
 *              "Hoek",         "Ren",        "Stimpazoid",
 *              "Tralfagalore", "Weaselteat", "XXX",
 *              "YYY",          "ZZZ",        "Hello",
 *              NULL};
 * 
 *  *
 *  * call this fn to pop up a combobox over the current cell, the cell
 *  * is updated to the users selection
 *  *
 * void EditCell (PGW pgw)
 *    {
 *    POS  pos;
 *    PSZ  *ppsz, **pppsz;
 *    INT  iSel;
 * 
 *    GrdGetPos (pgw, &pos);
 *    iSel = GnuComboBox (pszCHOICES, CYPos(pgw)+pos.iYWin, 
 *                                    CXPos(pgw)+pos.iXWin, 0, 0);
 *    if (iSel == -1) // esc?
 *       return;
 *    pppsz = (PPSZ *) pgw->pUser1;        // get the ptr and 
 *    ppsz  = pppsz[pos.iYData];           // update the cell data
 *    ppsz[pos.iXData] = pszCHOICES[iSel]; //
 *    GrdPaintCell (pgw, pos.iYData, pos.iXData, FALSE);
 *    }
 * 
 * int main (int argc, char *argv[])
 *    {
 *    PGRINFO pgr;
 *    PGW     pgw;
 *    INT     c, i, j;
 * 
 *    ScrInitMetrics ();
 * 
 *    *--- dummy up some data into pppszDATA ---*
 *    pppszDATA = (PPSZ *) malloc (50 * sizeof (PPSZ));
 *    for (i=0; i<50; i++)
 *       {
 *       pppszDATA[i] = (PPSZ) malloc (50 * sizeof (PSZ));
 *       for (j=0; j<50; j++)
 *          pppszDATA[i][j] = pszCHOICES[(rand () / 1000) % 10];
 *       }
 *    pgw = GrdCreateWin (10, 50, NULL); // create the grid window w/default paint
 *    pgw->bShowScrollPos = TRUE;        // show the scroll pos
 *    pgw->pszFooter  = strdup ("[<Esc> to exit]");
 *    pgw->iItemCount = 50;              // set row count
 *    pgw->pUser1     = pppszDATA;       // set data ptr (for default paint fn)
 *    pgw->iSelection = 0;               // row selection
 * 
 *    pgr = GrdGetPGR (pgw);             // get gris specific struct
 *    pgr->iColCount = 50;               // column count
 * 
 *    for (i=0; i<pgr->iColCount; i++)   // resize columns
 *       GrdResizeCol (pgw, i, 7);       //
 * 
 *    GnuPaintWin (pgw, (INT )-1);       // paint window client
 *    GrdRefreshLabels (pgw);            // paint labels
 *    GnuPaintBorder (pgw);              // paint border
 * 
 *    while (TRUE)                       // handle keyboard input
 *       {                               //
 *       c = KeyGet (TRUE);              // get key
 *       if (c == 0x1b)                  // <Esc> to exit
 *          break;                       //
 *       else if (c == 0x0D)             // <Enter> to edit cell
 *          EditCell (pgw);              //
 *       else if (!GrdDoKeys (pgw, c))   // Scroll handler
 *          GNUBeep (0);                    // unknown key
 *       }
 *    GrdDestroyWin (pgw);               // cleanup
 *    return 0;                          //
 *    }
 *
 */

#if !defined (GNUGRID_INCLUDED)
#define GNUGRID_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif


//extern "C"
//{

/*
 * position struct
 * This structure is passed to the cell paint fn
 * you can also get a pos referring to the selected cell by calling GrdGetPos
 */
typedef struct
   {
   COORD dwData;  // data 
   COORD dwWin ;  // window position
   INT   iLen;
   INT   iClip;
   INT   iJust;
   WORD  wAtt;
   } POS;
typedef POS *PPOS;


/*
 * typedef for the cell paint fn
 */
typedef INT  (*PAINTCELL) (struct _GW *pgw, PPOS ppos, BOOL bLabel);

/*
 * typedef for the cell paint text fn
 */
EXP typedef PSZ (*GETTEXT) (struct _GW *pgw, PPOS ppos, INT  uLabel);

/*
 * typedef for the cell paint text fn
 */
EXP typedef INT  (*GETATT) (struct _GW *pgw, PPOS ppos, INT  uLabel);


/*
 * this structure is created as part of a grid window
 * you can get a ptr to this struct via GrdGetPGR
 */
typedef struct
   {
   INT       iColCount;   // # of columns
   INT       iSelCol;     // The selected column
   INT       iScrollCol;  // The leftmost column (0 based)
   INT       iFrozen;     // number of frozen columns
   INT       iLeftEdge;   // left edge of 1st non frozen col
   PBYTE     pcSize;      // array of column sizes
   PBYTE     pcID;        // array of column ID's for user use

   INT       iLabelPos;   // label line relative to the window top
   INT       iLabelSize;  // #lines for label (0 = no labels)
   WORD      wLabelAtt;   // attribute for label line
   PPSZ      ppszLabels;  // used by default paint proc (must set manually)

   WORD      wNormalAtt;  // attribute for label line
   WORD      wSelectAtt;  // attribute for label line

   PVOID     pUser1;      // a good place for col specific paint fns
   PVOID     pUser2;      // a good place for col specific edit  fns
   PVOID     pUser3;      //
   PAINTCELL pfnPaint;    // the paint fn
   GETTEXT   pfnText;     // the get text fn for the paint fn
   GETATT    pfnAtt;      // fn to get attrib for col
   } GRINFO;
typedef GRINFO *PGRINFO;


//#define LYPos(pgw)   (CTopOf(pgw)+((PGRINFO)pgw->pUser3)->iLabelPos)
//#define LYSize(pgw)  (((PGRINFO)pgw->pUser3)->iLabelSize)

#define LTopOf(pgw)    (CTopOf(pgw)+((PGRINFO)pgw->pUser3)->iLabelPos)
#define LYSize(pgw)    (((PGRINFO)pgw->pUser3)->iLabelSize)
#define LBottomOf(pgw) (LTopOf(pgw)+LYSize(pgw)-1)

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/

/*
 * Creates a grid window
 * The Gnulib window libraries only handle the rows
 * so we add the framework to handle columns
 */
EXP PGW GrdCreateWin (INT  iYSize, INT  iXSize, PAINTCELL pfnPaint);


/*
 * Creates a grid window
 * This is like GrdCreateWin but allows you to specify
 * the window position
 */
EXP PGW GrdCreateWin2 (COORD dwPos, COORD dwSize, PAINTCELL pfnPaint);
       

/*
 * Destroys a grid window
 *
 */
EXP PGW GrdDestroyWin (PGW pgw);


/*
 * returns a ptr to the grid specific attributes
 *
 */
EXP PGRINFO GrdGetPGR (PGW pgw);


/*
 * Resizes a grid column
 * Call this once for each column you want to resize
 * The default size is 16 chars
 */
EXP INT  GrdResizeCol (PGW pgw, INT  iCol, INT  iColSize);

/*
 * Call this fn to freeze the 1st uCols columns
 * You must repaint the window after this
 */
EXP INT  GrdFreezeCol (PGW pgw, INT  iCols);

/*
 * Default key Handling for grid windows.
 *
 */
EXP INT  GrdDoKeys (PGW pgw, INT  c);


/*
 * Sets up labels for painting by the default paint proc
 *
 */
EXP void GrdSetupLabels (PGW pgw, INT uPos, INT uSize, WORD wAtt, PPSZ ppszLabels);

/*
 * repaints the labels
 *
 */
EXP INT  GrdRefreshLabels (PGW pgw);


/*
 * Call to select a cell
 * if bOnScreen is TRUE then the screen is scrolled to make it visible
 */
EXP BOOL GrdSelectCell (PGW pgw, INT iNewY, INT iNewX, BOOL bOnScreen);


/*
 * Call to paint a cell
 * if bOnScreen is TRUE then the screen is scrolled to make it visible
 */
EXP void GrdPaintCell (PGW pgw, INT iY, INT iX, BOOL bMakeVisible);


/*
 * gets the current data / screen coordinates
 *
 */
EXP void GrdGetPos (PGW pgw, PPOS ppos);


/*
 * makes a cell visible
 *
 */
EXP INT GrdMakeVisible (PGW pgw, INT iY, INT iX);


/**********************************************************************/


/*
 * This is the default paint procedure for a grid
 * If the paint fn is passed as NULL during the create call, this is used.
 *
 * This paint proc assumes:
 * the pgw->uLineCount and pgr->iColCount are valid
 *
 * the pUser1 points to an array of pointers
 * These pointers point to either a PPSZ or a structure with  
 * PSZ's as its first iCol elements
 */
EXP INT pfnDefaultPaintCell (PGW pgw, PPOS ppos, BOOL bLabel);


/*
 *
 *
 */
EXP INT GrdPaintLabels (PGW pgw, INT iStart, INT iEnd);


/*
 *
 *
 */
EXP INT GrdHorizMove (PGW pgw, INT iCols);


/*
 *
 *
 */
EXP INT GrdPaintColumns (PGW pgw, INT iStart, INT iEnd);



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
EXP INT GrdComboBox (PVOID pDat,      PPSZ ppszLabels,
                     INT   iYPos,     INT  iXPos, 
                     INT   iYSize,    INT  iXSize,
                     INT   iStartY,
                     PSZ   pszHeader, PSZ pszFooter);


//} // extern C

#endif // GNUCRC_INCLUDED
