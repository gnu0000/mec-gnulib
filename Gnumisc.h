/*
 * GnuMisc.h
 *
 * (C) 1992-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file provides misc functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * INT  Error (psz, ...)
 * INT  Msg (psz, ...)
 * INT  Warning (psz, ...)
 *
 * CHAR HexChar (i)
 * INT  HexVal (psz)
 * INT  HexCharVal (c)
 *
 * INT  Range (iMin, iVal, iMax)
 *
 * BOOL IsFlag (WORD wFlags, int iFlag)
 * BOOL IsFlag (DWORD dwFlags, int iFlag)
 * BOOL SetFlag (WORD* pwFlags, int iFlag, BOOL bSet)
 * BOOL SetFlag (DWORD* pdwFlags, int iFlag, int iSet)
 * void FlagList (WORD wFlags, BOOL bInvert, BYTE* pbFlagList)
 * void FlagList (DWORD dwFlags, BOOL bInvert, BYTE* pbFlagList)
 *
 * INT  GNUBeep (iGNUBeep)
 * INT  DoGNUBeep (iHz, iDuration)
 *
 * ULONG InitFineSleep (ulMils)
 * VOID  FineSleep     (ulMils)
 * VOID  DoSleep       (ulTime)
 *
 * INT   EnableErrors (i)
 * PSZ   GetLastError (piErr)
 *
 * INT   Rnd (INT iLimit);
 * ULONG RandULong (ULONG ulSize)
 *
 ****************************************************************************
 */

#if !defined (GNUMISC_INCLUDED)
#define GNUMISC_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif



/*
 * checks if a bit has been set
 */
EXP BOOL IsFlag (WORD wFlags, int iFlag);
//EXP BOOL IsFlag (DWORD dwFlags, int iFlag);

/*
 * sets or clears a flag bit.
 */
EXP BOOL SetFlag (WORD* pwFlags, int iFlag, BOOL bSet);
//EXP BOOL SetFlag (DWORD* pdwFlags, int iFlag, int iSet);

/*
 * makes an array of flag numbers from a flag bit field
 * pbFlagList must point to an existing array containing at least 16 bytes
 * the list is 0xFF terminated (unless full)
 * if bInvert is FALSE, there will be an array entry for each set bit
 * if bInvert is TRUE, there will be an array entry for each clear bit
 */
EXP void FlagList (WORD wFlags, BOOL bInvert, BYTE* pbFlagList);
//EXP void FlagList (DWORD dwFlags, BOOL bInvert, BYTE* pbFlagList);


/*
 * beeps
 */
EXP INT GNUBeep (INT iGNUBeep);


EXP INT DoGNUBeep (INT iHz, INT iDuration);


/*
 * converts a digit to its Hex representation
 *
 */
EXP CHAR HexChar (INT i);

/*
 * convert a hex digit to its decimal equivalent
 */
EXP INT HexCharVal (INT c);

/*
 * convert a 2 digit hex number to its decimal equivalent
 */
EXP INT HexValPsz (PSZ psz);


/*
 * min/max fn
 *
 */
EXP INT Range (INT iMin, INT iVal, INT iMax);


/*
 * Error   - Writes the string "Error: string"  and then exits with a 1.
 *
 * Msg     - Writes the string and then returns.
 *
 * Warning - Writes the string "Warning: string"  and then returns.
 *
 *
 *
 *
 */
EXP INT Error (PSZ psz, ...);

EXP INT Msg (PSZ psz, ...);

EXP INT Warning (PSZ psz, ...);

/*
 * Initializes the fine grained delay fn FineSleep
 * the param ulLen is the ammount of time to use to init the fn
 * in milliseconds.  The larger the value the more accurate the delay
 *
 * ulLen  variance
 * ------------------
 *    20  20.66%
 *    50   9.63%
 *   100   3.61%
 *   200   5.16%
 *   500   2.29%
 *  1000   1.42%
 *  1500   1.06%
 *  3000    .76%
 */
EXP ULONG InitFineSleep (ULONG ulMils);


EXP VOID DoSleep (ULONG ulTime);


/*
 * This is a better sleep fn
 * delay ulMils is in milliseconds.
 */
EXP VOID FineSleep (ULONG ulMils);


EXP INT EnableErrors (INT i);


EXP PSZ GetLastSysError (PINT piErr);


/*
 * returns a 'random' number from 0 to iLimit-1
 *
 */
EXP INT Rnd (INT iLimit);

EXP ULONG RandULong (ULONG ulSize);


#endif  // GNUMISC_INCLUDED


