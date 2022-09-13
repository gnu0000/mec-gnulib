/*
 * Str2.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides various file and string functions
 * that i wish were part of the standard library set.
 *
 */


#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnumisc.h"
#include "gnustr.h"

extern "C"
{


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * This fn extracts an identifier from the beginning of the string
 * This fn will skip any leading whitespace
 * This fn will then read the identifier
 * If !bAllowNum, the identifier must start with a letter or _
 * The delimiter is returned, if bEatDelim=TRUE then the delim
 * is skipped past.
 *
 */
INT StrGetIdent (PSZ *ppsz,
					  PSZ  pszWord,
					  BOOL bAllowNum,
					  BOOL bEatDelim)
	{
	if (pszWord)
		*pszWord = '\0';
	if (!ppsz || !*ppsz || !**ppsz)
		return -1;

	while (**ppsz == ' ' || **ppsz == '\t') //skip leading whitespace
		(*ppsz)++;

	if (!bAllowNum && !isalpha (**ppsz) && **ppsz != '_')
		return (**ppsz ? (bEatDelim ? *(*ppsz)++ : **ppsz) : 0);

	/*--- valid ident contains only chars, digits, and the _ char ---*/
	for (; isalnum (**ppsz) || **ppsz == '_'; (*ppsz)++)
		if (pszWord)
			*pszWord++ = **ppsz;

	if (pszWord)
		*pszWord = '\0';

	return (**ppsz ? (bEatDelim ? *(*ppsz)++ : **ppsz) : 0);
	}



//INT StrPPSZCut (PPSZ ppsz, PSZ pszLine)
//   {
//   INT uCols, i;
//   PSZ  psz;
//
//   psz = pszLine;
//   for (i=0; psz && i<256; )
//      {
//      ppsz[i++] = psz;
//      if (!(psz = strchr (psz, '\t')))
//         break;
//      *psz++ = '\0';
//      }
//   uCols = i;
//
//   for (; i<256; i++)
//      ppsz[i++] = NULL;
//
//   return uCols;
//   }
}