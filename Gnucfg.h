/*
 * GnuCfg.h
 *
 * (C) 1993-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file provides ini and config file handling functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * INT  CfgGetError (ppszErrStr)
 * PSZ  CfgGetLine (pszFile, pszSect, pszLine, pszRet)
 * FILE *CfgFindSection (pszFile, pszSect)
 * FILE *CfgFindNextSection (fp, pszSect)
 * BOOL CfgGetNextEntry (fp, pszVar, pszVal)
 * INT  CfgGetSection (pszCfgFile, pszSection, ppszData, bSkipBlank)
 * INT  CfgGetNextSection (fp, ppszSection, ppszData, bSkipBlank)
 * BOOL CfgEndOfSection (pszLine)
 * INT  CfgSetLine (pszFile, pszSect, pszLine, pszVal)
 *
 ****************************************************************************
 */

#if !defined (GNUCFG_INCLUDED)
#define GNUCFG_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif


//extern "C"
//{

/* This fn returns data from a configuration file in the ini format
 * The pszSection should not have the [] characters.
 * If there is more than 1 line in the section, the sections are combined
 * with the \n separator.
 * It is assumed that any path information needed is already in pszCfgFile
 *
 * returns:
 *  0  - OK
 *  1  - unable to open input file
 *  2  - unable to find section
 */
EXP INT CfgGetSection (PSZ pszCfgFile, PSZ pszSection, PSZ *ppszData, BOOL bSkipBlank);



/*
 * current error value is returned
 * if pszErrStr!=NULL the error string is returned
 *
 * 0 no error
 * 1 file not found
 * 2 section not found
 * 3 line not found
 * 4
 * 5
 */
EXP INT CfgGetError (PSZ *ppszErrStr);


/*
 * returns TRUE if pszLine is no longer 
 * part of the current section
 */
EXP BOOL CfgEndOfSection (PSZ pszLine);


/*
 * Opens the config file and looks for the named section
 * if the section is found, the file ptr is returned
 * file ptr points to start of 1st line in section
 * If file or section not found, error is set and NULL
 * is returned. Error values and strings can be obtained
 * from the ArgGetErr function
 */
EXP FILE * CfgFindSection (PSZ pszFile, PSZ pszSect);


/*
 * Reads the input stream looking for the next section
 * if the section is found, the file ptr is returned
 * If a section is not found, error is set and NULL
 * is returned. Error values and strings can be obtained
 * from the ArgGetErr function
 */
EXP FILE * CfgFindNextSection (FILE *fp, PSZ pszSect);


/*
 * Reads the next entry from the file
 * Returns FALSE when end of section or end of file is encountered
 */
EXP BOOL CfgGetNextEntry (FILE *fp, PSZ pszVar, PSZ pszVal);


/*
 * pszFile - config file to read
 * pszSect - section to find
 * pszLine - line to find
 * pszRet  - return string, (or NULL to malloc)
 *
 * pszRet or malloced string is returned
 *
 * NULL is returned on error
 */
EXP PSZ CfgGetLine (PSZ pszFile, PSZ pszSect, PSZ pszLine, PSZ pszRet);


/*
 * This fn is different in that it does not malloc!
 *
 */
EXP INT CfgGetNextSection (FILE *fp, PSZ pszSection, PSZ pszData, INT iMaxLen);


/*
 * Creates/Modifies an entry in an ini file.
 * pszFile is the ini file containing the entry. If this file does not exist
 *  a non-zero error value is returned.
 * pszSect is the section containing the entry. If !exist, it is created.
 * pszLine is the entry to create/modify. If !exist, it is created.
 * pszVal is the new value for the entry.
 *
 */
EXP INT CfgSetLine (PSZ pszFile, PSZ pszSect, PSZ pszLine, PSZ pszVal);

//} // extern C

#endif // GNUCFG_INCLUDED

