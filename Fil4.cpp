/*
 * Fil4.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides basic file manipulation utilities
 */


#include <string.h>
#include <stdio.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnufile.h"


extern "C"
{

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

BOOL FilWriteByte (FILE *fp, BYTE c)
	{
	return fwrite (&c, sizeof (BYTE), 1, fp);
	}


BOOL FilWriteShort (FILE *fp, SHORT i)
	{
	return fwrite (&i, sizeof (SHORT), 1, fp);
	}


BOOL FilWriteUShort (FILE *fp, USHORT u)
	{
	return fwrite (&u, sizeof (USHORT), 1, fp);
	}


BOOL FilWriteLong (FILE *fp, LONG l)
	{
	return fwrite (&l, sizeof (LONG), 1, fp);
	}


BOOL FilWriteULong (FILE *fp, ULONG ul)
	{
	return fwrite (&ul, sizeof (ULONG), 1, fp);
	}


BOOL FilWriteStr (FILE *fp, PSZ p)
	{
	if (p)
		fputs (p, fp);
	return (fputc ('\0', fp) != EOF);
	}



BYTE FilReadByte (FILE *fp)
	{
	BYTE c = 0;

	fread (&c, sizeof (BYTE), 1, fp);
	return c;
	}


SHORT FilReadShort (FILE *fp)
	{
	SHORT i = 0;

	fread (&i, sizeof (SHORT), 1, fp);
	return i;
	}


USHORT FilReadUShort (FILE *fp)
	{
	USHORT u = 0;

	fread (&u, sizeof (USHORT), 1, fp);
	return u;
	}


LONG FilReadLong (FILE *fp)
	{
	LONG l = 0;

	fread (&l, sizeof (LONG), 1, fp);
	return l;
	}


ULONG FilReadULong (FILE *fp)
	{
	ULONG ul = 0;

	fread (&ul, sizeof (ULONG), 1, fp);
	return ul;
	}

}

