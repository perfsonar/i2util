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
 *      Copyright 2012, University Corporation for Atmospheric Research.
 *
 *      This software may be used, subject to the terms of OSI's BSD-2 Clause
 *      License located at  http://www.opensource.org/licenses/bsd-license.php/
 */
#include <I2util/utilP.h>
#include <I2util/mach_dep.h>

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
const char	* const *I2GetSysErrList(
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
#define	NUM_ERRORS	152
	const int	nerr = NUM_ERRORS;
	static char	*errlist[NUM_ERRORS];
	static int 	first = 1;

	int		i;

	/*
	**	Build the `sys_errlist' for this system. We only need
	**	to do this once since the table is immutable
	*/
	I2ThreadMutexLock(&MyMutex);

	if (first) {
		for(i=0; i<nerr; i++) {
			errlist[i] = (char *) strdup(strerror(i));
		}
		first = 0;
	}

	I2ThreadMutexUnlock(&MyMutex);

	*count = nerr;
	return((const char * const *)errlist);
}
