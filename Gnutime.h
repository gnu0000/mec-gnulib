/*
 * GnuTime.h
 *
 * (C) 1998 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file provides time/date handling functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * PSZ    TimDateString (pszBuff, ulTime, iFormat)
 * PSZ    TimTimeString (pszBuff, ulTime, bAmPm)
 *
 * time_t TimMakeDateTime (pszDate, pszTime, bGMT)
 * int    TimParseDate (pszDate, piYear, piMonth, piDay)
 * int    TimParseTime (pszTime, piHour, piMin, piSec)
 *
 * INT    TimCompareDate (ulTime1, ulTime2, bCompareTime)
 *
 * INT    TimMonthToNum (pszMon)
 * INT    TimConvertYearTo1900Based (iYear)
 *
 ****************************************************************************
 */

#if !defined (GNUTIM_INCLUDED)
#define GNUTIM_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/*
 * converts a time_t value into a MM-DD-YY string
 * if pszBuff is NULL it it malloced
 *
 * iFormat:0 - MM-DD-YY
 *         1 - YY/MM/DD        see also strftime
 *         2 - DD/MM/YY        for lots of optinos
 *         3 - YYMMDD
 *         4 - MM/DD/YY
 */
EXP PSZ TimDateString (PSZ pszBuff, ULONG ulTime, INT iFormat);


/*
 * converts a time_t value into a HH-MM-SS string
 * if pszBuff is NULL it it malloced
 * if bAmPm is TRUE, time is in 12 hour format with AM or PM extension
 */
EXP PSZ TimTimeString (PSZ pszBuff, ULONG ulTime, BOOL bAmPm);


/*
 * Make a time_t value from date int's
 */
EXP ULONG TimMakeDate (INT iYear, INT iMonth, INT iDay);


/*
 * convert a 3 letter month abbreviation
 * into an int from 1 - 12
 */
EXP INT TimMonthToNum (PSZ pszMon);


/*
 * Converts a year into a format suitable
 * for the tm struct.  iYear can be a full
 * year or a 2 digit year.  Result is full
 * year - 1900
 */
EXP INT TimConvertYearTo1900Based (INT iYear);


/*
 * compares date and optionally time
 *
 */
EXP INT TimCompareDate (ULONG ulTime1, ULONG ulTime2, BOOL bCompareTime);


/*
 *	Converts a date and a time string into a time_t
 *	converts local time into GMT
 * 
 * bGMT : FALSE - string refers to local time
 *			 TRUE  - string is GMT
 *
 * return values
 *  2		 -	Invalid Date Format
 *  3		 -	Invalid year in date format
 *  4		 - Invalid time Format
 *  other -	a time_t representing the date/time
 */
ULONG TimMakeDateTime (PSZ pszDate, PSZ pszTime, BOOL bGMT);

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
int TimParseTime (PSZ pszTime, PINT piHour, PINT piMin, PINT piSec);


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
int TimParseDate (PSZ pszDate, PINT piYear, PINT piMonth, PINT piDay);


#ifdef __cplusplus
}
#endif

#endif  // GNUTIM_INCLUDED

