/*
 * Kbd0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This module provides the functions for
 * the GnuKbd module
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"
#include "gnumisc.h"
#include "gnukbd.h"
#include "kbd.h"

#define RECBUFFSIZE       1024

INT AutoRecordTrapFn (INT c);
INT AutoPlayTrapFn   (INT c);

static KTRAP  pkDEFTRAPS[] = {{K_CF1, AutoRecordTrapFn},
										{K_CF2, AutoPlayTrapFn}};

static PKTRAP pkTRAPS = pkDEFTRAPS;
static INT    iTRAPS = 2;


static INT   iRECMODE    = 0;      // recording mode: 1=recording 2=playing 0=nothing
static PSZ   pszREC      = NULL;   // recording key buffer
static INT   iRECSIZE    = 0;      // recording key buffer size
static INT   iRECPTR     = 0;      // recording key buffer ptr
static INT   iPLAYDELAY  = 0;      // recording playback delay in ms.

static BOOL  bINTRAP = FALSE;


/***************************************************************************/
/*                                                                         */
/*  Lowest Level Keyboard I/O Routines                                     */
/*                                                                         */
/***************************************************************************/


BOOL KeyAddTrap (INT iKey, PKEYFN pfnTrap)
	{
	INT i;

	/*--- if were gonna modify data, we must move it to the data seg ---*/
	if (pkTRAPS == pkDEFTRAPS)
		{
		pkTRAPS = (PKTRAP) malloc (sizeof (KTRAP) * 2);
		memcpy (pkTRAPS, pkDEFTRAPS, sizeof (KTRAP) * 2);
		}

	/*--- see if it is a change ---*/
	for (i=0; i<iTRAPS; i++)
		if (pkTRAPS[i].iKey == iKey)
			{
			pkTRAPS[i].pfnTrap = pfnTrap;
			return FALSE;
			}
	pkTRAPS = (PKTRAP) realloc (pkTRAPS, (iTRAPS + 1) * sizeof (KTRAP));
	pkTRAPS[iTRAPS].iKey    = iKey;
	pkTRAPS[iTRAPS].pfnTrap = pfnTrap;
	++iTRAPS;
	return TRUE;
	}



INT k_getch (void)
	{
	INT i;

	if (!iRECMODE)
		return getch ();

	if (iRECPTR >= iRECSIZE || (iRECMODE == 2 && kbhit ())) // new
		{
		iRECMODE = 0;
		return getch ();
		}

	if (iRECMODE == 1)
		pszREC[iRECPTR++] = (CHAR)(i = getch ());
	else /* (iRECMODE == 2) */
		{
		if (iPLAYDELAY)
			DoSleep (iPLAYDELAY);
		i = pszREC[iRECPTR++];
		}
	return i;
	}


INT k_ungetch (INT c)
	{
	if (iRECMODE && iRECSIZE)
		iRECPTR--;
	return ungetch (c);
	}


INT k_kbhit (void)
	{
	if (iRECMODE != 2)
		return kbhit ();

	return (iRECPTR < iRECSIZE);
//   return 0;
	}





/***************************************************************************/
/*                                                                         */
/*  Low Level Keyboard I/O Routines                                        */
/*                                                                         */
/***************************************************************************/

/*
 * Converts key to uppercase
 * If 2-byte key, returns 0x100 + 2nd CHAR
 */
INT KeyGet (BOOL bUpper)
	{
	INT i, c;

	c = k_getch ();

	if (c == 0x00 || c == 0xE0)
		c = 0x100 + k_getch ();
	else if (bUpper)
		c = toupper (c);

	for (i=0; !bINTRAP && (i<iTRAPS); i++)
		if ((c == pkTRAPS[i].iKey) && pkTRAPS[i].pfnTrap)
			{
			bINTRAP = TRUE;
			c = pkTRAPS[i].pfnTrap (c);
			bINTRAP = FALSE;
			if (c)
				return c;
			return KeyGet (bUpper);
			}
	return c;
	}


/*
 * This fn clears the keyboard buffer
 */
void KeyClearBuff ()
	{
	if (iRECMODE == 2)
		return;

	while (k_kbhit ())
		KeyGet (FALSE);
	return;
	}



/*
 * This fn gets a key in pszKeyList and returns it
 * Yhis fn first clears the keyboard buffer
 * if pszKeyList contains letters, they must be upper-case
 * This fn beeps on incorrect entrys
 * This fn does accepts 2-byte keys 2 ways:
 *  1> pszExtKeyList may contain a list of the second bytes of ext keys
 *  2> if pszKeyList contains \xFF, following keys are considered hi bytes
 *     of extended keys
 *
 */
INT KeyChoose (PSZ pszKeyList, PSZ pszExtKeyList)
	{
	INT   c;
	PSZ   psz, pszExt;
	BOOL  bExtKey;

	KeyClearBuff ();

	bExtKey = (pszKeyList && (pszExt = strchr (pszKeyList, '\xFF')));

	while (TRUE)
		{
		c = KeyGet (TRUE);

		if (pszKeyList && (c < 0x100) && (psz = strchr (pszKeyList, c)))
			{
			if (!bExtKey || psz < pszExt)
				return c;
			}

		if (pszExtKeyList && (c > 0xFF) && strchr (pszExtKeyList, (c-0x100)))
			return c;

		if (bExtKey && (c > 0xFF) && strchr (pszExt+1, (c-0x100)))
			return c;

		GNUBeep (0);
		}
	}



/***************************************************************************/
/*                                                                         */
/* Record/Play API                                                         */
/*                                                                         */
/***************************************************************************/



void KeyPlayDelay (INT iPlayDelay)
	{
	iPLAYDELAY = iPlayDelay;
	}


INT KeyRecord (PSZ psz, INT iSize)
	{
	pszREC   = psz;
	iRECSIZE = iSize;
	iRECPTR  = 0;
	return iRECMODE = (psz && iSize ? 1 : 0);
	}


INT KeyPlay (PSZ psz, INT iSize)
	{
//   if (iRECMODE)
//      return 0;

	pszREC   = psz;
	iRECSIZE = iSize;
	iRECPTR  = 0;
	return iRECMODE = (psz && iSize ? 2 : 0);
	}


INT KeyStop (void)
	{
	iRECMODE = 0;
	return iRECPTR;
	}


INT KeyInfo (PSZ *ppsz, PINT piSize, PINT piIdx)
	{
	if (ppsz)   *ppsz   = pszREC;
	if (piSize) *piSize = iRECSIZE;
	if (piIdx)  *piIdx  = iRECPTR;
	return 0;
	}

/***************************************************************************/
/*                                                                         */
/* These trap functions are added by default                               */
/*                                                                         */
/***************************************************************************/

static CHAR  szDEFBUFF [RECBUFFSIZE];  // buffer for auto record
static INT   iDEFBUFSIZE = 0;          // buffer size for auto record


/*--- Ctl-F1 ---*/
INT AutoRecordTrapFn (INT c)
	{
//   static BOOL bDEFRECORD = FALSE;
//
//   if (bDEFRECORD = (!bDEFRECORD && !iRECMODE))
//
//test
	if (!iRECMODE)
		{
		KeyRecord (szDEFBUFF, sizeof szDEFBUFF);
		GNUBeep (2);
		}
	else
		{
		iDEFBUFSIZE = KeyStop () - (c > 0xFF ? 2 : 1);
		GNUBeep (4);
		}
	return 0;
	}

/*--- Ctl-F2 ---*/
INT AutoPlayTrapFn (INT c)
	{
	c = 0;
	if (!iRECMODE) // start playback
		{
		KeyPlay (szDEFBUFF, iDEFBUFSIZE);
		}
	else if (iRECMODE == 1) // end record, keep play key in buffer
		{
		iDEFBUFSIZE = KeyStop ();
		GNUBeep (3);
		}
	else if (iRECMODE == 2) // loop playback
		{
		KeyPlay (szDEFBUFF, iDEFBUFSIZE);
		}
	return 0;
	}


INT SaveKeyBuffTrapFn (INT c)
	{
	FILE *fp;

	if (iRECMODE)
		return 0;

	if (fp = fopen ("key.buf", "wb"))
		{
		fwrite (szDEFBUFF, 1, iDEFBUFSIZE, fp);
		fclose (fp);
		GNUBeep (4);
		}
	return 0;
	}

INT LoadKeyBuffTrapFn (INT c)
	{
	FILE *fp;

	if (iRECMODE)
		return 0;

	if (fp = fopen ("key.buf", "rb"))
		{
		iDEFBUFSIZE = fread (szDEFBUFF, 1, RECBUFFSIZE, fp);
		fclose (fp);
		GNUBeep (4);
		}
	return 0;
	}

