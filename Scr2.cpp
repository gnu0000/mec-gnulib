/*
 * Scr2.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * Secondary Cursor manipulation
 * 
 * 
 **************************************************************************** 
 * 
 * void ScrPushCursor (bShow)
 * void ScrPopCursor (void)
 * 
 **************************************************************************** 
 * 
 */

#include <stdlib.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnuscr.h"


typedef struct _curpos
	{
	COORD          dwPos;
	BOOL           bShow;
	struct _curpos *next;
	} CURSORPOS;
typedef CURSORPOS *PCURSORPOS;


static PCURSORPOS PCSTACK = NULL;


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*
 * Pushes the cursor pos/visibility onto the stack
 * also shows/hides the cursor
 */
void ScrPushCursor (BOOL bShow)
	{
	PCURSORPOS pcp;

	pcp = (PCURSORPOS) malloc (sizeof (CURSORPOS));
	pcp->next = PCSTACK;
	PCSTACK = pcp;

	pcp->dwPos = ScrGetCursorPos ();
	pcp->bShow = ScrCursorVisible ();
	ScrShowCursor (bShow);
	}


/*
 * Pops the cursor pos/visibility from the stack
 *
 */
void ScrPopCursor (void)
	{
	PCURSORPOS pcp;

	if (!(pcp = PCSTACK))
		return;

	ScrSetCursorPos (pcp->dwPos);
	ScrShowCursor (pcp->bShow);
	PCSTACK = pcp->next;
	free (pcp);
	}

