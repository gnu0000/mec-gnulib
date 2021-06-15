/*
 * tim0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * Time Functions
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "gnutype.h"
#include "gnumem.h"
#include "gnumisc.h"
#include "gnustr.h"
#include "gnutime.h"

static CHAR *MON[] = {"XXX","Jan","Feb","Mar","Apr","May","Jun",
					"Jul","Aug","Sep","Oct","Nov","Dec", NULL};

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * a temporary hack!
 *
 */
struct tm *safelocaltime (ULONG *pulTime)
	{
	time_t ulTime;

	ulTime = *pulTime;

	if (ulTime > 2147403600) ulTime = 2147403600;  // 01/18/2038 is the last day before strftime crashes
																  // looks like a signed long you say?
	return localtime (&ulTime);
	}


/*
 * compares date and optionally time
 *
 */
INT TimCompareDate (ULONG ulTime1, ULONG ulTime2, BOOL bCompareTime)
	{
	struct tm *ptm;
	LONG   lVal1, lVal2;

	if (bCompareTime)
		return ulTime1 - ulTime2;
	if (!ulTime1 && !ulTime2)
		return 0;
	if (!ulTime1 || !ulTime2)
		return ulTime1 - ulTime2;

	/*--- only look at the date ---*/
	ptm = safelocaltime (&ulTime1);
	lVal1 = ptm->tm_mday + ptm->tm_mon * 31 + ptm->tm_year * 365;

	ptm = safelocaltime (&ulTime2);
	lVal2 = ptm->tm_mday + ptm->tm_mon * 31 + ptm->tm_year * 365;

	return lVal1 - lVal2;
	}


/***
PSZ TimGetTZName()
{
	TIME_ZONE_INFORMATION timeInfo;
	LONG timeOffset;

	DWORD dwStatus = GetTimeZoneInformation(&timeInfo);
	if (dwStatus == TIME_ZONE_ID_INVALID)
		return "";

	timeOffset = timeInfo.Bias / 30;	// number of half hours of difference

	switch (timeInfo.Bias)
	{
	case 10:	return "EST";
				break;
	case 12:	return "CST";
				break;
	case 14:	return "MST";
				break;
	case 16:	return "PST";
				break;
	default:	return "";
	}
}
***/


/*
 * converts a time_t value into a MM-DD-YY string
 * if pszBuff is NULL it it malloced
 */
PSZ TimDateString (PSZ pszBuff, ULONG ulTime, INT iFormat)
	{
	PSZ psz;
//	TCHAR szTimeDate[64];
	time_t ulTime_tmp = (time_t) ulTime;

	pszBuff = (pszBuff ? pszBuff : (PSZ) malloc (64));
	*pszBuff = '\0';

	switch (iFormat)
		{
		case 1:  psz = "%y/%m/%d";      break;     // Bams
		case 2:  psz = "%d/%m/%y";      break;     // euro
		case 3:  psz = "%y%m%d";        break;     // mainline
		case 4:  psz = "%m/%d/%y";      break;     // normal with slashes
		case 5:  psz = "%b %d, %Y";     break;     // Jan 31, 1998
		case 6:  psz = "%B %d, %Y";     break;     // January 31, 1998
		case 7:  psz = "%A, %B %d, %Y"; break;     // Monday, January 31, 1998
		case 10: psz = "%d%b";          break;     // 31Jan
/***
		case 11:  
			{
				// normal with slashes, + time and time zone ---
				sprintf(szTimeDate, "%s %s", "%m/%d/%y %I:%M%p", TimGetTZName());
				psz = szTimeDate;
				break;
			}
***/
		default: psz = "%m-%d-%y";      break;     // normal with dashes
		}
	if (!ulTime)
		strcpy (pszBuff, "        ");
	else
		strftime (pszBuff, 64, psz, gmtime(&ulTime_tmp));
	return pszBuff;
	}


/*
 * converts a time_t value into a HH-MM-SS string
 * if pszBuff is NULL it it malloced
 * if bAmPm is TRUE, time is in 12 hour format with AM or PM extension
 */
PSZ TimTimeString (PSZ pszBuff, ULONG ulTime, BOOL bAmPm)
	{
	pszBuff = (pszBuff ? pszBuff : (PSZ) malloc (16));
	*pszBuff = '\0';
	if (!ulTime)
		strcpy (pszBuff, "        ");
	else if (!bAmPm)
		strftime (pszBuff, 16, "%X", safelocaltime (&ulTime));
	else
		strftime (pszBuff, 16, "%I:%M:%S %p", safelocaltime (&ulTime));
	return pszBuff;
	}

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * Make a time_t value from date int's
 * 1 based month!
 */
ULONG TimMakeDate (INT iYear, INT iMonth, INT iDay)
	{
	struct tm tmDate;

	memset (&tmDate, 0, sizeof (struct tm));
	tmDate.tm_mday  = iDay;
	tmDate.tm_mon   = iMonth - 1;
	tmDate.tm_year  = iYear;
	return (ULONG) mktime (&tmDate);
	}

/*
 * convert a 3 letter month abbreviation
 * into an int from 1 - 12
 */
INT TimMonthToNum (PSZ pszMon)
	{
	INT i;

	for (i=1; i<13; i++)
		if (!strnicmp (pszMon, MON[i], 3))
			return i;
	return 0;
	}


/*
 * Converts a year into a format suitable
 * for the tm struct.  iYear can be a full
 * year or a 2 digit year.  Result is full
 * year - 1900
 */
INT TimConvertYearTo1900Based (INT iYear)
	{
	if (iYear > 200)          // full year spec
		return iYear -= 1900;
	if (iYear < 70)           // 2 digit spec
		return iYear += 100;
	return iYear;
	}

//
///*
// * Parses date strings of the forms:
// * null              (todays date)
// * MonDD             Jan21
// * DDMon[YY[YY]]     21Jan    21Jan96    21Jan1996
// * MMDDYY[YY]        012196   01211996
// * M[M]/D[D]/YY[YY]] 1/1/90   01/21/96   01/21/1996 01-21-96
// *
// * 0 - OK
// * 1 - Invalid Date Format
// * 2 - Invalid year in date format
// */
//ULONG TimParseDate (PSZ pszDate)
//	{
//	struct tm *ptm;
//	time_t tim;
//	PSZ    p, p2;
//	INT    iLen, iDay, iMonth, iYear;
//	CHAR   cTmp;
//
//	/*--- default fields to today ---*/
//	tim    = time (NULL);
//	ptm    = safelocaltime (&tim);
//	iDay	 = ptm->tm_mday ;
//	iMonth = ptm->tm_mon+1;
//	iYear  = ptm->tm_year;
//
//	/*--- Empty string - use todays date ---*/
//	if (!pszDate || !*pszDate) 
//		return tim;
//
//	iLen = strlen (pszDate);
//
//	/*--- Format: MonDD ---*/
//	if (isalpha (*pszDate))
//		{
//		if (!(iMonth = TimMonthToNum (pszDate)))
//			return 0;
//		for (p=pszDate+3; isalpha(*p); p++) // skip over month
//			;
//		if (!(iDay = atoi (p)))
//			return 0;
//		return TimMakeDate (iYear, iMonth, iDay);
//		}
//
//	/*--- Format DDMon[YY[YY]] ---*/
//	if (isdigit(pszDate[0]) && isdigit(pszDate[1]) && isalpha(pszDate[2]))
//		{
//		iDay = atoi (pszDate);
//		if (!(iMonth = TimMonthToNum (pszDate+2)))
//			return 0;
//		if (iLen <= 5)
//			return TimMakeDate (iYear, iMonth, iDay);
//		if (!(iYear = atoi (pszDate+5)))
//			return 0;
//
//		return TimMakeDate (TimConvertYearTo1900Based (iYear), iMonth, iDay);
//		}
//
//	/*--- Format: MMDDYY[YY] ---*/
//	if (iLen >= 6 &&  
//				isdigit(pszDate[0])   && isdigit(pszDate[1]) &&
//				isdigit(pszDate[2])   && isdigit(pszDate[3]) &&
//				isdigit(pszDate[4])   && isdigit(pszDate[5]))
//		{
//		cTmp = pszDate[2]; pszDate[2]='\0'; iMonth = atoi (pszDate + 0); pszDate[2] = cTmp;
//		cTmp = pszDate[4]; pszDate[4]='\0'; iDay   = atoi (pszDate + 2); pszDate[4] = cTmp;
//		iYear = atoi (pszDate + 4);
//		return TimMakeDate (TimConvertYearTo1900Based (iYear), iMonth, iDay);
//		}
//
//	/*--- Format: MM/DD[/YY[YY]], MM-DD-[YY[YY]] ---*/
//	if ((p = strchr (pszDate, '-')) || (p = strchr (pszDate, '/')))
//		{
//		cTmp = *p; *p = '\0'; iMonth = atoi (pszDate); *p = cTmp;
//		if ((p2 = strchr (p+1, '-')) || (p2 = strchr (p+1, '/')))
//			{
//			cTmp = *p2; *p2 = '\0'; iDay = atoi (p+1); *p2 = cTmp;
//			iYear = atoi (p2 + 1);
//			return TimMakeDate (TimConvertYearTo1900Based (iYear), iMonth, iDay);
//			}
//		iDay = atoi (p+1);
//		return TimMakeDate (iYear, iMonth, iDay);
//		}
//	return 0;
//	}


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/




/*
 *	converts a date string into year, month and day values
 *
 * Parses date strings of the forms:
 * null              (todays date)
 * MonDD             Jan21
 * DDMon[YY[YY]]     21Jan    21Jan96    21Jan1996
 * MMDDYY[YY]        012196   01211996
 * M[M]/D[D]/YY[YY]] 1/1/90   01/21/96   01/21/1996 01-21-96
 *
 *	returned month is 1-12 (not 0-11)
 *
 *	return value:
 * -------------
 * 0 - OK
 * 1 - empty string (returned value is today)
 * 2 - Invalid Date Format
 * 3 - Invalid year in date format
 */
int TimParseDate (PSZ pszDate, PINT piYear, PINT piMonth, PINT piDay)
	{
	tm *ptm = (tm *) malloc (sizeof(tm));
	time_t tim;
	PSZ    p, p2;
	INT    iLen;
	CHAR   cTmp;

	/*--- default fields to today ---*/
	tim      = time (NULL);
	ptm      = safelocaltime ((ULONG*) &tim);
	*piDay	= ptm->tm_mday ;
	*piMonth = ptm->tm_mon+1;
	*piYear  = ptm->tm_year;

	if (!pszDate || !*pszDate) 
		return 1;

	iLen = strlen (pszDate);

	/*--- Format: MonDD ---*/
	if (isalpha (*pszDate))
		{
		if (!(*piMonth = TimMonthToNum (pszDate)))
			return 2;
		for (p=pszDate+3; isalpha(*p); p++) // skip over month
			;
		if (!(*piDay = atoi (p)))
			return 2;
		return 0;
		}

	/*--- Format DDMon[YY[YY]] ---*/
	if (isdigit(pszDate[0]) && isdigit(pszDate[1]) && isalpha(pszDate[2]))
		{
		*piDay = atoi (pszDate);
		if (!(*piMonth = TimMonthToNum (pszDate+2)))
			return 2;
		if (iLen <= 5)
			return 0;
		if (!(*piYear = atoi (pszDate+5)))
			return 2;
		return 0;
		}

	/*--- Format: MMDDYY[YY] ---*/
	if (iLen >= 6 &&  
				isdigit(pszDate[0])   && isdigit(pszDate[1]) &&
				isdigit(pszDate[2])   && isdigit(pszDate[3]) &&
				isdigit(pszDate[4])   && isdigit(pszDate[5]))
		{
		cTmp = pszDate[2]; pszDate[2]='\0'; *piMonth = atoi (pszDate + 0); pszDate[2] = cTmp;
		cTmp = pszDate[4]; pszDate[4]='\0'; *piDay   = atoi (pszDate + 2); pszDate[4] = cTmp;
		*piYear = atoi (pszDate + 4);
		return 0;
		}

	/*--- Format: MM/DD[/YY[YY]], MM-DD-[YY[YY]] ---*/
	if ((p = strchr (pszDate, '-')) || (p = strchr (pszDate, '/')))
		{
		cTmp = *p; *p = '\0'; *piMonth = atoi (pszDate); *p = cTmp;
		if ((p2 = strchr (p+1, '-')) || (p2 = strchr (p+1, '/')))
			{
			cTmp = *p2; *p2 = '\0'; *piDay = atoi (p+1); *p2 = cTmp;
			*piYear = atoi (p2 + 1);
			return 0;
			}
		*piDay = atoi (p+1);
		return 0;
		}
	return 2;
	}


/*
 *	converts a time string into hour, minute and second values
 *
 * Parses time strings of the form:
 *	hh:mm[AM|PM]
 *
 *	return value:
 * -------------
 * 0 - OK
 * 1 - empty string (returned value is now)
 * 4 - Invalid time Format
 *
 */
int TimParseTime (PSZ pszTime, PINT piHour, PINT piMin, PINT piSec)
	{
	time_t tim;
	struct tm *ptm;
   PSZ    pTmp;

	/*--- default fields to today ---*/
	tim     = time (NULL);
	ptm     = safelocaltime ((ULONG*) &tim);
	*piHour = ptm->tm_hour;
	*piMin  = ptm->tm_min;
	*piSec  = ptm->tm_sec;

	if (!pszTime || !*pszTime)
		return 1;

	pTmp = strchr (pszTime, ':');
	if (strlen (pszTime) < 5 || !pTmp)
		return 4;

	*piHour = atoi (pszTime);
	*piMin  = atoi (pTmp+1);
	*piSec  = 0;

	if (*piHour == 12)
		*piHour = 0;

	// Look for AM/PM indicator ---
	pTmp += 3;
	while (isspace(*pTmp))
		pTmp++;

	if (toupper (*pTmp) == 'P')
		*piHour += 12;

	return 0;
	}

/*
 *	in minutes!
 *
 */
ULONG TimGetLocalTZOffset ()
	{
	DWORD dwRet;
	TIME_ZONE_INFORMATION TZInfo;
	
	dwRet = GetTimeZoneInformation (&TZInfo);

	return (dwRet != TIME_ZONE_ID_UNKNOWN ? TZInfo.Bias : 0);
	}

/*
 * TimParseDate uses mktime.  This fn assumes that the date string
 *	is referring to your local time.  It the string is actually GMT
 *	time, this fn can be used to undo the localization.
 */
ULONG TimUndoLocalization (ULONG ulTime)
	{
	return ulTime - TimGetLocalTZOffset () * 60;
	}


/*
 *	Converts a date and a time string into a time_t
 *	converts local time into GMT
 * 
 * bGMT : FALSE - string refers to local time
 *			 TRUE  - string is GMT
 *
 * return values
 *  0		 -	blank date
 *  2		 -	Invalid Date Format
 *  3		 -	Invalid year in date format
 *  4		 - Invalid time Format
 *  other -	a time_t representing the date/time
 */
ULONG TimMakeDateTime (PSZ pszDate, PSZ pszTime, BOOL bGMT)
	{
	int iRet;
	struct tm tmDate;
   int iYear, iMon, iDay, iHour, iMin, iSec;
	ULONG ulTime;

	iRet = TimParseDate (pszDate, &iYear, &iMon, &iDay);
	if (iRet == 1) // new: empty date
		return 0;
	if (iRet > 1)
		return iRet;
	iRet = TimParseTime (pszTime, &iHour, &iMin, &iSec);
	if (iRet > 1)
		return iRet;

	memset (&tmDate, 0, sizeof (struct tm));
	tmDate.tm_mday  = iDay;
	tmDate.tm_mon   = iMon - 1;
	tmDate.tm_year  = TimConvertYearTo1900Based (iYear);
	tmDate.tm_hour  = iHour;
	tmDate.tm_min   = iMin;
	tmDate.tm_sec   = iSec;

	ulTime = (ULONG) mktime (&tmDate);
	if (bGMT)
		ulTime = TimUndoLocalization (ulTime);
	return ulTime;
	}


