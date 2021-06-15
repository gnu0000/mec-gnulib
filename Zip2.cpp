/*
 * Zip2.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides the basic functions to:
 * 1> compress data and save to disk
 * 2> load data from disk and uncompress
 *
 * These fn's are intended for internal module use
 *
 *
 ****************************************************************************
 *
 *  compressed data format:
 *  The compressed data stream consists of sequential blocks.
 *  The format of a block is:
 *
 *  [2] ZIP_MARK   - A special value to check validity of the block
 *  [2] CDataSize  - The size of the CData field in bytes
 *  [2] DataSize   - The size of the data block after uncompressing
 *  [n] CData      - The compressed data
 *  [2] ZIP_MARK   - A special value to check validity of the block
 *
 *  The last block of the stream is followed by a [2] ZIP_TERM marker
 *
 ****************************************************************************
 *
 * INT _cmpWrtBlk  (*fp, pszBuff, iLen, iType, iWindow, piCLen)
 * INT _cmpReadBlk (*fp, pszBuff, uMaxLen, iBuffOffset, iType,
 *                     iWindow, piBuffLen, piUBlockLen, piCBlockLen)
 * BOOL   _cmpWriteTerm (FILE *fp)
 *
 ****************************************************************************
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuzip.h"
#include "gnucrc.h"
#include "gnufile.h"
#include "zip.h"


/*
 * inter-globals for implode/explode sys
 *
 */
static FILE *fpFILE;      // file to read/write
static PSZ  pszDATABUFF;  // staging buffer
static INT  iBUFFSIZE;    // staging buffer size
static INT  iBUFFSTART;   // buff data = data block + this offset
static INT  iDATASIZE;    // size of data in staging buffer (uncompressed data)
static INT  iDATAINDEX;   // data offset from start of block (uncompressed data)
static INT  iBLOCKSIZE;   // size of compressed block (compressed data)
static INT  iBLOCKINDEX;  // data offset from start of block (compressed data)

static ULONG  ulCRC        = 0;
static BOOL   bCRCENABLED  = FALSE;

/*************************************************************************/
/*                                                                       */
/* _cmpWrtBlk (basic write to disk fn)                                   */
/*                                                                       */
/*************************************************************************/


/*
 * xfers data from our staging buffer to zips internal buffer
 *
 */
INT fnReadBuff (PSZ pszBuff, PUINT puSize, PVOID pParam)
	{
	INT iRead;

	iRead = min ((INT)*puSize, iDATASIZE - iDATAINDEX);
	memcpy (pszBuff, pszDATABUFF + iDATAINDEX, iRead);
	iDATAINDEX += iRead;
	return iRead;
	}


/*
 * xfers data from zips internal buffer to the output file
 *
 */
void fnWriteFile (PSZ pszBuff, PUINT puSize, PVOID pParam)
	{
	if (bCRCENABLED)
		ulCRC = CRCBuff (ulCRC, pszBuff, *puSize);

	fwrite (pszBuff, 1, *puSize, fpFILE);
	iBLOCKSIZE += *puSize;
	}


/*
 * This fn takes a buffer containing data and writes it to disk as a 
 * compressed block.
 *
 * Params: fp       - The file to write to
 *         pszBuff  - The data buffer to write
 *         iLen     - The length of data in the buffer
 *         iType    - Use uDEFTYPE   or pcfp->uZipType  
 *         iWinSize - Use uDEFWINDOW or pcfp->uZipWindow
 *         piCLen   - Returns the size of the block written to disk
 *
 * Returns: Error Code (0 = ok)
 */
INT _cmpWrtBlk (FILE *fp, 
					 PSZ  pszBuff, 
					 INT  iLen, 
					 INT  iType,
					 INT  iWindow,
					 PINT piCLen)
	{
	INT   iRet;
	WORD  wMark = ZIP_MARK;
	ULONG ulPos1, ulPos2;

	if (piCLen) *piCLen = 0;

	if (!FilWriteUShort (fp, ZIP_MARK))   // write mark 
		return ZIP_CANT_WRITE;

	ulPos1 = ftell (fp);                  // store pos - we must write blocksize
	FilWriteUShort (fp, wMark);           // make room for the block len
	FilWriteUShort (fp, (USHORT)iLen);    // write data size

	fpFILE      = fp;                     // setup inter-zip globals
	pszDATABUFF = pszBuff;                //
	iDATASIZE   = iLen;                   //
	iDATAINDEX  = 0;                      //
	iBLOCKSIZE  = 0;                      //
	ulCRC       = CmpGetCRC ();
	bCRCENABLED = CmpCRCEnabled ();

	if (iRet = implode ((PFReadBuf) fnReadBuff, (PFWriteBuf) fnWriteFile, CmpGetBuff (), NULL, (PUINT)&iType, (PUINT)&iWindow))
		return ZIP_CANT_WRITE;

	if (!FilWriteUShort (fp, ZIP_MARK))      // write mark 
		return ZIP_CANT_WRITE;

	ulPos2 = ftell (fp);                     // store pos - we must restore
	fseek (fp, ulPos1, SEEK_SET);            // goto blocksize pos
	FilWriteUShort (fp, (USHORT)iBLOCKSIZE); // write blocksize (actually data size)
	fseek (fp, ulPos2, SEEK_SET);            // restore file position
	if (piCLen) *piCLen = iBLOCKSIZE + 8;    // set blocksize (data + header/trailer)
	return 0;
	}


/*************************************************************************/
/*                                                                       */
/* _cmpReadBlk (basic read from disk fn)                                 */
/*                                                                       */
/*************************************************************************/


/*
 * xfers data from a file to zips internal buffer
 *
 */
INT fnReadFile (PSZ pszBuff, PUINT puSize, PVOID pParam)
	{
	INT iRead;

	if (bCRCENABLED)
		ulCRC = CRCBuff (ulCRC, pszBuff, *puSize);

	iRead = fread(pszBuff, 1, min ((INT)*puSize, iBLOCKSIZE - iBLOCKINDEX), fpFILE);
	iBLOCKINDEX += iRead;
	return iRead;
	}


/*
 * xfers data from zips internal buffer to the staging buffer
 *
 */
void fnWriteBuff (PSZ pszBuff, PUINT puSize, PVOID pParam)
	{
	INT iSrcSize, iSize;

	iSrcSize = *puSize;

	/*--- Leading data to skip ---*/
	if (iDATAINDEX < iBUFFSTART)
		{
		iSize = min (iSrcSize, iBUFFSTART - iDATAINDEX);
		iDATAINDEX += iSize;
		iSrcSize   -= iSize;
		pszBuff    += iSize;
		}

	/*--- Data to read ---*/
	if (iSrcSize && (iDATAINDEX < iBUFFSTART + iBUFFSIZE))
		{
		iSize = min (iSrcSize, iBUFFSTART + iBUFFSIZE - iDATAINDEX);
		memcpy (pszDATABUFF + iDATASIZE, pszBuff, iSize);
		iDATAINDEX += iSize;
		iDATASIZE  += iSize;
		iSrcSize   -= iSize;
		}

	/*--- Trailing data to skip ---*/
	if (iSrcSize && (iDATAINDEX >= iBUFFSTART + iBUFFSIZE))
		iDATAINDEX += iSrcSize;
	}


/*
 * This fn reads a block of compressed data from disk and uncompresses
 * it (or part of it) to a buffer.
 *
 * Params: *fp         - The file to read from
 *         pszBuff     - The buffer to fill
 *         uMaxLen     - The available space in the buffer
 *         iBuffOffset - Skip this many bites before filling buffer from block
 *         iType       - use uDEFTYPE   or pcfp->uZipType  
 *         iWindow     - use uDEFWINDOW or pcfp->uZipWindow
 *         piBuffLen   - Returns the amount of data pit in the buffer
 *         piUBlockLen - Returns the amount of data in the block (maybe > buffer size)
 *         piCBlockLen - Returns the size of the compressed block
 *
 * Returns: Error Code (0 = ok)
 */
INT _cmpReadBlk (FILE *fp, 
					  PSZ  pszBuff, 
					  INT  iMaxLen, 
					  INT  iBuffOffset,
					  INT  iType,
					  INT  iWindow,
					  PINT piBuffLen,
					  PINT piUBlockLen, 
					  PINT piCBlockLen)
	{
	INT    iRet, iDataSize;
	USHORT wMark;

	if (piBuffLen)   *piBuffLen   = 0;
	if (piUBlockLen) *piUBlockLen = 0;
	if (piCBlockLen) *piCBlockLen = 0;

	wMark = FilReadUShort (fp);
	if (wMark == ZIP_TERM)           //
		return ZIP_END_OF_STREAM;     //
	if (wMark != ZIP_MARK)           //
		return ZIP_INVALID_DATA1;     //

	iBLOCKSIZE  = FilReadUShort (fp); // Read data size in block
	iDataSize   = FilReadUShort (fp); // Read uncomp data size

	fpFILE      = fp;                // Compressed data src
	pszDATABUFF = pszBuff;           // Staging buffer
	iBUFFSIZE   = iMaxLen;           // Size of staging buffer
	iBUFFSTART  = iBuffOffset;       // Start of buffer @
	iDATAINDEX  = 0;                 // Uncompressed offset from start of blk
	iBLOCKINDEX = 0;                 // Compressed   offset from start of blk
	iDATASIZE   = 0;                 // Size of data copied to staging buffer

	/*-- if iBuffOffset != 0, assume it is a re-read and don't regen crc ---*/
	ulCRC       = CmpGetCRC ();
	bCRCENABLED = CmpCRCEnabled () && !iBuffOffset;

	if (iRet = explode ((PFReadBuf) fnReadFile, (PFWriteBuf) fnWriteBuff, CmpGetBuff (), NULL))
		return ZIP_INVALID_DATA5;     //
	if (iDATAINDEX != iDataSize)     //
		return ZIP_INVALID_DATA3;     //

	if (FilReadUShort (fp) != ZIP_MARK) //
		return ZIP_INVALID_DATA4;        //

	if (piBuffLen)   *piBuffLen   = iDATASIZE;   // size of data copied to buffer
	if (piUBlockLen) *piUBlockLen = iDATAINDEX;  // full size of data in block
	if (piCBlockLen) *piCBlockLen = iBLOCKINDEX; // full size of data in block

	return 0;
	}


/*************************************************************************/
/*                                                                       */
/*                                                                       */
/*                                                                       */
/*************************************************************************/

BOOL _cmpWriteTerm (FILE *fp)
	{
	return FilWriteUShort (fp, ZIP_TERM);
	}




