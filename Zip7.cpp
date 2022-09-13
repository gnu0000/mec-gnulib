/*
 * Zip7.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides string compression / decompression fns
 * Internally a compressed string is a single data block
 *
 *
 ****************************************************************************
 *
 * INT Cmp0ImplodeStr (pDest, iDestLen, pSrc, iSrcLen)
 * INT Cmp0ExplodeStr (pDest, iDestLen, pSrc)
 *
 ****************************************************************************
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuzip.h"
#include "zip.h"

extern "C"
{


/*
 * inter-globals for implode/explode sys
 *
 */
static PSZ  pszINBUFF;
static INT iINBUFFSIZE;
static INT iINBUFFINDEX;
static PSZ  pszOUTBUFF;
static INT iOUTBUFFSIZE;
static INT iOUTBUFFINDEX;


/***********************************************************************/
/*                                                                     */
/*                                                                     */
/*                                                                     */
/***********************************************************************/


/*
 * xfers data from input buffer to zips internal buffer
 *
 */
INT fnReadStr (PSZ pszBuff, PUINT puSize, PVOID pParam)
	{
	INT iRead;

	iRead = min ((INT)*puSize, iINBUFFSIZE - iINBUFFINDEX);
	memcpy (pszBuff, pszINBUFF + iINBUFFINDEX, iRead);
	iINBUFFINDEX += iRead;
	return iRead;
	}


/*
 * xfers data from zips internal buffer to the output buffer
 *
 */
void fnWriteStr (PSZ pszBuff, PUINT puSize, PVOID pParam)
	{
	INT iWrite;

	iWrite = min ((INT)*puSize, iOUTBUFFSIZE - iOUTBUFFINDEX);
	memcpy (pszOUTBUFF + iOUTBUFFINDEX, pszBuff, iWrite);
	iOUTBUFFINDEX += iWrite;
	}


/*
 * Compresses a string, returns the length
 * if iSrcLen is 0 then assume null term
 * if you specify src len, include null term in length if present
 * if iDestLen is 0 then no bounds check is done
 *
 * The length of the destination data is returned
 *
 * Internally, this creates a single block from the string
 * the format is the same as the blocks read/written to disk
 */
INT Cmp0ImplodeStr (PVOID pDest, INT iDestLen, PVOID pSrc, INT iSrcLen)
	{
	INT iType, iWindow, iError;

	if (!CmpInitialized () || !pSrc)
		return _cmpSetErru (NULL, ZIP_NOT_INITIALIZED);

	if (iDestLen && iDestLen < 12)
		return _cmpSetErru (NULL, ZIP_BUFF_TO_SMALL);

	pszINBUFF     = (PSZ) pSrc;
	iINBUFFSIZE   = (iSrcLen ? iSrcLen : strlen ((PSZ) pSrc) + 1);
	iINBUFFINDEX  = 0;
	pszOUTBUFF    = (PSZ)pDest + 6; // 6 header bytes before data (and 2 after)
	iOUTBUFFSIZE  = (iDestLen ? iDestLen - 6: -1);
	iOUTBUFFINDEX = 0;

	*(PUSHORT)((PSZ)pDest + 0) = (USHORT)ZIP_MARK;
	*(PUSHORT)((PSZ)pDest + 4) = (USHORT)iINBUFFSIZE;

	iType   = (iSrcLen ? CMP_BINARY : CMP_ASCII);
	iWindow = (iSrcLen < 2048 ? 1024 : (iSrcLen < 4096 ? 2048 : 4096));

	if (iError = implode ((PFReadBuf) fnReadStr, (PFWriteBuf) fnWriteStr, CmpGetBuff (), NULL, (PUINT) &iType, (PUINT) &iWindow))
		return _cmpSetErru (NULL, ZIP_CANT_WRITE);

	*(PUSHORT)((PSZ)pDest + 2) = (USHORT)iOUTBUFFINDEX;

	if (iOUTBUFFINDEX + 2 >= iOUTBUFFSIZE)
		return _cmpSetErru (NULL, ZIP_BUFF_TO_SMALL);

	*(PUSHORT)((PSZ)pDest + iOUTBUFFINDEX + 6) = (USHORT)ZIP_MARK;

	return iOUTBUFFINDEX + 8;
	}



/*
 * Uncompresses a string, returns the length
 *
 * The full data length is returned.  This could be larger than iDestLen
 * in which case data was discarded
 */
INT Cmp0ExplodeStr (PVOID pDest, INT iDestLen, PVOID pSrc)
	{
	INT iBlockLen, iDataLen, iError;

	if (!CmpInitialized () || !pSrc)
		return _cmpSetErru (NULL, ZIP_NOT_INITIALIZED);

	if (*(PUSHORT)((PSZ)pSrc + 0) != ZIP_MARK)
		return _cmpSetErru (NULL, ZIP_INVALID_DATA1);

	iBlockLen = *(PUSHORT)((PSZ)pSrc + 2);
	iDataLen  = *(PUSHORT)((PSZ)pSrc + 4);

	if (*(PUSHORT)((PSZ)pSrc + iBlockLen + 6) != ZIP_MARK)
		return _cmpSetErru (NULL, ZIP_INVALID_DATA1);

	pszINBUFF     = (PSZ)pSrc + 6; // 6 header bytes before data
	iINBUFFSIZE   = iBlockLen;
	iINBUFFINDEX  = 0;
	pszOUTBUFF    = (PSZ) pDest;
	iOUTBUFFSIZE  = (iDestLen ? iDestLen : iDataLen);
	iOUTBUFFINDEX = 0;
		
	if (iError = explode ((PFReadBuf) fnReadStr, (PFWriteBuf) fnWriteStr, CmpGetBuff (), NULL))
		return _cmpSetErru (NULL, ZIP_INVALID_DATA5);

	return iDataLen;
	}


}