/*
 *
 * mem.h
 * Tuesday, 9/28/1993.
 * Internal Header File for GnuLib
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

WORD memGetDebugMode (void);

FILE *memGetDebugStream (void);

INT memErr (PSZ psz, ...);

void memLog (PSZ psz, ...);

#ifdef __cplusplus
}
#endif
