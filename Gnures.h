/*
 * GnuRes.h
 *
 * (C) 1994 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file provides resource loading functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * INT   ResGetErr (ppszErr)
 * FILE  *ResFindRecords (pszResFile, piRecs)
 * FILE  *ResGetPtr (pszResFile, pszRes, pres)
 * PVOID ResLoadData (pszResFile, pszRes, pBuff, iMaxLen, piReadLen)
 * PVOID ResLoadData2 (pszResFile, pszRes, pBuff, iMaxLen, piReadLen)
 * INT   ResExtract (fpDest, fpSrc, pres)
 * PSZ   *ResGetList (pszResFile, piResources)
 * BOOL  ResFindExe (pszFullPath, pszName, pszExt)
 *
 *
 * PRES  ResReadRecs (fpIn)
 * INT   ResAddBuffer (ppres, fpBase, pszName, pszBuff, iBuffLen, iComp, bBin, ulSpecial)
 * INT   ResAddFile (ppres, fpBase, pszInFile, iComp, bBin, ulSpecial)
 * INT   ResDupe (pres, fpBase, fpDest)
 * INT   ResStrip (pres, fpBase)
 * 
 ****************************************************************************
 */

#if !defined (GNURES_INCLUDED)
#define GNURES_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif


#ifdef __cplusplus
extern "C"
{
#endif

#define ADDEXEMARK 0x12488421LU


/*
 * Resource information structure.
 * An array of these structures is stored near the end of the exe.
 *
 * The actual resource file format is:
 *
 *    Format:
 *    --------
 *    OriginalFileData (ie the original exe file)
 *    AppendedFile#1
 *    .
 *    .
 *    .
 *    AppendedFile#n
 *    ADDREC#1          26
 *    .                 .
 *    .                 .
 *    .                 .
 *    ADDREC#n          26
 *    ADDEXEMARK        4
 *    #filesAppended    2
 *
 */

typedef struct
   {
   CHAR   szName [16];     // 16+
   ULONG  ulStart;         // 4+
   ULONG  ulOrgLen;        // 4+
   ULONG  ulResLen;        // 4+
   SHORT  iCompression;    // 2+
   SHORT  bBin;            // 2+
   ULONG  ulSpecial;       // 4 = 36
   } RES;
typedef RES *PRES;



/*
 * This function returns the current error code
 * Error codes are reset at each function call
 *
 * [O] ppszErr ... Pointer to the orror string.
 *                  This param may be NULL.
 *
 * RETURN VALUE .. 0 if there is no error, or the Error code
 */
EXP INT ResGetErr (PSZ *ppszErr);


/*
 * This fn finds the Resource Descriptor Records in the Resource File
 * The file handle is returned pointing to the first record.
 * If an error occurs, call ResGetErr to get error string
 * 
 * [I] pszResFile ... Filename containing the resource.
 *                     Typically,  it will be the name of the Exe file.
 *                     A ".EXE" will be appended if a "." is not part 
 *                     of the filename.
 * [O] piRecs ....... Pointer to the number of resources in the file.
 *                     This parameter may be NULL.
 *
 * RETURN VALUE ..... File handle pointing to the 1st record
 *                     NULL if there is an error.
 */
EXP FILE * ResFindRecords (PSZ pszResFile, PINT piRecs);


/*
 * This fn returns a file handle to the resource.
 * If an error occurs, call ResGetErr to get error string
 *
 * [I] pszResFile ... Filename containing the resource.
 *                     Typically,  it will be the name of the Exe file.
 *                     A ".EXE" will be appended if a "." is not part 
 *                     of the filename.
 * [I] pszRes ....... Name of the Resource to get a pointer to.
 *                     This param may also be a cast of an integer index.
 *                     This parameter may be NULL, which will match the
 *                     first resource in the file.
 * [O] pres ......... Structure ptr containing resource info.
 *                     This parameter may be NULL.
 *
 * RETURN VALUE ..... File handle pointing to the resource data
 *                      NULL if there is an error.
 *
 * Be sure to close the file ptr when done.
 */
EXP FILE * ResGetPtr (PSZ pszResFile, PSZ pszRes, PRES pres);


/*
 * This fn returns a pointer to the resource data.
 * If an error occurs, call ResGetErr to get error string
 *
 * [I] pszResFile ... Filename containing the resource.
 *                     Typically,  it will be the name of the Exe file.
 *                     A ".EXE" will be appended if a "." is not part 
 *                     of the filename.
 * [I] pszRes ....... Name of the Resource to load.
 *                     This param may also be a cast of an integer index.
 *                     This parameter may be NULL, which will match the
 *                     first resource in the file.
 * [I] pBuff  ....... Buffer where the resource is loaded to.
 *                     This param may be NULL, in which case the resource
 *                     data is malloced and returned.
 * [I] iMaxLen ...... Max size of the pBuff.
 *                     If pBuff is NULL, this is the max size to malloc
 *                     This param may be 0, meaning no bounds check.
 * [O] piReadLen .... Pointer to the size of the data read. 
 *                     This param may be NULL
 *
 * RETURN VALUE ..... Pointer to the resource data. This is either pBuff
 *                     or a newly malloced buffer. 
 */
EXP PVOID ResLoadData (PSZ pszResFile, PSZ pszRes, PVOID pBuff, INT iMaxLen, PINT piReadLen);

/*
 * This fn is just like ResLoadData except is is not linked to 
 * the compression library, hence it cannot handle compressed resources.
 */
EXP PVOID ResLoadData2 (PSZ pszResFile, PSZ pszRes, PVOID pBuff, INT iMaxLen, PINT piReadLen);


/*
 * Extracts a resource to a file
 * fpDest is the file to write to
 * fpSrc is the pointer to the resource (ResGetPtr will do it)
 * pres may be NULL iff resource is compressed
 *
 * returns error code: 0 - ok
 *                     3 - mem error
 *                     7 - error extracting
 *
 */
EXP INT ResExtract (FILE *fpDest, FILE *fpSrc, PRES pres);


/*
 * This fn returns a PPSZ list of resource names
 *
 * [I] pszResFile ... Filename containing the resource.
 *                     Typically,  it will be the name of the Exe file.
 *                     A ".EXE" will be appended if a "." is not part 
 *                     of the filename.
 * [O] piResources .. Pointer to the number of resources in the file.
 *                     This parameter may be NULL.
 *
 * RETURN VALUE ..... PPSZ pointer to the resource names.
 *
 * Be sure to free the return ppsz when done
 */
EXP PSZ * ResGetList (PSZ pszResFile, PINT piResources);



/*
 * This fn helps find a file.
 * Because argv[0] does not return path information under dos
 * you may need to go looking for the current exe file.
 *
 * [O] pszFullPath .. Full filespec of file if found. Empty string
 *                     if not found.
 * [I] pszName ...... Name of the file to look for. If NULL argv[0]
 *                     will be used for the filename.
 * [I] pszExt ......  The extention to use if one is not part of Name
 *                    Include the '.' in the ext.
 *
 * RETURN VALUE ..... TRUE if found, FALSE if not found.
 *
 * This fn does the following:
 * 1> If pszName contains path info, assume its correct and return it.
 * 2> Add extention if necessary and look in the current dir.
 * 3> Add extention if necessary and look at a:
 * 4> if still not found, return empty string.
 */
EXP BOOL ResFindExe (PSZ pszFullPath, PSZ pszName, PSZ pszExt);





/*********************************************************************/



/*
 * Reads resource list from a file
 * mallocs and returns the list. or NULL
 * if no resources
 */
EXP PRES ResReadRecs (FILE *fpIn);


/*
 * Adds a resource to the Base File fpBase
 * pres is updated
 * if bBin is FALSE, iBuffLen can be 0
 * Error code is returned
 * 
 */
EXP INT ResAddBuffer(PRES  *ppres,      // resource table (which is updated)
                     FILE  *fpBase,     // file containing resources (upd)
                     PSZ   pszName,     // new resource name
                     PSZ   pszBuff,     // new resource data
                     INT   iBuffLen,    // data size (0=null term)
                     INT   iComp,       // compress data
                     BOOL  bBin,        // mark as bin?  (just stored)
                     ULONG ulSpecial);  // special val?  (just stored)


/*
 * Adds a resource to the Base File fpBase
 * pres is updated
 * Like ResAddBuffer, except works with a file
 * Error code is returned
 *
 */
EXP INT ResAddFile (PRES  *ppres, 
                    FILE  *fpBase, 
                    PSZ   pszInFile, 
                    INT   iComp, 
                    BOOL  bBin, 
                    ULONG ulSpecial);


/*
 * Duplicates a resource from fpBase to fpDest
 * Error code is returned
 * pres must point to the full pres
 *
 */
EXP INT ResDupe (PRES pres, FILE *fpBase, FILE *fpDest);


///*
// * Extracts a resource to a file
// * pres is the resource RES to extract
// * pszDestFile is the file to create, it may be null, in which case
// * the resource name is used as the filename
// */
//INT ResExtract (PRES pres, FILE *fpBase, PSZ pszDestFile);


/*
 * Removes all resources from fpBase
 * pres is freed
 * Error code is returned
 *
 *
 */
EXP INT ResStrip (PRES pres, FILE *fpBase);


/*
 *
 *
 */
EXP void ResSetOpenMode (PSZ pszOpenFlags);


#ifdef __cplusplus
}
#endif

#endif  // GNURES_INCLUDED

