/*
 * Dir0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides base functions for
 * manipulating file directories and disk fn's
 */

#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <direct.h>
#include <windows.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnudir.h"


/*************************************************************************/
/*                                                                       */
/* Local support fn's                                                    */
/*                                                                       */
/*************************************************************************/

/*
 * Support fn for DirFindFile and DirFindAll
 */
static PFINFO dirNewFINFO (void)
	{
	PFINFO pfo;

	pfo         = (PFINFO) calloc (1, sizeof (FINFO));
	pfo->pvfbuf = calloc (1, sizeof (struct _finddata_t));
	pfo->next   = NULL;  // redundent but illustrative
	pfo->pUser  = NULL;
	return pfo;
	}


/*
 * Support fn for DirFindFile and DirFindAll
 */
static PFINFO dirSetFINFO (PFINFO pfo)
	{
	struct _finddata_t *pfbuf;

	pfbuf = (struct _finddata_t *) pfo->pvfbuf;
	pfo->pszName    = pfbuf->name;
	pfo->ulWriteDate= (ULONG) pfbuf->time_write;
	pfo->ulSize     = pfbuf->size;
	pfo->iAttr      = pfbuf->attrib;

	return pfo;
	}


/*
 * Support fn for DirFindFile and DirFindAll
 */
static PFINFO dirDupFINFO (PFINFO pfo)
	{
	PFINFO pfoNew;

	pfoNew = dirNewFINFO ();
	memcpy (pfoNew->pvfbuf, pfo->pvfbuf, sizeof (struct _finddata_t));
	return dirSetFINFO (pfoNew);
	}


/*
 * Support fn for DirFindFile and DirFindAll
 */
static PFINFO dirFreeFINFO (PFINFO pfo)
	{
	if (!pfo)
		return NULL;
	if (pfo->pvfbuf)
		free (pfo->pvfbuf);
	free (pfo);
	return NULL;
	}


static PFINFO dirFindClose (PFINFO pfo)
	{
	if (!pfo)
		return NULL;

	_findclose (pfo->lHandle);
	return dirFreeFINFO (pfo);
	}


/*
 * Support fn for DirFindFile and DirFindAll
 */
static PFINFO dirFindFirst (PSZ pszWildCard)
	{
	PFINFO pfo;

	pfo = dirNewFINFO ();
	pfo->lHandle = _findfirst (pszWildCard, (struct _finddata_t *) pfo->pvfbuf);

	if (pfo->lHandle == -1)
		return dirFindClose (pfo);

	return dirSetFINFO (pfo);
	}


/*
 * Support fn for DirFindFile and DirFindAll
 */
static PFINFO dirFindNext (PFINFO pfo)
	{
	if (_findnext (pfo->lHandle, (struct _finddata_t *) pfo->pvfbuf))
		return dirFindClose (pfo);

	return dirSetFINFO (pfo);
	}


/*************************************************************************/
/*                                                                       */
/* External fn's                                                         */
/*                                                                       */
/*************************************************************************/

/*
 * input?: pszWildCard - wildcard spec to match
 *         iAtts       - attributes to match 0 = match all
 *                       . .. and DIR attributes must be present to match the
 *                       . .. or directory entry.  The other attributes are
 *                       OR-ed meaning that any attribute match is a match
 *         pfo         - pass NULL 1st time, pass return other times
 *
 * return: NULL - no match, or handle/structure of match
 */
PFINFO DirFindFile (PSZ pszWildCard, INT iAtts, PFINFO pfo)
	{
	while (TRUE)
		{
		pfo = (pfo ? dirFindNext (pfo) : dirFindFirst (pszWildCard));

		if (!pfo || !iAtts)
			return pfo;

		/*--- if dir is '.' and no FILE_1DOTDIR flag, skip it ---*/
		if (pfo->pszName[0] == '.' && !pfo->pszName[1] && !(iAtts & FILE_1DOTDIR))
			continue;
			
		/*--- if dir is '..' and no FILE_2DOTDIR flag, skip it ---*/
		if (pfo->pszName[0] == '.' && pfo->pszName[1] == '.' && !(iAtts & FILE_2DOTDIR))
			continue;

		/*--- if asking for dir & curr file isn't a dir, skip it ---*/
		if ((iAtts & FILE_DIRECTORY) && !(pfo->iAttr & FILE_DIRECTORY))
			continue;

		if (iAtts & pfo->iAttr)
			return pfo;

		continue;
		}
	}


/*
 * used to terminate a DirFindFile loop before
 * all the matches are read
 */
PFINFO DirFindClose (PFINFO pfo)
	{
	return dirFindClose (pfo);
	}


/*
 * Returns a linked list of all matching files
 * use DirFindAllCleanup (PFINFO pfo) to close
 * 
 */
PFINFO DirFindAll (PSZ pszWildCard, INT iAtts)
	{
	PFINFO  pfoFirst, pfoPrev, pfoNew, pfoTmp;

	if (!(pfoTmp = DirFindFile (pszWildCard, iAtts, NULL)))
		return NULL;

	pfoFirst = dirDupFINFO (pfoTmp);
	
	for (pfoPrev = pfoFirst; pfoTmp = DirFindFile (NULL, 0, pfoTmp); pfoPrev = pfoNew)
		{
		pfoNew        = dirDupFINFO (pfoTmp);
		pfoPrev->next = pfoNew;
		}
	return pfoFirst;
	}


/*
 *
 *
 */
PFINFO DirFindAllCleanup (PFINFO pfo)
	{
	PFINFO pfoPrev;

	for (pfoPrev=NULL; pfo; pfo=pfo->next)
		{
		dirFreeFINFO (pfoPrev);
		pfoPrev = pfo;
		}
	return dirFreeFINFO (pfoPrev);
	}



/*
 *
 *
 */
INT DirGetCurrentDrive (void)
	{
	return _getdrive ();
	}



/*
 * uDrive - disk to get current path for
 *          use 0 to get curr path for curr drive
 *
 * if pszDir is null, a string is malloced
 * returns string to ptr or NULL for failure
 *
 */
PSZ DirGetCurrentDir (INT iDrive, PSZ pszDir)
	{
	PSZ  psz;
	INT i, iLen = 270;

	pszDir = (pszDir ? pszDir : (PSZ) malloc (iLen));
	*pszDir = '\0';

	psz = _getdcwd (iDrive, pszDir, iLen);

	/*--- strip drive spec to keep this fn consistent ---*/
	for (i=0; psz && psz[i+2]; i++)
		psz[i] = psz[i+3];
	return psz;
	}




#define DI_UNKNOWN     0
#define DI_NO_ROOT_DIR 1
#define DI_REMOVABLE   2
#define DI_FIXED       3
#define DI_REMOTE      4
#define DI_CDROM       5
#define DI_RAMDISK     6


/*
 * iDrive: 0=default 1=A: 2=B: etc...
 * piStat returns info
 */
BOOL DirGetDriveInfo (INT iDrive, PINT piStat)
	{
	char szDrive [8];
	INT  iStat;

	*szDrive = '\0';
	if (iDrive)
		sprintf (szDrive, "%c:\\", (CHAR)('A' + iDrive - 1));

	iStat = GetDriveType (szDrive);
	if (piStat)
		*piStat = iStat;

	return (iStat != DRIVE_UNKNOWN && iStat != DRIVE_NO_ROOT_DIR);
	}


/*
 *
 */
ULONG DirGetDriveMap (void)
	{
	ULONG  ulDrives;
	USHORT iDrive;

	ulDrives = 0x3L;
	for (iDrive=3; iDrive <27; iDrive++)
		if (DirGetDriveInfo (iDrive, NULL))
			ulDrives |= (1 << (iDrive-1));
	return ulDrives;
	}


