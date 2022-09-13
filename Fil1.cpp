/*
 * Fil1.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides additional file manipulation utilities
 */

#include <stdio.h>
#include <stdlib.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnufile.h"


extern "C"
{

/*
 * Struct for FilPushPos and FilPopPos
 */
typedef struct _node 
	{
	FILE   *fp;
	PULONG pos;
	INT    i;
	struct _node *next;
	} POSSTK;
POSSTK  *stk = NULL;


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


/* -> pushes the file position for FILE
 * -> a separate stack is kept for each FILE
 *     so pushing different FILEs wil not cause a conflict
 * -> returns the depth of the stack for FILE
 */
INT FilPushPos (FILE *fp)
	{
	ULONG  ulPos;
	POSSTK *tmpstk;

	if (!fp || ((ulPos = ftell (fp)) == -1))
		return 0;

	for (tmpstk = stk; tmpstk; tmpstk = tmpstk->next)
		if (tmpstk->fp == fp)
			{
			tmpstk->pos = (PULONG) realloc (tmpstk->pos, sizeof (ULONG)*(++tmpstk->i));
			tmpstk->pos[tmpstk->i-1] = ulPos;
			return tmpstk->i;
			}
	tmpstk = (POSSTK*) malloc (sizeof (POSSTK));
	tmpstk->fp = fp;
	tmpstk->pos = (PULONG) malloc (sizeof (ULONG));
	tmpstk->pos[0] = ulPos;
	tmpstk->i = 1;
	tmpstk->next = stk;
	stk = tmpstk;
	return 1;
	}



/* -> peeks file position for FILE
 * -> returns 0 on error
 */
ULONG FilPeekPos (FILE *fp)
	{
	POSSTK  *tmpstk;

	if (fp == NULL || stk == NULL)
		return 0;

	for (tmpstk = stk; tmpstk; tmpstk = tmpstk->next)
		if (tmpstk->fp == fp)
			return (tmpstk->i ? tmpstk->pos[tmpstk->i - 1] : 0);
	return 0;
	}



/* -> pops the file position for FILE
 * -> id uSet != 0 the FILE position is set to the last push position
 *     otherwise the position is popped and discarded.
 * -> returns the depth of the stack for FILE or -1 if there is
 *     no stack for FILE
 */
INT FilPopPos (FILE *fp, BOOL bSet)
	{
	POSSTK *prevstk, *tmpstk;
	INT    i;

	if (fp == NULL || stk == NULL)
		return -1;
	
	for (prevstk = tmpstk = stk; tmpstk; tmpstk = tmpstk->next) 
		{
		if (tmpstk->fp != fp)
			{
			prevstk = tmpstk;
			continue;
			}

		if (bSet) 
			fseek (fp, tmpstk->pos[--tmpstk->i], SEEK_SET);
		if (i = tmpstk->i)
			tmpstk->pos = (PULONG) realloc (tmpstk->pos, sizeof (ULONG)*(tmpstk->i));
		else
			{
			free (tmpstk->pos);

			if (stk == tmpstk)
				stk = tmpstk->next;
			else
				prevstk = tmpstk->next;

			free (tmpstk);
			}
		return i;
		}
	return -1;
	}


/*
 * Swaps the current file pos with the position on the stack
 * if bSet = TRUE, the file ptr is moved to the old stack position
 */
BOOL FilSwapPos (FILE *fp, BOOL bSet)
	{
	ULONG   ulPeekPos, ulPopPos;
	POSSTK  *tmpstk;

	if (fp == NULL || stk == NULL || ((ulPeekPos = ftell (fp)) == -1))
		return FALSE;
	
	for (tmpstk = stk; tmpstk; tmpstk = tmpstk->next)
		if (tmpstk->fp == fp)
			{
			ulPopPos = tmpstk->pos[tmpstk->i - 1];
			tmpstk->pos[tmpstk->i - 1] = ulPeekPos;
			if (bSet)
				fseek (fp, ulPopPos, SEEK_SET);
			return TRUE;
			}
	return FALSE;
	}


INT FilPeek (FILE *fp)
	{
	return (f_ungetc (f_getc (fp), fp));
	}


}