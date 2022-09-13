/*
 * Msc3.c
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
 * INT Rnd (INT iLimit)
 *
 ****************************************************************************
 *
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
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

/*
 * returns a 'random' number from 0 to iLimit-1
 *
 */
INT Rnd (INT iLimit)
	{
	return (INT) ((float)iLimit*rand()/(RAND_MAX+1.0));
	}


ULONG RandULong (ULONG ulSize)
	{
	ULONG   ul;
	PUSHORT pus;

	pus      = (PUSHORT)&ul;
	pus[0]   = rand ();
	pus[1]   = rand ();
	pus[0]  |= (pus[1] & 0x01) << 7;
	pus[1] >>= 1;

	ul %= ulSize;
	return ul;
	}

}