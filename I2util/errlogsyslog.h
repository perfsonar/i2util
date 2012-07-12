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
 *      Copyright 2012, University Corporation for Atmospheric Research.
 *
 *      This software may be used, subject to the terms of OSI's BSD-2 Clause
 *      License located at  http://www.opensource.org/licenses/bsd-license.php/
 *
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

extern I2Boolean	I2ErrLogSyslogReset(
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
