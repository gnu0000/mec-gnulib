/*
 * Crc0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides basic CRC capabilities
 */


#include <string.h>
#include <stdio.h>
#include "gnutype.h"
#include "gnucrc.h"


#define  MASK  0xA0000001


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * Do not modify unless you are really sure you know what 
 * you are doing!
 */
ULONG CRC32 (ULONG ulCRC, CHAR c)
	{
	INT i;

	for (i=0; i<8; i++)
		{
		if((ulCRC ^ c) & 1)
			ulCRC = (ulCRC>>1) ^ MASK;
		else
			ulCRC>>=1;
		c>>=1;
		}
	return ulCRC;
	}



/* generates CRC over mem pointed to by p, if 
 * uCount = 0, continue until \0 is found
 */
ULONG CRCBuff (ULONG ulCRC, PVOID pvoid, INT iCount)
	{
	PCHAR p;

	p = (PCHAR) pvoid;

	if (!p)
		return ulCRC;

	if (iCount)
		while (iCount--)
			ulCRC = CRC32 (ulCRC, *p++);
	else
		while (*(PCHAR)p)
			ulCRC = CRC32 (ulCRC, *p++);

	return ulCRC;
	}

