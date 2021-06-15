/*
 * zip8.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * Compression functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuzip.h"
#include "zip.h"

/*
 * BYTE   CmpReadByte
 * SHORT  CmpReadShort
 * USHORT CmpReadUShort
 * LONG   CmpReadLong
 * ULONG  CmpReadULong
 * BOOL   CmpWriteByte
 * BOOL   CmpWriteShort
 * BOOL   CmpWriteUShort
 * BOOL   CmpWriteLong
 * BOOL   CmpWriteULong
 */

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

BYTE CmpReadByte (PCFILE pcfp)
	{
	BYTE c;

	CmpRead (pcfp, &c, 1);
	return c;
	}

SHORT CmpReadShort (PCFILE pcfp)
	{
	SHORT s;

	CmpRead (pcfp, &s, sizeof (SHORT));
	return s;
	}

USHORT CmpReadUShort (PCFILE pcfp)
	{
	USHORT u;

	CmpRead (pcfp, &u, sizeof (USHORT));
	return u;
	}

LONG CmpReadLong (PCFILE pcfp)
	{
	LONG l;

	CmpRead (pcfp, &l, sizeof (LONG));
	return l;
	}

ULONG CmpReadULong (PCFILE pcfp)
	{
	ULONG ul;

	CmpRead (pcfp, &ul, sizeof (ULONG));
	return ul;
	}

BIG CmpReadFloat (PCFILE pcfp)
	{
	BIG bg;

	CmpRead (pcfp, &bg, sizeof (BIG));
	return bg;
	}

BOOL CmpWriteByte (PCFILE pcfp, BYTE c)
	{
	return CmpWrite (pcfp, &c, sizeof (BYTE));
	}

BOOL CmpWriteShort (PCFILE pcfp, SHORT  s)
	{
	return CmpWrite (pcfp, &s, sizeof (SHORT));
	}

BOOL CmpWriteUShort(PCFILE pcfp, USHORT us)
	{
	return CmpWrite (pcfp, &us, sizeof (USHORT));
	}

BOOL CmpWriteLong  (PCFILE pcfp, LONG l)
	{
	return CmpWrite (pcfp, &l, sizeof (LONG));
	}

BOOL CmpWriteULong (PCFILE pcfp, ULONG ul)
	{
	return CmpWrite (pcfp, &ul, sizeof (ULONG));
	}

BOOL CmpWriteFloat (PCFILE pcfp, BIG bg)
	{
	return CmpWrite (pcfp, &bg, sizeof (BIG));
	}


