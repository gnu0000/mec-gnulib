/*
 * Win7.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides the GnuFileWindow function for the
 * screen module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"
#include "gnukbd.h"
#include "gnustr.h"
#include "gnudir.h"
#include "gnumisc.h"
#include "gnuwin.h"
#include "gnutime.h"

extern "C"
{


static CHAR cCurrDrive  = 0;
static CHAR szCurrDir [128];
static CHAR sz [256];

static INT ERRORSTATE = 0;

static INT iLISTSIZE = 0;
static PSZ *apszList = NULL;


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

//
///*
// * uRes is a return value from a dos call.
// * This fn paints the error text on the window
// * It returns TRUE if there was an error
// */
//
//static BOOL ErrorCheck (PGW pgw, INT uRes)
//   {
//#if defined (__OS2__)
//   PSZ  psz;
//
//   if (ERRORSTATE)
//      GnuPaintNChar (NULL, TopOf(pgw) + 3, 0, 3, 0, ' ', 30);
//
//   ERRORSTATE = uRes;
//
//   if (!uRes)
//      return FALSE;
//
//   switch (uRes)
//      {
//      case ERROR_NO_MORE_FILES:     ERRORSTATE=0; return FALSE;  //normal
//      case ERROR_NOT_READY:         psz = "Drive Not Ready";      break;
//      case ERROR_DRIVE_LOCKED:      psz = "Drive Locked";         break;
//      case ERROR_CRC:               psz = "CRC Error on Drive";   break;
//      case ERROR_NOT_DOS_DISK:      psz = "Non Dos Disk";         break;
//      case ERROR_SECTOR_NOT_FOUND:  psz = "Sector Not Found";     break;
//      case ERROR_GEN_FAILURE:       psz = "General Disk Failure"; break;
//      case ERROR_SHARING_VIOLATION: psz = "Sharing Violation";    break;
//      case ERROR_WRONG_DISK:        psz = "Wrong Disk!";          break;
//      default:                      psz = "Unknown Disk Error"; 
//      }
//   GnuPaint (NULL, TopOf(pgw) + 3, 0, 3, 0, psz);
//   return TRUE;
//
//#else
//   return FALSE;
//#endif
//   }
//
//


static void GetInits ()
	{
	INT iCurrDisk;

	*szCurrDir = '\0';
	iCurrDisk = DirGetCurrentDrive ();
	cCurrDrive = (CHAR) ('A' + iCurrDisk - 1);
	DirGetCurrentDir (0, szCurrDir);
	if (*szCurrDir)
		strcat (szCurrDir, "\\");
	}


//int fnPthCmp (const PSZ *pp1, const PSZ *pp2)
int __cdecl fnPthCmp (const VOID *p1, const VOID *p2)
	{
	return stricmp (*(PPSZ)p1, *(PPSZ)p2);
	}


static void SortEm (INT iStart, INT iEnd)
	{
	qsort (apszList + iStart, iEnd-iStart, sizeof(PSZ), fnPthCmp);
	}


static void GetDrives ()
	{
	USHORT  i;
	INT     iStat;
	PSZ     psz;

	iLISTSIZE = 0;
	for (i=1; i<27; i++)
		if (DirGetDriveInfo (i, &iStat))
			{
			sprintf (sz, "[%c]", i - 1 + 'A');

			switch (iStat)
				{
				case DI_REMOVABLE: psz = " (Removable)"; break;
				case DI_FIXED    : psz = " (Fixed)";     break;
				case DI_REMOTE   : psz = " (Remote)";    break;
				case DI_CDROM    : psz = " (CDROM)";     break;
				case DI_RAMDISK  : psz = " (RamDisk)";   break;
				default          : psz = "";             
				}
			strcat (sz, psz);
			apszList[iLISTSIZE++] = strdup (sz);
			}
	SortEm (0, iLISTSIZE);
	}


static void GetPaths (PGW pgw)
	{
	PFINFO pfo = NULL;  

	if (ERRORSTATE)
		return;
	sprintf (sz, "%c:\\%s*.*", cCurrDrive, szCurrDir);
	while (TRUE)
		{
		EnableErrors (HARDERROR_DISABLE);
		pfo = DirFindFile (sz, 0, pfo);
		EnableErrors (HARDERROR_ENABLE);
		if (!pfo)
			break;

		if ((pfo->iAttr & FILE_DIRECTORY) &&
			 (pfo->pszName[0] != '.' || pfo->pszName[1] == '.'))
			{
			sprintf (sz, "%s\\", pfo->pszName);
			apszList[iLISTSIZE++] = strdup (sz);
			}
		}
	}


int __cdecl fnWin7FilCmp (const VOID *p1, const VOID *p2)
	{
	PSZ psz1, psz2;

	psz1 = *(PPSZ)p1, psz2 = *(PPSZ)p2;

	return stricmp (psz1, psz2);
	}

static void SortFiles (INT iStart)
	{
	qsort (apszList + iStart, iLISTSIZE-iStart, sizeof(PSZ), fnWin7FilCmp);
	}

static void GetFiles (PGW pgw, PSZ pszMatch)
	{
	PFINFO pfo = NULL;  
	INT    i, iStart;
	CHAR   szFileSpec[128], szDate [16], szTime [16];
	PSZ    p;

	if (ERRORSTATE)
		return;

	iStart = iLISTSIZE;

	while (pszMatch)
		{
		strcpy (szFileSpec, pszMatch);
		pszMatch = NULL;
		if (p = strchr (szFileSpec, ';'))
			{
			*p = '\0';
			pszMatch = ++p;
			}
		sprintf (sz, "%c:\\%s%s", cCurrDrive, szCurrDir, szFileSpec);

		while (TRUE)
			{
			EnableErrors (HARDERROR_DISABLE);
			pfo = DirFindFile (sz, FILE_NORMAL, pfo);
			EnableErrors (HARDERROR_ENABLE);

//         if (ErrorCheck (pgw, uRes))  --- res never set
//            return;

			if (!pfo)
				break;

			strcpy (sz, pfo->pszName);
			for (i = strlen (sz); i<12; i++)
				sz[i] = ' ';

			sprintf (sz+i, "%10ld  %s %s",
						pfo->ulSize,
						TimDateString (szDate, pfo->ulWriteDate, 0),
						TimTimeString (szTime, pfo->ulWriteDate, TRUE));

			apszList[iLISTSIZE++] = strdup (sz);
			}
		}
	SortFiles (iStart);
	}



static void ChangeDrive (PGW pgw, INT i)
	{
	INT iLen;

	cCurrDrive = apszList[i][1];
	iLen = sizeof (szCurrDir);

	EnableErrors (HARDERROR_DISABLE);
	DirGetCurrentDir (cCurrDrive - 'A' + 1, szCurrDir);
	EnableErrors (HARDERROR_ENABLE);

//   if (ErrorCheck (pgw, uRes))
//      *szCurrDir = '\0';
	if (*szCurrDir)
		strcat (szCurrDir, "\\");
	}



static void ChangePath (PGW pgw, INT i)
	{
	PSZ    p;

	if (!strcmp (apszList[i], "..\\"))
		{
		StrClip (szCurrDir, "\\");
		if (p = strrchr (szCurrDir, '\\'))
			p[1] = '\0';
		else
			*szCurrDir = '\0';
		}
	else
		strcat (szCurrDir, apszList[i]);
	}


static void ClearPathAndFile ()
	{
	for (; iLISTSIZE; iLISTSIZE--)
		{
		if (apszList[iLISTSIZE-1][strlen (apszList[iLISTSIZE-1]) - 1] == ']')
			return;
		if (apszList[iLISTSIZE-1])
			free (apszList[iLISTSIZE-1]);
		}
	}



static INT GetChoice (PGW pgw, PINT puType)
	{
	INT  c, iSel;

	while (TRUE)
		{
		if ((c = KeyGet (TRUE)) == K_ESC || c == K_RET)
			break;
		if (!GnuDoListKeys (pgw, c))
			GNUBeep (0);
		}
	iSel = pgw->iSelection;
	if (c == K_ESC)
		*puType = 0;
	else if (apszList[iSel][strlen (apszList[iSel]) - 1] == ']')
		*puType = 3;
	else if (apszList[iSel][strlen (apszList[iSel]) - 1] == '\\')
		*puType = 2;
	else
		*puType = 1;
	return iSel;
	}



static void PaintCurrentView (PGW pgw)
	{
	GnuPaintNChar (NULL, TopOf(pgw) + 2, 0, 3, 1, ' ', XSize(pgw)-2);
	sprintf (sz, "Viewing: %c:\\%s", cCurrDrive, szCurrDir);
	GnuPaint (NULL, TopOf(pgw) + 2, 0, 3, 1, sz);
	}




/*
 * the only difference between this list paint proc
 * and the default one is that this one paints drives and
 * subdirs in the bright color
 */
static INT LoadWinPaintProc (PGW pgw, INT iIndex, INT iLine)
	{
	PSZ  *ppszStr;
	WORD wAtt;
	INT  iLen;

	if (!pgw->pUser1 || iIndex >= pgw->iItemCount)
		return 0;

	ppszStr = (PSZ*) pgw->pUser1;

	if (pgw->iSelection == iIndex)
		wAtt = 2;
	else if (ppszStr[iIndex][strlen (ppszStr[iIndex]) - 1] == ']')
		wAtt = 1;
	else if (ppszStr[iIndex][strlen (ppszStr[iIndex]) - 1] == '\\')
		wAtt = 1;
	else
		wAtt = 0;

	iLen = GnuPaint2 (pgw, iLine, 0, 0, wAtt, 
							ppszStr[iIndex], CXSize(pgw));

	/*--- blank off rest of line ---*/
	if (iLen < CXSize(pgw))
		GnuPaintNChar (pgw, iLine, iLen, 0, wAtt, ' ', CXSize(pgw) - iLen);
	return 1;
	}







/*
 *  Params:
 *  -------
 *    pszFile     -- the returned file name possibly with path
 *    pszMatch    -- wildcard file spec to search for
 *    pszHeader   -- window display header
 *    pszWorkBuff -- Buffer large enough to hold list window data
 *
 * returns full path if needed
 * returns empty string and 0 if they chose none
 *
 */
INT GnuFileWindow (PSZ pszFile,
						 PSZ pszMatch,
						 PSZ pszHeader,
						 PSZ pszWorkBuff)
	{
	PGW   pgw;
	PMET  pmet;
	INT   iSel, iType;

	ScrPushCursor (FALSE);

	pmet = scrCheckGetMetrics ();
	apszList = (PSZ *) pszWorkBuff;
	if (cCurrDrive == 0 || cCurrDrive == 'A' || cCurrDrive == 'B')
		GetInits ();
	GetDrives ();

	pgw = GnuCreateWin (min (pmet->dwSize.Y-2, 21),
							  min (pmet->dwSize.X-2, 55),
							  LoadWinPaintProc);
	pgw->pszHeader      = pszHeader;
	pgw->pszFooter      = "[Select file or press <Esc> to exit window]";
	pgw->pUser1         = apszList;
	CTopOf(pgw)   += 3;
	CLeftOf(pgw)   += 1;
	CYSize(pgw)  -= 4;
	CXSize(pgw)  -= 2;
	pgw->bShowScrollPos = TRUE;
	GnuPaintBorder (pgw);

	iSel = 0;
	while (TRUE)
		{
		gnuShowScrollPos (pgw, pgw->iScrollPos, FALSE);
		GetPaths (pgw);
		GetFiles (pgw, pszMatch);
		pgw->iItemCount = iLISTSIZE;
		gnuFixScroll (pgw);
		GnuSelectLine (pgw, min(iSel, iLISTSIZE-1), TRUE);
		GnuPaintWin (pgw, (INT)-1);
		PaintCurrentView (pgw);
		GnuPaintBorder (pgw);

		iSel = GetChoice (pgw, &iType);
		if (iType == 0)                     /*--- abort ---*/
			break;
		if (iType == 1)                     /*--- file  ---*/
			break;
		if (iType == 2)                     /*--- path  ---*/
			ChangePath (pgw, iSel);
		if (iType == 3)                     /*--- drive ---*/
			ChangeDrive (pgw, iSel);
		ClearPathAndFile ();

		/*--- if drive selected, select line after drives next ---*/
		if (iType == 3)
			iSel = iLISTSIZE;
		}

	GnuDestroyWin (pgw);

	strcpy (sz, apszList[iSel]);
	sz[12] = '\0';                      /*--- clip size and date ---*/
	StrClip (sz, " ");

	if (iType)
		sprintf (pszFile, "%c:\\%s%s", cCurrDrive, szCurrDir, sz);
	else
		*pszFile = '\0';

	while (iLISTSIZE)
		if (apszList[--iLISTSIZE])
			free (apszList[iLISTSIZE]);

	ScrPopCursor ();
	return iType;
	}

}