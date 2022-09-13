/*
 * Zip5.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * File position functions.  These allow you to do fseek / ftell like
 * functions on the compressed data stream.  Note that seeking to a
 * spot before the current position may necessitate re-reading the
 * compressed file from the beginning! so use sparingly.
 *
 ****************************************************************************
 *
 * BOOL  CmpRewind (pcfp)
 * ULONG CmpGetPos (pcfp)
 * BOOL  CmpSetPos (pcfp, ulFilePos)
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
 * Rewinds the file ptr to the beginning of the data stream
 * The beginning of the data stream is defined as the file position at
 * the time of the CmpOpen or CmpOpen2 call
 */
BOOL CmpRewind (PCFILE pcfp)
	{
	if (!pcfp || !pcfp->fp)
		return FALSE;

	fseek (pcfp->fp, pcfp->ulOrigin, SEEK_SET);

	pcfp->ulUDataPos = 0; // reset uncompressed data index
	pcfp->ulCDataPos = 0; // reset compressed data index
	pcfp->iDataSize  = 0; // invalidate dat in buffer
	pcfp->iOffset    = 0; // a precaution

	return TRUE;
	}


/*
 * Get current file position
 */
ULONG CmpGetPos (PCFILE pcfp)
	{
	if (!pcfp)
		return 0;

	return pcfp->ulUDataPos + (ULONG)pcfp->iOffset;
	}



/*
 * Set file position
 */
BOOL CmpSetPos (PCFILE pcfp, ULONG ulFilePos)
	{
	BOOL bOK = TRUE;

	if (!pcfp)
		return FALSE;

	/*--- Before Current buffer ---*/
	if (ulFilePos < pcfp->ulUDataPos)
		CmpRewind (pcfp); // fall through

	/*--- After Current buffer ---*/

	/* Note.  I'm being lazy
	 * I could look at block headers to find the needed data block
	 * rather than reading each one in
	 */
	while (ulFilePos > pcfp->ulUDataPos + pcfp->iDataSize && bOK)
		bOK = !!LoadBuffer (pcfp);

	if (!bOK)
		return FALSE;  // ulFilePos larger than stream or error

	/*--- Now In Current buffer ---*/
	pcfp->iOffset = (INT) (ulFilePos - pcfp->ulUDataPos);
	return TRUE;
	}

}