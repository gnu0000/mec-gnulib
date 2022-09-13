/*
 * GnuKbd.h
 *
 * (C) 1992-1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * This file provides keyboard util functions
 *
 ****************************************************************************
 *
 *  QUICK REF FUNCTION INDEX
 *  ========================
 *
 * BOOL KeyAddTrap (iKey, PKEYFN pfnTrap)
 *
 * INT  k_getch (void)
 * INT  k_ungetch (c)
 * INT  k_kbhit (void)
 * INT  KeyGet (bUpper)
 * INT  KeyChoose (pszKeyList, pszExtKeyList)
 * void KeyClearBuff (void)
 *
 * INT  KeyEditCell (psz, dwPos, iMax, iCmd)
 * void KeyEditCellMode (pszNormalReturns, pszExtendedReturns, bUppCase)
 * BOOL KeyAddEditTrap (iKey, pfnTrap)
 *
 * INT  KeyRecord (psz, iSize)
 * INT  KeyPlay (psz, iSize)  
 * INT  KeyStop ()
 * INT  KeyInfo (ppsz, piSize, piIdx)
 * void KeyPlayDelay (iPlayDelay)
 *
 * INT  AutoRecordTrapFn  (c)    // mapped to Ctl-F1 by default
 * INT  AutoPlayTrapFn    (c)    // mapped to Ctl-F2 by default
 * INT  SaveKeyBuffTrapFn (c)    // not mapped by default
 * INT  LoadKeyBuffTrapFn (c)    // not mapped by default
 *
 ****************************************************************************
 */


#if !defined (GNUKBD_INCLUDED)
#define GNUKBD_INCLUDED

#if !defined (GNUTYPE_INCLUDED)
#include <GnuType.h>
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/*
 * typedef for fn for the 
 * KeyAddTrap and KeyAddEditTrap functions
 * The fn is called with the keypress as the parameter.
 *
 * When used with KeyAddTrap.  A non zero return is used as 
 * the next keypress
 *
 * When used with KeyAddEditTrap.  A non zero return is used as 
 * a signal to return from the EditCell function. 
 */
typedef INT (*PKEYFN) (INT c);


/*
 * This function associates a keystroke with a function
 * The user fn will get called whenever the matching key is pressed
 * extended keys should be 0x100 + 2nd byte
 * This will work as long as you use the fn's in this module for
 * your keyboard interface.
 * If the user fn returns a non zero value, that value is pushed
 * into the key buffer
 */
EXP BOOL KeyAddTrap (INT iKey, PKEYFN pfnTrap);


/*************************************************************************/
/*                                                                       */
/* Basic Key functions                                                   */
/*                                                                       */
/*************************************************************************/


/*
 * Replacements for std c equivalents.
 * All functions in this module use these fn's 
 * If you use these fn's you get the benefit of recording,
 * playbacks, debug breaks, and any other traps that are enabled.
 */
EXP INT k_getch (void);
EXP INT k_ungetch (INT c);
EXP INT k_kbhit (void);


/*
 * If 2-byte key, returns 0x100 + 2nd CHAR
 * if bUpper = TRUE, Converts key to uppercase
 */
EXP INT KeyGet (BOOL bUpper);


/*
 * This fn gets a key in pszKeyList and returns it
 * This fn first clears the keyboard buffer
 * if pszKeyList contains letters, they must be upper-case
 * This fn beeps on incorrect entrys
 * pszExtKeyList is 2nd byte of 2-byte keys
 */
EXP INT KeyChoose (PSZ pszKeyList, PSZ pszExtKeyList);


/*
 * This fn clears the keyboard buffer
 */
EXP void KeyClearBuff (void);


/*************************************************************************/
/*                                                                       */
/* Key Record/Play API                                                   */
/*                                                                       */
/*************************************************************************/

/*
 * Call this fn to start recording keystrokes
 * psz is the buffer to record keys to.
 */
EXP INT KeyRecord (PSZ psz, INT iSize);


/*
 * Call this fn to play a string of keystrokes as if they were 
 * entered from the keyboard. psz can come from KeyRecord or
 * from a string you make.
 */
EXP INT KeyPlay (PSZ psz, INT iSize);


/*
 * call this to stop a key record or playback
 * The return value is the count of keys recorded/played
 */
EXP INT KeyStop (void);


/*
 * This fn returns info about the current record buffer
 * including its buffer ptr, size, and index.
 */
EXP INT KeyInfo (PSZ *ppsz, PINT piSize, PINT piIdx);


/*
 * This sets the Playback delay in milliseconds
 */
EXP void KeyPlayDelay (INT iPlayDelay);





/*************************************************************************/
/*                                                                       */
/* Edit Cell functions                                                   */
/*                                                                       */
/*************************************************************************/


/*
 * String entry fn
 * This fn provides full editing capabilities.
 * Current screen attributes are used
 *
 * pszNormalReturns and pszExtendedReturns are strings telling which
 * keys to exit on. pszExtendedReturns is for 2-byte keys.
 *
 * uCmd options
 * ------------
 *  0 ..... Clear psz, edit
 *  1 ..... edit psz, start cursor at left
 *  2 ..... edit psz, start cursor at right
 *  other . Clear psz, process uCmd as 1st key
 *
 * This fn returns the key that caused the exit. If it was one of
 * the pszExtendedReturns keys, 0x100 is added to its 2nd byte
 *
 * If the user presses <Esc> this fn returns 0 and doesn't modify psz
 */
EXP INT KeyEditCell (PSZ   psz,    // starting string (and return)
                     COORD dwPos,  // starting pos
                     INT   iMax,   // max size of edit field
                     INT   iCmd);  // how to start

/*
 * Set EditCell mode variables:
 * pszNormalReturns   = list of normal keys to return on   (NULL=no change)
 * pszExtendedReturns = list of extended keys to return on (NULL=no change)
 * bUppCase           = 0 or 1 force keys to uppercase     ( >1 =no change)
 *
 * call with (NULL, NULL, 2) to reset all three to default values
 * call with (NULL, NULL, 3) to enable password entry mode
 * call with (NULL, NULL, 4) to disable password entry mode
 */
EXP void KeyEditCellMode (PSZ  pszNormalReturns,
                          PSZ  pszExtendedReturns,
                          BOOL bUppCase);



/*
 * This function associates a keystroke with a functions
 * while in the EditCell procedure.
 * The user fn will get called whenever the matching key is pressed
 * extended keys should be 0x100 + 2nd byte
 * If the user fn returns a non zero value, Edit cell will terminate
 * with that value. (exception: return of 27(esc) returns 0(abort))
 *
 */
EXP BOOL KeyAddEditTrap (INT iKey, PKEYFN pfnTrap);


/*
 * Special key values usable with KeyAddEditTrap 
 * 
 * 
 */ 
#define EC_DEFAULT  0xF000
#define EC_START    0xF001
#define EC_OKEND    0xF002
#define EC_ABORTEND 0xF003
#define EC_EACHKEY  0xF004


EXP INT AutoRecordTrapFn  (INT c);
EXP INT AutoPlayTrapFn    (INT c);
EXP INT SaveKeyBuffTrapFn (INT c);
EXP INT LoadKeyBuffTrapFn (INT c);




/*
 * Variables available to user functions installed with KeyAddEditTrap
 *
 */
extern EXP BOOL EC_bUPPCASE;     // hhmmmm
                               
/*--- These set via parameters to EditCell ---*/
extern EXP PSZ        ec_psz;          // passed in string buffer
extern EXP COORD      ec_dwPOS;        // start position passed in
extern EXP INT        ec_iMAX;         // passed max edit size 
extern EXP INT        ec_iCMD;         // passed how to start

/*--- Working variables in EditCell ---*/
extern EXP CHAR       ec_szTMP [256];     // used by KeyEditCell
extern EXP INT        ec_iLEN;         // current str len
extern EXP INT        ec_iPOS;         // current cursor pos
extern EXP CHAR_INFO  ec_cCELL;        // base screen, a space and a screen attribute
extern EXP CHAR       ec_cKEYCHAR;     // password protection char


/*
 * Default installed functions 
 *
 */
EXP INT ec_left    (INT c); /*--- 0x14B ---*/
EXP INT ec_right   (INT c); /*--- 0x14D ---*/
EXP INT ec_del     (INT c); /*--- 0x153 ---*/
EXP INT ec_ins     (INT c); /*--- 0x152 ---*/
EXP INT ec_home    (INT c); /*--- 0x147 ---*/
EXP INT ec_end     (INT c); /*--- 0x14F ---*/
EXP INT ec_bksp    (INT c); /*--- 8     ---*/
EXP INT ec_esc     (INT c); /*--- 27    ---*/
EXP INT ec_default (INT c); /*--- EC_DEFAULT  ---*/
EXP INT ec_start   (INT c); /*--- EC_START    ---*/
EXP INT ec_AbortEnd(INT c); /*--- EC_OKEND    ---*/
EXP INT ec_OKEnd   (INT c); /*--- EC_ABORTEND ---*/
EXP INT ec_EachKey (INT c); /*--- EC_EACHKEY  ---*/




// keypad keys 
//-------------------
#define K_UP    0x148
#define K_DOWN  0x150
#define K_LEFT  0x14b
#define K_RIGHT 0x14d
#define K_PGUP  0x149
#define K_PGDN  0x151
#define K_HOME  0x147
#define K_END   0x14f
#define K_PAD5  0x14c
#define K_INS   0x152
#define K_DEL   0x153
#define K_BSP   0x08
#define K_TAB   0x09
#define K_STAB  0x10f
#define K_ESC   0x1b
#define K_RET   0x0d
#define K_ENTER 0x0d


// ctrl-keypad keys 
//-------------------
#define K_CUP    0x18d
#define K_CDOWN  0x191
#define K_CLEFT  0x173
#define K_CRIGHT 0x174
#define K_CPGUP  0x184
#define K_CPGDN  0x176
#define K_CHOME  0x177
#define K_CEND   0x175
#define K_CPAD5  0x18f
#define K_CINS   0x192
#define K_CDEL   0x193
#define K_CBSP   0x07f
#define K_CTAB   0x194
#define K_CRET   0x00a


// misc ctrl keys 
//-------------------
#define K_C2         0x103
#define K_C6         0x01e
#define K_CMINUS     0x01f
#define K_CBACKSLASH 0x01c
#define K_CLBRACKET  0x01b
#define K_CRBRACKET  0x01d


// ctrl-char keys (not really needed)
//-------------------
#define K_CA    0x01
#define K_CB    0x02
#define K_CC    0x03
#define K_CD    0x04
#define K_CE    0x05
#define K_CF    0x06
#define K_CG    0x07
#define K_CH    0x08
#define K_CI    0x09
#define K_CJ    0x0a
#define K_CK    0x0b
#define K_CL    0x0c
#define K_CM    0x0d
#define K_CN    0x0e
#define K_CO    0x0f
#define K_CP    0x10
#define K_CQ    0x11
#define K_CR    0x12
#define K_CS    0x13
#define K_CT    0x14
#define K_CU    0x15
#define K_CV    0x16
#define K_CW    0x17
#define K_CX    0x18
#define K_CY    0x19
#define K_CZ    0x1a


// alt-char keys 
//-------------------
#define K_AA    0x11e
#define K_AB    0x130
#define K_AC    0x12e
#define K_AD    0x120
#define K_AE    0x112
#define K_AF    0x121
#define K_AG    0x122
#define K_AH    0x123
#define K_AI    0x117
#define K_AJ    0x124
#define K_AK    0x125
#define K_AL    0x126
#define K_AM    0x132
#define K_AN    0x131
#define K_AO    0x118
#define K_AP    0x119
#define K_AQ    0x110
#define K_AR    0x113
#define K_AS    0x11f
#define K_AT    0x114
#define K_AU    0x116
#define K_AV    0x12f
#define K_AW    0x111
#define K_AX    0x12d
#define K_AY    0x115
#define K_AZ    0x12c


// alt-digit keys 
//-------------------
#define K_A1    0x178
#define K_A2    0x179
#define K_A3    0x17a
#define K_A4    0x17b
#define K_A5    0x17c
#define K_A6    0x17d
#define K_A7    0x17e
#define K_A8    0x17f
#define K_A9    0x180
#define K_A0    0x181


// misc alt keys 
//-------------------
#define K_ABACKQUOTE 0x129
#define K_AMINUS     0x182
#define K_APLUS      0x183
#define K_ABACKSLASH 0x12b
#define K_ALBRACKET  0x11a
#define K_ARBRACKET  0x11b
#define K_ASEMICOLON 0x127
#define K_AQUOTE     0x128
#define K_ACOMMA     0x133
#define K_APERIOD    0x134
#define K_ASLASH     0x135
#define K_APADSLASH  0x1A4
#define K_APADMINUS  0x14a
#define K_APADPLUS   0x14e
#define K_APADENTER  0x1a6


// fn keys 
//-------------------
#define K_F1    0x13b
#define K_F2    0x13c
#define K_F3    0x13d
#define K_F4    0x13e
#define K_F5    0x13f
#define K_F6    0x140
#define K_F7    0x141
#define K_F8    0x142
#define K_F9    0x143
#define K_F10   0x144
#define K_F11   0x185
#define K_F12   0x186


// shift-fn keys 
//-------------------
#define K_SF1   0x154
#define K_SF2   0x155
#define K_SF3   0x156
#define K_SF4   0x157
#define K_SF5   0x158
#define K_SF6   0x159
#define K_SF7   0x15a
#define K_SF8   0x15b
#define K_SF9   0x15c
#define K_SF10  0x15d
#define K_SF11  0x187
#define K_SF12  0x188


// ctrl-fn keys 
//-------------------
#define K_CF1   0x15e
#define K_CF2   0x15f
#define K_CF3   0x160
#define K_CF4   0x161
#define K_CF5   0x162
#define K_CF6   0x163
#define K_CF7   0x164
#define K_CF8   0x165
#define K_CF9   0x166
#define K_CF10  0x167
#define K_CF11  0x189
#define K_CF12  0x18a


// alt-fn keys 
//-------------------
#define K_AF1   0x168
#define K_AF2   0x169
#define K_AF3   0x16a
#define K_AF4   0x16b
#define K_AF5   0x16c
#define K_AF6   0x16d
#define K_AF7   0x16e
#define K_AF8   0x16f
#define K_AF9   0x170
#define K_AF10  0x171
#define K_AF11  0x18b
#define K_AF12  0x18c


#ifdef __cplusplus
}
#endif

#endif  // GNUKBD_INCLUDED

