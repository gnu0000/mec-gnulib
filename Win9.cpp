/*
 * Win9.c
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
#include <stdarg.h>
#include <stdio.h>
#include "gnutype.h"
#include "gnuscr.h"
#include "gnuwin.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

INT GnuPaintStr (PGW  pgw,      // local window ?
					  INT  iRow,     // 0 based row
					  INT  iCol,     // 0 based col
					  INT  iJust,    // 0=lt 1=rt 2=ctr-Col 4=ctr-row 5=center-win
					  WORD wAtt,     // see GnuGetAtt table
					  PSZ  psz, ...)
	{
	CHAR    szOut[256];
	va_list vlst;

	va_start (vlst, psz);
	vsprintf (szOut, psz, vlst);
	va_end (vlst);
	return GnuPaint (pgw, iRow, iCol, iJust, wAtt, szOut);
	}

