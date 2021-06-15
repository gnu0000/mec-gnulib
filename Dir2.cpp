/*
 * Dir2.c
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
#include <io.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <direct.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnudir.h"

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

//#if defined (__OS2__)
//
////
//// this will need to be re-written beacuse the dos
//// find first/next is not re-entrant
////
//
///*
// * Deletes all files in the dir
// * and then removes the dir
// */
//BOOL DirRemoveDir (PSZ pszDir, BOOL bKillChildren)
//   {
//   HDIR hdir = 0;
//   CHAR szSpec     [256];
//   CHAR szSubDir   [256];
//   CHAR szFile     [256];
//   CHAR szChildDir [256];
//
//   sprintf (szSpec, "%s\\*.*", pszDir);
//
//   if (bKillChildren)
//      while (FindAFile (szSubDir, &hdir, szSpec, FILE_DIRECTORY, NULL))
//         {
//         sprintf (szChildDir, "%s\\%s", pszDir, szSubDir);
//         RemoveDir (szChildDir, bKillChildren);
//         }
//   hdir = 0;
//   while (FindAFile (szFile, &hdir, szSpec, FILE_NORMAL, NULL))
//      {
//      sprintf (szChildDir, "%s\\%s", pszDir, szFile);
//      unlink (szChildDir);
//      }
//   return !rmdir (pszDir);
//   }
//#endif


/*
 * This file deletes all closed, empty files in pszDir
 */
void DirRemoveEmptyFiles (PSZ pszDir)
	{
	CHAR   szSpec     [256];
	CHAR   szFileSpec [256];
	PFINFO pfo = NULL;

	sprintf (szSpec, "%s\\*.*", pszDir);

	while (pfo = DirFindFile (szSpec, FILE_NORMAL, pfo))
		{
		if (pfo->ulSize)
			continue;

		sprintf (szFileSpec, "%s\\%s", pszDir, pfo->pszName);
		unlink (szFileSpec);
		}
	}

