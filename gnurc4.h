/*
 * GNURC4.h
 *
 * (C) 1998 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * Stream encryption cypher
 * Note that the algorithm is symmetric - ie decryption ð decryption
 *
 * The encryption algorithm used in this module was derived from 
 * 'Applied Cryptography 2nd Ed' (c) 1996 by Bruce Schneider
 * and was developed by Ron Rivest for RSA Data Security Inc.
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * PSZ  RC4Crypt (pszOut, pszIn, iSrcLen, pszKey);
 * PSZ  RC4CryptStream (pszOut, pszIn, iSrcLen);
 * void RC4InitSBox (pszKey);
 *
 ****************************************************************************
 *
 * To encrypt a small buffer, simply call Crypt
 * To encrypt a stream call InitSBox and then call CryptStream as many
 * times as needed.
 */


#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Encrypt/Decrypt a block
 *
 * fn is:
 *    InitSBox (pszKey);
 *    return CryptStream (pszOut, pszIn, iSrcLen);
 */
EXP PSZ RC4Crypt (PSZ pszOut, PSZ pszIn, int iSrcLen, PSZ pszKey);


/*
 * Encrypt/Decrypt stream
 */
EXP PSZ RC4CryptStream (PSZ pszOut, PSZ pszIn, int iSrcLen);


/*
 * Initialize Cypher.
 * Call before each stream
 */
EXP void RC4InitSBox (PSZ pszKey);

#ifdef __cplusplus
}
#endif
