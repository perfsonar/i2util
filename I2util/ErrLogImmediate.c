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
 *	File:		ErrLogImmediate.c
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 10:27:23  2002
 *
 *	Description:	This file defines an "immediate-mode" logging
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
#include <I2util/util.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Function:	I2ErrLogImmediate()
 *
 * Description:	The I2ErrLogImmediate() function is a client logging function
 *		that may be passed to I2OpenErr() as the `log_func' argument.
 *
 *		Upon invocation, I2ErrLogImmediate() writes a logging message
 *		via a call to fprintf(). The behavior of I2ErrLogImmediate()
 *		may be controlled by the structure pointed to by `arg'. The
 *		members of the structure pointed to by `arg' that may be
 *		set include:
 *
 *			*fp		A file pointer passed to be passed to 
 *					fprintf()
 *
 *			line_info	A bit mask indicating how each output
 *					line should be formatted. The mask
 *					is a bitwise inclusive OR of the
 *					valid attribute bits. If line_info
 *					is zero nothing is printed.
 *
 *			tformat		A format string as defined by
 *					strftime used to format the local
 *					time if the `line_info' attribute
 *					I2RTIME is set.
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
 *		If I2NAME is set `ev->name', followed by a ":" is 
 *		copied to `arg->fp'.
 *
 *		If I2FILE is set the string "FILE=", followed by 
 *		`ev->file', followed by ",", followed by a space is copied 
 *		to `arg->fp'.
 *
 *		If I2LINE is set the string "LINE=", followed by the ascii
 *		representation of `ev->line', followed by ",", followed by 
 *		a space is copied to `arg->fp'.
 *
 *		If I2DATE is set the string "DATE=", followed by 
 *		`ev->date', followed by ",", followed by a space is copied 
 *		to `arg->fp'.
 *
 *		If I2RTIME is set the string "RTIME=", followed by 
 *		`time', followed by ",", followed by a space is copied 
 *		to `arg->fp', where `time' is string formatted by
 *		arg->tformat.
 *	
 *		If I2MSG is set `ev->msg' is copied to `arg->fp.
 *
 *		Finally, a trailing newline is copied to `arg->fp'
 *
 *
 * In Args:
 *
 *	*arg	A pointer to a I2LogImmediateAttr structure.
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */
void	I2ErrLogImmediate(
	struct I2ErrLogEvent	*ev,
	void			*arg,
	void			**data	__attribute__((unused))
) {
	I2LogImmediateAttr	*la = (I2LogImmediateAttr *) arg;
	FILE			*fp = la->fp;

	if(! fp) return;

	if(ev->mask & la->line_info & I2NAME)
		fprintf(fp, "%s: ", ev->name);
	if(ev->mask & la->line_info & I2FILE)
		fprintf(fp, "FILE=%s, ", ev->file);
	if(ev->mask & la->line_info & I2LINE)
		fprintf(fp, "LINE=%d, ", ev->line);
	if(ev->mask & la->line_info & I2DATE)
		fprintf(fp, "DATE=%s, ", ev->date);

	if(la->line_info & I2RTIME){
		time_t		clock;
		struct tm	*tm;
		char		ftime[64];

		time(&clock);
		tm = localtime(&clock);
		if( strftime(ftime,sizeof(ftime),la->tformat,tm))
			fprintf(fp, "RTIME=%s, ", ftime);
	}

	if(ev->mask & la->line_info & I2MSG)
		fprintf(fp, "%s", ev->msg);

	if(la->line_info)
		fprintf(fp, "\n");
}
