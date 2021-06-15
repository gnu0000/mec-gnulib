/*
 * Msc0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides miscelaneous functions
 * 
 * 
 **************************************************************************** 
 * 
 * INT Error (PSZ psz, ...)
 * INT Msg (PSZ psz, ...)
 * INT Warning (PSZ psz, ...)
 * 
 **************************************************************************** 
 * 
 */

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnumisc.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


INT Error (PSZ psz, ...)
	{
	va_list vlst;

	printf ("Error: ");
	va_start (vlst, psz);
	vprintf (psz, vlst);
	va_end (vlst);
	printf ("\n");
	exit (1);
	return 0;
	}


INT Msg (PSZ psz, ...)
	{
	va_list vlst;

	va_start (vlst, psz);
	vprintf (psz, vlst);
	va_end (vlst);
	printf ("\n");
	return 0;
	}


INT Warning (PSZ psz, ...)
	{
	va_list vlst;

	printf ("Warning: ");
	va_start (vlst, psz);
	vprintf (psz, vlst);
	va_end (vlst);

	printf ("\n");
	return 0;
	}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
