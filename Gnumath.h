/*
 * GnuMath.h
 *
 * (C) 1992-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file math functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * BIG  AToBIG (psz)
 *
 * BOOL MthValid (bgNum)
 * void MthInvalidate (pbgNum)
 * INT  MthMatch (pbg1, pbg2)
 * INT  MthIsError (*ppszErrStr, *puErrIdx)
 *
 * BIG  MthAddFunction  (pszName, pfn)
 * BIG  MthAddFunction2 (pszName, pszExpr)
 * BIG  MthAddVariable  (pszName, big)
 * BIG  MthAddConstant  (pszName, big)
 * BOOL MthTestForFn (psz)
 * BIG  MthGetVariable (PSZ pszName)
 * void MthClearAllVariables ();
 *
 * PSZ  MthFmat (pszNum, bgNum, iWhole, iDec, bCommas, bMoney)
 * PSZ  MthFmat (pszNum, bgNum, iWhole, iDec, bCommas, pszMoneySymbol)
 * PSZ  MthFmat2(pszNum, bgNum, iWhole, iDec, bCommas)
 *
 * void MthPercentIsMod (b)
 *
 * PMNODE FindNode (pszName, bCreateAll)
 * PMNODE GetNode (ppsz)
 * BIG    MthSetDirect (pmnode, bgVal)
 * BIG    VarVal (pmnode)
 *
 ****************************************************************************
 */

#if !defined (GNUMATH_INCLUDED)
#define GNUMATH_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif



typedef BIG (*PMTHFN) (BIG big);

/*
 * Don't look!
 */
union _fcvopt
   {
   PMTHFN pfn;             // fn ptr fn
   PSZ    psz;             // string fn
   BIG    big;             // const or var
   };

/*
 * Don't look!
 */
typedef struct _fcv
   {
   PSZ    pszName;         // name of thing
   CHAR   cType;           // F=fn, S=string fn, V=var C=const
   union  _fcvopt node;    // data ptr
   struct _fcv    *left;   //
   struct _fcv    *right;  //
   } MNODE;
typedef MNODE *PMNODE;


/*
 * This function converts a string containing a number or
 * expression into a BIG value.  On error the number
 * returned is marked as invalid.  This fn can handle just
 * about any expression you can dream up including std math
 * functions like sinh() and log()
 * the constants pi and e are also defined
 * you can also create/define variables, see below
 *
 * initial valid functions are:
 *       abs()    acos()   asin()   atan()   ceil()
 *       cos()    cosh()   exp()    floor()  log()
 *       log10()  sin()    sinh()   sqrt()   tan()
 *
 * operators in precidence order:
 *
 *      += /= =  %= *= -=  -- assignment
 *      ?                  -- ternary operator
 *      ||                 -- logical or
 *      &&                 -- logical and
 *      |                  -- bitwose or
 *      &                  -- bitwise and
 *      == !=              -- boolean comparison
 *      >= > <= <          -- boolean comparison
 *      << >>              -- base 10 shift
 *      +  -               -- add,subtract
 *      /  %  *            -- div,mod,mult
 *      ^                  -- power
 *      ()                 -- parenthesis
 *
 *
 * examples:
 * ---------
 *     psz: 10.5
 *  result: 10.5
 * comment: normal _atol like functionality
 *
 *     psz: 100 + .9 *(exp(1.2) / sin(pi/3)) % 20
 *  result: whatever
 * comment: % means mod.
 *
 *     psz: log (0)
 *  result: invalid number
 * comment: see error functions
 *
 *     psz: @x = 500
 *  result: 500
 * comment: @x is created and given the value 500. vars must start with @
 *
 *     psz: 100 + @x
 *  result: 600
 * comment: @x was created in the previous example
 *
 *     psz: 100 + @y
 *  result: 100
 * comment: @y was created. vars are initialized to 0
 */
EXP BIG AToBIG (PSZ psz);



/*
 * This tells if bgNum has been marked as invalid with MthInvalidate
 */
EXP BOOL MthValid (BIG bgNum);


/*
 * This marks a BIG number as invalid
 */
EXP BIG MthInvalidate (PBIG pbgNum);


/*
 * This returns a number saying how close the numbers are to
 * each other. The returns are:
 *
 * return   meaning
 * ----------------------------
 *  0 -     one or more invalid
 *  1 -     both valid and matched to within .000005
 *  2 -     both valid and matched to within .005
 *  3 -     both valid but not matched
 */
EXP INT MthMatch (PBIG pbg1, PBIG pbg2);



/*
 * This fn formats a number and returns it as a string
 * if iWhole=0, any sized number is printed, using e notation if necessary
 * if bMoney is true and number is neg.  Accounting format (parens) is used
 *
 */
EXP PSZ MthFmat (PSZ  pszNum,  // Return String
                 BIG  bgNum,   // Number
                 INT  iWhole,  // # of digits before decimal
                 INT  iDec,    // # of digits after decimal
                 BOOL bCommas, // use commas ?
                 BOOL bMoney); // use leading $ ?

/*
 * This fn formats a number and returns it as a string
 * if iWhole=0, any sized number is printed, using e notation if necessary
 * if pszMoneySymbol is not null and number is neg.  Accounting format (parens) is used
 */
PSZ MthFmat (PSZ  pszNum,          // Return String
				 BIG  bgNum,           // Number
				 INT  iWhole,          // # of digits before decimal
				 INT  iDec,            // # of digits after decimal
				 BOOL bCommas,         // use commas ?
				 PSZ  pszMoneySymbol); // use leading currency Symbol ?


/*
 * a little bit more generic
 */
EXP PSZ MthFmat2 (PSZ  pszNum,  // Return String
                  BIG  bgNum,   // Number
                  INT  iWhole,  // # of digits before decimal
                  INT  iDec,    // # of digits after decimal
                  BOOL bCommas);// use commas ?



/*
 *
 * After each call to AToBig, this fn stores the error state
 * This fn returns a non zero value in case of an error
 *
 * *ppszErrStr will contain a description of the error
 * *puErrIdx will contain the index of the offending CHAR
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
EXP INT MthIsError (PSZ *ppszErrStr, PINT piErrIdx);


/*
 * Adds a function that AToBIG can use
 * the function must be of type PMTHFN
 *
 * call with pfn = NULL to remove fn
 */
EXP BOOL MthAddFunction (PSZ pszName, PMTHFN pfn);


/*
 * Adds a function that AToBIG can use
 * the function is a string (ie: @@ * @@ / @x * pi )
 * there the variable @@ is the fn's parameter value
 *
 * call with pfn = NULL to remove fn
 *
 * functions can also by added via AToBIG and MthTestForFn like this:
 *
 *    AToBIG (&bg, "set MyFun() = @@ * sin (@@)");
 *
 */
EXP BOOL MthAddFunction2 (PSZ pszName, PSZ pszExpr);


/*
 * adds a constant. Like This:
 *
 *  MthAddConstant ("pi", 3.1415926);
 */
EXP BOOL MthAddConstant (PSZ pszName, BIG big);


/*
 * adds a variable. Prefix with a '@' Like This:
 *
 *  MthAddConstant ("@num", 212.0);
 */
EXP BOOL MthAddVariable (PSZ pszName, BIG big);


/*
 * checks to see if str is of the form:
 *    set Fn = @@ * 22
 * if so, it adds the function and returns true
 * This fn is automatically called by AToBIG
 */
EXP BOOL MthTestForFn (PSZ psz);


EXP BIG MthGetVariable (PSZ pszName);


EXP void MthClearAllVariables (void);


/*
 * Free all nodes allocated by the math module
 */
EXP void MthFreeMath (void);


/*
 *  by default, % is treaded as a modulus operator
 *  if you call this fn with a FALSE value,
 *  % is treated as a percent operator.
 */
EXP void MthPercentIsMod (BOOL b);



/*
 * Ways to change the value of a variable:
 *
 * way 1>  AToBIG (&bg, "@width = 10");
 *
 * way 2>  MthAddVariable ("@width", 10.0);
 *
 * way 3>  pmnode = FindNode ("@width", 0)
 *         MthSetDirect (pmnode, 10.0)
 *
 * way 1 is the easiest, 
 * way 3 is the fastest.
 * way 2 is as fast as way 3 of your only doing it once
 *
 * set a variable via it's internal index
 *
 *
 * these 4 fns are not normally needed
 *
 */
EXP PMNODE FindNode (PSZ pszName, BOOL bCreateAll);

EXP PMNODE GetNode (PPSZ ppsz);

EXP BIG MthSetDirect (PMNODE pmnode, BIG bgVal);

EXP BIG VarVal (PMNODE pmnode);


#endif  // GNUMATH_INCLUDED

