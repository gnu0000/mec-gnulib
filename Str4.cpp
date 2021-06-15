/*
 * Str2.c
 *
 * Craig Fitzgerald  1999
 *
 * Part of the GnuLib library
 *
 *
 * This file provides a inline replacement fn
 *
 */

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnustr.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


/*
 * Build a string:
 * pszDest   = string dest buffer
 * iParams   = # of optional parameters passed to fn
 * pszformat = format string containing %0 %1 %2, etc..
 *
 * in the pszformat string, %0 is replaced with the ist optional param,
 * %1 with the next, etc....
 *
 *  Example:
 *  strBuild (szBuff, 4, "1=%1, 2=%2, 3=%3\n" , "AAA", "BBB", "CCC", "DDD");
 *    szBuff will be: "1=BBB, 2=CCC, 3=DDD\n"
 */
PSZ StrBuild (PSZ pszDest, int iParams, PSZ pszFormat, ...)
   {
   PSZ pszBuff, pszParam[10];
	va_list vlst;
   int i;

	va_start (vlst, pszFormat);
   for (i=0; i<=iParams && i < 10; i++)
		pszParam[i] = va_arg (vlst, PSZ);
	va_end (vlst);

   for (pszBuff = pszDest; *pszFormat; )
      {
      if (*pszFormat == '%' && (pszFormat[1] >= '0' && pszFormat[1] < '0' + iParams))
         {
         pszFormat++;
         strcpy (pszBuff, pszParam[*pszFormat-'0']);
         pszBuff += strlen (pszParam[*pszFormat-'0']);
         pszFormat++;
         }
      else
         *pszBuff++ = *pszFormat++;
      }
   *pszBuff = '\0';

   return pszDest;
   }
