/*
 * GnuDir.h
 *
 * (C) 1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file is part of the EBS module
 *
 * This file provides the file and directory handling functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 * 
 * INT    DirGetCurrentDrive (void)
 * PSZ    DirGetCurrentDir (iDrive, pszDir)
 * ULONG  DirGetDriveMap (void)
 * PFINFO DirFindFile (pszWildCard, iAtts, pfo)
 * PFINFO DirFindAll (pszWildCard, iAtts)
 * PFINFO DirFindAllCleanup (pfo)
 * BOOL   DirPathExists (pszPath)
 * BOOL   DirMakePath (pszPath)
 * BOOL   DirRemoveDir (pszDir, bKillChildren)
 * void   DirRemoveEmptyFiles (pszDir)
 * PSZ    DirMakeFileName (pszDest, pszBase, pszOrg, pszExt)
 * PSZ    DirMakeValidName (pszDest, pszSrc)
 * PSZ    DirStripPath (pszDest, pszSrc, bKeepExt)
 * PSZ    DirSplitPath (pszDest, pszPath, uFlags)
 * int    DirUnlink (pszFile)
 * BOOL	  DirGetLongFileName (strLongFileName, strFileName)
 * 
 ****************************************************************************
 */

#if !defined (GNUDIR_INCLUDED)
#define GNUDIR_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include "GnuType.h"
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 **                                                                        **
 *                        Structures and Types                              *
 *                         defines and macros                               *
 **                                                                        **
 ****************************************************************************/

typedef struct _finfo
   {
   PSZ    pszName;      // File Name
   ULONG  ulWriteDate;  // File Date/Time
   ULONG  ulSize;       // File Size
   INT    iAttr;        // File Attributes

   PVOID  pUser;        // User Data pointer
   LONG   lHandle;      // Enum handle          - internal
   PVOID  pvfbuf;       // (struct _finddata_t) - internal
   struct _finfo *next; // for linked list      - internal

   } FINFO;
typedef FINFO *PFINFO;


/****************************************************************************
 **                                                                        **
 *                           Dir Functions                                  *
 **                                                                        **
 ****************************************************************************/


/*
 * Returns the number of the current drive (A=1)
 *
 */
EXP INT DirGetCurrentDrive (void);


/*
 * uDrive - disk to get current path for
 *          use 0 to get curr path for curr drive
 * if pszDir is null, a string is malloced
 */
EXP PSZ DirGetCurrentDir (INT iDrive, PSZ pszDir);


/*
 * possible uStat returns
 * currently 16 bit dos only
 */
#define DI_UNKNOWN     0  //
#define DI_NO_ROOT_DIR 1  //
#define DI_REMOVABLE   2  // nt only
#define DI_FIXED       3  // nt only
#define DI_REMOTE      4  //
#define DI_CDROM       5  // nt only
#define DI_RAMDISK     6  // nt only
                          

/*
 * 0=default, 1=A:, ...
 * currently 16 bit dos only
 */
EXP BOOL DirGetDriveInfo (INT iDrive, PINT piStat);


/*
 * Returns a BitField representing existing drives.
 *
 */
EXP ULONG DirGetDriveMap (void);


/*
 * FindFirst / Find Next utility
 * pszWildCard is the search spec to search for
 * iAtts specify the attributes to look at for the files/dirs
 * pfo is the handle to use.  For the first call, preset pfo = NULL
 * and use the returned pfo in subsequent calls
 * 
 * Attributes defined in gnutype.h:
 *    FILE_NORMAL	 
 *    FILE_READONLY  
 *    FILE_HIDDEN	 
 *    FILE_SYSTEM	 
 *    FILE_LABEL	    
 *    FILE_DIRECTORY 
 *    FILE_ARCHIVED  
 * 
 *    FILE_1DOTDIR
 *    FILE_2DOTDIR
 */
EXP PFINFO DirFindFile (PSZ pszWildCard, INT iAtts, PFINFO pfo);

/*
 * use to terminate a DirFindFile loop before
 * all the matches are read
 */
EXP PFINFO DirFindClose (PFINFO pfo);


/*
 * FindFirst / Find Next utility
 * pszWildCard is the search spec to search for
 * the returned pfo is a linked list of all matching files/dirs
 * call DirFindAllCleanup to dispose of pfo.
 */
EXP PFINFO DirFindAll (PSZ pszWildCard, INT iAtts);



/*
 * used with DirFindAll. deallocates pfo
 */
EXP PFINFO DirFindAllCleanup (PFINFO pfo);


/***********************************************************************/

/*
 * Returns true if the path exists
 */
EXP BOOL DirPathExists (PSZ pszPath);


/*
 * makes a path, multi level paths OK
 */
EXP BOOL DirMakePath (PSZ pszPath);


///*
// * Deletes all files in the dir
// * and then removes the dir
// */
//BOOL DirRemoveDir (PSZ pszDir, BOOL bKillChildren);


/*
 * This file deletes all closed, empty files in pszDir
 */
EXP void DirRemoveEmptyFiles (PSZ pszDir);


/*
 * Makes a filename (pszDest)
 * 1> pszBase if it can, 
 *       attach pszExt if it exists and pszBase has no ext
 *
 * 2> use pszOrg (return NULL if it doesn't exist)
 *       replace/add pszExt if it exists
 */
EXP PSZ DirMakeFileName (PSZ pszDest, PSZ pszBase, PSZ pszOrg, PSZ pszExt);


/*
 * This fn takes a string, and works it over until it is
 * a valid file/dir name
 *
 */
EXP PSZ DirMakeValidName (PSZ pszDest, PSZ pszSrc);


/*
 * This fn strips the leading path information from a filespec.
 * string is malloced if pszDest is null.
 */
EXP PSZ DirStripPath (PSZ pszDest, PSZ pszSrc, BOOL bKeepExt);

/*
 * Sets the attributes for a file/dir
 *
 */
EXP BOOL DirSetFileAtt (PSZ pszFile, INT iAtts);




#define DIR_DRIVE  1
#define DIR_DIR    2
#define DIR_NAME   4
#define DIR_EXT    8

/*
 * This fn splits up a filename into component pieces
 * uFlags determine which piece(s) to return
 *
 * wFlags:
 *    DIR_DRIVE
 *    DIR_DIR
 *    DIR_NAME
 *    DIR_EXT
 *
 */
EXP PSZ DirSplitPath (PSZ pszDest, PSZ pszPath, WORD wFlags);


EXP INT DirChangeDrive (INT iDrive);

/*
 * Handles drive and path
 */
EXP int DirUnlink (PSZ pszFile);


/*
 * Retrieves the long file name of a file.
 * Returns FALSE if the file does not exist, TRUE otherwise.
 * strLongFileName must point to a buffer at least _MAX_PATH bytes long.
 *
 * strLongFileName: buffer receiving the long file name
 * strFileName: buffer containing the name of the file
 */
EXP BOOL DirGetLongFileName(LPTSTR strLongFileName, LPTSTR strFileName);

#ifdef __cplusplus
}
#endif

#endif  // GNUDIR_INCLUDED

