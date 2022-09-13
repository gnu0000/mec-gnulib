/*
 * Zip6.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * File to file compression / decompression.
 * It is a simple matter to emulate these functions behavior using
 * fread/CmpWrite or CmpRead/fwrite but these functions do not use
 * the intervening buffer and are somewhat faster.
 *
 *
 ****************************************************************************
 *
 * ULONG CmpExplodeFile (pcfpIn,  fpOut, pulReadSize)
 * ULONG CmpImplodeFile (pcfpOut, fpIn,  pulReadSize)
 *
 ****************************************************************************
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuzip.h"
#include "zip.h"

extern "C"
{


/***********************************************************************/
/*                                                                     */
/*                                                                     */
/*                                                                     */
/***********************************************************************/


/*
 * Uncompresses one file into another
 * pcfpIn is the compressed file to read.  This handle must point to 
 *   the beginning of the stream (or at least be at the start of a block)
 * fpOut is where the data is written to
 * pulReadSize is the size of the data read (this can be NULL).
 * Returns: the number of bytes written
 *
 * The advantage of this fn over using your own CmpRead/fwrite loop
 * is that it uses no intervening buffer
 *
 */
ULONG CmpExplodeFile (PCFILE pcfpIn, FILE *fpOut, PULONG pulReadSize)
	{
	ULONG ulOldDataPos, ulOutSize = 0;
	INT   iRead;

	ulOldDataPos = pcfpIn->ulCDataPos;
	while (!pcfpIn->bEOS)
		{
		iRead = LoadBuffer (pcfpIn);
		if ((INT)fwrite (pcfpIn->pszBuff, 1, iRead, fpOut) != iRead)
			break;
		ulOutSize += (ULONG)iRead;
		}
	if (pulReadSize)
		*pulReadSize = pcfpIn->ulCDataPos - ulOldDataPos;
	return ulOutSize;
	}


/*
 * Compresses one file into another
 * pcfpOut is the compressed file destination.  This handle must point to 
 *   the beginning of the stream (or at least be flushed)
 * fpIn is where the data is coming from
 * pulReadSize is the size of the data read (this can be NULL).
 * Returns: the number of bytes written
 *
 * The advantage of this fn over using your own fread/CmpWrite loop
 * is that it uses no intervening buffer
 *
 */
ULONG CmpImplodeFile (PCFILE pcfpOut, FILE *fpIn, PULONG pulReadSize)
	{
	ULONG  ulInSize = 0, ulOutSize = 0;
	UINT   iRead, iWrote;

	pcfpOut->bWriting = TRUE;
	while (iRead = fread (pcfpOut->pszBuff, 1, pcfpOut->iBuffSize, fpIn))
		{
		pcfpOut->iDataSize = iRead;
		if (!(iWrote = CmpFlush (pcfpOut)))
			break; // a problem
		ulOutSize += (ULONG)iWrote;
		ulInSize  += (ULONG)iRead;
		}
	if (pulReadSize)
		*pulReadSize = ulInSize;
	return ulOutSize;
	}

}