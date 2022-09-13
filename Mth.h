/*
 * Mth.h
 *
 * Part of GnuMath.h
 * Part of the GnuLib library
 *
 * (C) 1994 Info Tech Inc.
 *
 * Craig Fitzgerald
 * 
 * Internal header file for GnuMath
 */

#ifdef __cplusplus
extern "C"
{
#endif

#define STARTFNS  15
#define STARTFNS2 0
#define STARTCON  2
#define STARTVAR  2

#define ATOMLEVEL 13

#define OP_EXP   1
#define OP_MOD   2
#define OP_DIV   3
#define OP_MUL   4
#define OP_ADD   5
#define OP_SUB   6
#define OP_SRT   7
#define OP_SLT   8
#define OP_GT    9
#define OP_LT    10
#define OP_GE    11
#define OP_LE    12
#define OP_EQU   13
#define OP_NEQ   14
#define OP_BAND  15
#define OP_BOR   16
#define OP_AND   17
#define OP_OR    18
#define OP_COND  19
#define OP_SET   20
#define OP_SADD  21
#define OP_SSUB  22
#define OP_SMOD  23
#define OP_SDIV  24
#define OP_SMUL  25
#define OP_COMA  26


/*
 *
 */
#define MARK 0x12345678L


void mthInitMath (void);

#ifdef __cplusplus
}
#endif

