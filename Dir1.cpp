/*
 * Dir1.c
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
#include <stdarg.h>
#include <string.h>
#include <direct.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnumisc.h"
#include "gnudir.h"

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 *
 */
BOOL DirPathExists (PSZ pszPath)
	{
	BOOL bExists;
	CHAR szCurrPath [256];

	getcwd (szCurrPath, sizeof szCurrPath);
	if (bExists = !chdir (pszPath))
		chdir (szCurrPath);

	return bExists;
	}


/*
 *
 */
BOOL DirMakePath (PSZ pszPath)
	{
	PSZ  psz;
	CHAR szTmpPath[256], szStartPath[256];

	psz = pszPath;
	if (!psz || !*psz)                     // no path, easy to make.
		return TRUE;
	if (strlen (psz) > 1 && psz[1] == ':') // skip drive spec
		psz += 2;
	if (*psz == '\\')                      // skip root backslash
		psz++;

	getcwd (szStartPath, sizeof szStartPath);

	while (*psz)                           // loop through dirs
		{
		for (; *psz && *psz!='\\'; psz++)   // next path element
			;
		strcpy (szTmpPath, pszPath);
		szTmpPath[psz-pszPath] = '\0';      // full path spec to this point

		if (chdir (szTmpPath))              // !exists?
			if (mkdir (szTmpPath))           // !make? 
				{
				chdir (szStartPath);          // restore curr dir
				return FALSE;                 // error
				}
		chdir (szStartPath);
		if (*psz)
			psz++;
		}
	return TRUE;
	}


/*
 *
 *
 *
 */
INT DirChangeDrive (INT iDrive)
	{
#if defined (__WATCOMC__)
	INT iTotal;

	_dos_setdrive (iDrive, &iTotal);
	return 0;
#else
	return _chdrive (iDrive);
#endif
	}

