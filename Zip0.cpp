/*
 * Zip0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * Init fn and misc info functions for the zip module.
 * CmpInit must be the first fn called in this module.
 * General zip library information is in GnuZip.h
 *
 *
 ****************************************************************************
 *
 * BOOL CmpInit (pszZipBuff, uDefType, uDefWindow)
 *
 * PSZ  CmpGetCurrBuff ()
 * BOOL CmpInitialized ()
 * INT  _cmpDefaultType ()
 * INT  _cmpDefaultWindow ()
 *
 ****************************************************************************
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuzip.h"
#include "zip.h"


static BOOL  bINITIALIZED = FALSE;      // System init bool
static PSZ   pszZIPBUFF   = NULL;       // Buffer used by implode / explode
static BOOL  bZIPBUFFMALLOC;            // True if zip buffer was malloced
static INT   iDEFTYPE     = CMP_BINARY; // Default zip mode
static INT   iDEFWINDOW   = 4096;       // Default zip window size

static ULONG ulCRC        = 0;
static BOOL  bCRCEnabled  = TRUE;

/***********************************************************************/
/*                                                                     */
/* Misc Functions                                                      */
/*                                                                     */
/***********************************************************************/


/*
 * External access to the zip buffer 
 * should somebody want it
 */
PSZ CmpGetBuff (void)
	{
	return pszZIPBUFF;
	}

BOOL CmpInitialized (void)
	{
	return bINITIALIZED;
	}

ULONG CmpEnableCRC (BOOL bEnable)
	{
	bCRCEnabled = bEnable;
	return ulCRC;
	}

BOOL CmpCRCEnabled (void)
	{
	return bCRCEnabled;
	}

ULONG CmpSetCRC (ULONG ulNewCRC)
	{
	return ulCRC = ulNewCRC;
	}

ULONG CmpGetCRC (void)
	{
	return ulCRC;
	}

ULONG CmpGenerateCRC (FILE *fp)
	{
	return 0;
	}



INT _cmpDefaultType (void)
	{
	return iDEFTYPE;
	}

INT _cmpDefaultWindow (void)
	{
	return iDEFWINDOW;
	}



/***********************************************************************/
/*                                                                     */
/* Module Initialization                                               */
/*                                                                     */
/***********************************************************************/


/*
 * This fn must be called before any other fn in this API module
 * pszZipBuff must be either NULL or > 35256
 * if called with NULL the buffer is malloced
 *
 *    uDefType (data encode range specifier)
 *    -----
 *    0          - Leave as default
 *    CMP_BINARY - Best compression method for binary data [default]
 *    CMP_ASCII  - Best compression method for text data 
 *
 *    uDefWindow (sliding search window size)
 *    -------
 *    0    -  Leave as default
 *    1024 -  Fastest compression
 *    2048 -
 *    4096 -  Best compression (usually) [default]
 *
 * The uDefType and uDefWindow may be overridden on a stream by stream
 * basis using the CmpSetMethod function
 *
 * If you use a small Work Buffer, use a uWindow size <= uBuffSize
 *  for optimal results
 *
 * returns FALSE on error
 * calling multiple times wont hurt anything
 */
BOOL CmpInit (PSZ pszZipBuff, INT iDefType, INT iDefWindow)
	{
	if (iDefType == CMP_BINARY || iDefType == CMP_ASCII)
		iDEFTYPE = iDefType;
	if (iDefWindow == 1024 || iDefWindow == 2048 || iDefWindow == 4096)
		iDEFWINDOW = iDefWindow;

	if (bINITIALIZED)
		return TRUE;

	pszZIPBUFF = (pszZipBuff ? pszZipBuff : (PSZ) malloc(35256U));
	if (!pszZIPBUFF)
		return _cmpSetErru (NULL, ZIP_MEMORY);

	bZIPBUFFMALLOC = !pszZipBuff;
	bINITIALIZED = TRUE;
	return TRUE;
	}

