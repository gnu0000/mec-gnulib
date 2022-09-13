/******************************************************************************
Module name: GetLongFileName.cpp
Written by:  Jeffrey Richter
Notices:     Written 1997 by Jeffrey Richter

Changes:
	GL (Nov 99): Transformed WinMain into a function.

******************************************************************************/


#include <afx.h>
#include <wchar.h>
#include <shellapi.h>
#include "gnudir.h"

extern "C"
{

// --- DirGetLongFileName() -----------------------------------------
// Retrieves the long file name of a file.
// Returns FALSE if the file does not exist, TRUE otherwise.
// strLongFileName must be at least _MAX_PATH bytes long.
//
BOOL DirGetLongFileName(LPTSTR strLongFileName, LPTSTR strFileName) 
{
	SHFILEINFO	sfi;
	if (!SHGetFileInfo(strFileName, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME))
	{
		return FALSE;
	}

	DirSplitPath(strLongFileName, strFileName, DIR_DRIVE | DIR_DIR);
	strcat(strLongFileName, sfi.szDisplayName);
	return TRUE;
}

}