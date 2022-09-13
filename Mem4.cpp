/*
 * Mem4.c
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

#include "gnutype.h"
#include "gnumem.h"

extern "C"
{

#pragma optimize ("egl", off)


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


ULONG MemAvailConv (void)
	{
#if defined (VER_DOS16M)
	WORD wPara;

	_asm mov ah, 48h
	_asm mov bx, 0xFFFF
	_asm int 21h
	_asm mov wPara, bx;

	return (ULONG)wPara << 4;
#else
	return (ULONG) 655350;
#endif
	}

#pragma optimize ("", on)
}