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
 *	File:		random.c
 *
 *	Author:		Anatoly Karp
 *	                Jeff W. Boote
 *			Internet2
 *
 *	Date:		Sun Jun 02 11:50:52 MDT 2002
 *
 *	Description:	
 */
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <I2util/util.h>

struct I2RandomSourceRec{
	I2ErrHandle	eh;
	int		type;
	int		fd;	/* used for I2RAND_DEV */
};

/*
** Initialize the source of random bytes. Possible types are:
** I2RAND_DEV - random device (eg, /dev/urandom or /dev/random) - <data>
** is then interpreted as char* pathname of the device.
** I2RAND_EGD - enthropy generating daemon (EGD) - <data> is
** then interpreted as char* pathname to the local socket the EGD daemon
** is listening on.
** Returns 0 on success, or -1 on failure.
*/
I2RandomSource
I2RandomSourceInit(I2ErrHandle eh, int type, void* data)
{
	I2RandomSource	rand_src;

	if(!eh)
		return NULL;

	if( !(rand_src = malloc(sizeof(struct I2RandomSourceRec)))){
		I2ErrLog(eh,"malloc():%M");
		return NULL;
	}
	rand_src->eh = eh;
	rand_src->type = type;

	switch (type) {
		case I2RAND_DEV:
	
			if(!data)
				data = I2_RANDOMDEV_PATH;
			if( (rand_src->fd = open((char *)data, O_RDONLY))<0){
				I2ErrLog(eh, "I2randomBytes:open():%M");
				return NULL;
			}
			break;
		case I2RAND_EGD:
			I2ErrLog(eh,
			"I2randomBytes: I2RAND_EGD not yet implemented");
			free(rand_src);
			return NULL;
			/* UNREACHED */
		default:
			I2ErrLog(eh,
			"I2randomBytes:unknown/unsupported random source type");
			free(rand_src);
			return NULL;
			/* UNREACHED */
	}

	return rand_src;
}

/*
** This function generates <count> many random bytes and
** places them in the location pointed to by <ptr>. It is
** a responsibility of the caller to have allocated
** sufficient space. Returns 0 on success, and -1 on failure.
*/
int
I2RandomBytes(
	I2RandomSource	src,
	unsigned char 	*ptr,
	int		count
	)
{
	if(!src)
		return -1;

	switch (src->type) {
		case I2RAND_DEV:
			if (I2Readn(src->fd, ptr, count) != count) {
				I2ErrLog(src->eh,
					"I2randomBytes: I2Readn() failed: %M");
				return -1;
			}
			break;
		case I2RAND_EGD:
		default:
			/* UNREACHED */
			I2ErrLog(src->eh,
		"I2randomBytes: unknown/unsupported random source type");
			return -1;
	}

	return 0;
}

void
I2RandomSourceClose(
	I2RandomSource	src
	)
{
	if(!src)
		return;

	switch (src->type) {
		case I2RAND_DEV:
			close(src->fd);
			break;
		case I2RAND_EGD:
		default:
			/* UNREACHED */
			I2ErrLog(src->eh,
		"I2randomBytes: unknown/unsupported random source type");
	}

	free(src);

	return;
}

