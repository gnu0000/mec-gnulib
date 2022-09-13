/*
 * Kbd1.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides the KeyEditCell functionality
 *
 * KeyEditCell is a fn used to get a line of input from the keyboard.
 * it has full editing capabilities.
 *
 * KeyEditCellMode modifies the behavior of KeyEditCell by allowing you
 * to specify what keys terminate data entry, and to force chars uppercase
 *
 * KeyAddEditTrap modifies the behavior of KeyEditCell by allowing you to
 * bind a key to a function (predefined or yours).  
 * Note that any key traps added by KeyAddTrap take precidence over this.
 *
 * here are the predefined functions:
 *
 *  ec_left    (INT c)  ec_end      (INT c)    
 *  ec_right   (INT c)  ec_bksp     (INT c)   
 *  ec_ctleft  (INT c)  ec_esc      (INT c)
 *  ec_ctright (INT c)  ec_default  (INT c)
 *  ec_del     (INT c)  ec_start    (INT c)
 *  ec_ins     (INT c)  ec_AbortEnd (INT c)
 *  ec_home    (INT c)  ec_OKEnd    (INT c)
 *                      ec_EachKey  (INT c)
 *
 * here are the values to play with in the key functions:
 *
 * *--- These 3 globals set by KbdEditCellMode ---*
 * PSZ  EC_pszNORMAL
 * PSZ  EC_pszEXTENDED
 * BOOL EC_bUPPCASE
 *                               
 * *--- These set via parameters to EditCell ---*
 * PSZ  ec_psz;          // passed in string buffer
 * INT  ec_iYPOS;        // passed Y pos
 * INT  ec_iXPOS;        // passed X pos
 * INT  ec_iMAX;         // passed max edit size 
 * INT  ec_iCMD;         // passed how to start
 *
 * *--- Working variables in EditCell ---*
 * CHAR ec_szTMP[256];   // used by KeyEditCell
 * INT  ec_iLEN;         // current str len   
 * INT  ec_iPOS;         // current cursor pos
 * BYTE ec_cCELL[2];     // start screen cell 
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"
#include "gnumisc.h"
#include "gnukbd.h"
#include "kbd.h"

extern "C"
{

#define pszDefNormal   "\x1B\x0D"

static PKTRAP EC_pkTRAPS = NULL;
static BOOL bInitialized = FALSE;


/*--- These 3 globals set by KbdEditCellMode ---*/
static PSZ  EC_pszNORMAL   = pszDefNormal;
static PSZ  EC_pszEXTENDED = NULL;
static BOOL EC_bUPPCASE    = FALSE;
static BOOL EC_bPASSMODE   = FALSE;
										
/*--- These set via parameters to EditCell ---*/
EXP PSZ        ec_psz;          // passed in string buffer
EXP COORD      ec_dwPOS;        // start position passed in
EXP INT        ec_iMAX;         // passed max edit size 
EXP INT        ec_iCMD;         // passed how to start

/*--- Working variables in EditCell ---*/
EXP CHAR       ec_szTMP [256];  // used by KeyEditCell
EXP INT        ec_iLEN;         // current str len   
EXP INT        ec_iPOS;         // current cursor pos
EXP CHAR_INFO  ec_cCELL;        // base screen, a space and a screen attribute
EXP CHAR       ec_cKEYCHAR='*'; // password protection char



/*******************************************************************/
/*                                                                 */
/* Default key functions                                           */
/*                                                                 */
/*******************************************************************/


static void _wrtstr (void)
	{
	if (EC_bPASSMODE)
		ScrWriteNChar (ec_cKEYCHAR, ec_dwPOS, ec_iLEN);
	else
		ScrWriteStr (ec_szTMP, ec_dwPOS, ec_iLEN);
	}


INT ec_left (INT c)   /*--- 0x14B ---*/
	{
	c = 0;
	if (ec_iPOS)
		ec_iPOS--;
	else
		GNUBeep (0);
	return 0;
	}

INT ec_right (INT c)  /*--- 0x14D ---*/
	{
	c = 0;
	if (ec_iPOS < ec_iLEN)
		ec_iPOS++;
	else
		GNUBeep (0);
	return 0;
	}


INT ec_del (INT c)    /*--- 0x153 ---*/
	{
	if (ec_iPOS >= ec_iLEN)
		GNUBeep (0);
	else
		{
		for (c=ec_iPOS; c<ec_iLEN; c++)
			ec_szTMP[c] = ec_szTMP[c+1];
		ec_iLEN -= !!ec_iLEN;
		ScrWriteNChar (' ', ec_dwPOS, ec_iMAX);
		_wrtstr ();
		}
	return 0;
	}


INT ec_ins (INT c)    /*--- 0x152 ---*/
	{
	if (ec_iPOS >= ec_iMAX)
		GNUBeep (0);
	else
		{
		for (c=ec_iLEN+1; c>ec_iPOS; c--)
			ec_szTMP[c] = ec_szTMP[c-1];
		ec_szTMP[ec_iPOS] = ' ';
		ec_szTMP[ec_iMAX] = '\0';
		ec_iLEN = min (ec_iMAX , ec_iLEN+1);
		_wrtstr ();
		}
	return 0;
	}

INT ec_home (INT c)   /*--- 0x147 ---*/
	{
	c = 0;
	ec_iPOS = 0;
	return 0;
	}

INT ec_end (INT c)    /*--- 0x14F ---*/
	{
	c = 0;
	ec_iPOS = ec_iLEN;
	return 0;
	}

INT ec_bksp (INT c)   /*--- 8   ---*/
	{
	if (!ec_iPOS)
		GNUBeep (0);
	else
		{
		for (c=ec_iPOS; c<=ec_iLEN; c++)
			ec_szTMP[c-1] = ec_szTMP[c];
		ec_iLEN -= !!ec_iLEN;
		ScrWriteNChar (' ', ec_dwPOS, ec_iMAX);
		_wrtstr ();
		ec_iPOS--;
		}
	return 0;
	}

INT ec_ctleft (INT c)   
	{
	for (c=ec_iPOS; c; c--)              // skip curr word
		if (strchr (" \t,", ec_szTMP[c]))
			break;
	for (; c; c--)                       // skip whitespace
		if (!strchr (" \t,", ec_szTMP[c]))
			break;
	ec_iPOS = c;
	return 0;
	}

INT ec_ctright (INT c)  
	{
	for (c=ec_iPOS; c < ec_iLEN; c++)    // skip curr word
		if (strchr (" \t,", ec_szTMP[c]))
			break;
	for (; c < ec_iLEN; c++)             // skip whitespace
		if (!strchr (" \t,", ec_szTMP[c]))
			break;
	ec_iPOS = c;
	return 0;
	}

INT ec_esc (INT c)
	{
	return c;
	}


INT ec_default (INT c)
	{
	INT i;

	if (ec_iPOS >= ec_iMAX || c > 0x100)
		{
		GNUBeep (0); 
		return 0;
		}

	if (EC_bUPPCASE)
		c = toupper (c);

	for (i=ec_iLEN+1; i>ec_iPOS; i--)
		ec_szTMP[i] = ec_szTMP[i-1];
	ec_szTMP[ec_iPOS] = (CHAR) c;
	ec_szTMP[ec_iMAX] = '\0';
	ec_iPOS++;
	ec_iLEN = min (ec_iMAX , ec_iLEN+1);
	_wrtstr ();
	return 0;
	}


INT ec_start (INT c)
	{
	return c = 0;
	}


/*
 * return us actual value returned from fn
 */
INT ec_AbortEnd (INT c)
	{
	ScrPopCursor ();
	return c = 0;
	}


/*
 * return us actual value returned from fn
 */
INT ec_OKEnd (INT c)
	{
	strcpy (ec_psz, ec_szTMP);
	ScrPopCursor ();
	return c;
	}


/*
 * return us actual value returned from fn
 */
INT ec_EachKey (INT c)
	{
	return 0;
	}



/***************************************************************************/
/*                                                                         */
/* Internal Key mapping maintenance fns                                    */
/*                                                                         */
/***************************************************************************/

/*
 * finds the key trap
 * if not found and pfnTrap is NULL, fn returns NULL
 * if not found and pfnTrap is !NULL, new node is created
 */
static PKTRAP FindEditTrapNode (PKTRAP pkTree, INT iKey, BOOL bCreate)
	{
	PKTRAP pkMatch;

	if (!pkTree)
		{
		if (!bCreate)
			return NULL;
		pkTree = (PKTRAP) calloc (1, sizeof (KTRAP));
		pkTree->left = pkTree->right = NULL;
		pkTree->iKey = iKey;
		return pkTree;
		}
	if (iKey < pkTree->iKey)
		{
		pkMatch = FindEditTrapNode (pkTree->left, iKey, bCreate);
		pkTree->left = (pkTree->left ? pkTree->left : pkMatch);
		}
	else if (iKey > pkTree->iKey)
		{
		pkMatch = FindEditTrapNode (pkTree->right, iKey, bCreate);
		pkTree->right = (pkTree->right ? pkTree->right : pkMatch);
		}
	else
		pkMatch = pkTree;
	return pkMatch;
	}


static PKEYFN FindEditTrap (INT iKey)
	{
	PKTRAP pk;

	if (pk = FindEditTrapNode (EC_pkTRAPS, iKey, FALSE))
		return pk->pfnTrap;
	return NULL;
	}


static INT ExecTrap (INT iTrap, INT iKey)
	{
	PKEYFN pfn;

	if (!(pfn = FindEditTrap (iTrap)))
		return 0;
	  
	return pfn (iKey);
	}


static BOOL _keyAddEditTrap (INT iKey, PKEYFN pfnTrap)
	{
	PKTRAP pk;

	pk = FindEditTrapNode (EC_pkTRAPS, iKey, TRUE);
	if (!EC_pkTRAPS)
		EC_pkTRAPS = pk;
	pk->pfnTrap = pfnTrap;
	return TRUE;
	}


/***************************************************************************/
/*                                                                         */
/* Internal Init function                                                  */
/*                                                                         */
/***************************************************************************/

void ec_Init (void)
	{
	/*--- Initialize trap struct if needed ---*/
	if (bInitialized)
		return;

	_keyAddEditTrap (EC_DEFAULT ,  ec_default );
	_keyAddEditTrap (EC_START   ,  ec_start   );
	_keyAddEditTrap (EC_OKEND   ,  ec_OKEnd   );
	_keyAddEditTrap (EC_ABORTEND,  ec_AbortEnd); 
	_keyAddEditTrap (EC_EACHKEY ,  ec_EachKey );

	_keyAddEditTrap (K_LEFT     ,  ec_left    );
	_keyAddEditTrap (K_RIGHT    ,  ec_right   );
	_keyAddEditTrap (K_DEL      ,  ec_del     );
	_keyAddEditTrap (K_INS      ,  ec_ins     );
	_keyAddEditTrap (K_HOME     ,  ec_home    );
	_keyAddEditTrap (K_END      ,  ec_end     );
	_keyAddEditTrap (K_BSP      ,  ec_bksp    );
	_keyAddEditTrap (K_ESC      ,  ec_esc     );
	_keyAddEditTrap (K_CLEFT    ,  ec_ctleft  );
	_keyAddEditTrap (K_CRIGHT   ,  ec_ctright );

	bInitialized = TRUE;
	}


/***************************************************************************/
/*                                                                         */
/* User Functions                                                          */
/*                                                                         */
/***************************************************************************/


BOOL KeyAddEditTrap (INT iKey, PKEYFN pfnTrap)
	{
	if (!bInitialized)
		ec_Init ();
		
	return _keyAddEditTrap (iKey, pfnTrap);
	}


/*
 * Set EditCell mode variables:
 * pszNormalReturns   = list of normal keys to return on   (NULL=no change)
 * pszExtendedReturns = list of extended keys to return on (NULL=no change)
 * bUppCase           = 0 or 1 force keys to uppercase     ( >1 =no change)
 *
 * call with (NULL, NULL, 2) to reset all three to default values
 * call with (NULL, NULL, 3) to enable password entry mode
 * call with (NULL, NULL, 4) to disable password entry mode
 */
void KeyEditCellMode (PSZ  pszNormalReturns,
									 PSZ  pszExtendedReturns,
									 BOOL bUppCase)
	{
	/*--- reset condition ---*/
	if (!pszNormalReturns && !pszExtendedReturns)
		{
		if (bUppCase == 2)   
			{
			EC_pszNORMAL   = pszDefNormal;
			EC_pszEXTENDED = NULL;
			EC_bUPPCASE    = FALSE;
			}
		if (bUppCase == 3)
			EC_bPASSMODE   = TRUE;
		if (bUppCase == 4)
			EC_bPASSMODE   = FALSE;
		}
	if (pszNormalReturns)
		EC_pszNORMAL = pszNormalReturns;
	if (pszExtendedReturns)
		EC_pszEXTENDED = pszExtendedReturns;
	if (bUppCase < 2)
		EC_bUPPCASE = bUppCase;
	}


/* String entry fn
 * This fn provides full editing capabilities.
 * Current screen attributes are used
 *
 * If x and/or y are -1 then the current col and/or row is used.
 *
 * uCmd options
 * ------------
 *  0 ...... Clear psz, edit
 *  1 ...... edit psz, start cursor at left
 *  2 ...... edit psz, start cursor at right
 *  other .. Clear psz, process uCmd as 1st key
 *
 * This fn returns the key that caused the exit. If it was one of 
 * the pszExtendedReturns keys, 0x100 is added to its 2nd byte
 *
 * If the user presses <Esc> this fn returns 0 and doesn't modify psz
 */
INT KeyEditCell (PSZ   psz,   // starting string (and return)
					  COORD dwPos, // starting pos
					  INT   iMax,  // max size of edit field
					  INT   iCmd)  // how to start
	{
	INT    c;
	PKEYFN pfn;
	INT    i;

	if (!bInitialized)
		ec_Init ();

	ec_psz   = psz;              
	ec_iMAX  = iMax;             
	ec_iCMD  = iCmd;             

	if (dwPos.X == -1) dwPos.X = ScrGetCursorPos ().X;
	if (dwPos.Y == -1) dwPos.Y = ScrGetCursorPos ().Y;
	ec_dwPOS = dwPos;

	ScrPushCursor (TRUE);

	/*--- clear out the screen space first ---*/
	ec_cCELL = ScrReadCell (dwPos);           // read screen attribute
	ec_cCELL.Char.AsciiChar = ' ';            // set char to space
	ScrWriteNCell (ec_cCELL, dwPos, ec_iMAX); // write space/attribute string

	/*--- set user buffer and working vars ---*/
	ScrSetCursorPos (dwPos);
	strcpy (ec_szTMP, ec_psz);
	ec_szTMP[ec_iMAX] = '\0';
	*ec_szTMP = (ec_iCMD==1 || ec_iCMD==2 ? *ec_szTMP : (CHAR)'\0');
	ec_iLEN = strlen (ec_szTMP);
	ec_iPOS = (ec_iCMD==1 ? 0 : ec_iLEN);

	_wrtstr ();
	if (ExecTrap (EC_START, iCmd))   /*--- START ---*/
		return 0;

	while (TRUE)
		{
		ScrSetCursorPos (MKCOORD (ec_dwPOS.X + ec_iPOS, ec_dwPOS.Y));

		c = (ec_iCMD > 2 ? ec_iCMD : KeyGet (FALSE));
		ec_iCMD = 0;

		if (!(pfn = FindEditTrap (c)))
			{
			/*--- Normal or extended return key ? ---*/
			if ((c < 0x100 && EC_pszNORMAL  && strchr (EC_pszNORMAL,   c)) ||
				 (c > 0xFF && EC_pszEXTENDED && strchr (EC_pszEXTENDED, c & 0xFF)))
				return (ExecTrap (EC_OKEND, c));

			pfn = FindEditTrap (EC_DEFAULT);    /*--- DEFAULT ---*/
			}
		if (i = pfn (c))
				return (ExecTrap ((i==27 ? EC_ABORTEND: EC_OKEND), c));

		if (pfn = FindEditTrap (EC_EACHKEY)) /*--- EACH KEY ---*/
			if (i = pfn (c))
				return (ExecTrap ((i==27 ? EC_ABORTEND: EC_OKEND), c));
		}
	}

}