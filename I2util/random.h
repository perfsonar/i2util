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
 *	File:		random.h
 *
 *	Author:		Anatoly Karp
 *			Internet2
 *
 *	Date:		Sun Jun  2 12:19:22 MDT 2002
 *
 *	Description:	
 */

#ifndef	_i2_random_h_
#define	_i2_random_h_

#include <stdio.h>
#include <sys/types.h>

#include <stdarg.h>
#include <I2util/util.h>

BEGIN_C_DECLS

extern void
I2RandomBytes(
	char	*ptr,
	int	count
	);

END_C_DECLS

#endif	/* _i2_random_h_	*/
