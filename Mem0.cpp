/*
 * Mem0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * Memory functions
 */

#if defined (MEM_ERRORS)
#undef MEM_ERRORS
#endif
#if defined (MEM_DEBUG)
#undef MEM_DEBUG
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "gnutype.h"
#include "gnumem.h"
#include "mem.h"

static WORD wFLAGS;
static FILE *FOUT;


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


void MemSetDebugMode (WORD wMode)
	{
	wFLAGS = wMode;
	}

void MemSetDebugStream (PVOID fp)
	{
	FOUT = (FILE *) fp;
	}

WORD memGetDebugMode (void)
	{
	return wFLAGS;
	}

FILE  *memGetDebugStream (void)
	{
	return FOUT;
	}


INT memErr (PSZ psz, ...)
	{
	FILE    *fpLog;
	va_list vlst;

	if (!(fpLog = memGetDebugStream ()))
		fpLog = stderr;

	fprintf (fpLog, "Mem Error: ");
	va_start (vlst, psz);
	vfprintf (fpLog, psz, vlst);
	va_end (vlst);
	fprintf (fpLog, "\n");

	exit (1);
	return 0;
	}


void memLog (PSZ psz, ...)
	{
	FILE    *fpLog;
	va_list vlst;

	if (!(memGetDebugMode () & MEM_LOG) || !(fpLog = memGetDebugStream ()))
		return;

	va_start (vlst, psz);
	vfprintf (fpLog, psz, vlst);
	va_end (vlst);
	fprintf (fpLog, "\n");
	}
