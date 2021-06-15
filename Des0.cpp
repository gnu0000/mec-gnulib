/*
 * Des.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * The top portion of this module (the des fn and supporting fn's) were
 * derived directly from 'NUMERICAL RECIPES in C' (c) 1988 Cambridge Press
 *
 * The bottom portion of this module contains the fn's which are exported
 * and are, hopefully, more user friendly
 *
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnudes.h"

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

typedef struct IMMENSE 
	{
	ULONG l, r;
	} immense;

typedef struct GREAT 
	{
	UINT l, c, r;
	} great;

typedef struct CHARMAP
	{
	UCHAR c[8];
	} charmap;

typedef union
	{
	immense m;
	charmap s;
	} COMBO;


static unsigned long bit[33];    /* defining declaration */


/*
 * Returns bit in position bitno of structure source. The rightmost nbits
 * of each component of source have been filled. 
 *
 */

// this function has been checked

static ULONG getbit(immense source, int bitno, int nbits)
	{ 
	if (bitno <= nbits)
		return (bit[bitno] & source.r ? 1L : 0L);
	else
		return (bit[bitno-nbits] & source.l ? 1L : 0L);
	}



/*
 * Key schedule calculation, returns kn given key and n=1,2,...,16;
 * must be called with n in that order 
 */

// this function has been checked

static void ks(immense key, int n, great *kn)
	{
	static immense icd;
	static CHAR ipc1[57]={0,57,49,41,33,25,17,9,1,58,50,                 //checked
								 42,34,26,18,10,2,59,51,43,35,27,19,11,3,60,
								 52,44,36,63,55,47,39,31,23,15,7,62,54,46,38,
								 30,22,14,6,61,53,45,37,29,21,13,5,28,20,12,4};
	static CHAR ipc2[49]={0,14,17,11,24,1,5,3,28,15,6,21,                //checked
								 10,23,19,12,4,26,8,16,7,27,20,13,2,41,52,31,
								 37,47,55,30,40,51,45,33,48,44,49,39,56,34,
								 53,46,42,50,36,29,32};
	int it,i,j,k,l;

	if (n == 1) // Initial selection and permutation.
		{        
		icd.r = icd.l = 0L;
		for(j=28,k=56; j>=1; j--,k--) 
			{
			icd.r = (icd.r <<= 1) | getbit(key,ipc1[j],32);
			icd.l = (icd.l <<= 1) | getbit(key,ipc1[k],32);
			}
		}

	/* Perform one or two shifts according to the value of n */
	if (n == 1 || n == 2 || n == 9 || n == 16) 
		it = 1;
	else 
		it = 2;   

	/*Circular left-shifts of the two halves of the array icd */
	for(i=1; i<=it; i++) 
		{ 
		icd.r = (icd.r | ((icd.r & 1L) << 28)) >> 1;
		icd.l = (icd.l | ((icd.l & 1L) << 28)) >> 1;
		}                /* Done with the shifts. */

	(*kn).r = (*kn).c = (*kn).l = 0;
	for(j=16,k=32,l=48; j>=1 ;j--,k--,l--)   /* The sub-master key is a */
		{                                   /* selection of bits from the shifted icd. */
		(*kn).r=((*kn).r <<= 1) | (unsigned short) getbit(icd,ipc2[j],28);                 
		(*kn).c=((*kn).c <<= 1) | (unsigned short) getbit(icd,ipc2[k],28);
		(*kn).l=((*kn).l <<= 1) | (unsigned short) getbit(icd,ipc2[l],28);
		}
	}

/*
 * Returns the cipher functiuon of ir and k in iout. 
 */
static void cyfun(ULONG ir, great k, PULONG iout)
	{
	static CHAR iet[49]={0,32,1,2,3,4,5,4,5,6,7,8,9,8,9,
		  10,11,12,13,12,13,14,15,16,17,16,17,18,19,
		  20,21,20,21,22,23,24,25,24,25,26,27,28,29,
		  28,29,30,31,32,1};
	static CHAR ipp[33]={0,16,7,20,21,29,12,28,17,1,15,
		  23,26,5,18,31,10,2,8,24,14,32,27,3,9,19,13,
		  30,6,22,11,4,25};
	static CHAR is[16][4][9]={ 
		  0,14,15,10,7,2,12,4,13,0,0,3,13,13,14,10,13,1,
		  0,4,0,13,10,4,9,1,7,0,15,13,1,3,11,4,6,2,
		  0,4,1,0,13,12,1,11,2,0,15,13,7,8,11,15,0,15,
		  0,1,14,6,6,2,14,4,11,0,12,8,10,15,8,3,11,1,
		  0,13,8,9,14,4,10,2,8,0,7,4,0,11,2,4,11,13,
		  0,14,7,4,9,1,15,11,4,0,8,10,13,0,12,2,13,14,
		  0,1,14,14,3,1,15,14,4,0,4,7,9,5,12,2,7,8,
		  0,8,11,9,0,11,5,13,1,0,2,1,0,6,7,12,8,7,
		  0,2,6,6,0,7,9,15,6,0,14,15,3,6,4,7,4,10,
		  0,13,10,8,12,10,2,12,9,0,4,3,6,10,1,9,1,4,
		  0,15,11,3,6,10,2,0,15,0,2,2,4,15,7,12,9,3,
		  0,6,4,15,11,13,8,3,12,0,9,15,9,1,14,5,4,10,
		  0,11,3,15,9,11,6,8,11,0,13,8,6,0,13,9,1,7,
		  0,2,13,3,7,7,45,7,14,0,1,4,8,13,2,15,10,8,
		  0,8,4,5,10,6,8,13,1,0,1,14,10,3,1,5,10,4,
		  0,11,1,0,13,8,3,14,2,0,7,2,7,8,13,10,7,13,
		  0,3,9,1,1,8,0,3,10,0,10,12,2,4,5,6,14,12,
		  0,15,5,11,15,15,7,10,0,0,5,11,4,9,6,11,9,15,
		  0,10,7,13,2,5,13,12,9,0,6,0,8,7,0,1,3,5,
		  0,12,8,1,1,9,0,15,6,0,11,6,15,4,15,14,5,12,
		  0,6,2,12,8,3,3,9,3,0,12,1,5,2,15,13,5,6,
		  0,9,12,2,3,12,4,6,10,0,3,8,14,5,0,1,0,9,
		  0,12,13,7,5,15,4,7,14,0,11,10,14,12,10,14,12,11,
		  0,7,6,12,14,5,10,8,13,0,14,12,3,11,9,7,15,0,
		  0,5,12,11,11,13,14,5,5,0,9,6,12,1,3,0,2,0,
		  0,3,9,5,5,6,1,0,15,0,10,0,11,12,10,6,14,3,
		  0,9,0,4,12,0,7,10,0,0,5,9,11,10,9,11,15,14,
		  0,10,3,10,2,3,13,5,3,0,0,5,0,7,4,0,2,5,
		  0,0,5,2,4,14,5,6,12,0,3,11,15,14,8,3,8,9,
		  0,5,2,14,8,0,11,9,5,0,6,14,2,2,5,8,3,6,
		  0,7,10,8,15,9,11,1,7,0,8,5,1,9,6,8,6,2,
		  0,0,15,7,4,14,6,2,8,0,13,9,12,14,3,13,12,11};

	//Next follows the table of bits in the integers 0 to 15
	//with the bit pattern reversed.
	static CHAR ibin[16]={0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};
	great ie;
	unsigned long itmp,ietmp1,ietmp2;
	CHAR iec[9];
	int jj,irow,icol,iss,j,l,m;

	ie.r = ie.c = ie.l = 0;  /* Expand ir to 48 bits and combine it with k. */
	for(j=16,l=32,m=48;j>=1;j--,l--,m--) 
		{
		ie.r = (ie.r <<=1) | (bit[iet[j]] & ir ? 1 : 0);
		ie.c = (ie.c <<=1) | (bit[iet[l]] & ir ? 1 : 0);
		ie.l = (ie.l <<=1) | (bit[iet[m]] & ir ? 1 : 0);
		}
	ie.r ^= k.r;
	ie.c ^= k.c;
	ie.l ^= k.l;
	ietmp1=((unsigned long) ie.c << 16)+(unsigned long) ie.r;
	ietmp2=((unsigned long) ie.l << 8)+((unsigned long) ie.c >> 8);
	for(j=1,m=5;j<=4;j++,m++) 
		{
		iec[j]=(CHAR)(ietmp1 & 0x3fL);
		iec[m]=(CHAR)(ietmp2 & 0x3fL);
		ietmp1 >>= 6;
		ietmp2 >>= 6;
		}
	itmp=0L;
	for(jj=8;jj>=1;jj--) /* Loop over 8 groups of 6 bits. */
		{
		j=iec[jj];
		irow=((j & 0x1) << 1)+((j & 0x20) >> 5); // Find place in S-Box table
		icol=((j & 0x2) << 2)+(j & 0x4) +        //
			  ((j & 0x8) >> 2)+((j &0x10) >> 4);  //
		iss = is[icol][irow][jj];                // Look up the number in the S-Box table and
		itmp = (itmp <<= 4) | ibin[iss];         // plug its bits into the output.
		}
	*iout=0L;                
	/* Final permutation. */    
	for(j=32;j>=1;j--)
		{
		*iout = (*iout <<= 1) | (bit[ipp[j]] & itmp ? 1 : 0);
		}
	}



				




/*
 * Data Encryption Standard. 
 * Encrypts  bits, stored as 2 long unsigned integers, 
 * in structure inp into out using key. Set newkey=1 when the
 * key is new.  Set isw=0 for encryption, =1 for decryption 
 */
void des(immense inp, immense key, int *newkey, int isw, immense *out)
	{
	static CHAR ip[65]=
		  {0,58,50,42,34,26,18,10,2,60,52,44,36,          // checked
		  28,20,12,4,62,54,46,38,30,22,14,6,64,56,48,40,  //
		  32,24,16,8,57,49,41,33,25,17,9,1,59,51,43,35,   //
		  27,19,11,3,61,53,45,37,29,21,13,5,63,55,47,39,  //
		  31,23,15,7};                                    //
	static CHAR ipm[65]=
		  {0,40,8,48,16,56,24,64,32,39,7,47,15,           // checked
		  55,23,63,31,38,6,46,14,54,22,62,30,37,5,45,13,  //
		  53,21,61,29,36,4,44,12,52,20,60,28,35,3,43,11,  //
		  51,19,59,27,34,2,42,10,50,18,58,26,33,1,41,9,   //
		  49,17,57,25};                                   //
	static great kns[17];
	static int initflag=1;
	int ii,i,j,k;
	unsigned long ic,shifter;
	immense itmp;

	if (initflag) // Store bit array. 
		{
		initflag = 0;
		bit[1] = shifter = 1L;
		for(j=2; j<=32; j++) 
			bit[j] = (shifter <<= 1);
		}

	if (*newkey) // Get the 16 sub-master keys from the master key
		{         
		*newkey=0;
		for(i=1; i<=16; i++) 
			ks (key, i, &kns[i]);
		}

	itmp.r = itmp.l = 0L;      // The initial permutation. 
	for(j=32,k=64;j>=1;j--,k--) 
		{
		itmp.r = (itmp.r <<= 1) | getbit (inp,ip[j],32);
		itmp.l = (itmp.l <<= 1) | getbit (inp,ip[k],32);
		}

	for(i=1; i<=16; i++) // The 16 stages of encryption
		{  
		ii = (isw == 1 ? 17-i : i);  //Sub-master keys used in reverse order for decryption
		cyfun (itmp.l,kns[ii],&ic);  // Get cipher function. 
		ic ^= itmp.r;    // Pass one half-word through unchanged, while encrypting  the
		itmp.r = itmp.l; // other half-word and exchanging the two half-words output. 
		itmp.l = ic;
		}               // Done with the 16 stages. 
	ic = itmp.r;         // A final exchange of the two half-words is required.
	itmp.r = itmp.l;
	itmp.l = ic;
	(*out).r = (*out).l = 0L;
	for (j=32,k=64; j>=1; j--,k--)     /* Final output permutation. */
		{
		(*out).r = ((*out).r <<= 1) | getbit(itmp,ipm[j],32);
		(*out).l = ((*out).l <<= 1) | getbit(itmp,ipm[k],32);
		}
	}

/***************************************************************************/
/*                                                                         */
/* The following fn's are derived from the above des fn.                   */
/*                                                                         */
/* Any problems with coding style and/or cleanliness in the code after     */
/* this point are entirely due to the fact that I reviewed the above code  */
/* and it warped my brain                                                  */
/*                                               -clf                      */
/*                                                                         */
/***************************************************************************/


static CHAR HexChar (INT iVal)
	{
	if (iVal < 10)
		return (CHAR)('0' + iVal);
	return (CHAR)('A' + iVal - 10);
	}



static INT HexVal (INT c)
	{
	c = (CHAR) toupper (c);
	if (c < 'A')
		return c - '0';
	return c - 'A' + 10;
	}



static void ClipStr (PSZ psz)
	{
	int i;

	for (i=strlen(psz); i; i--)
		{
		if (psz[i-1] != ' ')
			return;
		psz[i-1] = '\0';
		}
	}



static void MakeKey (COMBO *pmKey, PSZ pszKey)
	{
	INT  i;

	for (pmKey->m.l = pmKey->m.r = 0; *pszKey; )
		for (i=0; i<8; i++)
			pmKey->s.c[i] += (CHAR)(*pszKey ? *pszKey++ : ' ');
	}



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
PSZ DesEncrypt (PSZ pszDest, PSZ pszSrc, PSZ pszKey)
	{
	COMBO mIn, mOut, mKey;
	int   i, inewkey = 1;
	PSZ   pszD;

	if (!pszSrc || !pszKey || !*pszSrc || !*pszKey)
		return NULL;

	/*--- malloc if pszDest comes in NULL ---*/
	if (!(pszD = pszDest))
		{
		i = strlen (pszSrc);
		if (!(pszD = pszDest = (pszDest ? pszDest : (PSZ) malloc (2*(i+8-i%8+1)))))
			return NULL;
		}

	/*--- first, create the key ---*/
	MakeKey (&mKey, pszKey);

	/*--- encrypt, 8 bytes at a time ---*/
	while (*pszSrc)
		{
		for (i=0; i<8; i++)
			mIn.s.c[i] = (CHAR)(*pszSrc ? *pszSrc++ : '\0');

		des (mIn.m, mKey.m, &inewkey, 0, &mOut.m);

		/*--- convert binary data to hex string ---*/
		for (i=0; i<8; i++)
			{
			*pszD++ = HexChar (mOut.s.c[i] >>   4);
			*pszD++ = HexChar (mOut.s.c[i] & 0x0F);
			}
		}
	*pszD = '\0';
	return pszDest;
	}



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
PSZ DesDecrypt (PSZ pszDest, PSZ pszSrc, PSZ pszKey)
	{
	COMBO mIn, mOut, mKey;
	int   i, inewkey = 1;
	PSZ   pszD;

	if (!pszSrc || !pszKey || !*pszSrc || !*pszKey)
		return NULL;

	/*--- first, create the key ---*/
	MakeKey (&mKey, pszKey);

	ClipStr (pszSrc);

	/*--- malloc if pszDest comes in NULL ---*/
	if (!(pszD = pszDest))
		{
		i = strlen (pszSrc);
		if (!(pszD = pszDest = (pszDest ? pszDest : (PSZ) malloc ((i+8-i%8)/2+1))))
			return NULL;
		}

	while (*pszSrc)
		{
		/*--- convert binary data to hex string ---*/
		for (i=0; i<8; i++)
			{
			mIn.s.c[i]  = (UCHAR)(HexVal (*pszSrc ? *pszSrc++ : '0') << 4);
			mIn.s.c[i] |= (UCHAR)(HexVal (*pszSrc ? *pszSrc++ : '0'));
			}
		des (mIn.m, mKey.m, &inewkey, 1, &mOut.m);

		for (i=0; i<8; i++)
			*pszD++ = mOut.s.c[i];
		}
	*pszD = '\0';
	return pszDest;
	}



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
 * uDestLen = iSrcLen + 8 - iSrcLen % 8
 *
 */
PVOID DesBuff (PVOID pDest,
							PVOID pSrc,  INT iSrcLen,
							PVOID pKey,  INT iKeyLen,
							BOOL  bEncrypt)
	{
	COMBO mIn, mOut, mKey;
	INT   i, iIdx = 0;
	int   inewkey = 1;
	PSZ   pszSrc, pszKey, pszDest;

	if (!pSrc || !pKey || !iSrcLen || !iKeyLen)
		return NULL;

	pszSrc = (PSZ) pSrc; // I need to do this because otherwise the #$%#$%
	pszKey = (PSZ) pKey; // IBM CSET2 compuler gives me a lvalue err later

	if (!bEncrypt)
		iSrcLen += 8 - iSrcLen % 8;

	/*
	 * Here i conditionally malloc the destination buffer.
	 * When mallocing I round up to the next 8-byte boundry
	 * I also add one extra byte to the buffer.
	 * Why you ask?, because this api does not null terminate
	 * and a user may use this fn on strings, in which case he may
	 * want to null terminate his result. Note that buffers whose size
	 * does not lie on an 8-byte boundry will always be null terminated
	 * anyway, because 0 is the filler CHAR
	 *
	 */
	pDest = (pDest ? pDest : malloc (iSrcLen + (8 - iSrcLen % 8) + 1));
	pszDest = (PSZ) pDest;

	/*--- first, create the key ---*/
	for (mKey.m.l = mKey.m.r = 0, iIdx=0; iIdx<iKeyLen; )
		for (i=0; i<8; i++, iIdx++)
			mKey.s.c[i] += (CHAR)(iIdx<iKeyLen ? *pszKey++ : '\0');

	/*--- encrypt, 8 bytes at a time ---*/
	for (iIdx=0; iIdx<iSrcLen; )
		{
		for (i=0; i<8; i++, iIdx++)
			mIn.s.c[i] = (CHAR)(iIdx<iSrcLen ? *pszSrc++ : '\0');

		des (mIn.m, mKey.m, &inewkey, !bEncrypt, &mOut.m);

		for (i=0; i<8; i++)
			*pszDest++ = mOut.s.c[i];
		}
	return pDest;
	}



//   //printf ("\n mIn: ");
//   //for (j=0; j<8; j++) printf ("%3.3d ", mIn.s.c[j]);
//   //printf ("\nmOut: ");
//   //for (j=0; j<8; j++) printf ("%3.3d ", mOut.s.c[j]);
//   //printf ("\nmKey: ");
//   //for (j=0; j<8; j++) printf ("%3.3d ", mKey.s.c[j]);
//   //printf ("\n");
//
//int main (int argc, CHAR *argv[])
//   {
//   CHAR szDest1[512];
//   CHAR szDest2[512];
//   PSZ  p0, p1;
//
//   if (argc != 3)
//      return printf ("USAGE: des targetstring keystring\n");
//
//   printf ("-------- DesEncrypt / DesDecrypt API --------\n");
//   DesEncrypt (szDest1, argv[1], argv[2]);
//   DesDecrypt (szDest2, szDest1, argv[2]);
//   printf (" Input: %s\n", argv[1]);
//   printf ("   Key: %s\n", argv[2]);
//   printf ("Cypher: %s\n", szDest1);
//   printf ("Output: %s\n", szDest2);
//
//   printf ("---------------- DesBuff API ----------------\n");
//   p0 = DesBuff (NULL, argv[1], strlen (argv[1]), argv[2], strlen (argv[2]), TRUE);
//   p1 = DesBuff (NULL, p0,      strlen (argv[1]), argv[2], strlen (argv[2]), FALSE);
//   printf (" Input: %s\n", argv[1]);
//   printf ("   Key: %s\n", argv[2]);
//   printf ("Cypher: %s\n", p0);
//   printf ("Output: %s\n", p1);
//
//   return 0;
//   }

