/*
 * Mem1.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * Memory Functions
 */

#if defined (MEM_ERRORS)
#undef MEM_ERRORS
#endif
#if defined (MEM_DEBUG)
#undef MEM_DEBUG
#endif

#include "stdafx.h"
#include "mem.h"

extern "C"
{


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


INT MemStrcmp (PSZ psz1, PSZ psz2)
	{
	if (!psz1 && !psz2)  return 0;
	if (!psz1)           return -1;
	if (!psz2)           return 2;
	return strcmp (psz1, psz2);
	}


INT MemStrncmp (PSZ psz1, PSZ psz2, INT iCount)
	{
	if (!psz1 && !psz2)  return 0;
	if (!psz1)           return -1;
	if (!psz2)           return 2;
	return strncmp (psz1, psz2, iCount);
	}


INT MemStricmp (PSZ psz1, PSZ psz2)
	{
	if (!psz1 && !psz2)  return 0;
	if (!psz1)           return -1;
	if (!psz2)           return 2;
	return stricmp (psz1, psz2);
	}



INT MemStrnicmp (PSZ psz1, PSZ psz2, INT iCount)
	{
	if (!psz1 && !psz2)  return 0;
	if (!psz1)           return -1;
	if (!psz2)           return 2;
	return strnicmp (psz1, psz2, iCount);
	}


INT MemStrlen (PSZ psz)
	{
	if (!psz)
		return 0;
	return strlen (psz);
	}


PSZ MemStrcpy (PSZ psz1, PSZ psz2)
	{
	if (!psz2)
		*psz1 = '\0';
	else
		strcpy (psz1, psz2);
	return psz1;
	}


PSZ MemStrncpy (PSZ psz1, PSZ psz2, INT iCount)
	{
	if (!psz2)
		*psz1 = '\0';
	else
		strncpy (psz1, psz2, iCount);
	return psz1;
	}



PPSZ  MemFreePPSZ (PSZ *ppsz, INT iNum)
	{
	PSZ *pp;
	int i;

	if (!ppsz)
		return NULL;

	if (iNum)
		{
//	BoundsChecker doesn't like this implementation...
//
//		while (iNum)
//			if (ppsz [--iNum])
//				free (ppsz [iNum]);
	
		for (i=0, pp = ppsz; i<iNum; i++, pp++)
			if (*pp)
				free (*pp);
		}
	else
		{
		for (pp = ppsz; *pp; pp++)
			if (*pp)
				free (*pp);
		}
	free (ppsz);
	return NULL;
	}


PVOID MemFreeData (PVOID p)
	{
	if (p)
		free (p);
	return NULL;
	}


PSZ strndup (PSZ psz, INT iLen)
	{
	PSZ pszCpy;

	if (!iLen)
		return NULL;

	pszCpy = (PSZ) malloc (iLen);
	return (PSZ) memcpy (pszCpy, psz, iLen);
	}
}