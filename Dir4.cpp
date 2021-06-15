/*
 * Dir4.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides base functions for
 * manipulating file directories and disk fn's
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnudir.h"

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * Makes a filename (pszDest)
 * 1> pszBase if it can, 
 *       attach pszExt if it exists and pszBase has no ext
 *
 * 2> use pszOrg (return NULL if it doesn't exist)
 *       replace/add pszExt if it exists
 */
PSZ DirMakeFileName (PSZ pszDest, PSZ pszBase, PSZ pszOrg, PSZ pszExt)
	{
	PSZ p1, p2;

	*pszDest = '\0';

	if (pszBase)
		{
		strcpy (pszDest, pszBase);
		if (!strchr (pszDest, '.') && pszExt)
			strcat (pszDest, pszExt);
		return pszDest;
		}
	if (!pszOrg)
		return NULL;
	strcpy (pszDest, pszOrg);

	p1 = strrchr (pszDest, '\\');
	p2 = (p1 ? p1 + 1: pszDest);

	if (p1 = strrchr (p2, '.'))
		*p1 = '\0';
	if (pszExt)
		strcat (p2, pszExt);
	return pszDest;
	}


/*
 * This fn takes a string, and works it over until it is
 * a valid file/dir name
 *
 */
PSZ DirMakeValidName (PSZ pszDest, PSZ pszSrc)
	{
	PSZ   psz;

	if (!pszSrc || !*pszSrc)
		return strcpy (pszDest, "UNKNOWN");
	strcpy (pszDest, pszSrc);

	/*---limit len ---*/
	if (strlen (pszDest) > 8)
		pszDest[8] = '\0';

	/*--- remove bad chars ---*/
	for (psz = pszDest; *psz; psz++)
		if (!isalnum (*psz))
			*psz = '-';

	return pszDest;
	}


/*
 * This fn strips the leading path information from 
 * a filespec.
 * string is malloced if pszDest is null.
 */
PSZ DirStripPath (PSZ pszDest, PSZ pszSrc, BOOL bKeepExt)
	{
	PSZ psz, p1;

	psz = pszSrc;
	if (p1 = strrchr (psz, '\\'))
		psz = p1+1;
	if (p1 = strrchr (psz, ':'))
		psz = p1+1;

	pszDest = (pszDest ? pszDest : (PSZ) malloc (strlen (psz)+1));
	strcpy (pszDest, psz);

	if (!bKeepExt && (p1 = strchr (pszDest, '.')))
		*p1 = '\0';

	return pszDest;
	}

