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

#include <stdio.h>
#include <I2util/util.h>
#include <I2util/errlogimmediate.h>
#include <I2util/errlogsyslog.h>


BEGIN_C_DECLS


#define	I2NAME		(1L << 0)
#define	I2FILE		(1L << 1)
#define	I2LINE		(1L << 2)
#define	I2DATE		(1L << 3)
#define	I2RTIME		(1L << 4)
#define	I2MSG		(1L << 5)

typedef	void	*I2ErrHandle;

typedef void	(*I2ErrLogFuncPtr) (	/* client logging function	*/
	const char	*program_name,
	const char      *file,  
	int             line,
	const char      *date,  
	const char      *msg,
	void            *arg,
	void		**data
	);

typedef	char	*(*I2ErrRetrieveFuncPtr)(      /* client fetch func    	*/
	void	*arg,
	void	**data
	);

#define	I2ErrLog	I2ErrLocation_(__FILE__, __DATE__, __LINE__); \
				I2ErrLogFunction_
#define	I2ErrLogP	I2ErrLocation_(__FILE__, __DATE__, __LINE__); \
				I2ErrLogPFunction_




extern I2ErrHandle	I2ErrOpen(
		const char		*program_name,
		I2ErrLogFuncPtr		log_func,
		void			*log_func_arg,
		I2ErrRetrieveFuncPtr	retrieve_func,
		void			*retrieve_func_arg
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

extern void	I2ErrLogFunction_(
	I2ErrHandle	dpeh,
	const char	*format, ...
);

extern void	I2ErrLogPFunction_(
	I2ErrHandle	dpeh,
	int		err_code,
	const char	*format, ...
);


extern int	I2ErrList(
	I2ErrHandle 	dpeh, 
	unsigned 	start, 
	unsigned 	num, 
	const char 	**err_list
);

END_C_DECLS

#endif	/*	_i2_errlog_h_	*/
