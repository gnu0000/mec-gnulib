/*
 * Dir5.c
 *
 * Craig Fitzgerald  1996
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

extern "C"
{


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


/*
 * This fn splits up a filename into component pieces
 * uFlags determine which piece(s) to return
 *
 * uFlags:
 *    DIR_DRIVE
 *    DIR_DIR
 *    DIR_NAME
 *    DIR_EXT
 */
PSZ DirSplitPath (PSZ pszDest, PSZ pszPath, WORD wFlags)
	{
	PSZ psz, pszPtr;

	*pszDest = '\0';

	if (!pszPath || !*pszPath)
		return NULL;

	pszPtr = pszPath;

	if (isalpha (*pszPtr) && pszPtr[1] == ':') // we have drive spec
		{
		if (wFlags & DIR_DRIVE)
			{
			strncpy (pszDest, pszPtr, 2);
			pszDest[2] = '\0';
			}
		pszPtr += 2;
		}
	if (psz = strrchr (pszPtr, '\\'))
		{
		if (wFlags & DIR_DIR)
			{
			strcat (pszDest, pszPtr);
			*(strrchr (pszDest, '\\') + 1) = '\0';
			}
		pszPtr = psz+1;
		}
	if (psz = strrchr (pszPtr, '.'))
		{
		if (wFlags & DIR_NAME)
			{
			strcat (pszDest, pszPtr);
			*(strrchr (pszDest, '.')) = '\0';
			}
		}
	pszPtr = psz;
	if (pszPtr && *pszPtr && (wFlags & DIR_EXT))
		strcat (pszDest, pszPtr + ((wFlags > DIR_EXT) ? 0 : 1));

	return pszDest;
	}
}