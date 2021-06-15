/*
 * Res3.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides a file find function
 * Because DOSs' argv[0] is BRAIN DEAD!!
 *
 */

#include <io.h>
#include <string.h>
#include <stdio.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnumisc.h"
#include "gnures.h"
#include "res.h"

///*
// * Undocumented, but still supported
// * This is used to find the resource file name if one is not
// * explicitly given
// */
//extern CHAR **__argv;


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

static BOOL Exists (PSZ psz)
	{
	INT iRet;

	EnableErrors (HARDERROR_DISABLE);
	iRet = access (psz, 0);
	EnableErrors (HARDERROR_ENABLE);
	return !iRet;
	}


///*
// * This fn helps find a file.
// * Because argv[0] does not return path information under dos
// * you may need to go looking for the current exe file.
// *
// * [O] pszFullPath .. Full filespec of file if found. Empty string
// *                     if not found.
// * [I] pszName ...... Name of the file to look for. If NULL argv[0]
// *                     will be used for the filename.
// * [I] pszExt ......  The extention to use if one is not part of Name
// *                    Include the '.' in the ext.
// *
// * RETURN VALUE ..... TRUE if found, FALSE if not found.
// *
// * This fn does the following:
// * 1> If pszName contains path info, assume its correct and return it.
// * 2> Add extention if necessary and look in the current dir.
// * 3> Add extention if necessary and look at a:
// * 4> if still not found, return empty string.
// */
//BOOL ResFindExe (PSZ pszFullPath, PSZ pszName, PSZ pszExt)
//   {
//   CHAR szName[256];
//   CHAR szExt[32];
//
//   /*--- OS/2 does this for us ---*/
//   strcpy (szName, (pszName ? pszName : __argv[0]));
//   strcpy (szName, (pszName ? pszName : __argv[0]));
//   strcpy (pszFullPath, szName);
//   if (strchr (szName, '\\'))
//      return TRUE;
//
//   strcpy (szExt,  (pszExt  ? pszExt  : ".exe"));
//   if (!strchr (szName, '.'))
//      strcat (szName, szExt);
//
//   /*--- First we look in the current directory ---*/
//   strcpy (pszFullPath, szName);
//   if (Exists (pszFullPath))
//      return TRUE;
//
//   /*--- now we look at the A drive  ---*/
//   sprintf (pszFullPath, "a:%s", szName);
//   if (Exists (pszFullPath))
//      return TRUE;
//
//   *pszFullPath = '\0';
//   return FALSE;
//   }

