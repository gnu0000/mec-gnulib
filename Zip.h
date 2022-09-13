/*
 * Zip.h
 *
 * Internal header file
 * for GnuZip module
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

PSZ _cmpSetErrp (PCFILE pcfp, INT iErr);
INT _cmpSetErru (PCFILE pcfp, INT iErr);

INT LoadBuffer (PCFILE pcfp);

//extern uDEFTYPE  ;
//extern uDEFWINDOW;


#ifdef __cplusplus
}
#endif
