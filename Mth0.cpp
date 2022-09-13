/*
 * Mth0.c
 *
 * Craig Fitzgerald  1993-1995
 *
 * Part of the GnuLib library
 *
 * 
 * This file provides the math functions for Math
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "gnutype.h"
#include "gnumem.h"
#include "gnustr.h"
#include "gnumath.h"
#include "mth.h"
#include "gnumath.h"


extern "C"
{

static BIG _Eval (PSZ *ppszExp, INT iLevel);


static INT EVALERR    = 0;
static PSZ EVALERRSTR = NULL;
static INT EVALERRIDX = 0;
static PSZ EVALERRPOS = NULL;
static PSZ pszErrPtr  = NULL;


/*
 * Holds fn's vars and constants
 */
PMNODE pTREE    = NULL;
PMNODE pLASTVAR = NULL;


static BOOL bPERCENT         = FALSE;
static BOOL bIgnoreFunctions = FALSE;


typedef struct
   {
   INT iLevel;
   INT iOP;
   PSZ pszOP;
   } OP;
typedef OP *POP;


/*
 * Ordered so that they will match correctly when
 * searched in a linear fashion.
 * (ie '*=' will match '*=' before '*')
 */
static OP OPERANDS[] =
		  {
			{9,  OP_SRT,  ">>" },
			{9,  OP_SLT,  "<<" },
			{8,  OP_GE,   ">=" }, 
			{8,  OP_LE,   "<=" }, 
			{7,  OP_EQU,  "==" }, 
			{7,  OP_NEQ,  "!=" }, 
			{4,  OP_AND,  "&&" },
			{3,  OP_OR,   "||" },
			{1,  OP_SADD, "+=" }, 
			{1,  OP_SSUB, "-=" }, 
			{1,  OP_SMOD, "%=" }, 
			{1,  OP_SDIV, "/=" }, 
			{1,  OP_SMUL, "*=" },
			{12, OP_EXP,  "^"  }, 
			{11, OP_MOD,  "%"  }, 
			{11, OP_DIV,  "/"  }, 
			{11, OP_MUL,  "*"  }, 
			{10, OP_ADD,  "+"  }, 
			{10, OP_SUB,  "-"  }, 
			{8,  OP_GT,   ">"  }, 
			{8,  OP_LT,   "<"  }, 
			{6,  OP_BAND, "&"  },
			{5,  OP_BOR,  "|"  },
			{1,  OP_SET,  "="  }, 
			{2,  OP_COND, "?"  },
			{0,  OP_COMA, ","  },  // This is an operator, so make sure to strip
			{0,  0,       NULL }}; // Terminator



static CHAR *MathErrors[] = 
							{"No Error",                         // 0
							 "Closing Parenthesis expected ')'", // 1
							 "Opening Parenthesis expected '('", // 2
							 "Unrecognized character",           // 3
							 "Empty String",                     // 4
							 "Function Overflow",                // 5
							 "Function param out of range",      // 6
							 "Illegal function value",           // 7
							 "Function result out of range",     // 8
							 "Divide by zero error",             // 9
							 "Colon Expected ':'",               // 10
							 "Not enough memory",                // 11
							 "Cannot change identifier type",    // 12
							 NULL};                                  



/********************************************************************/
/*                                                                  */
/*  Error Routines                                                  */
/*                                                                  */
/********************************************************************/

BIG mthSetErr (INT iErr, PSZ pszErrPos)
	{
	BIG bg; 

	bg = 0.0;

	if (EVALERR)
		return bg;
	EVALERR    = iErr;
	EVALERRSTR = MathErrors [iErr];
	EVALERRPOS = pszErrPos;
	return bg;
	}

#if !defined (DEBUG) && !defined (_DEBUG)

/*
 * Math NMI error function
 *
 *     return mthSetErr (2, *ppszExp);
 *
 */
#if defined (__WATCOMC__)
  int matherr (struct exception *except)
#else
int __cdecl _matherr (struct _exception *except)
#endif
	{
	switch (except->type)
		{
		case DOMAIN:    /*--- argument out of range        ---*/
			except->retval = 0;
			mthSetErr (6, pszErrPtr);
			break;

		case OVERFLOW:  /*--- result too large             ---*/
			except->retval = 0;
			mthSetErr (5, pszErrPtr);
			break;

		case SING:      /*--- illegal value ie log(0)      ---*/
			except->retval = 0;
			mthSetErr (7, pszErrPtr);
			break;

		case TLOSS:     /*--- total loss                   ---*/
			except->retval = 0;
			mthSetErr (8, pszErrPtr);
			break;

		case UNDERFLOW: /*--- result too small             ---*/
			except->retval = 0;
			mthSetErr (5, pszErrPtr);
			break;

		default:
			return 0;
		}
	return 1;
	}

#endif

/*
 *
 * After each call to AToBig, this fn stores the error state
 * This fn returns a non zero value in case of an error
 *
 * pszErrStr will contain a description of the error
 * *piErrIdx will contain the index of the offending CHAR
 *    from the string passed to AToBIG
 *
 * ret   error string
 *------------------------------------------------
 * 0     No Error                         
 * 1     Closing Parenthesis expected ')' 
 * 2     Opening Parenthesis expected '(' 
 * 3     Unrecognized character           
 * 4     Empty String                     
 * 5     Function Overflow                
 * 6     Function param out of range      
 * 7     Illegal function value           
 * 8     Function result out of range     
 * 
 */
INT MthIsError (PSZ *ppszErrStr, PINT piErrIdx)
	{
	if (ppszErrStr)
		*ppszErrStr = EVALERRSTR;
	if (piErrIdx)
		*piErrIdx = EVALERRIDX;
	return EVALERR;
	}




/********************************************************************/
/*                                                                  */
/*  BIG Math Routines                                               */
/*                                                                  */
/********************************************************************/

/*
 * Returns TRUE is the number is not marked as invalid
 *
 */
BOOL MthValid (BIG bgNum)
	{
	PULONG  pulTmp = (PULONG)(PVOID)&(bgNum);

	if (pulTmp[0] != (ULONG)MARK)
		return TRUE;

	/*--- not compatible with prev file release ---*/
	return (pulTmp[1] != (ULONG)MARK);

	//return FALSE;
	}


/*
 * Marks the number as invalid
 *
 */
BIG MthInvalidate (PBIG pbgNum)
	{
	PULONG pulTmp = (PULONG)(PVOID)(pbgNum);

	pulTmp[0] = (ULONG)MARK;

	/*--- not compatible with prev file release ---*/
	pulTmp[1] = (ULONG)MARK;

   return *pbgNum;
	}


/*
 * Call this with TRUE to make the % a mod operator
 * Call this with FALSE to make the % a percent operator
 */
void MthPercentIsMod (BOOL b)
	{
	bPERCENT = !b;
	}


/*
 * returns:
 *  4 - one or more invalid
 *  1 - both valid and matched to within .000005
 *  2 - both valid and matched to within .005
 *  3 - both valid but not matched
 *
 */
INT MthMatch (PBIG pbg1, PBIG pbg2)
	{
	if (!MthValid (*pbg1)  || !MthValid (*pbg2))
		return 0;
	if (*pbg1 - *pbg2 > 0.005    || *pbg1 - *pbg2 < -0.005)
		return 3;
	if (*pbg1 - *pbg2 > 0.000005 || *pbg1 - *pbg2 < -0.000005)
		return 2;
	return 1;
	}


/***************************************************************************/
/*                                                                         */
/* Variable, constant, function and string function handling fns           */
/*                                                                         */
/***************************************************************************/

/*
 * Internal - Creates a new tree node
 * The tree holds all Variables, Constants, and Functions
 */
static PMNODE NewMNODE (PSZ pszName)
	{
	PMNODE pmnode;

	if (!(pmnode = (PMNODE) calloc (1, sizeof (MNODE))))
		{
		mthSetErr (11, NULL); // memory problem
		return NULL;
		}
	pmnode->pszName = strdup (pszName);
	if (*pszName != '@')
		return pmnode;
	pmnode->cType   = 'V';
	pmnode->node.big = 0.0;
	return pmnode;
	}


/*
 * Internal - used only by the external fn FindNode
 * find a named identifier in the tree
 * If the id does not exist, a new node will be created
 * if bCreateAll is TRUE -OR- the id is for a variable
 */
static PMNODE _findmnode (PMNODE pmnode, PSZ pszName, INT bCreateAll)
	{
	PMNODE pmnodeMatch;
	INT    i;

	if (!pmnode)
		return (bCreateAll || *pszName=='@' ? NewMNODE (pszName) : NULL);

	if (!(i = stricmp (pszName, pmnode->pszName)))
		pmnodeMatch = pmnode;
	else if (i < 0)
		{
		pmnodeMatch = _findmnode (pmnode->left, pszName, bCreateAll);
		pmnode->left = (pmnode->left ? pmnode->left : pmnodeMatch);
		}
	else if (i > 0)
		{
		pmnodeMatch = _findmnode (pmnode->right, pszName, bCreateAll);
		pmnode->right = (pmnode->right ? pmnode->right : pmnodeMatch);
		}
	return pmnodeMatch;
	}


/*
 * Finds the node in the tree
 * If no match is found:
 *    if bCreateAll is TRUE a new node is added to the tree and returned
 *    if bCreateAll is FALSE a new node is added if pszName is a variable
 *    (a variable starts with a '@') else NULL is returned
 *
 * on return you will be able to tell if it is a new node
 * because cType will be 0
 */
PMNODE FindNode (PSZ pszName, BOOL bCreateAll)
	{
	PMNODE pmnodeMatch;

	pmnodeMatch = _findmnode (pTREE, pszName, bCreateAll);
	pTREE = (pTREE ? pTREE : pmnodeMatch);
	return pmnodeMatch;
	}


/*
 * Add a function.
 * The added function must have a definition/declaration of 
 * type PMTHFN which basically means:
 *
 * BIG function (BIG var)
 *
 */
BOOL MthAddFunction (PSZ pszName, PMTHFN pfn)
	{
	PMNODE pmnode;

	if (!(pmnode = FindNode (pszName, TRUE)))
		return !!mthSetErr (11, NULL); // memory problem
	if (pmnode->cType && pmnode->cType != 'F')
		return !!mthSetErr (12, NULL); // identifier for different type
	pmnode->cType = 'F';
	pmnode->node.pfn = pfn;
	return TRUE;
	}


/*
 *
 *
 *
 *
 */
BOOL MthAddFunction2 (PSZ pszName, PSZ pszExpr)
	{
	PMNODE pmnode;

	if (!(pmnode = FindNode (pszName, TRUE)))
		return !!mthSetErr (11, NULL); // memory problem
	if (pmnode->cType && pmnode->cType != 'S')
		return !!mthSetErr (12, NULL); // identifier for different type
	pmnode->cType = 'S';
	MemFreeData (pmnode->node.psz);
	pmnode->node.psz = strdup (pszExpr);
	return TRUE;
	}

BOOL MthAddVariable  (PSZ pszName, BIG big)
	{
	PMNODE pmnode;

	if (!(pmnode = FindNode (pszName, TRUE)))
		return !!mthSetErr (11, NULL); // memory problem
	if (pmnode->cType && pmnode->cType != 'V')
		return !!mthSetErr (12, NULL); // identifier for different type
	pmnode->cType = 'V';
	pmnode->node.big = big;
	return TRUE;
	}

BOOL MthAddConstant  (PSZ pszName, BIG big)
	{
	PMNODE pmnode;

	if (!(pmnode = FindNode (pszName, TRUE)))
		return !!mthSetErr (11, NULL); // memory problem
	if (pmnode->cType)
		return !!mthSetErr (12, NULL); // identifier must be new
	pmnode->cType = 'C';
	pmnode->node.big = big;
	return TRUE;
	}


BIG MthSetDirect (PMNODE pmnode, BIG bgVal)
	{
	if (pmnode && (pmnode->cType == 'V' || pmnode->cType == 'C'))
		pmnode->node.big = bgVal;
	return bgVal;
	}


BIG MthGetVariable (PSZ pszName)
	{
	PMNODE pmnode;

	if (!(pmnode = FindNode (pszName, FALSE)))
		return mthSetErr (7, NULL); // memory problem
	if (pmnode->cType == 'V' || pmnode->cType == 'C')
		return pmnode->node.big;
	return mthSetErr (12, NULL); // memory problem
	}


/*
 * This is a find node that expects the id at the start of the ppsz string
 * this fn updates the ppsz position to be past the identifier.
 * If no match is found and bCreate is FALSE, or if the leading string
 * is not a valid ID, then NULL is returned
 */
PMNODE GetNode (PPSZ ppsz)
	{
	CHAR   szIdentifier [128];
	PSZ    pszSrc, pszDest;
	BOOL   bVariable;
	PMNODE pmnode;

	pszSrc  = *ppsz;
	pszDest = szIdentifier;

	if (bVariable = (*pszSrc == '@'))// identifiers may start with a '@'
		*pszDest++ = *pszSrc++;       // which means they are variables
	if (*pszSrc == '@')              //
		*pszDest++ = *pszSrc++;       // "@@" is a special case identifier
	else                             //
		{                             //
		if (!__iscsymf (*pszSrc))     // After an optional '@' an id must
			return NULL;               // start with a letter or _ char
		while (__iscsym (*pszSrc))    // then it may contain letters, _ and
			*pszDest++ = *pszSrc++;    // digits.
		}                             //
	*pszDest = '\0';
	if (pmnode = FindNode (szIdentifier, FALSE))
		*ppsz = pszSrc;               // on a match update ppsz ptr

	if (bVariable && pmnode)         // This is a kludge to remember the last
		pLASTVAR = pmnode;            // variable accessed on the left of the
												// = sign for asignment purposes
	return pmnode;
	}


BIG VarVal (PMNODE pmnode)
	{
	if (pmnode && pmnode->cType == 'C' || pmnode->cType == 'V')
		return pmnode->node.big;
	return 0;
	}


static void _mthClearAll (PMNODE pmnode)
	{
	if (!pmnode)
		return;
	if (pmnode->cType == 'V')
		pmnode->node.big = 0.0;

	_mthClearAll (pmnode->left);
	_mthClearAll (pmnode->right);
	}


void MthClearAllVariables (void)
	{
	_mthClearAll (pTREE);
	}


static void _mthFreeNode (PMNODE pmnode)
	{
	if (!pmnode)
		return;

	_mthFreeNode (pmnode->left);
	_mthFreeNode (pmnode->right);

	MemFreeData(pmnode->pszName);
	MemFreeData(pmnode);
	}


void MthFreeMath (void)
	{
	_mthFreeNode (pTREE);
	pTREE = NULL;
	}


/***************************************************************************/
/*                                                                         */
/* Init Fns                                                                */
/*                                                                         */
/***************************************************************************/


void mthInitMath (void)
	{
	/*
	 * this is the first node added to the TREE.  This allows us to cheat
	 * and access this node directly by accessing pTREE.  We want to
	 * access this node quickly because it the string function param variable
	 */
	MthAddVariable ("@@", 0.0);

	MthAddConstant ("pi", 3.1415926535);
	MthAddConstant ("e" , 2.302585    );

	MthAddFunction ("sin",    (PMTHFN) sin  );
	MthAddFunction ("cos",    (PMTHFN) cos  );
	MthAddFunction ("tan",    (PMTHFN) tan  );
	MthAddFunction ("asin",   (PMTHFN) asin );
	MthAddFunction ("acos",   (PMTHFN) acos );
	MthAddFunction ("atan",   (PMTHFN) atan );
	MthAddFunction ("abs",    (PMTHFN) fabs );
	MthAddFunction ("ceil",   (PMTHFN) ceil );
	MthAddFunction ("cosh",   (PMTHFN) cosh );
	MthAddFunction ("sinh",   (PMTHFN) sinh );
	MthAddFunction ("exp",    (PMTHFN) exp  );
	MthAddFunction ("floor",  (PMTHFN) floor);
	MthAddFunction ("log10",  (PMTHFN) log10);
	MthAddFunction ("log",    (PMTHFN) log  );
	MthAddFunction ("sqrt",   (PMTHFN) sqrt );
	}


/***************************************************************************/
/*                                                                         */
/*   Utility functions for parsing                                         */
/*                                                                         */
/***************************************************************************/

static int SkipWhite (PSZ *ppsz)
	{
	while (**ppsz && strchr (" \t", **ppsz)) 
		(*ppsz)++;
	return **ppsz;
	}


static int mthEat (PSZ *ppsz, PSZ pszList)
	{
	SkipWhite (ppsz);
	if (!**ppsz || !strchr (pszList, **ppsz))
		return 0;
	return *(*ppsz)++;
	}


static BOOL IsNumber (PSZ psz)
	{
	SkipWhite (&psz);
	if (*psz && strchr ("-+", *psz))
		{
		psz++;
		SkipWhite (&psz);
		}
	return !!(psz && strchr ("0123456789.", *psz)) ;
	}


#define NUMSIZE 128

static BIG EatNumber (PSZ *ppszExp)
	{
	BIG  big;
	CHAR szTmp[NUMSIZE];
	PSZ  p, p2;


	SkipWhite (ppszExp);
	
	/*--- _atold and atof doesn't like numbers like .1 and - 2 ---*/
	p  = *ppszExp;
	p2 = szTmp;
	if (strchr ("+-", *p))
		*p2++ = *p++;
	SkipWhite (&p);
	if (*p == '.')
		*p2++ = '0';
	strncpy (p2, p, NUMSIZE - (p2 - szTmp));
	szTmp [NUMSIZE - 1] = '\0';
	big = atof (szTmp);

	if (**ppszExp && strchr ("-+",               **ppszExp)) 
		(*ppszExp)++;
	SkipWhite (ppszExp);
	while (**ppszExp && strchr ("0123456789",    **ppszExp)) 
		(*ppszExp)++;
	if (**ppszExp && strchr (".",                **ppszExp)) 
		(*ppszExp)++;
	while (**ppszExp && strchr ("0123456789",    **ppszExp)) 
		(*ppszExp)++;
	if (**ppszExp && strchr ("dDeE",             **ppszExp))
		{
		(*ppszExp)++;
		if (**ppszExp && strchr ("-+",            **ppszExp)) 
			(*ppszExp)++;      
		while (**ppszExp && strchr ("0123456789", **ppszExp)) 
			(*ppszExp)++;
		}
	return big;
	}



/***************************************************************************/
/*                                                                         */
/*                 parse/eval Code                                         */
/*                                                                         */
/***************************************************************************/

/*
 * eats a parenthesized expression
 * open parens already eaten
 *
 */
static BIG ParenExp (PSZ *ppszExp)
	{
	BIG    big;

	big = _Eval (ppszExp, 0);
	if (!mthEat (ppszExp, ")"))
		return mthSetErr (1, *ppszExp);
	return big;
	}

static BIG EvalAtom (PSZ *ppszExp)
	{
	INT    i, l;
	BIG    bgTmp=0.0, bgTmp2=0.0;
	PSZ    pszTmp, pszFnExpr;
	PMNODE pmnode;

	i = l =0;
	/*--- Number ---*/
	if (IsNumber (*ppszExp))
		return EatNumber (ppszExp);

	/*--- Parenthesized Expression ---*/
	if (mthEat (ppszExp, "("))
		return ParenExp (ppszExp);

	/*--- are we looking at an identifier? ---*/
	if (**ppszExp != '@' && !__iscsymf (**ppszExp))
		return mthSetErr (3, *ppszExp);

	pszTmp = *ppszExp;

	/*--- not a variable ID and no matching fn or const found ---*/
	if (!(pmnode = GetNode (ppszExp)))
		return mthSetErr (3, *ppszExp);

	/*--- variable or constant ---*/
	if (pmnode->cType == 'V' || pmnode->cType == 'C')
		return pmnode->node.big;

	/*--- math function ---*/
	if (pmnode->cType == 'F')
		{
		if (!mthEat (ppszExp, "("))
			return mthSetErr (2, *ppszExp);

		bgTmp = ParenExp (ppszExp);
		if (EVALERR)
			return bgTmp;

		pszErrPtr = pszTmp;
		return pmnode->node.pfn(bgTmp);
		}

	/*--- math function expression ---*/
	if (pmnode->cType == 'S')
		{
		if (!mthEat (ppszExp, "("))
			return mthSetErr (2, *ppszExp);

		bgTmp = ParenExp (ppszExp);
		if (EVALERR)
			return bgTmp;

		pszErrPtr = pszTmp;
		pszFnExpr = pmnode->node.psz;

		bgTmp2    = pTREE->node.big; /*--- keep @@ scope local ---*/
		pTREE->node.big = bgTmp;

		if (bIgnoreFunctions)    /*--- recursion protection in non chosen ternary branches ---*/
			bgTmp = 1;
		else
			bgTmp = _Eval (&pszFnExpr, 0);

		pTREE->node.big = bgTmp2;
		return bgTmp;
		}

	/*--- we should never get here ---*/
	return mthSetErr (3, *ppszExp);
	}


static INT EatOp (PSZ *ppsz, INT iLevel)
	{
	INT iLen;
	POP pop;

	SkipWhite (ppsz);
	for (pop = OPERANDS; pop->iOP; pop++)
		{
		iLen = strlen (pop->pszOP);
		if (strncmp (*ppsz, pop->pszOP, iLen))
			continue;

		/*--- found op but at wrong presidence level ---*/
		if (pop->iLevel != iLevel)
			return 0;

		(*ppsz) += iLen;
		return pop->iOP;
		}
	return 0; // no match
	}


/*
 *
 *
 */
static BIG _Eval (PSZ *ppszExp, INT iLevel)
	{
	PMNODE pLast;
	BIG    big=0.0, bgTmp=0.0, bgTmp2=0.0;
	int    Op, j;
	PSZ    p;

	j=0;
	if (iLevel == ATOMLEVEL)
		return EvalAtom (ppszExp);
	big = _Eval (ppszExp, iLevel+1);

	p = *ppszExp;
	while (TRUE)
		{
		pLast = pLASTVAR; 
		switch (Op = EatOp (ppszExp, iLevel))
			{
			case OP_EXP : 
				big  = pow (big, _Eval (ppszExp, iLevel +1)); 
				break;
			case OP_MOD :
				if (bPERCENT)
					big /= (BIG)100.0; 
				else
					big = fmod(big, _Eval (ppszExp, iLevel +1));
				break;
			case OP_DIV :
				bgTmp = _Eval (ppszExp, iLevel +1);
				if (bgTmp < 1E-10 && bgTmp > -1E-10)
					mthSetErr (9, p);
				else
					big /= bgTmp;
				break;
			case OP_MUL : 
				big *= _Eval (ppszExp, iLevel +1); 
				break;
			case OP_ADD : 
				big += _Eval (ppszExp, iLevel +1); 
				break;
			case OP_SUB : 
				big -= _Eval (ppszExp, iLevel +1); 
				break;
			case OP_SLT : 
				big *= pow (10, _Eval (ppszExp, iLevel +1)); 
				break;
			case OP_SRT : 
				big /= pow (10, _Eval (ppszExp, iLevel +1)); 
				break;
			case OP_GT  : 
				big = (big >  _Eval (ppszExp, iLevel +1) ? 1.0 : 0.0); 
				break;
			case OP_LT  : 
				big = (big <  _Eval (ppszExp, iLevel +1) ? 1.0 : 0.0); 
				break;
			case OP_GE  : 
				big = (big >= _Eval (ppszExp, iLevel +1) ? 1.0 : 0.0); 
				break;
			case OP_LE  : 
				big = (big <= _Eval (ppszExp, iLevel +1) ? 1.0 : 0.0); 
				break;
			case OP_EQU : 
				big = (big == _Eval (ppszExp, iLevel +1) ? 1.0 : 0.0); 
				break;
			case OP_NEQ : 
				big = (big != _Eval (ppszExp, iLevel +1) ? 1.0 : 0.0); 
				break;
			case OP_BAND: 
				big = (BIG)((ULONG)big & (ULONG)_Eval (ppszExp, iLevel +1)); 
				break;
			case OP_BOR : 
				big = (BIG)((ULONG)big | (ULONG)_Eval (ppszExp, iLevel +1)); 
				break;
			case OP_AND : 
				bgTmp =  _Eval (ppszExp, iLevel +1);
				big = (big && bgTmp ? 1.0 : 0.0); 
				break;
			case OP_OR  : 
				bgTmp =  _Eval (ppszExp, iLevel +1);
				big = (big || bgTmp ? 1.0 : 0.0); 
				break;
			case OP_SET : 
				big = _Eval (ppszExp, iLevel);                               
				MthSetDirect (pLast, big); 
				break;
			case OP_SADD: 
				bgTmp = _Eval (ppszExp, iLevel); 
				big = VarVal(pLast) + bgTmp; 
				MthSetDirect (pLast, big); 
				break;
			case OP_SSUB: 
				bgTmp = _Eval (ppszExp, iLevel); 
				big = VarVal(pLast) - bgTmp; 
				MthSetDirect (pLast, big); 
				break;
			case OP_SMUL: 
				bgTmp = _Eval (ppszExp, iLevel); 
				big = VarVal(pLast) * bgTmp; 
				MthSetDirect (pLast, big); 
				break;
			case OP_SMOD: 
				bgTmp = _Eval (ppszExp, iLevel); 
				big = fmod (VarVal(pLast), bgTmp); 
				MthSetDirect (pLast, big); 
				break;
			case OP_SDIV:
				bgTmp = _Eval (ppszExp, iLevel); 
				if (bgTmp < 1E-10 && bgTmp > -1E-10)
					mthSetErr (9, p);
				else
					big = VarVal(pLast) / bgTmp;
				MthSetDirect (pLast, big); 
				break;
			case OP_COND: 
//            bgTmp  = _Eval (ppszExp, iLevel);
//            if (!mthEat (ppszExp, ":"))
//               return mthSetErr (10, *ppszExp);
//            bgTmp2 = _Eval (ppszExp, 0);
//            big = (big ? bgTmp : bgTmp2);

				bIgnoreFunctions = !big;
				bgTmp  = _Eval (ppszExp, iLevel);
				if (!mthEat (ppszExp, ":"))
					{
					bIgnoreFunctions = FALSE;
					return mthSetErr (10, *ppszExp);
					}
				bIgnoreFunctions = !!big;
				bgTmp2 = _Eval (ppszExp, 0);
				big = (big ? bgTmp : bgTmp2);
				bIgnoreFunctions = FALSE;
				break;

			case OP_COMA:
				bgTmp = _Eval (ppszExp, iLevel); 
				break;
			default : return big;
			}
		}
	}


/*
 * looks for a string of the form:
 *
 *  set fnName = string
 *       or
 *  set fnName() = string
 *
 */
BOOL MthTestForFn (PSZ psz)
	{
	CHAR szName[60];
	PSZ  p;

	if (!(psz = StrSkipBy (psz, " \t")))
		return FALSE;

	if (strnicmp (psz, "set ", 4))
		return FALSE;

	if (!(psz = StrSkipBy (psz + 4, " \t")))
		return FALSE;

	for (p=szName; psz && isalnum (*psz); psz++)
		*p++ = *psz;
	*p = '\0';

	if (!(psz = StrSkipTo (psz, "=")))
		return FALSE;

	MthAddFunction2 (szName, psz+1);
	return TRUE;
	}



BIG AToBIG (PSZ psz)
	{
	int c;
	PSZ p;
	BIG bgTmp;

	if (!pTREE)
		mthInitMath ();

	pLASTVAR = NULL;
	EVALERR  = 0;
	p        = psz;

	if (!p || *p == '\0' || *p == '\x0A' || *p == '\x0D')
		mthSetErr (4, p);
	else
		{
		if (MthTestForFn (p))
			return 0.0;

		bgTmp = _Eval (&p, 0);

		if ((c = SkipWhite (&p)) && c != '\x0D' && c!= '\x0A')
			mthSetErr (3, p);

//      if (*p && !strchr (" \t\x0D\x0A", *p))
//         mthSetErr (3, p);
		}
	EVALERRIDX = (EVALERR && EVALERRPOS ? EVALERRPOS - psz : 0);

	if (EVALERR)
		MthInvalidate (&bgTmp);

	return bgTmp;
	}


}