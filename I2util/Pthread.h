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
 *	File:		Pthread.h
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 10:17:50  2002
 *
 *	Description:	
 */
#ifndef	_I2Pthread_H_
#define	_I2Pthread_H_
#include <I2util/util.h>

BEGIN_C_DECLS

#ifndef	I2THREADS_ENABLE

typedef	int	I2Thread_T;
typedef	int	I2ThreadAttr_T;
typedef	int	I2ThreadMutex_T;
typedef	int	I2ThreadMutexattr_T;
typedef	int	I2ThreadCond_T;
typedef	int	I2ThreadCondattr_T;

#define	I2PTHREAD_MUTEX_INITIALIZER	0

#define I2ThreadCreate(thread,attr,start,arg,msg)	(void)0
#define	I2ThreadJoin(thread,retval,msg)			(void)0
#define	I2ThreadSelf()					(void)0
#define	I2ThreadMutexInit(mutex,attr)			(void)0
#define	I2ThreadMutexLock(mutex)			(void)0
#define	I2ThreadMutexUnlock(mutex)			(void)0
#define	I2ThreadAttrInit(attr)				(void)0
#define	I2ThreadAttrSetDetatchState(attr,detach)	(void)0
#define	I2ThreadCondDestroy(cond)			(void)0
#define	I2ThreadMutexDestroy(mutex)			(void)0
#define I2ThreadCondInit(cond,attr)			(void)0
#define	I2ThreadCondSignal(cond)			(void)0
#define	I2ThreadCondWait(cond,mutex)			(void)0

#else	/*	THREADS_ENABLE	*/

#include <pthread.h>

typedef	pthread_t		I2Thread_T;
typedef	pthread_attr_t		I2ThreadAttr_T;
typedef	pthread_mutex_t		I2ThreadMutex_T;
typedef	pthread_mutexattr_t	I2ThreadMutexattr_T;
typedef	pthread_cond_t		I2ThreadCond_T;
typedef	pthread_condattr_t	I2ThreadCondattr_T;

#define	I2PTHREAD_MUTEX_INITIALIZER	PTHREAD_MUTEX_INITIALIZER

extern	int I2ThreadCreate(
	I2Thread_T *thread,
	I2ThreadAttr_T *attr,
        void *(*start)(void *),
	void *arg,
	const char *msg
);

extern int I2ThreadJoin(
	I2Thread_T	thread,
	void		**retval,
	const char	*msg
);

extern I2Thread_T I2ThreadSelf();

extern int	I2ThreadMutexInit(
	I2ThreadMutex_T	*mutex,
	I2ThreadMutexattr_T	*attr
);

extern int	I2ThreadMutexLock(
	I2ThreadMutex_T		*mutex
);

extern int	I2ThreadMutexUnlock(
	I2ThreadMutex_T		*mutex
);

extern int	I2ThreadAttrInit(I2ThreadAttr_T	*attr);

extern int	I2ThreadAttrSetDetatchState(
	I2ThreadAttr_T *attr,
	int		detach
);


extern int	I2ThreadCondDestroy(
	I2ThreadCond_T	*cond
);

extern int	I2ThreadMutexDestroy(
	I2ThreadMutex_T	*mutex
);

extern int	I2ThreadCondInit(
	I2ThreadCond_T		*cond,
	I2ThreadCondattr_T	*attr
);
	

extern int	I2ThreadCondSignal(
	I2ThreadCond_T	*cond
);

extern int	I2ThreadCondWait(
	I2ThreadCond_T	*cond,
	I2ThreadMutex_T	*mutex
);

#endif	/* I2THREADS_ENABLE	*/

END_C_DECLS

#endif
