/*
 * Res0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides various resource reading functions
 * It is used in conjunction with my GNURES program
 */

#include <stdio.h>
#include <stdlib.h>
#include <share.h>
#include <string.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnures.h"
#include "res.h"


static PSZ ppszResErrors[] = 
			  {"No Error",                             // 0 
				"Unable to open Resource File",         // 1 
				"Unable to find resource",              // 2 
				"Unable to allocate memory",            // 3 
				"Cannot handle Compressed resource",    // 4
				"No Resource in file",                  // 5
				"Unable to create file",                // 6
				"Error Extracting Resource",            // 7
				"Unable to Open Data File",             // 8
				"Nothing to do"};                       // 9

#define RESERRORCOUNT 10

static CHAR szOPENFLAGS[5] = "rb";
static INT  iOPENFLAGS     = SH_DENYNO;
static INT  iRESERROR      = 0;

/*
 *
 * Undocumented, but still supported
 * This is used to find the resource file name if one is not
 * explicitly given
 */
//extern CHAR **__argv;

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * Internal function
 */
PVOID ResSetErr (INT i, FILE *fp)
	{
	if (!iRESERROR)
		iRESERROR = i;
	if (fp)
		fclose (fp);
	return NULL;
	}

/*
 * Internal function
 */
INT ResSetErrN (INT iErr, FILE *fp)
	{
	if (!iRESERROR)
		iRESERROR = iErr;
	if (fp)
		fclose (fp);
	return iErr;
	}


/*
 * This function returns the current error code
 * Error codes are reset at each function call
 *
 * [O] ppszErr ... Pointer to the error string.
 *                  This param may be NULL.
 *
 * RETURN VALUE .. 0 if there is no error, or the Error code
 */
INT ResGetErr (PSZ *ppszErr)
	{
	if (ppszErr)
		*ppszErr = ppszResErrors[iRESERROR];
	return iRESERROR;
	}




/*
 * This fn finds the Resource Descriptor Records in the Resource File
 * The file handle is returned pointing to the first record.
 * If an error occurs, call ResGetErr to get error string
 * 
 * [I] pszResFile ... Filename containing the resource.
 *                     Typically,  it will be the name of the Exe file.
 *                     A ".EXE" will be appended if a "." is not part 
 *                     of the filename.
 *                     This parameter may be NULL, in which case argv[0]
 *                     will be used to try to find the current exe file.
 * [O] puRecs ....... Pointer to the number of resources in the file.
 *                     This parameter may be NULL.
 *
 * RETURN VALUE ..... File handle pointing to the 1st record
 *                     NULL if there is an error.
 */
FILE * ResFindRecords (PSZ pszResFile, PINT piRecs)
	{
	FILE  *fpExe;
	CHAR  szName[256];
	ULONG ulMark = 0;
	LONG  lOffset, lRecPos;
	INT   iRecords = 0;

	ResSetErr (0, NULL);

// CLF
// needed to comment this out to compile 
// with certain versions of the runtime
//
//
//   /* --- try to find exe file if null ---
//    * This will only work if we are in OS/2 with OS/2 libs, or
//    * with DOS with DOS libs, or
//    * if we are in the current directory.
//    */
//   if (!pszResFile || !*pszResFile)
//      pszResFile = __argv[0];

	strcpy (szName, pszResFile);
	if (!strchr (szName, '.'))
		strcat (szName, ".EXE");

	if (!(fpExe = _fsopen (szName, szOPENFLAGS, iOPENFLAGS)))
		return (FILE*) ResSetErr (1, NULL);

	for (lOffset=-6; lOffset > -200 &&  ulMark != ADDEXEMARK; lOffset--)
		{
		fseek (fpExe, lOffset, SEEK_END);
		fread (&ulMark, sizeof (ULONG), 1, fpExe);
		}
	lOffset++;

	if (ulMark != ADDEXEMARK) /*--- No Resources Found ---*/
		{
		if (piRecs)
			*piRecs = 0;
		return (FILE*) ResSetErr (2, fpExe);
		}
	else /*--- Resources Found ---*/
		{
		iRecords = 0;
		fread (&iRecords, sizeof (USHORT), 1, fpExe);
		lRecPos = lOffset - iRecords * sizeof (RES);
		fseek (fpExe, lRecPos, SEEK_END);
		if (piRecs)
			*piRecs = iRecords;
		}
	return fpExe;
	}




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
FILE * ResGetPtr (PSZ pszResFile, PSZ pszRes, PRES pres)
	{
	FILE *fpExe;
	INT  i, iRecs, iIndex;
	PRES presTmp;

	if (!(fpExe = ResFindRecords (pszResFile, &iRecs)))
		return fpExe;
	if (!(presTmp = (pres ? pres : (PRES) malloc (sizeof (RES)))))
		return (FILE*) ResSetErr (3, fpExe);

	/*--- Resource index given ---*/
	if (((ULONG)(PVOID)pszRes < 1000L))
		{
		iIndex = (INT)(ULONG)(PVOID)pszRes;
		if (iIndex >= iRecs)
			return (FILE*) ResSetErr (2, fpExe);

		fseek (fpExe, sizeof (RES) * iIndex, SEEK_CUR);
		fread (presTmp, sizeof (RES), 1, fpExe);
		fseek (fpExe, presTmp->ulStart, SEEK_SET);
		if (!pres)
			free (presTmp);
		return fpExe;
		}
	
	/*--- Resource Name given ---*/
	for (i=0; i<iRecs; i++)
		{
		fread (presTmp, sizeof (RES), 1, fpExe);

		if (stricmp (pszRes, presTmp->szName))
			continue;
		fseek (fpExe, presTmp->ulStart, SEEK_SET);
		if (!pres)
			free (presTmp);
		return fpExe;
		}

	/*--- Resource not found ---*/
	memset (presTmp, 0, sizeof (RES));
	if (!pres)
		free (presTmp);
	return (FILE*) ResSetErr (2, fpExe);
	}





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
PSZ * ResGetList (PSZ pszResFile, PINT piResources)
	{
	FILE *fpExe;
	PSZ  *ppsz;
	RES  res;
	INT  i, iRecs;

	if (piResources)
		*piResources = 0;

	if (!(fpExe = ResFindRecords (pszResFile, &iRecs)))
		return NULL;
	
	if (piResources)
		*piResources = iRecs;

	if (!(ppsz = (PSZ*) malloc (sizeof (PSZ) * (iRecs+1))))
		return (PSZ*) ResSetErr (3, fpExe);

	for (i=0; i<iRecs; i++)
		{
		fread (&res, sizeof (RES), 1, fpExe);
		if (!(ppsz[i] = strdup (res.szName)))
			return (PSZ*) ResSetErr (3, fpExe);
		}
	fclose (fpExe);
	ppsz[iRecs] = NULL;
	return ppsz;
	}


/*
 *
 *
 */
void ResSetOpenMode (PSZ pszOpenFlags)
	{
	if (!pszOpenFlags || !*pszOpenFlags)
		strcpy (szOPENFLAGS, "rb");
	else if (strlen (pszOpenFlags) < 5)
		strcpy (szOPENFLAGS, pszOpenFlags);

	if (strchr (szOPENFLAGS, '+') || strchr (szOPENFLAGS, 'w') || strchr (szOPENFLAGS, 'W'))
		iOPENFLAGS = SH_DENYWR;
	else
		iOPENFLAGS = SH_DENYNO;
	}

