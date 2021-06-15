/*
 * Arg0.h
 *
 * Internal Header File for GnuArg
 *
 */


#define STRLEN  512

typedef struct instblk
   {
   INT    iIndex;
   PSZ    pszVal;
   } INST;
typedef INST *PINST;

typedef struct
   {
   PSZ    pszParam;
   BOOL   bMinimal;
   BOOL   bNoCase;
   CHAR   cValType;
   INT    iCount;
   PINST  pinst;
   } ARG;
typedef ARG  *PARG;

BOOL argGetInfo (PARG *ppargG, PINST *ppinstG);
INT argSetErr (INT uErr, PSZ pszErr);
INT argDigest (PSZ pszCL);


