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
#include <I2util/config.h>
#ifdef  HAVE_SYSLOG_NAMES
#define SYSLOG_NAMES
#endif
#include <syslog.h>

#include <I2util/util.h>
/*
 * I prefer to get these arrays from syslog.h... But, they doesn't exist on
 * all systems. I do what I can.
 *
 * If it isn't in the system's syslog.h - I have taken the portion I need
 * from freebsd.
 */
#ifndef	HAVE_SYSLOG_NAMES
/*
 * Copyright (c) 1982, 1986, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)syslog.h	8.1 (Berkeley) 6/2/93
 */
typedef struct _code {
	const char	*c_name;
	int		c_val;
} CODE;

static CODE prioritynames[] = {
	{ "alert",	LOG_ALERT,	},
	{ "crit",	LOG_CRIT,	},
	{ "debug",	LOG_DEBUG,	},
	{ "emerg",	LOG_EMERG,	},
	{ "err",	LOG_ERR,	},
	{ "error",	LOG_ERR,	},	/* DEPRECATED */
	{ "info",	LOG_INFO,	},
	{ "notice",	LOG_NOTICE,	},
	{ "panic", 	LOG_EMERG,	},	/* DEPRECATED */
	{ "warn",	LOG_WARNING,	},	/* DEPRECATED */
	{ "warning",	LOG_WARNING,	},
	{ NULL,		-1,		}
};

#ifndef LOG_AUTHPRIV
#define LOG_AUTHPRIV LOG_AUTH
#endif
#ifndef LOG_FTP
#define LOG_FTP LOG_DAEMON
#endif

static CODE facilitynames[] = {
	{ "auth",	LOG_AUTH,	},
	{ "authpriv",	LOG_AUTHPRIV,	},
	{ "cron", 	LOG_CRON,	},
	{ "daemon",	LOG_DAEMON,	},
	{ "ftp",	LOG_FTP,	},
	{ "kern",	LOG_KERN,	},
	{ "lpr",	LOG_LPR,	},
	{ "mail",	LOG_MAIL,	},
	{ "news",	LOG_NEWS,	},
	{ "syslog",	LOG_SYSLOG,	},
	{ "user",	LOG_USER,	},
	{ "uucp",	LOG_UUCP,	},
	{ "local0",	LOG_LOCAL0,	},
	{ "local1",	LOG_LOCAL1,	},
	{ "local2",	LOG_LOCAL2,	},
	{ "local3",	LOG_LOCAL3,	},
	{ "local4",	LOG_LOCAL4,	},
	{ "local5",	LOG_LOCAL5,	},
	{ "local6",	LOG_LOCAL6,	},
	{ "local7",	LOG_LOCAL7,	},
	{ NULL,		-1,		}
};
#endif

/*
 * Function:	I2ErrLogSyslogFacility
 *
 * Description:	Given a string name, looks for the integer id that matches.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	integer id : -1 on error.
 * Side Effect:	
 */
int
I2ErrLogSyslogFacility(
	const char	*name
	)
{
	CODE	*ptr = facilitynames;
	int	val=-1;

	while(ptr->c_name){
		if(strncasecmp(ptr->c_name,name,strlen(ptr->c_name)) == 0){
			val = ptr->c_val;
			break;
		}
		ptr++;
	}

	return val;
}

/*
 * Function:	I2ErrLogSyslogFacilityName
 *
 * Description:	Given an integer, return a "name" for the facility that
 * 		matches that integer.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	const char * (static memory) or NULL
 * Side Effect:	
 */
const char
*I2ErrLogSyslogFacilityName(
	int	fac
		)
{
	CODE	*ptr = facilitynames;

	while(ptr->c_name){
		if(ptr->c_val == fac)
			return ptr->c_name;
		ptr++;
	}

	return NULL;
}

/*
 * Function:	I2ErrLogSyslogPriority
 *
 * Description:	Given a string name, looks for the integer id that matches.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	integer id : -1 on error.
 * Side Effect:	
 */
int
I2ErrLogSyslogPriority(
	const char	*name
	)
{
	CODE	*ptr = prioritynames;
	int	val=-1;

        if(strncasecmp(name,"none",5) == 0){
            val = I2LOG_NONE;
        }
        else{
	    while(ptr->c_name){
		if(strncasecmp(ptr->c_name,name,strlen(ptr->c_name)) == 0){
			val = ptr->c_val;
                        break;
		}
		ptr++;
	    }
        }

	return val;
}

/*
 * Function:	I2ErrLogSyslogPriorityName
 *
 * Description:	Given an integer, return a "name" for the "priority" that
 * 		matches that integer.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	const char * (static memory) or NULL
 * Side Effect:	
 */
const char
*I2ErrLogSyslogPriorityName(
	int	fac
		)
{
	CODE	*ptr = prioritynames;
        static char *none = "none";

        if(fac == I2LOG_NONE){
            return none;
        }

	while(ptr->c_name){
		if(ptr->c_val == fac)
			return ptr->c_name;
		ptr++;
	}

	return NULL;
}


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
	struct I2ErrLogEvent	*ev,
	void			*arg,
	void			**data
)
{
	I2ErrLogSyslogAttr	*sa = (I2ErrLogSyslogAttr *) arg;
	char			buf[4096], *bufptr;
	size_t			size=sizeof(buf);
	int			rc;
        int                     prio;
	
	bufptr = buf;

	if(*data == NULL && sa->ident){
		openlog((char *) sa->ident, sa->logopt, sa->facility);
		*data = (void *) 1;
	}

	if(ev->mask & sa->line_info & I2NAME) {
		rc = snprintf(bufptr,size,"%s: ", ev->name);
		bufptr += rc;
		size -= rc;
	}

	if(ev->mask & sa->line_info & I2FILE){
		rc = snprintf(bufptr,size,"FILE=%s, ", ev->file);
		bufptr += rc;
		size -= rc;
	}

	if(ev->mask & sa->line_info & I2LINE){
		rc = snprintf(bufptr,size,"LINE=%d, ", ev->line);
		bufptr += rc;
		size -= rc;
	}

	if(ev->mask & sa->line_info & I2DATE){
		rc = snprintf(bufptr,size,"DATE=%s, ", ev->date);
		bufptr += rc;
		size -= rc;
	}

	if(sa->line_info & I2RTIME){
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

	if(ev->mask & sa->line_info & I2MSG){
		rc = sprintf(bufptr, "%s", ev->msg);
		bufptr += rc;
	}

	if(bufptr == buf)
		return;

	if(ev->mask & I2LEVEL)
            prio = ev->level;
	else
            prio = sa->priority;

        if(prio != I2LOG_NONE)
	    syslog(prio, "%s", buf);

#ifndef HAVE_SYSLOG_PERROR
        if(sa->logopt | LOG_PERROR){
            fprintf(stderr,"%s\n", buf);
        }
#endif

        return;
}

/*
 * Function:	I2ErrResetSyslog
 *
 * Description:	
 * 		Used to reset syslog error logging. Useful in case of
 * 		forking etc...
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
I2Boolean	I2ErrLogSyslogReset(
		void	*arg __attribute__((unused)),
		void	**data
		)
{
	closelog();
	*data = NULL;

	return True;
}

