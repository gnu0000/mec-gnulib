/*
 *
 * kbd.h
 * Tuesday, 9/21/1993.
 * Internal Header File for GnuLib
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _ktrap
   {
   INT    iKey;
   PKEYFN pfnTrap;

   struct _ktrap *left;
   struct _ktrap *right;
   } KTRAP;
typedef KTRAP *PKTRAP;


#ifdef __cplusplus
}
#endif


