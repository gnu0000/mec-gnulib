/*
 * Zip4.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * High level compression functions
 *
 * This file contains functions to provide translapent access to 
 * compressed data stored in files.  Open a file with CmpOpen or
 * CmpOpen2, read and write using CmpRead & CmpWrite, and close
 * with CmpClose.
 *
 * As we write, we add the data to a staging buffer (pcFile->pszBuff)
 * until it is full, at which point we write out a block.  One large
 * write can result in several blocks.  The block size is limited by
 * the size of the staging buffer, which is defined by a CmpOpen param.
 * the block size could be slightly bigger than the staging buffer, because
 * compressing random data can make the data get larger, but not much.
 * Note that the last block will usually be smaller than the others.
 *
 * As we read, we first read a block into the staging buffer.  We then copy
 * data from the staging buffer to the CmpRead buffer as needed.  When the
 * staging buffer is empty, we read in the next block.  One large read
 * could cause several blocks to be read in.  Several small reads could
 * come from the same block.  The condition could arise where the size
 * of the uncompressed data block will not fit in the staging buffer.
 * This will only occur when buffer A, used when writing, is larger than
 * buffer B, which is used for reading.  Because if this, we must:
 *
 * Store the file position before reading a block
 * If data doesn't fit, store how much did fit. and skip rest of block
 * When buffer is exhausted, restore file pos and re-read block, skipping
 * the data that was read in last time, reading in the next piece. ...
 * The LoadBuffer fn takes care of this for us (isn't that nce of it?).
 *
 *
 ****************************************************************************
 *
 * PCFILE CmpOpen (pszFile, pszFlags, pszWorkBuff, iBuffSize)
 * PCFILE CmpOpen2(fp, pszWorkBuff, iBuffSize)
 * PCFILE CmpClose (pcfp)
 *
 * INT   CmpRead (pcfp, pBuff, iLen)
 * INT   CmpReadStr (pcfp, pBuff, iMaxLen)
 * INT   CmpWrite (pcfp, pBuff, iLen)
 *
 * INT   CmpFlush (pcfp)
 * INT   CmpIsErr (pcfp)
 * BOOL   CmpEOS (pcfp)
 * PCFILE CmpWriteTerm (pcfp)
 * PCFILE CmpSetMethod (pcfp, iType, iWindow)
 * void   CmpSetTerminators (iWriteTerm, iReadTerms)
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


static INT iWRITETERM = ZIP_NULL;  // End of line terminator id's
static INT iREADTERM  = ZIP_NULL;  // End of line terminator id's
static INT iGLOBALERROR = 0;       // last global error

/***********************************************************************/
/*                                                                     */
/*  Error Handling Functions                                           */
/*                                                                     */
/***********************************************************************/


/*
 * Returns 0 if all is ok
 * Returns error code on error
 */
INT CmpIsErr (PCFILE pcfp)
	{
	if (!pcfp)
		return iGLOBALERROR;
	return pcfp->iError;
	}


/*
 * called internally to set an error and return NULL
 * call with pcfp=NULL to set the global error
 */
PSZ _cmpSetErrp (PCFILE pcfp, INT iErr)
	{
	if (pcfp)
		pcfp->iError = iErr;
	else
		iGLOBALERROR = iErr;
	return NULL;   
	}


/*
 * called internally to set an error and return 0
 * call with pcfp=NULL to set the global error
 */
INT _cmpSetErru (PCFILE pcfp, INT iErr)
	{
	_cmpSetErrp (pcfp, iErr);
	return 0;
	}


/***********************************************************************/
/*                                                                     */
/* CmpWrite (compress & write                                          */
/*                                                                     */
/***********************************************************************/

/*
 * writes the contents of the staging buffer as a block to disk.
 * The format of a block is defined in the header file
 * The size of the block is returned or 0 if error
 */
INT CmpFlush (PCFILE pcfp)
	{
	INT iRet, iCLen;
	ULONG  ulGlobalCRC;

	if (!pcfp || !pcfp->iDataSize || !pcfp->bWriting)
		return 0;

	ulGlobalCRC = CmpGetCRC ();
	CmpSetCRC (pcfp->ulCRC);

	iRet = _cmpWrtBlk (pcfp->fp, pcfp->pszBuff, pcfp->iDataSize,
							 pcfp->iZipType, pcfp->iZipWindow, &iCLen);

	pcfp->ulCRC = CmpGetCRC ();
	CmpSetCRC (ulGlobalCRC);

	if (iRet)
		_cmpSetErru (pcfp, iRet);

	pcfp->ulUDataPos += pcfp->iDataSize;  // update uncomp data position 
	pcfp->ulCDataPos += iCLen;            // update comp   data position 
	pcfp->iDataSize = 0;                  // buffer now empty

	return (iRet ? 0 : iCLen);            // retern amt wrote unless error
	}


/*
 * Adds data to the staging buffer. When the buffer is full it is dumped.
 * This fn only adds as much data as can fit in the available space in 
 * the staging buffer.  The amount actually added is returned
 */
static INT AddToBuffer (PCFILE pcfp, PSZ pszData, INT iDataLen)
	{
	INT i, j;

	for (j=0, i = pcfp->iDataSize; i < pcfp->iBuffSize && j < iDataLen; i++, j++)
		pcfp->pszBuff[i] = pszData[j];

	pcfp->iDataSize = i;
	if (i && (i >= pcfp->iBuffSize)) // Dump buffer if full
		if (!CmpFlush (pcfp))
			return 0; // we got an error
	return j;
	}

/*
 * Compresses and Writes data to a file
 * if iLen == 0 then data is assumed to be NULL terminated
 * Fn returns length of data written.  This should by iLen if not 0
 *  or the length of the string plus the length of the terminator (1 or 2)
 * If an error, Call CmpIsErr and/or CmpGetErr to get error val / string
 */
INT CmpWrite (PCFILE pcfp, PVOID pBuff, INT iLen)
	{
	PSZ    pszPtr;
	INT iAdded = 0, iWrote;
	BOOL   bTerm;

	if (!CmpInitialized () || !pcfp)
		return _cmpSetErru (NULL, ZIP_NOT_INITIALIZED);

	pcfp->bWriting = TRUE;
	pszPtr = (PSZ) pBuff;
	bTerm  = !iLen;
	iLen = (iLen ? iLen : (pszPtr ? strlen (pszPtr) : 0));

	for (iWrote = 0; iWrote < iLen; pszPtr += iAdded, iWrote += iAdded)
		{
		if (!(iAdded = AddToBuffer (pcfp, pszPtr, iLen - iWrote)))
			break; // there was an error
		}

	if ((iAdded || !iLen) && bTerm) // no error and terminator required
		{
		if (iWRITETERM & ZIP_CRLF)  
			pszPtr = "\x0D\x0A", iLen = 2;
		else if (iWRITETERM & ZIP_LF)    
			pszPtr = "\x0A",     iLen = 1;
		else // ZIP_NULL
			pszPtr = "\x00",     iLen = 1;

		iAdded = AddToBuffer (pcfp, pszPtr, iLen);
		iWrote += iAdded;
		}
	return iWrote;
	}


/***********************************************************************/
/*                                                                     */
/* CmpRead (read & uncompress)                                         */
/*                                                                     */
/***********************************************************************/


/*
 * Loads data from a block on disk and writes it to the staging buffer
 * Consecutive calls to this fn read consecutive data.  If the staging
 * buffer is smaller than the blocks data length.  That block will be
 * read more than once on subsequent calls.
 */
INT LoadBuffer (PCFILE pcfp)
	{
	INT iRet;
	ULONG  ulGlobalCRC;

	if (pcfp->bEOS)
		return ZIP_END_OF_STREAM;

	/*
	 * Update uncompressed data offset
	 * pcfp->ulUDataPos = the number of bytes read in previous buffers
	 */
	pcfp->ulUDataPos += (ULONG) pcfp->iDataSize;

	/*
	 * if pcfp->iBuffStartOffset != 0 then the previous read only read
	 * in a partial block.  We must now setup to re-read the block
	 * and read in the next part of the data.
	 */
	if (pcfp->iBuffStartOffset)
		fseek (pcfp->fp, pcfp->ulFilePos, SEEK_SET);  // goto blocksize pos
	else
		{
		pcfp->ulCDataPos += (ULONG) pcfp->iBlockCSize;
		pcfp->ulFilePos   = ftell (pcfp->fp);
		}

	pcfp->iOffset = 0;

	ulGlobalCRC = CmpGetCRC ();
	CmpSetCRC (pcfp->ulCRC);

	iRet = _cmpReadBlk (pcfp->fp, pcfp->pszBuff, pcfp->iBuffSize, 
							  pcfp->iBuffStartOffset, pcfp->iZipType, 
							  pcfp->iZipWindow,   &pcfp->iDataSize, 
							  &pcfp->iBlockUSize, &pcfp->iBlockCSize);

	pcfp->ulCRC = CmpGetCRC ();
	CmpSetCRC (ulGlobalCRC);

	if (iRet == ZIP_END_OF_STREAM)
		{
		pcfp->ulCDataPos += 2;      // I'm not sure i wanna do this
		pcfp->bEOS        = TRUE;
		pcfp->iDataSize   = 0;
		return _cmpSetErru (pcfp, ZIP_END_OF_STREAM);
		}

	/*
	 * If we didn't read to the end of the block then were gonna have
	 * to re-read the block next time
	 */
	if (pcfp->iBlockUSize > pcfp->iBuffStartOffset + pcfp->iDataSize)
		pcfp->iBuffStartOffset += pcfp->iDataSize;
	else
		pcfp->iBuffStartOffset = 0;

	return pcfp->iDataSize;
	}                    



/*
 * Extracts data from the staging buffer.  
 * If the buffer is empty it is loaded with data
 * If the data does not fully exist in the buffer, only the portion 
 * present is extracted.
 */
static INT ReadFromBuffer (PCFILE pcfp, PSZ pszData, INT iLen, PINT piNullTerm)
	{
	INT i, j, c;

	if (pcfp->iOffset >= pcfp->iDataSize)
		if (!LoadBuffer (pcfp))
			return 0; // we got an error

	for (j=0, i=pcfp->iOffset; i < pcfp->iDataSize && j < iLen; i++, j++)
		{
		c = pszData[j] = pcfp->pszBuff[i];

		if (*piNullTerm && 
			 (((iREADTERM & ZIP_NULL) && !c)          ||
			  ((iREADTERM & ZIP_LF  ) && (c == '\n')) || 
			  ((iREADTERM & ZIP_CRLF) && (c == '\n')) ))
			{
			(*piNullTerm)++;
			pcfp->iOffset = i+1;  // skipp curr ptr past null sep
			return j+1;           // null sep part of string length
			}
		}
	pcfp->iOffset = i;
	return j;
	}


/*
 * Reads data from the file
 * if iLen  == 0 then data is assumed to be NULL terminated (See CmpReadStr)
 * Data is read into pBuff
 * Fn returns the number of bytes actually read
 * If an error, Call CmpIsErr and/or CmpGetErr to get error val / string
 */
INT CmpRead (PCFILE pcfp, PVOID pBuff, INT iLen)
	{
	PSZ  pszPtr;
	INT iNullTerm;
	INT iAdded, iRead;

	if (!CmpInitialized () || !pcfp)
		return _cmpSetErru (NULL, ZIP_NOT_INITIALIZED);

	pszPtr    = (PSZ) pBuff;
	iNullTerm = !iLen;  // 0=no 1=yes
	iLen      = (iLen ? iLen : 32767);

	for (iRead = 0; iRead < iLen; pszPtr += iAdded)
		{
		if (!(iAdded = ReadFromBuffer (pcfp, pszPtr, iLen-iRead, &iNullTerm)))
			break;

		iRead += iAdded;
		if (iNullTerm > 1) // This means null terminated, and null term read
			break;
		}
	return iRead;
	}


/*
 * Reads a null terminated string from the file
 * if iMaxLen is the largest string the buffer can handle (including term)
 * Data is read into pBuff
 * Fn returns the number of bytes actually read (including null term)
 * If an error, Call CmpIsErr and/or CmpGetErr to get error val / string
 */
INT CmpReadStr (PCFILE pcfp, PVOID pBuff, INT iMaxLen)
	{
	PSZ    pszPtr;
	INT iAdded, iRead, iNullTerm = 1;

	if (!CmpInitialized () || !pcfp)
		return _cmpSetErru (NULL, ZIP_NOT_INITIALIZED);

	pszPtr = (PSZ) pBuff;
	for (iRead = 0; iRead < iMaxLen; pszPtr += iAdded)
		{
		if (!(iAdded = ReadFromBuffer (pcfp, pszPtr, iMaxLen-iRead, &iNullTerm)))
			break;
		iRead += iAdded;
		if (iNullTerm > 1)
			break;
		}
	return iRead;
	}

/***********************************************************************/

/*
 * This fn defines the line terminators used to read and write strings
 * iWriteTerm is the terminator to use when writing a string
 * iReadTerms are the terminators that define a string to read in
 *
 * iWriteTerm is one of
 * iRead Term is one or more of
 * ZIP_NULL
 * ZIP_LF
 * ZIP_CRLF
 */
void CmpSetTerminators (INT iWriteTerm, INT iReadTerms)
	{
	iWRITETERM = iWriteTerm;
	iREADTERM  = iReadTerms;
	}


/***********************************************************************/
/*                                                                     */
/* Open / Close functions                                              */
/*                                                                     */
/***********************************************************************/

/*
 * Normally, this is handled automatically when you call CmpClose
 * on a stream.  
 * Call this fn when you want more than one compression stream in a
 * single file (like in a zip library)
 *
 * To access 2 consecutive compression streams, read until you get EOS
 * then set pcfp->bEOS=FALSE and then start reading the next stream
 *
 */
PCFILE CmpWriteTerm (PCFILE pcfp)
	{
	if (!CmpInitialized () || !pcfp)
		return (PCFILE)_cmpSetErrp (pcfp, ZIP_NOT_INITIALIZED);

	CmpFlush (pcfp);
	if (_cmpWriteTerm (pcfp->fp))
		pcfp->ulCDataPos += sizeof (INT);
	else
		_cmpSetErru (pcfp, ZIP_CANT_WRITE);
	return pcfp;
	}



/*
 * Cleanup after reading / writing.
 * This Fn:
 *    Closes file handle (if initialized with CmpInitFile)
 *    Frees the pszFileBuff if one was malloced
 *    Performs an all important buffer flush if writing. 
 */
PCFILE CmpClose (PCFILE pcfp)
	{
	if (!CmpInitialized () || !pcfp)
		return (PCFILE)_cmpSetErrp (NULL, ZIP_NOT_INITIALIZED);

	if (pcfp->bWriting)
		CmpWriteTerm (pcfp); // this flushes first

	if (pcfp->bBufferMalloced && pcfp->pszBuff)
		free (pcfp->pszBuff);

	if (pcfp->bCloseAtTerm)
		fclose (pcfp->fp);
		
	free (pcfp);
	return NULL;
	}


/*
 * see CmpOpen comments
 *
 */
PCFILE CmpOpen2(FILE *fp, PSZ pszWorkBuff, INT iBuffSize)
	{
	PCFILE pcfp;

	if (!CmpInitialized ())
		return (PCFILE)_cmpSetErrp (NULL, ZIP_NOT_INITIALIZED);

	if (!(pcfp = (PCFILE) calloc (sizeof (CFILE), 1)))
		return (PCFILE)_cmpSetErrp (NULL, ZIP_MEMORY);

	if (pszWorkBuff)
		{
		pcfp->pszBuff = pszWorkBuff;
		pcfp->bBufferMalloced = FALSE;
		}
	else
		{
		if (!(pcfp->pszBuff = (iBuffSize ? (PSZ) malloc (iBuffSize) : NULL)))
			return (PCFILE)_cmpSetErrp (NULL, ZIP_MEMORY);
		pcfp->bBufferMalloced = !!iBuffSize;
		}
	pcfp->iBuffSize    = iBuffSize;
	pcfp->fp           = fp;
	pcfp->bCloseAtTerm = FALSE;
	pcfp->iZipType     = _cmpDefaultType ();
	pcfp->iZipWindow   = _cmpDefaultWindow ();
	pcfp->ulOrigin     = ftell (fp);

	return pcfp;
	}

/*
 * One of these fns must be called before reading/writing a file
 * These fns are similar, the difference being that CmpInitFile2 is 
 *  called if you already have a file handle
 *
 * CmpOpen: 
 *  pszFile     - The file to open for reading or writing
 *  pszFlags    - Same as flags for fopen, used to open the file
 *  pszWorkBuff - Buffer to use to (de)compress data. If null, it is malloced
 *  iBuffSize   - Size of buffer or size to malloc.  If writing, this can be 0
 *
 * CmpOpen2: 
 *  fpFile      - The file handle.
 *  pszWorkBuff - Buffer to use to (de)compress data. If null, it is malloced
 *  iBuffSize   - Size of buffer or size to malloc.  If writing only, this can
 *                 be 0.
 *
 * Use "wb" for pszFlags if your going to write to the file
 * Use "rb" for pszFlags if your going to read from the file
 *
 * A PCFILE file handle is returned, or NULL on error
 *
 * When compressing (CmpWrite) a larger buffer creates smaller files
 * When uncompressing (CmpRead) a larger buffer gives better perfrormance
 * A serious performance penalty occurs if you read a file using a smaller
 *  buffer than was used to write the file.
 *
 * 4k or 8k is a good bufer size
 */
PCFILE CmpOpen (PSZ pszFile, PSZ pszFlags, PSZ pszWorkBuff, INT iBuffSize)
	{
	FILE *fp;
	PCFILE pcfp;

	if (!CmpInitialized ())
		return (PCFILE)_cmpSetErrp (NULL, ZIP_NOT_INITIALIZED);

	if (!(fp = fopen (pszFile, pszFlags)))
		return (PCFILE)_cmpSetErrp (NULL, ZIP_CANT_OPEN);

	if (!(pcfp = CmpOpen2(fp, pszWorkBuff, iBuffSize)))
		fclose (fp);
	else
		pcfp->bCloseAtTerm = TRUE;

	return pcfp;
	}


/*
 * After you have opened a file stream with CmpInitFile or CmpInitFile2,
 * you can call this fn to alter the compression method.
 * The options are:
 *
 *    iType (data encode range specifier)
 *    -----
 *    CMP_BINARY - Best compression method for binary data [default]
 *    CMP_ASCII  - Best compression method for text data 
 *
 *    iWindow (sliding search window size)
 *    -------
 *    1024 -  Fastest compression
 *    2048 -
 *    4096 -  Best compression (usually) [default]
 *
 * If you use a small Work Buffer, use a iWindow size <= iBuffSize
 *  for optimal results
 */
PCFILE CmpSetMethod (PCFILE pcfp, INT iType, INT iWindow)
	{
	if (!CmpInitialized () || !pcfp)
		return (PCFILE)_cmpSetErrp (NULL, ZIP_NOT_INITIALIZED);

	if (iType == CMP_BINARY || iType == CMP_ASCII)
		pcfp->iZipType = iType;
	if (iWindow == 1024 || iWindow == 2048 || iWindow == 4096)
		pcfp->iZipWindow = iWindow;
	return pcfp;
	}



/***********************************************************************/
/*                                                                     */
/* Misc Functions                                                      */
/*                                                                     */
/***********************************************************************/


BOOL CmpEOS (PCFILE pcfp)
	{
	return pcfp->bEOS;
	}
}