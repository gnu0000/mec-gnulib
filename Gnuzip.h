/*
 * GnuZip.h
 *
 * (C) 1994-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file provides compression functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 * 
 * *** High Level API (transparent access) ***
 * -----------------------------------
 * BOOL   CmpInit  (pszZipBuff, uDefType, uDefWindow)
 *
 * PCFILE CmpOpen  (pszFile, pszFlags, pszWorkBuff, uBuffSize)
 * PCFILE CmpOpen2 (fp, pszWorkBuff, uBuffSize)
 * PCFILE CmpClose (pcfp)
 *
 * BOOL  CmpRewind (pcfp)
 * ULONG CmpGetPos (pcfp)
 * BOOL  CmpSetPos (pcfp, ulFilePos)
 * -------------------------------------------------
 * INT    CmpRead  (pcfp, pBuff, uLen)
 * BYTE   CmpReadByte
 * SHORT  CmpReadShort
 * USHORT CmpReadUShort
 * LONG   CmpReadLong
 * ULONG  CmpReadULong
 * BIG    CmpReadFloat
 * INT    CmpReadStr(pcfp, pBuff, uMaxLen)
 *
 * INT    CmpWrite (pcfp, pBuff, uLen)
 * BOOL   CmpWriteByte
 * BOOL   CmpWriteShort
 * BOOL   CmpWriteUShort
 * BOOL   CmpWriteLong
 * BOOL   CmpWriteULong
 * ULONG  CmpWriteFloat
 * -------------------------------------------------
 * INT   CmpIsErr  (pcfp)
 * PSZ   CmpGetErr (PCFILE pcfp)
 *
 * INT    CmpFlush (pcfp)
 * BOOL   CmpEOS   (pcfp)
 * PCFILE CmpWriteTerm (pcfp)
 *
 * ULONG  CmpEnableCRC (BOOL bEnable);
 * BOOL   CmpCRCEnabled (void);
 * ULONG  CmpGetCRC (void);
 * ULONG  CmpSetCRC (ULONG ulNewCRC);
 * ULONG  CmpGenerateCRC (FILE *fp);
 * PCFILE CmpSetMethod (pcfp, uType, uWindow)
 * void   CmpSetTerminators (uWriteTerm, uReadTerms)
 *
 * ULONG  CmpImplodeFile (pcfpOut, fpIn,  pulReadSize)
 * ULONG  CmpExplodeFile (pcfpIn,  fpOut, pulReadSize)
 *
 * PSZ    CmpGetBuff ()
 * BOOL   CmpInitialized ()
 *
 * *** Low level API ***
 * ---------------------------------------------
 * INT  Cmp0Read       (fp, pszBuff, uMaxLen, puLen)
 * INT  Cmp0Write      (fp, pszBuff, uLen,    bTerm, puLen)
 * INT  Cmp0ReadBlock  (fp, pszBuff, uMaxLen, puLen)
 * INT  Cmp0WriteBlock (fp, pszBuff, uLen,    puLen)
 * INT  Cmp0ImplodeStr (pDest, uDestLen, pSrc, uSrcLen)
 * INT  Cmp0ExplodeStr (pDest, uDestLen, pSrc)
 * INT  Cmp0ImplodeFile(fpDest, fpSrc, bTerm, pulDestLen, pulSrcLen)
 * INT  Cmp0ExplodeFile(fpDest, fpSrc, pulDestLen, pulSrcLen)
 * INT  Cmp0SetStatusFn(pfnStatusFn)
 * INT  Cmp0SetBlockSize (uSize)
 * void Cmp0SetXlateBuffer (PFNX pfnXlate)
 *
 * *** Internal functions ***
 * ---------------------------------------------
 * INT  _cmpWrtBlk  (*fp, pszBuff, uLen, uType, uWindow, puCLen)
 * INT  _cmpReadBlk (*fp, pszBuff, uMaxLen, uBuffOffset, uType,
 *                     uWindow, puBuffLen, puUBlockLen, puCBlockLen)
 * BOOL   _cmpWriteTerm (FILE *fp)
 * INT  _cmpDefaultType ()
 * INT  _cmpDefaultWindow ()
 *
 *
 * Old API (buffer and file translates)
 * ---------------------------------------------
 * Cmp2Init  (pszBuff, uWinSize, iMode);
 * Cmp2szIsz (pszImploded, uCBuffSize, puSize, pszExploded, uDataSize)
 * Cmp2szEsz (pszExploded, uBuffSize, puSize, pszImploded)
 * Cmp2szIfp (*fp, puSize, pszBuff, uDataSize)
 * Cmp2fpEsz (pszBuff, uBuffSize, puSize, *fp)
 * Cmp2fpIfp (*fpOut, puOut, *fpIn, uDataSize, puIn)
 * Cmp2fpEfp (*fpOut, puOut, *fpIn, puIn)
 * BOOL  Cmp2EnableCRC (bEnable, bRead)
 * BOOL  Cmp2CRCEnabled (bRead)
 * ULONG Cmp2SetCRC    (ulVal, bRead)
 * ULONG Cmp2GetCRC    (bRead)
 *
 *
 * Base API (from PKWARE Data Compression Library)
 * --------------------------------------------------------
 * INT    implode (readfn, writefn, pszWorkBuff, uType, uSize)
 * INT    explode (readfn, writefn, pszWorkBuff)
 * ULONG  crc32 (pszBuff, uSize, ulOldCRC)
 *
 ****************************************************************************
 *
 * This module contains 3 independent API's for working with data compression.
 * The high level API is the simplest to use.
 * The Old API is used to compress and uncompress data from buffers
 * and files.  This API is older and more cumbersome. The low level API is 
 * taken directly from the PKWARE Zip library. This API is simply the implode
 * and explode function itself.
 *
 * *** High Level API (transparent access) ***
 * -------------------------------------------
 * You cannot read and write a file at the same time.
 * To change from read to write or vise versa, close and re-open the file.
 * You must call CmpClose when done or the file will be corrupt.
 * The last 2 bytes of a compressed stream is a termination marker.  This
 * must be removed to append compressed data.
 *
 *
 * To Write a compressed file:
 *   call CmpInit to initialize the compression module if you didn't already
 *   call CmpOpen to open a file and retrieve a CFILE handle.
 *   write to the file using CmpWrite, you can call any number of times
 *   call CmpClose when done.
 *
 * To Read a compressed file:
 *   call CmpInit to initialize the compression module if you didn't already
 *   call CmpOpen to open a file and retrieve a CFILE handle.
 *   read data from the file using CmpRead calls.
 *   call CmpClose when done.
 *
 *
 * Old API (buffer and file translates)
 * ---------------------------------------------
 *
 * In this API, the following nomenclature/notation is used:
 *  Imploded data is data that has been compressed 
 *  Exploded data is data that has been uncompressed, hence the original data
 *  The functions in this module implode and explode data to and from
 *  memory buffers and files.  Functions are named as follows:
 *
 *  (Source)(operation)(Destination)
 *
 *  where Source and Destination either sz or fp (for buffer or file)
 *  and operation is either I (for Implode) or E (for Explode)
 *
 *  so that the function szIfp takes the data from a buffer, compresses it
 *  and writes the compressed data to a file handle.
 *
 * The parameters are ordered so that the destination parameters always
 * preceed the source parameters. Note that When the source data is 
 * compressed data, there is no source data size parameter, as the size
 * of a compressed stream is stored in that stream.
 *
 * see individual function header declarations below for more info
 * about the function.
 *
 *
 * Base API (from PKWARE Data Compression Library)
 * --------------------------------------------------------
 *
 * See PKWARE ZipLib Docs
 */

#if !defined (GNUZIP_INCLUDED)
#define GNUZIP_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif


/*
 * Compression module errors
 * Values returned from CmpIsError
 */
#define ZIP_OK              0
#define ZIP_END_OF_STREAM   1
#define ZIP_NOT_INITIALIZED 2
#define ZIP_CANT_OPEN       3
#define ZIP_CANT_READ       4
#define ZIP_CANT_WRITE      5
#define ZIP_MEMORY          6
#define ZIP_INVALID_DATA1   7
#define ZIP_INVALID_DATA2   8
#define ZIP_INVALID_DATA3   9
#define ZIP_INVALID_DATA4   10
#define ZIP_INVALID_DATA5   11
#define ZIP_BUFF_TO_SMALL   12
#define ZIP_ABORT           13



/***********************************************************************/
/*                                                                     */
/* High Level API                                                      */
/*                                                                     */
/***********************************************************************/

/*
 * This is the CFILE handle data structure
 * use CFILE as a handle similar to the way you use a FILE handle
 *
 */
typedef struct
   {
   FILE  *fp;             // File handle to read to / write from
   ULONG ulOrigin;        // File position at open - used by CmpRewind
   ULONG ulFilePos;       // File position for current data block
   ULONG ulUDataPos;      // offset of uncompressed data (to start of current buffer)
   ULONG ulCDataPos;      // offset of compressed data (to start of current block)
   ULONG ulCRC;           // file CRC generated during reads / writes

   PSZ   pszBuff;         // Staging buffer
   INT   iBuffSize;       // Size of Staging Buffer
   INT   iDataSize;       // Size of data in Staging Buffer
   INT   iOffset;         // Offset into Staging buffer to current data
   INT   iBuffStartOffset;// Buffer data starts at this offset in block data

   INT   iBlockCSize;     // Size of compressed data in current data block
   INT   iBlockUSize;     // Size of data in data block when uncompressed

   BOOL  bEOS;            // TRUE if End Of Stream (Reading)
   BOOL  bWriting;        // TRUE if were writing
   BOOL  bCloseAtTerm;    // TRUE if we close fp at cleanup (CmpOpen vs CmpOpen2)
   BOOL  bBufferMalloced; // TRUE if buffer was malloced internally

   INT   iError;          // the current Error code
   INT   iZipType;        // Data encode range specifier (CMP_BINARY)
   INT   iZipWindow;      // Sliding search window size (4096)
   } CFILE;
typedef CFILE *PCFILE;


/*
 * Internal use
 */
#define ZIP_MARK 0x1248
#define ZIP_TERM 0x3579

/*
 * Valid range specifiers
 * uType field in CmpInit and CmpSetMethod
 */
#define CMP_BINARY     0
#define CMP_ASCII      1


/*
 * Valid line terminator specifiers for CmpSetTerminators
 */
#define ZIP_NULL       1
#define ZIP_LF         2
#define ZIP_CRLF       4


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
EXP BOOL CmpInit (PSZ pszZipBuff, INT iDefType, INT iDefWindow);


/*
 * One of these fns must be called before reading/writing a file
 * These fns are similar, the difference being that CmpInitFile2 is 
 *  called if you already have a file handle
 *
 * CmpOpen: 
 *  pszFile     - The file to open for reading or writing
 *  pszFlags    - Same as flags for fopen, used to open the file
 *  pszWorkBuff - Buffer to use to (de)compress data. If null, it is malloced
 *  uBuffSize   - Size of buffer or size to malloc.  If writing, this can be 0
 *
 * CmpOpen2: 
 *  fpFile      - The file handle.
 *  pszWorkBuff - Buffer to use to (de)compress data. If null, it is malloced
 *  uBuffSize   - Size of buffer or size to malloc.  If writing only, this can
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
EXP PCFILE CmpOpen (PSZ pszFile, PSZ pszFlags, PSZ pszWorkBuff, INT iBuffSize);
EXP PCFILE CmpOpen2(FILE *fp, PSZ pszWorkBuff, INT iBuffSize);


/*
 * Cleanup after reading / writing.
 * This Fn:
 *    Closes file handle (if initialized with CmpInitFile)
 *    Frees the pszFileBuff if one was malloced
 *    Performs an all important buffer flush if writing. 
 */
EXP PCFILE CmpClose (PCFILE pcfp);


/*
 * Reads data from the file
 * if uLen  == 0 then data is assumed to be NULL terminated (See CmpReadStr)
 * Data is read into pBuff
 * Fn returns the number of bytes actually read
 * If an error, Call CmpIsErr and/or CmpGetErr to get error val / string
 */
EXP INT CmpRead (PCFILE pcfp, PVOID pBuff, INT iLen);


/*
 * Reads a null terminated string from the file
 * if uMaxLen is the largest string the buffer can handle (including term)
 * Data is read into pBuff
 * Fn returns the number of bytes actually read (including null term)
 * If an error, Call CmpIsErr and/or CmpGetErr to get error val / string
 */
EXP INT CmpReadStr (PCFILE pcfp, PVOID pBuff, INT iMaxLen);


/*
 * Compresses and Writes data to a file
 * if uLen == 0 then data is assumed to be NULL terminated
 * Fn returns length of data written.  This should by uLen if not 0
 *  or the length of the string plus the length of the terminator (1 or 2)
 * If an error, Call CmpIsErr and/or CmpGetErr to get error val / string
 */
EXP INT CmpWrite (PCFILE pcfp, PVOID pBuff, INT iLen);


EXP BYTE  CmpReadByte   (PCFILE pcfp);
EXP SHORT CmpReadShort  (PCFILE pcfp);
EXP USHORT CmpReadUShort(PCFILE pcfp);
EXP LONG  CmpReadLong   (PCFILE pcfp);
EXP ULONG CmpReadULong  (PCFILE pcfp);
EXP BIG   CmpReadFloat  (PCFILE pcfp);
EXP BOOL  CmpWriteByte  (PCFILE pcfp, BYTE c);
EXP BOOL  CmpWriteShort (PCFILE pcfp, SHORT  s);
EXP BOOL  CmpWriteUShort(PCFILE pcfp, USHORT us);
EXP BOOL  CmpWriteLong  (PCFILE pcfp, LONG l);
EXP BOOL  CmpWriteULong (PCFILE pcfp, ULONG ul);
EXP BOOL  CmpWriteFloat (PCFILE pcfp, BIG bg);



/*
 * Rewinds the file ptr to the beginning of the data stream
 * The beginning of the data stream is defined as the file position at
 * the time of the CmpOpen or CmpOpen2 call
 */
EXP BOOL CmpRewind (PCFILE pcfp);


/*
 * Get current file position
 */
EXP ULONG CmpGetPos (PCFILE pcfp);


/*
 * Set file position
 */
EXP BOOL CmpSetPos (PCFILE pcfp, ULONG ulFilePos);

/*
 * Returns 0 if all is ok
 * Returns error code on error
 */
EXP INT CmpIsErr (PCFILE pcfp);


/*
 * Returns NULL if all is ok
 * Returns error string on error
 */
EXP PSZ CmpGetErr (PCFILE pcfp);


/*
 * writes the contents of the staging buffer as a block to disk.
 * The format of a block is defined in the header file
 * The size of the block is returned or 0 if error
 */
EXP INT CmpFlush (PCFILE pcfp);


EXP BOOL CmpEOS (PCFILE pcfp);


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
EXP PCFILE CmpWriteTerm (PCFILE pcfp);


/*
 * Enable / Disable generation of CRC during Reads / writes
 *
 */
EXP ULONG CmpEnableCRC (BOOL bEnable);
EXP BOOL CmpCRCEnabled (void);


/*
 * Set the current CRC value
 *
 */
EXP ULONG CmpSetCRC (ULONG ulNewCRC);


/*
 * Get the current CRC value
 *
 */
EXP ULONG CmpGetCRC (void);


/*
 * Load the compressed stream, generating its CRC
 *
 */
EXP ULONG CmpGenerateCRC (FILE *fp);


/*
 * After you have opened a file stream with CmpInitFile or CmpInitFile2,
 * you can call this fn to alter the compression method.
 * The options are:
 *
 *    uType (data encode range specifier)
 *    -----
 *    CMP_BINARY - Best compression method for binary data [default]
 *    CMP_ASCII  - Best compression method for text data 
 *
 *    uWindow (sliding search window size)
 *    -------
 *    1024 -  Fastest compression
 *    2048 -
 *    4096 -  Best compression (usually) [default]
 *
 * If you use a small Work Buffer, use a uWindow size <= uBuffSize
 *  for optimal results
 */
EXP PCFILE CmpSetMethod (PCFILE pcfp, INT iType, INT iWindow);


/*
 * This fn defines the line terminators used to read and write strings
 * uWriteTerm is the terminator to use when writing a string
 * uReadTerms are the terminators that define a string to read in
 *
 * uWriteTerm is one of
 * uRead Term is one or more of
 * ZIP_NULL
 * ZIP_LF
 * ZIP_CRLF
 */
EXP void CmpSetTerminators (INT iWriteTerm, INT iReadTerms);


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
EXP ULONG CmpExplodeFile (PCFILE pcfpIn, FILE *fpOut, PULONG pulReadSize);


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
EXP ULONG CmpImplodeFile (PCFILE pcfpOut, FILE *fpIn, PULONG pulReadSize);


typedef void (*STATUSPROC) (ULONG ul);

EXP VOID Cmp0SetStatusFn (STATUSPROC pfnStatusFn);


/*
 * External access to the zip buffer 
 * should somebody want it
 */
EXP PSZ CmpGetBuff (void);


EXP BOOL CmpInitialized (void);


/*
 * Read a block stream into a string up to /including the terminator
 * fp      - file to read from
 * pszBuff - destination buffer
 * uMaxLen - size of destination buffer
 * puLen   - size of data read in to buffer
 *
 * returns: Error Code (0 = OK)
 */
EXP INT Cmp0Read (FILE *fp, PSZ pszBuff, INT iMaxLen, PINT piLen);


/*
 * Write a block stream. Does not write the terminator
 * fp      - file to write to
 * pszBuff - src buffer
 * iLen    - size to write
 * bTerm   - writes termination marker if true
 * piLen   - size of data written (the compressed stream length)
 *
 * returns: Error Code (0 = OK)
 */
EXP INT Cmp0Write (FILE *fp, PSZ pszBuff, INT iLen, BOOL bTerm, PINT piLen);


/*
 * read a block
 * fp      - file to read from
 * pszBuff - destination buffer
 * iMaxLen - size of destination buffer
 * piLen   - size of data read in to buffer
 *
 * returns: Error Code (0 = OK)
 */
EXP INT Cmp0ReadBlock (FILE *fp, PSZ pszBuff, INT iMaxLen, PINT piLen);


/*
 * write a block.
 * fp      - file to write to
 * pszBuff - src buffer
 * iLen    - size to write
 * piLen   - size of data written (the block length)
 *
 * returns: Error Code (0 = OK)
 */
EXP INT Cmp0WriteBlock (FILE *fp, PSZ pszBuff, INT iLen, PINT piLen);


/*
 * Compresses a string, returns the length
 * if uSrcLen is 0 then assume null term
 * if you specify src len, include null term in length if present
 * if uDestLen is 0 then no bounds check is done
 *
 * The length of the destination data is returned
 *
 * Internally, this creates a single block from the string
 * the format is the same as the blocks read/written to disk
 */
EXP INT Cmp0ImplodeStr (PVOID pDest, INT iDestLen, PVOID pSrc, INT iSrcLen);


/*
 * Uncompresses a string, returns the length
 *
 * The full data length is returned.  This could be larger than iDestLen
 * in which case data was discarded
 */
EXP INT Cmp0ExplodeStr (PVOID pDest, INT iDestLen, PVOID pSrc);


EXP INT Cmp0ImplodeFile (FILE *fpDest, FILE *fpSrc, BOOL bTerm, PULONG pilDestLen, PULONG pulSrcLen);


/*
 * This will only read files with block sizes <= uDEFBLOCKSIZE
 * Otherwise, use CmpExplodeFile!
 */
EXP INT Cmp0ExplodeFile (FILE *fpDest, FILE *fpSrc, PULONG pulDestLen, PULONG pulSrcLen);


EXP INT Cmp0SetBlockSize (INT iSize);



/*
 * This fn reads a block of compressed data from disk and uncompresses
 * it (or part of it) to a buffer.
 *
 * Params: *fp         - The file to read from
 *         pszBuff     - The buffer to fill
 *         iMaxLen     - The available space in the buffer
 *         iBuffOffset - Skip this many bites before filling buffer from block
 *         iType       - use uDEFTYPE   or pcfp->uZipType  
 *         iWinSize    - use uDEFWINDOW or pcfp->uZipWindow
 *         piBuffLen   - Returns the amount of data put in the buffer
 *         piUBlockLen - Returns the amount of data in the block (maybe > buffer size)
 *         piCBlockLen - Returns the size of the compressed block
 *
 * Returns: Error Code (0 = ok)
 */
EXP INT _cmpReadBlk (FILE  *fp, 
                  PSZ   pszBuff, 
                  INT  iMaxLen, 
                  INT  iBuffOffset,
                  INT  iType,
                  INT  iWindow,
                  PINT piBuffLen,
                  PINT piUBlockLen, 
                  PINT piCBlockLen);

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
EXP INT _cmpWrtBlk (FILE  *fp, 
                 PSZ   pszBuff, 
                 INT  iLen, 
                 INT  iType,
                 INT  iWindow,
                 PINT piCLen);


EXP BOOL _cmpWriteTerm (FILE *fp);


EXP INT _cmpDefaultType (void);


EXP INT _cmpDefaultWindow (void);


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/***********************************************************************/
/*                                                                     */
/* Medium Level API                                                    */
/*                                                                     */
/***********************************************************************/

#define MAXSAFEBLOCKSIZE 50000U

typedef void (*UPDATEFN) (INT iIncSize, INT iSoFar, INT iTotalSize);


/*
 * Cmp2SetUpdateFn is used to install update functions.  An update function
 * is a fn that is called repeatedly while zipping/unzipping to allow you
 * to display progress to the user.  iType specifies which procedure to 
 * tie into.  Set pfn to NULL to turn off calling the update fn.
 *
 *   iType:  0 - ReadBuff
 *           1 - WriteBuff
 *           2 - ReadFile
 *           3 - WriteFile
 *
 */
EXP void Cmp2SetUpdateFn (INT iType, UPDATEFN pfn);

/*
 * Initialization routine for compression module
 * This fn need only be called one time.
 * 
 * pszBuff
 *   Work buffer. Either provide a buffer of at least 35256U bytes
 *   or set it to NULL and let it malloc it for you
 *
 * iWinSize
 *   the sliding window size for implode. the choices are 1,2, or 3.
 *   with 1 being the fastest with least compression. default is 3
 *
 * iMode
 *   the modes are 1 and 2 with 1 being the best for binary data and
 *   2 being the best for ascii data. the default is 1.
 *
 */
EXP INT Cmp2Init (PSZ pszBuff, INT iWinSize, INT iMode);


/*
 * Buffer ---> IMPLODE ---> Buffer
 *
 * Implodes data from a buffer to a buffer
 * if uDataSize=0, source buffer is assumed to be null term.
 * if uCBuffSize=0, no bounds checking is performed for pszImploded
 * on return pUSize contains the size of the Imploded data
 * piSize may be null.
 */
EXP INT Cmp2szIsz (PSZ   pszImploded, // Dest Imploded data buff
                INT  iCBuffSize,  // Dest buff Size
                PINT piSize,      // Dest Imploded data size
                PSZ   pszExploded, // Src Exploded data buff
                INT  iDataSize);  // Src Exploded data size


/*
 * Buffer ---> EXPLODE ---> Buffer
 *
 * Explodes data from a buffer to a buffer
 * iBuffSize is the size of the Exploded buffer (0=no check)
 * on return piSize contains the size of the Exploded data
 * piSize may be null.
 */
EXP INT Cmp2szEsz (PSZ   pszExploded, // Dest Exploded data buff
                INT  iBuffSize,   // Dest buff size
                PINT piSize,      // Dest Exploded data size
                PSZ   pszImploded);// Src  Imploded data buffer



/*
 * Buffer ---> IMPLODE ---> File
 *
 * Implodes data from a buffer to a file
 * iSize is the size of the buffer to write.
 * if iSize=0, the buffer is considered to be null terminated
 * the buffer must still however be smaller than 64k
 * piSize may be null.
 */
EXP INT Cmp2szIfp  (FILE  *fp,       // Dest file for Imploded data
                 PINT piSize,    // Dest Imploded data size
                 PSZ   pszBuff,   // Src data buffer
                 INT  iDataSize);// Src data size


/*
 * File ---> EXPLODE ---> Buffer
 *
 * Explodes data from file to buffer.
 * if iBuffSize is 0, buffer overflow is not checked.
 * on return piSize contains the size of the data returned.
 * piSize may be null.
 */
EXP INT Cmp2fpEsz (PSZ   pszBuff,   // Dest buffer
                INT  iBuffSize, // Dest buffer size
                PINT piSize,    // Dest Exploded data size
                FILE  *fp);      // Src file with Imploded data


/*
 * File ---> IMPLODE ---> File
 *
 * Implodes data from a file to a file
 * piIn  is the size of the data read in  (may be null)
 * piOut is the size of the data written  (may be null)
 * iDataSize is the size of the data to read in, 0 means read entire file.
 * if iDataSize is 0 and the file is >64k, only the first 64k is read,
 *  and you will need to call this in a look until feof(fpIn)
 *  (and you will then need to read the file in a loop too)
 */
EXP INT Cmp2fpIfp (FILE  *fpOut,    // Dest file for imploded data
                PINT piOut,     // Dest size of imploded data
                FILE  *fpIn,     // Src file of data to implode
                INT  iDataSize, // Src size of data to implode
                PINT piIn);     // Src size of data imploded



/*
 * File ---> EXPLODE ---> File
 *
 * Explodes data from a file to a file
 * piIn  is the size of the data read in  (may be null)
 * piOut is the size of the data written  (may be null)
 */
EXP INT Cmp2fpEfp (FILE  *fpOut, // Dest file for exploded data
                PINT piOut,  // Dest size of exploded data
                FILE  *fpIn,  // Src file with imploded data
                PINT piIn);  // Src size of data expanded

EXP PSZ Cmp2GetCurrBuff (void);

EXP ULONG CRC_32 (ULONG ulCRC, CHAR c);

EXP ULONG CRC_BUFF (ULONG ulCRC, PSZ psz, INT iLen);

/*
 * Enables/Disables CRC tracking for Reading/Writing
 *
 * bRead = TRUE  enable/disable Read CRC
 * bRead = FALSE enable/disable Write CRC
 */
EXP BOOL Cmp2EnableCRC (BOOL bEnable, BOOL bRead);


EXP BOOL Cmp2CRCEnabled (BOOL bRead);


/*
 * Sets CRC value for Reading/Writing
 *
 * bRead = TRUE  set Read CRC
 * bRead = FALSE set Write CRC
 */
EXP ULONG Cmp2SetCRC  (ULONG ulVal, BOOL bRead);


/*
 * Gets CRC value for Reading/Writing
 *
 * bRead = TRUE  get Read CRC
 * bRead = FALSE get Write CRC
 */
EXP ULONG Cmp2GetCRC  (BOOL bRead);



typedef void (*PFNX) (PSZ pszBuff, INT iLen);
EXP void Cmp0SetXlateBuffer (PFNX pfnXlate);
EXP PFNX Cmp0GetXlateBuffer (void);




/***********************************************************************/
/*                                                                     */
/* Low Level Functions from PKWARE Data Compression Library            */
/*                                                                     */
/* PKWARE Data Compression Library                                     */
/* Copyright 1990,1991 by PKWARE Inc.  All Rights Reserved.            */
/*                                                                     */
/***********************************************************************/

#define CMP_BINARY             0
#define CMP_ASCII              1

#define CMP_NO_ERROR           0
#define CMP_INVALID_DICTSIZE   1
#define CMP_INVALID_MODE       2
#define CMP_BAD_DATA           3
#define CMP_ABORT              4


//#define _implode(rf,wf,bu,ty,sz) implode((rf),(wf),(bu),NULL,(ty),(sz))
//#define _explode(rf,wf,bu)       explode((rf),(wf),(bu),NULL)
//#define _cmp_read(fn,p1,p2)  INT fn (PSZ p1, PINT p2, PVOID pParam)
//#define _cmp_write(fn,p1,p2) void fn (PSZ p1, PINT p2, PVOID pParam)


typedef UINT (*PFReadBuf) (PSZ buf, PUINT size, PVOID param);
typedef void (*PFWriteBuf)(PSZ buf, PUINT size, PVOID param);

EXP UINT implode(PFReadBuf read_buf,
                 PFWriteBuf write_buf,
                 PSZ    work_buf,
                 PVOID  param,
                 PUINT  type,
                 PUINT  dsize);

EXP UINT explode(PFReadBuf read_buf,
                 PFWriteBuf write_buf,
                 PSZ    work_buf,
                 PVOID  param);

#ifdef __cplusplus
}
#endif

#endif  // GNUZIP_INCLUDED

