/*
 * Msc4.c
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
 * INT EnableErrors (INT i)
 * PSZ  GetLastSysError (PINT piErr)
 *
 ****************************************************************************
 *
 */

#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnumisc.h"

extern "C"
{


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

INT EnableErrors (INT i)
	{
	return 0;
	}


PSZ GetLastSysError (PINT piErr)
	{
	if (piErr)
		*piErr = 0;
	return NULL;
	}

}