/*
 * Scr6.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * misc stuff
 *
 * INT ScrIncrementColor (uModColor, uStartColor)
 *
 */

#include <stdlib.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 *
 *
 */
static void FadeDAC (INT iPal, INT iStep, INT iStart, INT iEnd,
							INT iR1, INT iG1, INT iB1, 
							INT iR2, INT iG2, INT iB2)
	{
	if (iStep < iStart || iStep > iEnd)
		return;
	iStep -= iStart, iEnd -= iStart;
	ScrSetDACReg (ScrGetPALReg (iPal), iR1+(iR2-iR1)*iStep/iEnd, iG1+(iG2-iG1)*iStep/iEnd, iB1+(iB2-iB1)*iStep/iEnd);
	}



/*
 * This fn changes the color associated with the uModColor palette index
 * legal values for uModColor and uStartColor are 0 thru 15 (the pal colors)
 * This fn is intended to be called several times
 * Call with iPal>15 to initialize.
 * The 1st 60 calls fade from the start color to the 1st rainbow color
 * Then each cubsequent call slowly fades to different colors.
 */
INT ScrIncrementColor (INT iModColor, INT iStartColor)
	{
	static INT uR, uG, uB, iStep;

	if (iModColor > 15) // init
		{
		ScrGetDACReg (ScrGetPALReg (iStartColor), &uR, &uG, &uB);
		return (iStep = 0);
		}
//   iStep = (iStep == 180 ? 61 : iStep+1);
//
//   FadeDAC (iModColor, iStep, 0,   10,    uR, uG, uB,   uR, uG, uB);
//   FadeDAC (iModColor, iStep, 11,  60,    uR, uG, uB,   21, 63, 63);
//   FadeDAC (iModColor, iStep, 61,  90,    21, 63, 63,   21, 63, 21);
//   FadeDAC (iModColor, iStep, 91,  120,   21, 63, 21,   63, 63, 21);
//   FadeDAC (iModColor, iStep, 121, 150,   63, 63, 21,   63, 45, 63);
//   FadeDAC (iModColor, iStep, 151, 180,   63, 45, 63,   21, 63, 63);


//   iStep = (iStep > 200 ? 81 : iStep+1);
//
//   FadeDAC (iModColor, iStep, 0,   20,    uR, uG, uB,   uR, uG, uB);
//   FadeDAC (iModColor, iStep, 21,  80,    uR, uG, uB,   21, 63, 63);
//   FadeDAC (iModColor, iStep, 81,  110,   21, 63, 63,   21, 63, 21);
//   FadeDAC (iModColor, iStep, 111, 140,   21, 63, 21,   63, 63, 21);
//   FadeDAC (iModColor, iStep, 141, 170,   63, 63, 21,   63, 45, 63);
//   FadeDAC (iModColor, iStep, 171, 200,   63, 45, 63,   21, 63, 63);


//   iStep = (iStep > 200 ? 81 : iStep+1);
//
//   FadeDAC (iModColor, iStep, 0,   10,    uR, uG, uB,   uR, uG, uB);
//   FadeDAC (iModColor, iStep, 11,  80,    uR, uG, uB,   21, 63, 63);
//   FadeDAC (iModColor, iStep, 81,  110,   21, 63, 63,   21, 63, 21);
//   FadeDAC (iModColor, iStep, 111, 140,   21, 63, 21,   63, 63, 21);
//   FadeDAC (iModColor, iStep, 141, 170,   63, 63, 21,   63, 45, 63);
//   FadeDAC (iModColor, iStep, 171, 200,   63, 45, 63,   21, 63, 63);

	iStep = (iStep > 172 ? 35 : iStep+1);

	FadeDAC (iModColor, iStep, 0,   3 ,    uR, uG, uB,   uR, uG, uB);
	FadeDAC (iModColor, iStep, 4,   34,    uR, uG, uB,   31, 31, 63);
	FadeDAC (iModColor, iStep, 35,  42,    31, 31, 63,   31, 31, 63);
	FadeDAC (iModColor, iStep, 43,  103,   31, 31, 63,   63, 30, 24);
	FadeDAC (iModColor, iStep, 104,  111,  63, 30, 24,   63, 30, 24);
	FadeDAC (iModColor, iStep, 112,  172,  63, 30, 24,   31, 31, 63);

	return iStep;
	}


