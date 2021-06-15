/*
 * GnuType.h
 *
 * (C) 1995 Info Tech Inc.
 *
 * Craig Fitzgerald
 *
 * 
 * This header should always be included first by programs using gnulib
 * This header defines user types and defines based on platform
 * If compiling for OS/2, this header will include the appropriate OS/2 headers
 *
 * GNULIB assumes you are compiling for dos only executables, unless
 * you first define __OS2__
 *
 ****************************************************************************
 */

#if !defined (GNUTYPE_INCLUDED)
#define GNUTYPE_INCLUDED
#pragma once

// when building a gnulib dll, define GNULIB_BUILD_DLL
// when using gnulib as a dll, define GNULIB_DLL (for now)
// when using gnulib in a lib, do nothing special
//
#if defined GNULIB_DLL
   #define EXP __declspec(dllexport)
#elif defined GNULIB_BUILD_DLL
   #define EXP __declspec(dllimport)
#else
   #define EXP
#endif


//extern "C"
//{

/*
 * defines to help support different compilers: choices are:
 *
 * VER_NT    and VER_NTW     if NT    using Watcom compiler
 * VER_NT    and VER_NTM     if NT    using Microsoft C/C++ 5.0
 * VER_DOS16 and VER_DOS16W  if DOS16 using Watcom compiler
 * VER_DOS16 and VER_DOS16M  if DOS16 using Microsoft C/C++ 1.52c
 * VER_DOS32 and VER_DOS32W  if DOS32 using Watcom compiler
 *
 * also
 * _WATCOMC_  will always be defined for watcom compiler
 * _MSC_VER   will always be defined for msc compilers
 */
#if defined (_MSC_VER)
   #if (_MSC_VER < 1000)
      #define VER_DOS
      #define VER_DOSM
      #define VER_DOS16
      #define VER_DOS16M
      #define  _cdecl  
   #else
      #define VER_NT
      #define VER_NTM
      #define  _cdecl
   #endif           
#endif


//All this crap is defined so windows.h doesn't include
//stuff that we don't need for gnulib
#define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // VK_*
#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS      // SM_*
#define NOMENUS           // MF_*
#define NOICONS           // IDI_*
#define NOKEYSTATES       // MK_*
#define NOSYSCOMMANDS     // SC_*
#define NORASTEROPS       // Binary and Tertiary raster ops
#define NOSHOWWINDOW      // SW_*
#define OEMRESOURCE       // OEM Resource values
#define NOATOM            // Atom Manager routines
#define NOCLIPBOARD       // Clipboard routines
#define NOCOLOR           // Screen colors
#define NOCTLMGR          // Control and Dialog routines
#define NODRAWTEXT        // DrawText() and DT_*
#define NOGDI             // All GDI defines and routines
#define NOKERNEL          // All KERNEL defines and routines
#define NOUSER            // All USER defines and routines
#define NONLS             // All NLS defines and routines
#define NOMB              // MB_* and MessageBox()
#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE        // typedef METAFILEPICT
#define NOMINMAX          // Macros min(a,b) and max(a,b)
#define NOMSG             // typedef MSG and associated routines
#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL          // SB_* and scrolling routines
#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND           // Sound driver routines
#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#define NOWH              // SetWindowsHook and WH_*
#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#define NOCOMM            // COMM driver routines
#define NOKANJI           // Kanji support stuff.
#define NOHELP            // Help engine interface.
#define NOPROFILER        // Profiler interface.
#define NODEFERWINDOWPOS  // DeferWindowPos routines
#define NOMCX             // Modem Configuration Extensions
#define VC_EXTRALEAN		  // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#define setc(coord,y,x) (coord.X=x, coord.Y=y)
#define setr(rect,t,l,b,r) (rect.Top=t,rect.Left=l,rect.Bottom=b,rect.Right=r)

#if !defined (MAKEULONG)
#define MAKEULONG(l, h) ((ULONG)(((USHORT)(l)) | ((ULONG)((USHORT)(h)))<<16))
#define MAKEUSHORT(l, h) (((USHORT)(l)) | ((USHORT)(h)) << 8)
#define MAKESHORT(l, h)  ((INT)MAKEUSHORT(l, h))
#define LOUCHAR(w)	((UCHAR)(UINT)(w))
#define HIUCHAR(w)	((UCHAR)(((UINT)(w) >> 8) & 0xff))
#define LOUSHORT(l)	((USHORT)(ULONG)(l))
#define HIUSHORT(l)	((USHORT)(((ULONG)(l) >> 16) & 0xffff))
#define MakeByte(Lnib, Hnib) ((BYTE)((BYTE)((Hnib) << 4) | (BYTE)(Lnib)))
#endif

#if !defined (MAKEDWORD)
#define MAKEDWORD(l, h) ((DWORD)(((WORD)(l)) | ((DWORD)((WORD)(h)))<<16))
// already defined by windows
//#define MAKEWORD (l, h) ((WORD)(((BYTE)(l)) | ((WORD)((BYTE)(h)))<<8))
//#define LOWORD
//#define HIWORD
//#define LOBYTE
//#define HIBYTE
#endif


#if !defined (MK_FP)
#define MK_FP(seg,ofs) ((void far *)(((unsigned long)(seg)<<16)|(unsigned)(ofs)))
#endif // !defined (MK_FP)

#if !defined (poke)
#define poke(a,b,c)   (*((int  far*)MK_FP((a),(b))) = (int)(c))
#define pokeb(a,b,c)  (*((CHAR far*)MK_FP((a),(b))) = (CHAR)(c))
#define peek(a,b)     (*((int  far*)MK_FP((a),(b))))
#define peekb(a,b)    (*((CHAR far*)MK_FP((a),(b))))
#endif  // ifndef poke

#define HARDERROR_DISABLE 0
#define HARDERROR_ENABLE  1

#define FILE_NORMAL	  0x0000
#define FILE_READONLY  0x0001
#define FILE_HIDDEN	  0x0002
#define FILE_SYSTEM	  0x0004
#define FILE_LABEL	  0x0008
#define FILE_DIRECTORY 0x0010
#define FILE_ARCHIVED  0x0020

#define FILE_1DOTDIR   0x0040
#define FILE_2DOTDIR   0x0080

typedef double        BIG;
typedef BIG           *PBIG;
typedef PSZ           *PPSZ;

//H} // extern C

#endif // GNUTYPE_INCLUDED

