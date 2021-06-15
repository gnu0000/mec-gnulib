/*
 * GnuDes.h
 *
 * (C) 1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * Portions of this module (the des fn and supporting fn's) were
 * derived directly from 'NUMERICAL RECIPES in C' (c) 1988 Cambridge Press
 *
 * This file provides encryption/decryption functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * PSZ   DesEncrypt (pszDest, pszSrc, pszKey)
 * PSZ   DesDecrypt (pszDest, pszSrc, pszKey)
 * PVOID DesBuff    (pDest, pSrc, uSrcLen, pKey, uKeyLen, bEncrypt)
 *
 ****************************************************************************
 */

#if !defined (GNUDES_INCLUDED)
#define GNUDES_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif


//extern "C"
//{

/*
 * Des String Encryption
 *
 * Given an input string and a Key String, this fn will produce
 * an encrypted string.  The encrypted string is a null term string
 * of hex chars, and is twice as long as the input string plus up to
 * to as many as 15 chars.
 *
 * pszDest may be null, in which case the dest will be malloced and returned
 *
 */
EXP PSZ DesEncrypt (PSZ pszDest, PSZ pszSrc, PSZ pszKey);



/*
 * Des String Decryption
 *
 * Given an encrypted input string and a Key String, this fn will reproduce
 * the original string.  The encrypted string must be a null term string
 * produced from DesEncrypt, and must have the exact same key.
 *
 * pszDest may be null, in which case the dest will be malloced and returned
 *
 */
EXP PSZ DesDecrypt (PSZ pszDest, PSZ pszSrc, PSZ pszKey);



/*
 * Des Encryption/Decryption
 *
 * This fn will Encrypt or Decrypt a buffer, based on a key
 * buffer. 
 *
 * The destination buffer pDest will be almost the same length as 
 * the input buffer (rounded up the next 8 byte boundry)
 * if pDest is NULL the buffer will be malloc'ed and returned
 * NULL is returned on error (a bad param)
 *
 * Note that if you provide your own buffer it must be large enough to
 * hold the buffer (input size rounded up the next 8 byte boundry)
 *
 * If you call this fn with bEncrypt=FALSE (decrypt) and the SrcLen is not 
 * on an 8 byte boundry, the SrcLen will be rounded up for you.
 *
 * uDestLen = uSrcLen + 8 - uSrcLen % 8
 *
 */
EXP PVOID DesBuff (PVOID pDest,
                     PVOID pSrc,  INT iSrcLen,
                     PVOID pKey,  INT iKeyLen,
                     BOOL  bEncrypt);


//} // extern C

#endif  // GNUDES_INCLUDED

