/*
 * GnuVar.h
 *
 * (C) 1992-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This mod implements a method to store name/value lists
 *
 ****************************************************************************
 *
 * QUICK REF FUNCTION INDEX
 * ========================
 *
 * PSZ  VarSet (ppvList, pszVar, pszVal)
 * PSZ  VarGet (pvList, pszVar, pszVal)
 * LONG VarGetl (pvList, pszVar)
 * BOOL VarTrue (pvList, pszVar)
 *
 * BOOL VarReadFile (ppvList, pszFile)
 * BOOL VarReadCfg (ppvList, pszFile, pszSection)
 * BOOL VarReadCfg2 (ppvList, pszFile, pszSection, bSkipBlank)
 * BOOL VarReadStr (ppvList, pszString, pszDelim)
 *
 * PSZ  VarSplit (PSZ pszVar, PSZ pszVal, PSZ pszElement)
 * PVAR FreeVar (pv)
 * PSZ  VarResolve (pszDest, pszSrc, pvVars)
 * void VarDump (fp, pvList)
 * PSZ VarSet2 (ppvList, pszVar, pszVal, bCleanStrings, bMallocVal)
 *
 ****************************************************************************
 */

#if !defined (GNUVAR_INCLUDED)
#define GNUVAR_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif


//extern "C"
//{

/*
 * var storage data structure
 * This structure is not useful externally
 * Use PVAR as a handle to the user functions
 */
typedef struct _vars
   {
   PSZ    pszVar;      // Variable name string
   PSZ    pszVal;      // Variable value string
   LONG   lVal;        // atol () of val string
   struct _vars *next; // ptr to next entry in list
   } VAR;
typedef VAR *PVAR;


/*
 * sets a var in the var list
 * creates a new var if needed
 * changes a value if already present
 * preset PVAR to null if it's a new handle
 */
EXP PSZ VarSet (PVAR *ppvList, PSZ pszVar, PSZ pszVal);


/*
 *	normally not used
 */
EXP PSZ VarSet2 (PVAR *ppvList, PSZ pszVar, PSZ pszVal, BOOL bCleanStrings, BOOL bMallocVal);

/*
 * gets a var from the var list
 * returns NULL if it isn't there
 */
EXP PSZ VarGet (PVAR pvList, PSZ pszVar, PSZ pszVal);


/*
 * This is an easy way to get a numeric val
 * but there is no way to tell if there was an error
 */
EXP LONG VarGetl (PVAR pvList, PSZ pszVar);


/*
 * Retrieves a bool var
 * if the var isnt there or is blank then it returns FALSE
 * if the var is 'no' or 'false' or '0' or 'null' then it returns FALSE
 * else its TRUE
 */
EXP BOOL VarTrue (PVAR pvList, PSZ pszVar);


/*
 * converts an cfg element (name=value) into
 * its component name and value strings
 *
 */
EXP PSZ VarSplit (PSZ pszVar, PSZ pszVal, PSZ pszElement);


/*
 * frees a var list
 * always returns NULL
 */
EXP PVAR FreeVar (PVAR pv);


/*
 * this reads a param file
 * the file is like a ini file without [] sections
 */
EXP BOOL VarReadFile (PVAR *ppvList, PSZ pszFile);


/*
 * reads a section from an ini file
 * and stores it in the var list
 * preset PVAR to null if it's a new handle
 *	Assumes name=value pairs on each line
 */
EXP BOOL VarReadCfg (PVAR *ppvList, PSZ pszFile, PSZ pszSection);

/*
 * reads a section from an ini file
 *	reads entire section block as value of var
 *	section name is name of var
 */
EXP BOOL VarReadCfg2 (PVAR *ppvList, PSZ pszFile, PSZ pszSection, BOOL bSkipBlank);

/*
 * reads a pszDelim delimited string
 * elements are name=value elements
 */
EXP BOOL VarReadStr (PVAR *ppvList, PSZ pszString, PSZ pszDelim);


/*
 * replaces @vars with thier values within the string
 */
EXP PSZ VarResolve (PSZ pszDest, PSZ pszSrc, PVAR pvVars);


/*
 * for debug: dumps vars to fp
 */
EXP void VarDump (FILE *fp, PVAR pvList);

// A non thread-safe enum interface
//
//
EXP BOOL VarEnumReset (PVAR pvList);
EXP BOOL VarEnum (PSZ *ppszVar, PSZ *ppszVal);

EXP PVAR VarCopyList (PVAR pvList);

//} // extern C

#endif  // GNUVAR_INCLUDED
