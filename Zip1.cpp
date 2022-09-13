/*
 * Zip1.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This file provides an error string function for the zip module
 *
 *
 ****************************************************************************
 *
 * PSZ CmpGetErr (PCFILE pcfp)
 *
 ****************************************************************************
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuzip.h"
#include "zip.h"

extern "C"
{


static char *ppszZIPERRORS[] = 
	{
	"No Error",                //                      0
	"Module Not Initialized",  //  ZIP_NOT_INITIALIZED 1
	"Unable to open file",     //  ZIP_CANT_OPEN       2
	"Unable to read file",     //  ZIP_CANT_READ       3
	"Unable to write file",    //  ZIP_CANT_WRITE      4
	"Insuffucient memory",     //  ZIP_MEMORY          5
	"Invalid Data #1",         //  ZIP_INVALID_DATA1   6
	"Invalid Data #2",         //  ZIP_INVALID_DATA2   7
	"Invalid Data #3",         //  ZIP_INVALID_DATA3   8
	"Invalid Data #4",         //  ZIP_INVALID_DATA4   9
	"Invalid Data #5",         //  ZIP_INVALID_DATA5   10
	"Buffer Too Small",        //  ZIP_BUFF_TO_SMALL   11
	"Abort",                   //  ZIP_ABORT           12
	NULL};

#define uMAXERROR ZIP_ABORT


/***********************************************************************/
/*                                                                     */
/*                                                                     */
/*                                                                     */
/***********************************************************************/


/*
 * Returns NULL if all is ok
 * Returns error string on error
 */
PSZ CmpGetErr (PCFILE pcfp)
	{
	INT iError;

	iError = CmpIsErr (pcfp);
	if (iError <= uMAXERROR)
		return ppszZIPERRORS [iError];
	return NULL;
	}


}