/*
 *      $Id$
 */
/************************************************************************
*									*
*			     Copyright (C)  2002			*
*				Internet2				*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		Pthread.c
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 14:01:41  2002
 *
 *	Description:	
 *
 *
 *		Based on code from UCAR DCS tools. Copyright information
 *		from UCAR follows:
 *
 *      Copyright 2012, University Corporation for Atmospheric Research.
 *
 *      This software may be used, subject to the terms of OSI's BSD-2 Clause
 *      License located at  http://www.opensource.org/licenses/bsd-license.php/
 */
#include "Pthread.h"
#include <stdio.h>

#ifdef	I2THREADS_ENABLE

int	I2ThreadCreate(
	I2Thread_T	*thread,
	I2ThreadAttr_T	*attr,
        void		*(*start)(void *),
	void		*arg,
	const char	*msg
) {
	int	rc;


	rc = pthread_create(thread, attr, start, arg);

#ifdef	DEBUG
	fprintf(
		stderr, "Created thread (%lx), rc=%d, msg=%s\n",
		(unsigned long) *thread, rc, msg
	);
#endif
	if (rc < 0) return(-1);
	return(0);
}


int I2ThreadJoin(
	I2Thread_T	thread,
	void		**retval,
	const char	*msg
) {
	int	rc;
	rc = pthread_join(thread, retval);
#ifdef	DEBUG
	fprintf(
		stderr, "Joined thread (%lx), rc=%d, msg=%s\n", 
		(unsigned long) thread, rc, msg
	);
#endif
	return(0);
}

I2Thread_T	I2ThreadSelf() 
{
	return(pthread_self());
}

int	I2ThreadMutexInit(
	I2ThreadMutex_T	*mutex,
	I2ThreadMutexattr_T	*attr
) {
	return(pthread_mutex_init(mutex, attr));
}

int	I2ThreadMutexLock(
	I2ThreadMutex_T		*mutex
) {
	return(pthread_mutex_lock(mutex));
}

int	I2ThreadMutexUnlock(
	I2ThreadMutex_T		*mutex
) {
	return(pthread_mutex_unlock(mutex));
}

int	I2ThreadAttrInit(
	I2ThreadAttr_T	*attr
) {
	return(pthread_attr_init(attr));
}

int	I2ThreadAttrSetDetatchState(
	I2ThreadAttr_T *attr,
	int		detach
) {
	return(pthread_attr_setdetachstate(attr, detach));
}

int	I2ThreadCondDestroy(
	I2ThreadCond_T	*cond
) {
	return(pthread_cond_destroy(cond));
}

int	I2ThreadMutexDestroy(
	I2ThreadMutex_T	*mutex
) {
	return(pthread_mutex_destroy(mutex));
}

int	I2ThreadCondInit(
	I2ThreadCond_T		*cond,
	I2ThreadCondattr_T	*attr
) {
	return(pthread_cond_init(cond, attr));
}
	

int	I2ThreadCondSignal(
	I2ThreadCond_T	*cond
) {
	return(pthread_cond_signal(cond));
}

int	I2ThreadCondWait(
	I2ThreadCond_T	*cond,
	I2ThreadMutex_T	*mutex
) {
	return(pthread_cond_wait(cond, mutex));
}


#else	/*	I2THREADS_ENABLE	*/
/*
 * Hopefully we don't need these - but if macro versions don't work
 * they are still here...
 */
#if	defined(NOT) && NOT
int I2ThreadCreate(
	I2Thread_T	*thread,
	I2ThreadAttr_T	*attr,
        void		*(*start)(void *),
	void		*arg,
	const char	*msg
) {
	*thread = (I2Thread_T)NULL;
	start(arg);
	return(0);
}

int I2ThreadJoin(
	I2Thread_T	thread,
	void		**retval,
	const char	*msg
) {
	return(0);
}

I2Thread_T	I2ThreadSelf() 
{
	return(0);
}

int	I2ThreadMutexInit(
	I2ThreadMutex_T	*mutex,
	I2ThreadMutexattr_T	*attr
) {
	return(0);
}

int	I2ThreadMutexLock(
	I2ThreadMutex_T		*mutex
) {
	return(0);
}

int	I2ThreadMutexUnlock(
	I2ThreadMutex_T		*mutex
) {
	return(0);
}

int	I2ThreadAttrInit(
	I2ThreadAttr_T	*attr
) {
	return(0);
}

int	I2ThreadAttrSetDetatchState(
	I2ThreadAttr_T *attr,
	int		detach
) {
	return(0);
}

int	I2ThreadCondDestroy(
	I2ThreadCond_T	*cond
) {
	return(0);
}

int	I2ThreadMutexDestroy(
	I2ThreadMutex_T	*mutex
) {
	return(0);
}

int	I2ThreadCondInit(
	I2ThreadCond_T		*cond,
	I2ThreadCondattr_T	*attr
) {
	return(0);
}
	

int	I2ThreadCondSignal(
	I2ThreadCond_T	*cond
) {
	return(0);
}

int	I2ThreadCondWait(
	I2ThreadCond_T	*cond,
	I2ThreadMutex_T	*mutex
) {
	return(0);
}
#endif	/* NOT */

#endif	
