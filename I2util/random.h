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

/*
** Types of randomness sources.
*/
#define	I2RAND_UNINITIALIZED	0
#define	I2RAND_DEV	1
#define	I2RAND_EGD	2

/*
 * This macro should be set by the autoconf-<I2util/config.h> stuff.
 *
 * (This macro will be used to set the "data" of I2RandomSourceInit if
 * type is I2RAND_DEV.)
 *
 */
#ifndef	I2_RANDOMDEV_PATH
#error	"I2RandomBytes currently requires kernel support (EGD support forthcoming)"
#endif

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
	int		count
	);

extern void
I2RandomSourceClose(
	I2RandomSource	src
	);

END_C_DECLS

#endif	/* _i2_random_h_	*/
