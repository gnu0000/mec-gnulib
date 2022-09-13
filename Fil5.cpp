/*
 * Fil5.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * For use by Windows Apps
 * This file provides wrappers for the various file io fns
 * so that all file manipulation can be done in the DLL
 */

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "gnutype.h"
#include "gnufile.h"

extern "C"
{


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

FILE *FilOpen (PSZ pszFileName, PSZ pszMode)
	{
	return fopen (pszFileName, pszMode);
	}

INT FilClose (FILE *fp)
	{
	return fclose (fp);
	}

INT FilEOF (FILE *fp)
	{
	return feof (fp);
	}

INT FilSeek (FILE *fp, long lPos, INT iSet)
	{
	return fseek (fp, lPos, iSet);
	}

INT FilRead (PSZ pszBuff, size_t uSize, size_t uNum, FILE *fp)
	{
	return fread (pszBuff, uSize, uNum, fp);
	}

INT FilWrite (PSZ pszBuff, size_t uSize, size_t uNum, FILE *fp)
	{
	return fwrite (pszBuff, uSize, uNum, fp);
	}

long FilTell (FILE *fp)
	{
	return ftell (fp);
	}

INT FilPuts (PSZ psz, FILE *fp)
	{
	return fputs (psz, fp);
	}
	
PSZ FilGets (PSZ psz, INT iSize, FILE *fp)
	{
	return fgets (psz, iSize, fp);
	}	
	
INT FilPutc (INT c, FILE *fp)
	{
	return fputc (c, fp);
	}
	
INT FilGetc (FILE *fp)
	{
	return fgetc (fp);
	}	

}