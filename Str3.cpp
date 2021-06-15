/*
 * Str3.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * String Functions
 *
 */

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnustr.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

PSZ StrReplace (PPSZ ppszDest, PSZ pszSrc)
	{
	if (!ppszDest || (*ppszDest == pszSrc))
		return NULL;

	MemFreeData (*ppszDest);
	*ppszDest = (pszSrc ? strdup (pszSrc) : NULL);
	return *ppszDest;
	}

