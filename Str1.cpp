/*
 * Str1.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides various file and string functions
 * that i wish were part of the standard library set.
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnustr.h"



/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/

/*
 *
 */
PSZ StrNum (PSZ psz, INT i)
	{
	sprintf (psz, "%u", i);
	return psz;
	}

///*
// * This fn concatenates n strings together
// * and returns that string
// */
//PSZ StrMyCat (PSZ pszDest, PSZ pszSrc, ...)
//	{
//	va_list vlst;
//
//	if (!pszDest)
//		return NULL;
//
//	*pszDest = '\0';
//
//	va_start (vlst, pszSrc);
//	while (pszSrc)
//		{
//		strcat (pszDest, pszSrc);
//		pszSrc = va_arg (vlst, PSZ);
//		}
//	va_end (vlst);
//	return pszDest;
//	}


// This fn appends n strings on to the end of pszDest
// the last param to this fn must be a NULL terminator
//
PSZ StrMyCat (PSZ pszDest, PSZ pszSrc, ...)
	{
	va_list vlst;

	if (!pszDest)
		return NULL;

	va_start (vlst, pszSrc);
	while (pszSrc)
		{
		strcat (pszDest, pszSrc);
		pszSrc = va_arg (vlst, PSZ);
		}
	va_end (vlst);
	return pszDest;
	}



/*
 * Returns TRUE if the string is TRUE or Yes or 1 etc...
 *
 * Have you heard
 * Life is a turd
 * Johannisburgh
 */
BOOL StrTrue (PSZ psz)
	{
	INT c;

	StrClip (StrStrip (psz, " \t"), " \t");
	if (!psz || !*psz)
		return FALSE;
	c = toupper (*psz);

	return (c == 'T' || c == 'Y'|| c == '1');
	}

