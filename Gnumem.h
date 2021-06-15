/*
 * GnuMem.h
 *
 * (C) 1993-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This module has 2 parts:
 *   The first part is general memory utility functions like MemFreeData
 * The second part is a memory diagnostic system.  This system is enabled
 * by defining MEM_ERRORS or MEM_DEBUG in your source file before including
 * this file. if MEM_ERRORS is defined errors are trapped.  If MEM_DEBUG is
 * enabled, memory tracking and diagnostics are enabled.  If you use 
 * MEM_DEBUG, also use MemSetDebugMode to define what to define the memory
 * tracking options.
 *
 *   If you want to track memory errors/usage in the GNULIB library functions
 * you must link with a version of the library that was built using the
 * MEM_ERRORS or MEM_DEBUG options.
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * ULONG MemAvailConv (void)
 *
 * PVOID MemFreeData (p)
 * void  MemFreePPSZ (ppsz, uNum)
 * INT   MemStrcmp (psz1, psz2)
 * INT   MemStrncmp (psz1, psz2, uCount)
 * INT   MemStricmp (psz1, psz2)
 * INT   MemStrnicmp (psz1, psz2, uCount)
 * INT   MemStrlen (psz)
 * PSZ   MemStrcpy (psz1, psz2)
 * PSZ   MemStrncpy (psz1, psz2, uCount)
 *
 * void  MemSetDebugMode (wMode);
 * void  MemSetDebugStream (*fp);
 *
 * PVOID malloc     (uSize)    - \
 * PVOID calloc     (uNum,uSize)  |
 * PVOID realloc    (p, uSize) -  | These fn's may get replaced if 
 * PSZ   strdup     (psz)      -  | MEM_ERRORS or MEM_DEBUG is defined
 * PSZ   strndup    (psz)      -  |
 * void  free       (p)        - /
 * void  memtest    ()         - \  
 * void  memtestptr (p)        -  | Call them if you want them. they do
 * void  memlist    ()         - /  nothing unless MEM_DEBUG is defined
 *
 ****************************************************************************
 *
 *  To enable memory Error checking:
 *     > #define MEM_ERRORS before including this module
 *     > Call MemSetDebugMode at the beginning of your prog if you want
 *       to enable MEM_NULLOK (this the only option supported w/MEM_ERRORS
 *     > call MemSetDebugStream () send log somewhere. (maybe stdout)
 *
 *  To enable memory Debugging / tracking:
 *     > #define MEM_DEBUG before including this module
 *     > Call MemSetDebugMode at the beginning of your prog enabling the
 *       desired debugging features.
 *     > call MemSetDebugStream () send log somewhere. (maybe stdout)
 *     > Be sure to exit the prog with exit()
 *     > use memtest() memtestptr() and memlist() if and when desired()
 *
 *  memtest() memtestptr() and memlist() do nothing if MEM_DEBUG is not
 *  defined
 *
 *    When using MEM_DEBUG, the function MemSetDebugMode(INT) controlls
 *    how this module works, MemSetDebugMode parameter values are:
 *
 *    0            - do default tracking / testing
 *    MEM_TEST     - do full mem test at every chance
 *    MEM_LOG      - log mem calls to FOUT
 *    MEM_CLEAR    - clear mem before free's
 *    MEM_EXTRA    - alloc extra 256 bytes for each malloc
 *    MEM_EXITLIST - print list of unfreed data on exit
 *    MEM_NULLOK   - don't abort on freeing NULL, just log it
 *
 *
 * Using MEM_DEBUG cand give you the following:
 *  > Detects pointer overruns and underruns.
 *  > Verifys calls to free.
 *  > Detects dependence on buffer data returned with malloc.
 *  > Detects incorrect use of realloc.
 *  > Handles Out Of memory conditions.
 *  > Logs all allocations and deallocations (and list them if MEM_LOG)
 *  > Memory leak detection (if using MEM_EXITLIST and/or mem_memlist)
 *  > Detects using freed ptr (especially if using MEM_CLEAR)
 *  > provide list of alloced data (if using MEM_EXITLIST or mem_memlist)
 *
 ****************************************************************************
 */

#if !defined (GNUMEM_INCLUDED)
#define GNUMEM_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif
 

//extern "C" {

/*
 * Flags for use with MemSetDebugMode (wMode)
 *
 */
#define MEM_TEST     0x1  // mem test every chance (use with MEM_DEBUG)
#define MEM_LOG      0x2  // log calls             (use with MEM_DEBUG)
#define MEM_CLEAR    0x4  // clear free's          (use with MEM_DEBUG)
#define MEM_EXTRA    0x8  // alloc extra bytes     (use with MEM_DEBUG)
#define MEM_EXITLIST 0x10 // print list on exit    (use with MEM_DEBUG)
#define MEM_NULLOK   0x20 // allow free(NULL)      (use with MEM_DEBUG)
#define MEM_ALL      0x37 // all of the above

/*
 * returns the total free conventional memory available
 *
 */
EXP ULONG MemAvailConv (void);


 /*
  * frees a PPSZ
  * if uNum=0, elements are freed until a NULL terminator
  * returns NULL always
  */
EXP PPSZ MemFreePPSZ (PSZ *ppsz, INT iNum);

 /*
  * ok if p is null, returns NULL always
  */
EXP PVOID MemFreeData (PVOID p);


/*
 * see above for param values
 */
EXP void MemSetDebugMode (WORD wMode);

/*
 * You can redirect the debug output with a call to this fn
 * the parameter to pass is a FILE stream ptr
 */
EXP void MemSetDebugStream (PVOID fp);


/*
 * this is a strdup with a fixed length
 * this mallocs exactly uLen bytes and does not null terminate
 *
 */
EXP PSZ strndup (PSZ psz, INT iLen);



/*
 * equivalent to std c counterparts
 * but can handle NULL ptr's
 */
EXP INT  MemStrcmp (PSZ psz1, PSZ psz2);
EXP INT  MemStrncmp (PSZ psz1, PSZ psz2, INT iCount);
EXP INT  MemStricmp (PSZ psz1, PSZ psz2);
EXP INT  MemStrnicmp (PSZ psz1, PSZ psz2, INT iCount);
EXP INT  MemStrlen (PSZ psz);
EXP PSZ  MemStrcpy (PSZ psz1, PSZ psz2);
EXP PSZ  MemStrncpy (PSZ psz1, PSZ psz2, INT iCount);



/***********************************************************************/
/*                                                                     */
/* End of human-useful information                                     */
/*                                                                     */
/***********************************************************************/

#if defined (MEM_DEBUG)

EXP PVOID Mem2_malloc  (INT iSize, PSZ pszFile, INT iLine);
EXP PVOID Mem2_calloc  (INT iNum, INT iSize, PSZ pszFile, INT iLine);
EXP PVOID Mem2_realloc (PVOID p, INT iSize, PSZ pszFile, INT iLine);
EXP PSZ   Mem2_strdup  (PSZ psz, PSZ pszFile, INT iLine);
EXP PSZ   Mem2_strndup (PSZ psz, INT iLen, PSZ pszFile, INT iLine);
EXP void  Mem2_free    (PVOID p, PSZ pszFile, INT iLine);
EXP void  Mem2_exit    (int i, PSZ pszFile, INT iLine);
EXP void  Mem2_test    (PSZ pszFile, INT iLine);
EXP void  Mem2_testptr (PVOID p, PSZ pszFile, INT iLine);
EXP void  Mem2_list    (PSZ pszFile, INT iLine);
EXP PPSZ  Mem2_freeppsz(PSZ *ppsz, INT iNum, PSZ pszFile, INT iLine);
EXP PVOID Mem2_freedata(PVOID p, PSZ pszFile, INT iLine);

#define malloc(u)        Mem2_malloc (u,__FILE__,__LINE__)
#define calloc(u,v)      Mem2_calloc (u,v,__FILE__,__LINE__)
#define realloc(p,u)     Mem2_realloc (p,u,__FILE__,__LINE__)
#define strdup(p)        Mem2_strdup (p,__FILE__,__LINE__)
#define strndup(p,u)     Mem2_strndup (p,u,__FILE__,__LINE__)
#define free(p)          Mem2_free (p,__FILE__,__LINE__)
#define exit(i)          Mem2_exit (i,__FILE__,__LINE__)
#define memtest()        Mem2_test (__FILE__,__LINE__)
#define memtestptr(p)    Mem2_testptr (p,__FILE__,__LINE__)
#define memlist()        Mem2_list (__FILE__,__LINE__)
#define MemFreePPSZ(p,u) Mem2_freeppsz(p,u,__FILE__,__LINE__)
#define MemFreeData(p)   Mem2_freedata(p,__FILE__,__LINE__)

#else

EXP void  Mem1_test    (PSZ pszFile, INT iLine);
EXP void  Mem1_testptr (PVOID p, PSZ pszFile, INT iLine);
EXP void  Mem1_list    (PSZ pszFile, INT iLine);
#define memtest()     Mem1_test (__FILE__,__LINE__)
#define memtestptr(p) Mem1_testptr (p,__FILE__,__LINE__)
#define memlist()     Mem1_list (__FILE__,__LINE__)

#endif // (MEM_DEBUG)


#if defined (MEM_ERRORS) & !defined (MEM_DEBUG)

EXP PVOID Mem1_malloc  (INT iSize, PSZ pszFile, INT iLine);
EXP PVOID Mem1_calloc  (INT iNum, INT iSize, PSZ pszFile, INT iLine);
EXP PVOID Mem1_realloc (PVOID p, INT iSize, PSZ pszFile, INT iLine);
EXP PSZ   Mem1_strdup  (PSZ psz, PSZ pszFile, INT iLine);
EXP PSZ   Mem1_strndup (PSZ psz, INT iLen, PSZ pszFile, INT iLine);
EXP void  Mem1_free    (PVOID p, PSZ pszFile, INT iLine);
EXP void  Mem1_exit    (int i, PSZ pszFile, INT iLine);
EXP PPSZ  Mem1_freeppsz(PSZ *ppsz, INT iNum, PSZ pszFile, INT iLine);
EXP PVOID Mem1_freedata(PVOID p, PSZ pszFile, INT iLine);

#define malloc(u)        Mem1_malloc (u,__FILE__,__LINE__)
#define calloc(u,v)      Mem1_calloc (u,v,__FILE__,__LINE__)
#define realloc(p,u)     Mem1_realloc (p,u,__FILE__,__LINE__)
#define strdup(p)        Mem1_strdup (p,__FILE__,__LINE__)
#define strndup(p,u)     Mem1_strdup (p,u,__FILE__,__LINE__)
#define free(p)          Mem1_free (p,__FILE__,__LINE__)
#define exit(i)          Mem1_exit (i,__FILE__,__LINE__)
#define MemFreePPSZ(p,u) Mem1_freeppsz(p,u,__FILE__,__LINE__)
#define MemFreeData(p)   Mem1_freedata(p,__FILE__,__LINE__)

#endif // (MEM_ERRORS)



//} // extern C


#endif  // GNUMEM_INCLUDED

