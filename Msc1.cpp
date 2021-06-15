/*
 * Msc1.c
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
 * CHAR HexChar (INT i)
 * INT HexCharVal (INT c)
 * INT HexValPsz (PSZ psz)
 * 
 ****************************************************************************
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnumisc.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * converts a digit to its Hex representation
 *
 */
CHAR HexChar (INT i)
	{
	CHAR c;

	c = (CHAR) i;
	c += '0';
	if (c > '9')
		c += 7;
	return c;
	}

/*
 * convert a hex digit to its decimal equivalent
 */
INT HexCharVal (INT c)
	{
	c = toupper (c);
	if (!strchr ("0123456789ABCDEF", c))
		return 0;
	return (c - '0' - 7 * (c > '9'));
	}


/*
 * convert a 2 digit hex number to its decimal equivalent
 */
INT HexValPsz (PSZ psz)
	{
	if (!psz || !*psz || !psz[1])
		return 0;
	return 16 * HexCharVal (psz[0]) + HexCharVal (psz[1]);
	}


/*
 * min/max fn
 *
 */
INT Range (INT iMin, INT iVal, INT iMax)
	{
	if (iMin > iVal) return iMin;
	if (iMax < iVal) return iMax;
	return iVal;
	}
