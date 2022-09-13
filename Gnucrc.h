/*
 * GnuCRC.h
 *
 * (C) 1992-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file provides basic CRC capabilities
 *
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * ULONG CRC32 (ulCRC, c)
 * ULONG CRCBuff (ulCRC, p, uCount)
 *
 ****************************************************************************
 */

#if !defined (GNUCRC_INCLUDED)
#define GNUCRC_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif


#ifdef __cplusplus
extern "C"
{
#endif

EXP ULONG CRC32 (ULONG ulCRC, CHAR c);

EXP ULONG CRCBuff (ULONG ulCRC, PVOID p, INT iCount);

#ifdef __cplusplus
}
#endif

#endif // GNUCRC_INCLUDED
