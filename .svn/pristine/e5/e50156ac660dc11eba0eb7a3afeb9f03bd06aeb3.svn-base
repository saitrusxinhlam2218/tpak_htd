 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  semops.h; Rel: 10.1.2.3; Get date: 5/24/95 at 14:42:28
 * @(#)  Last delta: 5/24/95 at 14:42:23
 * @(#)  SCCS File: /taxi/sccs/s.semops.h
 *                                                                           *
 *****************************************************************************/
/*
 *	semop.h	- Declares the functions in the semaphore API
 *
 *
 *      Author: Mats Lfstrm, init ab, mla@init.se
 */
#if !defined(_SEMOPS_H_)
#define	_SEMOPS_H_
#if defined(__STDC__)
int SetReadLock(int);
int ReleaseReadLock(int);
int SetWriteLock(int);
int ReleaseWriteLock(int);
#else
int SetReadLock();
int ReleaseReadLock();
int SetWriteLock();
int ReleaseWriteLock();
#endif
#endif
