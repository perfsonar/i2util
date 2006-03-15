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
 *	File:		mach_dep.c
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Wed Apr 24 11:36:34  2002
 *
 *	Description:	This module attempts to isolate all machine
 *			dependencies from platform to platform.
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
#include <stdarg.h>
#include <string.h>
#include <errno.h>

static I2ThreadMutex_T MyMutex = I2PTHREAD_MUTEX_INITIALIZER;

/*
** Function:	I2GetSysErrList()
**
** Description:	Return the list of system error messages associated with
**		`errno'. The contents of the list returned by this function
**		should be copied into applicaiton memory.
**
** In Args:
**
**	count		A pointer to an integer.
**
** Out Args:
**
**	count		Pointer to an integer indicating the number of
**			entries in the list returned by I2GetSysErrList()
**
** Return Values:
**
**			I2GetSysErrList() returns an array of character
**			strings.
**
** Side Effects:
*/
char	**I2GetSysErrList(
	int	*count
) {
	/*
	 *	The ANSI Standard does not define the globals `sys_nerr'
	 *	and `sys_errlist'. Sigh. Hence there is no real way to 
	 *	determine the range valid values of `errno'.
	 *	Guess large and loop calling strerror...
	 *	NUM_ERRORS should be the "largest" errno number for all the
	 *	systems we care about.
	 */
#ifndef	SYSNERRWORKS
#define	NUM_ERRORS	152
	const int	sys_nerr = NUM_ERRORS;
	static char	*sys_errlist[NUM_ERRORS];
	static int 		first = 1;

	int		i;

	/*
	**	Build the `sys_errlist' for Solaris systems. We only need
	**	to do this once since the table is immutable
	*/
	I2ThreadMutexLock(&MyMutex);

	if (first) {
		for(i=0; i<sys_nerr; i++) {
			sys_errlist[i] = (char *) strdup(strerror(i));
		}
		first = 0;
	}

	I2ThreadMutexUnlock(&MyMutex);

#endif

	*count = sys_nerr;	
	return(sys_errlist);
}
