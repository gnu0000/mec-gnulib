/*
 * Mem2.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This module provides the functions for GnuMem module
 * these mem functions are used by all the GnuLib functtions
 * that dynamically allocate memory.
 */

#if defined (MEM_ERRORS)
#undef MEM_ERRORS
#endif
#if defined (MEM_DEBUG)
#undef MEM_DEBUG
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gnutype.h"
#include "gnumem.h"
#include "mem.h"

extern "C"
{


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


PVOID Mem1_malloc (INT iSize, PSZ pszFile, INT iLine)
	{
	PVOID p;

	if (!iSize)
		return NULL;

	if (p = malloc (iSize))
		return p;

	memErr (" (%s,%u): Could not malloc %u bytes.", pszFile, iLine, iSize);
	return NULL;
	}


PVOID Mem1_calloc (INT iNum, INT iSize, PSZ pszFile, INT iLine)
	{
	PVOID p;

	if (!iSize * iNum)
		return NULL;

	if (p = malloc (iSize * iNum))
		return p;

	memset (p, 0, iNum * iSize);

	memErr (" (%s,%u): Could not calloc %u bytes.", pszFile, iLine, iSize * iNum);
	return NULL;
	}


PVOID Mem1_realloc (PVOID p, INT iSize, PSZ pszFile, INT iLine)
	{
	FILE  *fpLog;

	if (p = realloc (p, iSize))
		return p;

	if (fpLog = memGetDebugStream ())
		fprintf (fpLog, "Could not realloc %u bytes\nSource file %s, line %u",
				iSize, pszFile, iLine);
	exit (1);
	return NULL;
	}


PSZ Mem1_strdup (PSZ psz, PSZ pszFile, INT iLine)
	{
	INT  iLen;
	PSZ  psz2;
	FILE *fpLog;

	if (!psz)
		return NULL;

	iLen = strlen (psz);

	if (!(psz2 = (PSZ) malloc (iLen + 1)))
		{                                                         
		if (fpLog = memGetDebugStream ())
			fprintf (fpLog, "Could not strdup %u bytes\n Source file %s, line %u",
				  iLen+1, pszFile, iLine);
		exit (1);
		}
	strcpy (psz2, psz);
	return psz2;
	}


PSZ Mem1_strndup (PSZ psz, INT iLen, PSZ pszFile, INT iLine)
	{
	PSZ    psz2;
	FILE  *fpLog;

	if (!psz)
		return NULL;

	if (!(psz2 = (PSZ) malloc (iLen)))
		{                                                         
		if (fpLog = memGetDebugStream ())
			fprintf (fpLog, "Could not strndup %u bytes\n Source file %s, line %u",
				  iLen+1, pszFile, iLine);
		exit (1);
		}
	memcpy (psz2, psz, iLen);
	return psz2;
	}


void Mem1_free (PVOID p, PSZ pszFile, INT iLine)
	{
	FILE *fpLog;

	if (!p)
		{
		if (memGetDebugMode () & MEM_NULLOK)
			{
			if (fpLog = memGetDebugStream ())
				fprintf (fpLog, "Freeing NULL ptr Source file %s, line %u\n", pszFile, iLine);
			return;
			}
		else
			memErr ("(%s,%u): Freeing NULL ptr.", pszFile, iLine);
		}
	free (p);
	}

void Mem1_exit (int i, PSZ pszFile, INT iLine)
	{
	exit (i);
	}

void Mem1_testptr (PVOID p, PSZ pszFile, INT iLine)
	{
	}

void Mem1_test (PSZ pszFile, INT iLine)
	{
	}

void Mem1_list (PSZ pszFile, INT iLine)
	{
	}


PPSZ Mem1_freeppsz (PSZ *ppsz, INT iNum, PSZ pszFile, INT iLine)
	{
	PSZ *pp;

	if (!ppsz)
		return NULL;

	if (iNum)
		{
		while (iNum)
			if (ppsz [--iNum])
				Mem1_free (ppsz [iNum], pszFile, iLine);
		}
	else
		{
		for (pp = ppsz; *pp; pp++)
			if (*pp)
				Mem1_free (*pp, pszFile, iLine);
		}
	Mem1_free (ppsz, pszFile, iLine);
	return NULL;
	}


PVOID  Mem1_freedata(PVOID p, PSZ pszFile, INT iLine)
	{
	if (p)
		Mem1_free (p, pszFile, iLine);
	return NULL;
	}

}