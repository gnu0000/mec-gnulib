/*
 * Dir3.c
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

#include <time.h>
#include <dos.h>
#include <sys\utime.h>
#include <stdio.h>
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

BOOL DirSetFileAtt (PSZ pszFile, INT iAtts)
	{
#if defined (VER_NT)
	return SetFileAttributes (pszFile, iAtts);
#else
	return !_dos_setfileattr (pszFile, iAtts & ~_A_SUBDIR);
#endif
	}

