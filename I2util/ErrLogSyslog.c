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
 *	File:		ErrLogSyslog.c
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 11:05:18  2002
 *
 *	Description:	This file defines a "syslog-mode" logging
 *			function to be used as the `log_func' argument
 *			to the I2OpenErr() function.
 *
 *			Modified from code writen by John Clyne at UCAR...
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <I2util/util.h>

/*
 * Function:	I2ErrLogSyslog()
 *
 * Description:	The I2ErrLogSyslog() function is a client logging function
 *		that may be passed to DPOpenErr() as the `log_func' argument.
 *
 *		Upon invocation, I2ErrLogSyslog() writes a logging message
 *		to the system log via  a call to syslog(). The behavior of 
 *		I2ErrLogSyslog()
 *		may be controlled by the structure pointed to by `arg'. The
 *		members of the structure pointed to by `arg' that may be
 *		set include:
 *
 *
 *			*ident		A string passed as the first parameter
 *					to openlog() the first time 
 *					I2ErrLogSys() is called iff ident 
 *					is not NULL.
 *
 *			logopt		An integer passed as the second
 *					parameter to openlog iff ident
 *					is not NULL.
 *
 *			facility	An integer passed as the third
 *					parameter to openlog iff ident is
 *					not NULL.
 *
 *			priority	An integer log priority passed to 
 *					syslog as its first argument.
 *	
 *			line_info	A bit mask indicating how each output
 *					line should be formatted. The mask
 *					is a bitwise inclusive OR of the
 *					valid attribute bits. If line_info
 *					is zero nothing is printed.
 *
 *		Valid attribute mask bits include:
 *
 *			I2NAME	
 *			I2FILE
 *			I2LINE
 *			I2DATE
 *			I2RTIME
 *			I2MSG
 *
 *		If I2NAME is set `program_name', followed by a ":" is 
 *		copied to `arg->fp'.
 *
 *		If I2FILE is set the string "FILE=", followed by 
 *		`file', followed by ",", followed by a space is copied 
 *		to `arg->fp'.
 *
 *		If I2LINE is set the string "LINE=", followed by the ascii
 *		representation of `line', followed by ",", followed by 
 *		a space is copied to `arg->fp'.
 *
 *		If I2DATE is set the string "DATE=", followed by 
 *		`date', followed by ",", followed by a space is copied 
 *		to `arg->fp'.
 *
 *		If I2RTIME is set the string "RTIME=", followed by 
 *		`time', followed by ",", followed by a space is copied 
 *		to `arg->fp', where `time' is string formatted by
 *		arg->tformat.
 *	
 *		If I2MSG is set `msg' is copied to `arg->fp.
 *
 *
 * In Args:
 *
 *	*arg	A pointer to a I2ErrLogSyslogAttr structure.
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */
void	I2ErrLogSyslog(
	const char	*program_name,
	const char	*file, 	
	int 		line,
	const char	*date,
	const char	*msg,
	void		*arg,
	void		**data	/* not used	*/
) {
	I2ErrLogSyslogAttr	*sa = (I2ErrLogSyslogAttr *) arg;
	char			buf[4096], *bufptr;
	size_t			size=sizeof(buf);
	int			rc;
	
	bufptr = buf;

	if (*data == NULL && sa->ident) {
		openlog((char *) sa->ident, sa->logopt, sa->facility);
		*data = (void *) 1;
	}

	if (sa->line_info & I2NAME) {
		rc = snprintf(bufptr,size,"%s: ", program_name);
		bufptr += rc;
		size -= rc;
	}

	if (sa->line_info & I2FILE) {
		rc = snprintf(bufptr,size,"FILE=%s, ", file);
		bufptr += rc;
		size -= rc;
	}

	if (sa->line_info & I2LINE) {
		rc = snprintf(bufptr,size,"LINE=%d, ", line);
		bufptr += rc;
		size -= rc;
	}

	if (sa->line_info & I2DATE) {
		rc = snprintf(bufptr,size,"DATE=%s, ", date);
		bufptr += rc;
		size -= rc;
	}

	if (sa->line_info & I2RTIME) {
		time_t		clock;
		struct tm	*tm;
		char		ftime[64];

		time(&clock);
		tm = localtime(&clock);
		if( strftime(ftime,sizeof(ftime),sa->tformat,tm)){
			rc = snprintf(bufptr,size,"RTIME=%s, ",ftime);
			bufptr += rc;
			size -= rc;
		}
	}

	if (sa->line_info & I2MSG) {
		rc = DPSprintf(bufptr, "%s", msg);
		bufptr += rc;
	}

	if (bufptr != buf) {
		syslog(sa->priority, "%s", buf);
	}
}
