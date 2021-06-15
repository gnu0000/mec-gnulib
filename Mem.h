/*
 *
 * mem.h
 * Tuesday, 9/28/1993.
 * Internal Header File for GnuLib
 *
 */



WORD memGetDebugMode (void);

FILE *memGetDebugStream (void);

INT memErr (PSZ psz, ...);

void memLog (PSZ psz, ...);

