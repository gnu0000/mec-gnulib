/*
 * Fil2.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides a read CSV field fn.
 */

#include <stdio.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnufile.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * This function reads in a csv field from the stream.
 * pszStr is the string read in.  If it's NULL, the data is discarded.
 * if iMaxLen is reached, the rest of the field is read in and discarded.
 *
 * bLastItem:  0 - comma is expected as the delimeter
 *             1 - EOL or EOF is expected as the delimeter
 *             2 - comma, EOL or EOF is expected as the delimeter
 *             3 - comma, EOL or EOF is expected, the delimeter is returned
 *
 * return values:
 *   0 - ok
 *   1 - EOF on stream before any data could be read
 *   2 - comma expected but got EOL or EOF
 *   3 - EOL/EOF expected but got comma
 * CHAR- The delimeter CHAR (if bLastItem==3)
 *
 */
INT FilReadCSVField (FILE *fp, PSZ pszStr, INT iMaxLen, BOOL bLastItem)
	{
	BOOL bQuoting = FALSE;
	INT  c, i = 0;

	while (TRUE)
		{
		c = f_getc (fp);
		if (c == EOF || c == '\n' || c == ',' && !bQuoting)
			break;
		if (++i >= iMaxLen)
			{
			FilReadTo (fp, NULL, (bLastItem==1 ? "\n" : ",\n"), 32767, TRUE);
			c = (bLastItem ? '\n' : ',');
			break;
			}

		if (c == '"')
			{
			if ('"' == (c = f_getc (fp)))
				{
				if (pszStr) *pszStr++ = (CHAR)c;
				continue;
				}
			f_ungetc (c, fp);
			bQuoting = !bQuoting;
			continue;
			}
		if (pszStr) *pszStr++ = (CHAR)c;
		}
	if (pszStr) 
		*pszStr = '\0';

	if (!i && !bLastItem && c == EOF)
		return 1;
	if (!bLastItem && (c == '\n' || c == EOF))
		return 2;
	if (bLastItem==1 && c == ',')
		return 3;
	if (bLastItem==3)
		return c;
	return 0;
	}

