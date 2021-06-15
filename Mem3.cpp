/*
 * Mem3.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 *
 * This module provides the functions for GnuMem module
 * these mem functions are used by all the GnuLib functtions
 * that dynamically allocate memory.
 */

#if defined (MEM_ERRORS)
#undef MEM_ERRORS
#endif
#if defined (MEM_DEBUG)
#undef MEM_DEBUG
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minmax.h>

#include "gnutype.h"
#include "gnumem.h"
#include "mem.h"

#define INTERNAL "internal"
#define PREMARK   0x4D654D21 // !MeM
#define POSTMARK  0x4D454D21 // !MEM

#define MALLOC   1
#define REALLOC  2
#define STRDUP   3

typedef struct _mmn
	{
	struct _mmn *Next;
	struct _mmn *Prev;
	PSZ     pszFile;
	INT     iMethod;
	INT     iLine;
	INT     iSize;
	ULONG   ulBlockIdx;
	ULONG   ulPreMark;
	PVOID   pData;
	} MEM;
typedef MEM *PMEM;


static ULONG ulFREES      = 0L; // number of calls to free
static ULONG ulMALLOCS    = 0L; // number of calls to malloc
static ULONG ulREALLOCS   = 0L; // number of calls to realloc
static ULONG ulCURRMALLOCS= 0L; // number of calls to malloc not freed
static ULONG ulBLOCKIDX   = 0L; // increment for each mallloc
static ULONG ulMAXMALLOC  = 0L; // largest malloc
static ULONG ulMAXMEM    = 0L; // largest total mem malloced

static PMEM memtree = NULL;


/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/


static PMEM mem__malloc (INT iSize, PSZ pszFile, INT iLine)
	{
	PMEM   pnode, pnodeTmp;
	INT iMallocSize;
	ULONG  ulTotalSize;


	if (!(pnode = (PMEM) malloc (sizeof (MEM))))
		memErr ("(%s,%u): INTERNAL - Cannot malloc %u bytes.", pszFile, iLine, iSize);

	/*--- Add new node to beginning of tree ---*/
	pnode->Next      = memtree;
	pnode->Prev      = NULL;
	pnode->iLine     = iLine;
	pnode->iSize     = iSize;
	pnode->ulPreMark = PREMARK;
	pnode->ulBlockIdx= 0;
	 
	if (memtree)
		memtree->Prev = pnode;
	memtree = pnode;

	if (!(pnode->pszFile = strdup (pszFile)))
		memErr ("(%s,%u): INTERNAL - Cannot malloc a few bytes.", pszFile, iLine);

	iMallocSize = iSize + sizeof (ULONG) + 
					  ((memGetDebugMode () & MEM_EXTRA) ? 256 : 0);

	if (!(pnode->pData = malloc (iMallocSize)))
		memErr ("(%s,%u): Cannot malloc %u bytes.", pszFile, iLine, iSize);

	memset (pnode->pData, 0xAA, iSize);

	*(PULONG)((PSZ)(pnode->pData) + iSize) = POSTMARK;

	for (ulTotalSize = 0, pnodeTmp=memtree; pnodeTmp; pnodeTmp=pnodeTmp->Next)
		ulTotalSize += (ULONG)pnodeTmp->iSize;
	ulMAXMEM = max (ulMAXMEM, ulTotalSize);

	return pnode;
	}


static PMEM mem__find (PVOID p, PSZ pszFile, INT iLine)
	{
	PMEM pnode;

	for (pnode=memtree; pnode; pnode=pnode->Next)
		if (pnode->pData == p)
			return pnode;
	memErr ("(%s,%u): pointer not in allocaton table: %p", pszFile, iLine, p);
	return NULL;
	}


static void mem__free (PMEM pnode)
	{
	if (pnode->Prev)
		pnode->Prev->Next = pnode->Next;
	if (pnode->Next)
		pnode->Next->Prev = pnode->Prev;
	if (memtree == pnode)
		memtree = pnode->Next;

	if ((memGetDebugMode () & MEM_CLEAR))
		memset (pnode->pData, 0xBB, pnode->iSize);

	free (pnode->pData);
	free (pnode->pszFile);
	free (pnode);
	}


static void mem__test (PMEM pnode, PSZ pszFile, INT iLine)
	{
	if (pnode->ulPreMark != PREMARK)
		memErr ("%5.5ld (%s,%u) ptr underrun - found at (%s,%u)",
					pnode->ulBlockIdx, pnode->pszFile, 
					pnode->iLine, pszFile, iLine);

	if (*(PULONG)((PSZ)(pnode->pData) + pnode->iSize) != POSTMARK)
		memErr ("%5.5ld (%s,%u) ptr overrun - found at (%s,%u)",
					pnode->ulBlockIdx, pnode->pszFile, 
					pnode->iLine, pszFile, iLine);
	}

void Mem2_test (PSZ pszFile, INT iLine)
	{
	PMEM pnode;

	for (pnode=memtree; pnode; pnode=pnode->Next)
		mem__test (pnode, pszFile, iLine);
	}

/************************************************************************/
/*                                                                      */
/* Exported functions below                                             */
/*                                                                      */
/************************************************************************/


PVOID Mem2_malloc (INT iSize, PSZ pszFile, INT iLine)
	{
	PMEM pnode;

	pnode = mem__malloc (iSize, pszFile, iLine);
	pnode->iMethod = MALLOC;
	pnode->ulBlockIdx = ulBLOCKIDX++;

	ulMALLOCS++;
	ulCURRMALLOCS++;
	ulMAXMALLOC = max (ulMAXMALLOC, (ULONG)iSize);

	memLog (" %5.5ld  MALLOC: Ptr:%p  Len:%5u  (%s,%u).", 
				pnode->ulBlockIdx, pnode->pData, iSize, pszFile, iLine);

	if (memGetDebugMode () & MEM_TEST)
		Mem2_test (pszFile, iLine);

	return pnode->pData;
	}


PVOID Mem2_calloc (INT iNum, INT iSize, PSZ pszFile, INT iLine)
	{
	PMEM pnode;

	pnode = mem__malloc (iNum * iSize, pszFile, iLine);
	pnode->iMethod = MALLOC;
	pnode->ulBlockIdx = ulBLOCKIDX++;

	ulMALLOCS++;
	ulCURRMALLOCS++;
	ulMAXMALLOC = max (ulMAXMALLOC, (ULONG)iSize);

	memLog (" %5.5ld  CALLOC: Ptr:%p  Len:%5u  (%s,%u).", 
				pnode->ulBlockIdx, pnode->pData, iSize, pszFile, iLine);

	memset (pnode->pData, 0, iNum * iSize);

	if (memGetDebugMode () & MEM_TEST)
		Mem2_test (pszFile, iLine);

	return pnode->pData;
	}


void Mem2_free (PVOID p, PSZ pszFile, INT iLine)
	{
	PMEM pnode;

	if (!p)
		{
		if (memGetDebugMode () & MEM_NULLOK)
			{
			memLog (" XXXXX    FREE: Ptr:NULL  Len:    0  (%s,%u).", pszFile, iLine);
			return;
			}
		else
			memErr ("(%s,%u): Attempt to free a null ptr.", pszFile, iLine);
		}
	pnode = mem__find (p, pszFile, iLine);

	memLog (" %5.5ld    FREE: Ptr:%p  Len:%5u  (%s,%u) from (%s,%u).", 
			  pnode->ulBlockIdx, (PSZ)(pnode->pData), pnode->iSize, 
			  pszFile, iLine, pnode->pszFile, pnode->iLine);

	mem__free (pnode);

	ulFREES++;
	ulCURRMALLOCS--;

	if (memGetDebugMode () & MEM_TEST)
		Mem2_test (pszFile, iLine);
	}



PVOID Mem2_realloc (PVOID p, INT iSize, PSZ pszFile, INT iLine)
	{
	PMEM  pnewnode, pnode;

	/*--- do a new malloc ---*/
	pnewnode = mem__malloc (iSize, pszFile, iLine);
	pnewnode->iMethod = REALLOC;
	ulREALLOCS++;
	ulCURRMALLOCS++;
	ulMAXMALLOC = max (ulMAXMALLOC, (ULONG)iSize);

	if (!p)
		{
		pnewnode->ulBlockIdx = ulBLOCKIDX++;

		memLog (" %5.5ld reALLOC: Ptr:%p  Len:%5u  (alloc)(%s,%u).", 
				  pnewnode->ulBlockIdx, (PSZ)(pnewnode->pData), 
				  pnewnode->iSize, pszFile, iLine);

		if (memGetDebugMode () & MEM_TEST)
			Mem2_test (pszFile, iLine);

		return pnewnode->pData;
		}

	pnode = mem__find (p, pszFile, iLine);
	memcpy (pnewnode->pData, p, min (pnode->iSize, iSize));
	pnewnode->ulBlockIdx = pnode->ulBlockIdx; // keep block idx the same

	mem__free (pnode);
	ulCURRMALLOCS--;

	memLog (" %5.5ld REALLOC: Ptr:%p->%p  Len:%5u->%5u  (%s,%u).",
			  pnode->ulBlockIdx,  
			  (PSZ)(pnode->pData), (PSZ)(pnewnode->pData),
			  pnode->iSize, pnewnode->iSize, pszFile, iLine);

	if (memGetDebugMode () & MEM_TEST)
		Mem2_test (pszFile, iLine);

	return pnewnode->pData;
	}


PSZ Mem2_strdup (PSZ psz, PSZ pszFile, INT iLine)
	{
	PMEM   pnode;
	INT iLen;

	if (!psz)
		memErr ("(%s,%u): Cannot strdup a null ptr", pszFile, iLine);

	iLen = strlen (psz);

	pnode = mem__malloc (iLen+1, pszFile, iLine);
	pnode->iMethod    = STRDUP;
	pnode->ulBlockIdx = ulBLOCKIDX++;
	ulMALLOCS++;
	ulCURRMALLOCS++;
	ulMAXMALLOC = max (ulMAXMALLOC, (ULONG)iLen);

	memcpy (pnode->pData, psz, iLen);
	((PSZ)pnode->pData)[iLen] = '\0';

	memLog (" %5.5ld  STRDUP: Ptr:%p  Len:%5u  (%s,%u).",
			  pnode->ulBlockIdx, (PSZ)(pnode->pData), iLen+1, pszFile, iLine);

	if (memGetDebugMode () & MEM_TEST)
		Mem2_test (pszFile, iLine);

	return (PSZ) pnode->pData;
	}


PSZ Mem2_strndup (PSZ psz, INT iLen, PSZ pszFile, INT iLine)
	{
	PMEM   pnode;

	if (!psz)
		memErr ("(%s,%u): Cannot strdup a null ptr", pszFile, iLine);

	pnode = mem__malloc (iLen, pszFile, iLine);
	pnode->iMethod    = STRDUP;
	pnode->ulBlockIdx = ulBLOCKIDX++;
	ulMALLOCS++;
	ulCURRMALLOCS++;
	ulMAXMALLOC = max (ulMAXMALLOC, (ULONG)iLen);

	memcpy (pnode->pData, psz, iLen);
	memLog (" %5.5ld STRNDUP: Ptr:%p  Len:%5u  (%s,%u).",
			  pnode->ulBlockIdx, (PSZ)(pnode->pData), iLen, pszFile, iLine);

	if (memGetDebugMode () & MEM_TEST)
		Mem2_test (pszFile, iLine);

	return (PSZ) pnode->pData;
	}


void Mem2_list (PSZ pszFile, INT iLine)
	{
	PMEM   pnode;
	INT i;
	CHAR   c;
	ULONG  ulTotalMem  = 0;
	ULONG  ulActivePtrs= 0;
	FILE  *fpLog;

	if (!(fpLog = memGetDebugStream ()))
		return;

	fprintf (fpLog, "          ======== MEMORY STILL ALLOCATED ========\n");
	fprintf (fpLog, "\n");
	fprintf (fpLog, " BLKIDX FILE                 LINE | MODE     SIZE PTR       DATA\n");
	fprintf (fpLog, " =================================|===========================================\n");
	//                00000  XXXXXXXXXXXXXXXXXXXX 9999 | REALLOC 99999 XXXX:XXXX [................]\n");

	for (pnode=memtree; pnode; pnode=pnode->Next)
		{
		mem__test (pnode, pszFile, iLine);

		fprintf (fpLog, " %5.5ld  %-20s %4u | %-7s %5u %p [",
			pnode->ulBlockIdx, pnode->pszFile, pnode->iLine,
			(pnode->iMethod == MALLOC ? "malloc" : (pnode->iMethod == REALLOC ? "realloc" : "strdup")),
			pnode->iSize, pnode->pData);
		for (i=0; i<16 && i<pnode->iSize; i++)
			fputc (((c = ((PSZ)pnode->pData)[i]) >= ' ' ? c : '.'), fpLog);
		fprintf (fpLog, "]\n");

		ulActivePtrs++;
		ulTotalMem += pnode->iSize;
		}
	fprintf (fpLog, " =================================|===========================================\n");
	fprintf (fpLog, "  Total mallocs :%6lu    Largest Block :%6lu\n",
		ulMALLOCS, ulMAXMALLOC);
	fprintf (fpLog, "  Total reallocs:%6lu    Unfreed Blocks:%6lu\n",
		ulREALLOCS, ulCURRMALLOCS);
	fprintf (fpLog, "  Total Frees   :%6lu    Unfreed Memory:%6lu\n",
		ulFREES, ulTotalMem);
	fprintf (fpLog, "  Max Memory Use:%6lu\n", ulMAXMEM);
	}


void Mem2_exit (int uExit, PSZ pszFile, INT iLine)
	{
	if (memGetDebugMode () & MEM_EXITLIST)
		Mem2_list (pszFile, iLine);
	else
		Mem2_test (pszFile, iLine);
	memLog ("  Exit Val      : %5u", uExit);
	exit (uExit);
	}
	

void Mem2_testptr (PVOID p, PSZ pszFile, INT iLine)
	{
	PMEM pnode;

	pnode = mem__find (p, pszFile, iLine);
	mem__test (pnode, pszFile, iLine);
	}



void mtst (void)
	{
	Mem2_test ("CVP Test", 0);
	}

void mtstp (PVOID p)
	{
	Mem2_testptr (p, "CVP Test", 0);
	}



PPSZ Mem2_freeppsz (PSZ *ppsz, INT iNum, PSZ pszFile, INT iLine)
	{
	PSZ *pp;

	if (!ppsz)
		return NULL;

	if (iNum)
		{
		while (iNum)
			if (ppsz [--iNum])
				Mem2_free (ppsz [iNum], pszFile, iLine);
		}
	else
		{
		for (pp = ppsz; *pp; pp++)
			if (*pp)
				Mem2_free (*pp, pszFile, iLine);
		}
	Mem2_free (ppsz, pszFile, iLine);
	return NULL;
	}


PVOID Mem2_freedata(PVOID p, PSZ pszFile, INT iLine)
	{
	if (p)
		Mem2_free (p, pszFile, iLine);
	else
		{
		memLog (" 00000 FREEDAT: Ptr:0000:0000  Len:    0  (%s,%u).",
				  pszFile, iLine);
		}
	return NULL;
	}


