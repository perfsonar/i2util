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
 *	                Jeff W. Boote
 *			Internet2
 *
 *	Date:		Sun Jun  2 12:19:22 MDT 2002
 *
 *	Description:	
 */

#ifndef	_i2_random_h_
#define	_i2_random_h_

#include <I2util/util.h>
#include <stdio.h>
#include <sys/types.h>

#include <stdarg.h>

/*
** Types of randomness sources.
*/
#define	I2RAND_UNINITIALIZED	0
#define	I2RAND_DEV	1
#define	I2RAND_EGD	2

BEGIN_C_DECLS

typedef struct I2RandomSourceRec	*I2RandomSource;

extern I2RandomSource
I2RandomSourceInit(
	I2ErrHandle	eh,
	int		type,
	void*		data
	);

extern int
I2RandomBytes(
	I2RandomSource	src,
	unsigned char	*ptr,
	unsigned int	count
	);

extern void
I2RandomSourceClose(
	I2RandomSource	src
	);

END_C_DECLS

#endif	/* _i2_random_h_	*/
