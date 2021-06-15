/*
 * Dir6.c
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
#include <direct.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnudir.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * Handles drive and path
 */
int DirUnlink (PSZ pszFile)
	{
	char szDrive[32], szDD[256], szNE[256], szOldDir[256];
	INT iDrive, iRet;

	DirSplitPath (szDrive, pszFile, DIR_DRIVE          );
	DirSplitPath (szDD   , pszFile, DIR_DRIVE | DIR_DIR);
	DirSplitPath (szNE   , pszFile, DIR_NAME | DIR_EXT );

	/*--- get current ddive/dir ---*/
	iDrive = _getdrive ();
	_getcwd (szOldDir, sizeof (szOldDir));

	/*--- change to file ddive/dir ---*/
	if (*szDrive && (toupper(*szDrive) != 'A' + iDrive -1))
		if (_chdrive (toupper(*szDrive) - 'A' + 1))
			return -1;
	if (_chdir (szDD))
			return -1;

	iRet = unlink (szNE);

	/*--- change back ---*/
	_chdrive (iDrive);
	_chdir   (szOldDir);

	return iRet;
	}

