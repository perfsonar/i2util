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
 *	File:		errlog.h
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 14:21:24  2002
 *
 *	Description:
 *		Generic Error logging API.
 *
 *		Modified from code writen by John Clyne at UCAR...
 *
 *		Based on code from UCAR DCS tools. Copyright information
 *		from UCAR follows:
 *
 *		Copyright 1997 University Corporation for Atmospheric Research,
 *		Scientific Computing Division.  All rights reserved.
 *
 *			
 *		Permission to use, copy, modify and distribute this software
 *		and its	documentation for any academic, educational and
 *		scientific research purpose is hereby granted without fee,
 *		provided that the above copyright notice and this permission
 *		notice appear in all copies of this software and its
 *		documentation, and that the software is not sold and/or made
 *		the subject of any commercial activity.  Parties interested
 *		in commercial licensing should contact the copyright holder.
 */
#ifndef	_i2_errlog_h_
#define _i2_errlog_h_

#include <I2util/util.h>

#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <errno.h>

BEGIN_C_DECLS

/*
 * TODO: Verify that this is a portable constant to use for LOG_PERROR...
 */
#ifndef HAVE_SYSLOG_PERROR
#define LOG_PERROR 0x20
#endif

#define	I2NAME		(1L << 0)
#define	I2FILE		(1L << 1)
#define	I2LINE		(1L << 2)
#define	I2DATE		(1L << 3)
#define	I2RTIME		(1L << 4)
#define	I2MSG		(1L << 5)
#define	I2CODE		(1L << 6)
#define	I2LEVEL		(1L << 7)

/* basically used to disable syslog messages, but will work for others.    */
/* set as "priority" to disable *ALL* syslog messages */
#define I2LOG_NONE  8

/* Used as "no error" code. */
#define I2EUNKNOWN  0
/*
 * mask is bitwise OR of above bitmasks indicating which of the
 * remaining fields are valid.
 * (setting level to a negative number disables logging entirely.)
 */
struct I2ErrLogEvent{
	int		mask;
	const char	*name;	/* I2NAME */
	const char	*file;	/* I2FILE */
	int		line;	/* I2LINE */
	const char	*date;	/* I2DATE */
	int		code;	/* I2CODE */
	int		level;	/* I2LEVEL - matches Syslog priority */
	const char	*msg;	/* I2MSG */
};

typedef	void	*I2ErrHandle;

typedef void	(*I2ErrLogFuncPtr) (	/* client logging function	*/
	struct I2ErrLogEvent	*err_event,
	void			*arg,
	void			**data
	);

typedef I2Boolean	(*I2ErrLogResetFuncPtr)( /* reset logging function*/
	void	*arg,
	void	**data
	);

typedef	char	*(*I2ErrRetrieveFuncPtr)(      /* client fetch func    	*/
	void	*arg,
	void	**data
	);


/*
 * extern void I2ErrLog(eh,fmt,...);
 */
#define	I2ErrLog	I2ErrLocation_(__FILE__, __DATE__, __LINE__); \
				I2ErrLogFunction_
/*
 * extern void I2ErrLogP(eh,err_code,fmt,...);
 */
#define	I2ErrLogP	I2ErrLocation_(__FILE__, __DATE__, __LINE__); \
				I2ErrLogPFunction_

/*
 * extern void I2ErrLogT(eh,err_level,err_code,fmt,...);
 */
#define I2ErrLogT	I2ErrLocation_(__FILE__,__DATE__,__LINE__); \
				I2ErrLogTFunction_



extern I2ErrHandle	I2ErrOpen(
		const char		*program_name,
		I2ErrLogFuncPtr		log_func,
		void			*log_func_arg,
		I2ErrRetrieveFuncPtr	retrieve_func,
		void			*retrieve_func_arg
);

extern void	I2ErrSetResetFunc(
		I2ErrHandle		eh,
		I2ErrLogResetFuncPtr	reset_func
		);

/*
 * If the ErrReset function fails, this will return NULL - and the
 * ErrHandle will be invalid.
 */
extern I2ErrHandle	I2ErrReset(
		I2ErrHandle		eh
		);

extern void	I2ErrClose(I2ErrHandle dpeh);

extern void	I2ErrRep(
	I2ErrHandle	dpeh,
	FILE		*fp
);

extern char    *I2ErrGetMsg(
	I2ErrHandle	dpeh
);

extern int    I2ErrGetCode(
	I2ErrHandle	dpeh
);

extern void	I2ErrLocation_(
	const char	*file,
	const char	*date,
	int		line
);

/*
 * Function:    I2ErrLogVT
 *
 * Description:    
 *
 * In Args:    
 *  (setting level to a negative number disables logging entirely.)
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
extern void
I2ErrLogVT(
	I2ErrHandle	dpeh,
	int		level,
	int		code,
	const char	*format,
	va_list		ap
	);

extern void	I2ErrLogFunction_(
	I2ErrHandle	dpeh,
	const char	*format,
	...
);

extern void	I2ErrLogPFunction_(
	I2ErrHandle	dpeh,
	int		err_code,
	const char	*format,
	...
);

extern void	I2ErrLogTFunction_(
	I2ErrHandle	dpeh,
	int		priority,
	int		err_code,
	const char	*format,
	...
	);


extern int	I2ErrList(
	I2ErrHandle 	dpeh, 
	unsigned 	start, 
	unsigned 	num, 
	const char 	**err_list
);

END_C_DECLS

#include <I2util/errlogimmediate.h>
#include <I2util/errlogsyslog.h>

#endif	/*	_i2_errlog_h_	*/
