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
 *	File:		errlogsyslog.h
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 14:35:40  2002
 *
 *	Description:
 *		Modified from code writen by John Clyne at UCAR...
 *
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
#ifndef	_i2_errlogsyslog_h_
#define	_i2_errlogsyslog_h_

BEGIN_C_DECLS

typedef struct  I2ErrLogSyslogAttr_ {
	const char	*ident;
	int		logopt;
	int		facility;
	int		priority;
	unsigned long	line_info;
	const char	*tformat;
	} I2ErrLogSyslogAttr;

extern void	I2ErrLogSyslog(
	struct I2ErrLogEvent	*ev,
	void			*arg,
	void			**data
);

extern void	I2ErrLogSyslogReset(
	void		*arg,
	void		**data
	);

extern int	I2ErrLogSyslogFacility(
	const char	*name
	);

const char *I2ErrLogSyslogFacilityName(
		int	fac
		);

extern int	I2ErrLogSyslogPriority(
	const char	*name
	);

const char *I2ErrLogSyslogPriorityName(
		int	prio
		);

END_C_DECLS

#endif	/* _i2_errlogsyslog_h_	*/
