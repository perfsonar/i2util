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
 *			Internet2
 *
 *	Date:		Sun Jun 02 11:50:52 MDT 2002
 *
 *	Description:	
 */
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <I2util/util.h>

static I2ErrHandle rand_eh;
static FILE*       fp = NULL;
static int         rand_type = I2RAND_UNINITIALIZED;

/*
** Initialize the source of random bytes. Possible types are:
** I2RAND_DEV - random device (eg, /dev/urandom or /dev/random) - <data>
** is then interpreted as char* pathname of the device.
** I2RAND_EGD - enthropy generating daemon (EGD) - <data> is
** then interpreted as char* pathname to the local socket the EGD daemon
** is listening on.
** Returns 0 on success, or -1 on failure.
*/
int
I2RandomSourceInit(I2ErrHandle eh, int type, void* data)
{
	if(rand_type != I2RAND_UNINITIALIZED){
		I2ErrLog(rand_eh,
				"the random source has already been opened.");
		return -1;
	}
	if(!eh)
		return -1;
	

	switch (type) {
	case I2RAND_DEV:

		if(!data)
			data = I2_RANDOMDEV_PATH;
		if ((fp = fopen((char *)data, "rb")) == NULL) {
			I2ErrLog(eh, "I2randomBytes: fopen() failed");
			return -1;
		}
		break;
	case I2RAND_EGD:
	default:
		I2ErrLog(eh, "I2randomBytes: unknown/unsupported random source type");
		return -1;
		/* UNREACHED */
	}

	rand_type = type;
	rand_eh = eh;

	return 0;
}

/*
** This function generates <count> many random bytes and
** places them in the location pointed to by <ptr>. It is
** a responsibility of the caller to have allocated
** sufficient space. Returns 0 on success, and -1 on failure.
*/
int
I2RandomBytes(unsigned char *ptr, int count)
{
	switch (rand_type) {
	case I2RAND_DEV:
		if (fread(ptr, 1, count, fp) != (size_t)count) {
			I2ErrLog(rand_eh, "I2randomBytes: fread() failed: %M");
			return -1;
		}
		break;
	case I2RAND_EGD:
	default:
		I2ErrLog(rand_eh, "I2randomBytes: unknown/unsupported random source type");
		return -1;
		/* UNREACHED */
	}

	return 0;
}
