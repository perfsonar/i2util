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
static int         rand_type;

/*
** Initialize the source of random bytes. Possible types are:
** RAND_DEV - random device (eg, /dev/urandom or /dev/random) - <data>
** is then interpreted as char* pathname of the device.
** RAND_DAEMON - enthropy generating daemon (EGD) - <data> is
** then interpreted as struct sockaddr_un Unix domain socket address
** structire, ready to communicate with EGD. Returns 0 on success,
** or -1 on failure.
*/
int
I2RandomSourceInit(I2ErrHandle eh, int type, void* data)
{
	assert(eh);
	
	rand_type = type;
	rand_eh = eh;

	switch (type) {
	case RAND_DEV:
		if (fp) {
		   I2ErrLog(eh, "the random source has already been opened.");
		   return -1;
		}
	
		assert(data);

		if ((fp = fopen((char *)data, "rb")) == NULL) {
			I2ErrLog(eh, "I2randomBytes: fopen() failed");
			return -1;
		}
		break;
	case RAND_DAEMON:
		/*
		  XXX - currently un-supported.
		*/
		return -1;
		/* UNREACHED */
	default:
		I2ErrLog(eh, "I2randomBytes: unknown random source type");
		return -1;
		/* UNREACHED */
	}
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
	case RAND_DEV:
		if (fread(ptr, 1, count, fp) != (size_t)count) {
			I2ErrLog(rand_eh, "I2randomBytes: fread() failed: ", 
				 errno);
			return -1;
		}
		break;
	case RAND_DAEMON:
		/*
		  XXX - currently un-supported.
		*/
		return -1;
		/* UNREACHED */
	default:
		I2ErrLog(rand_eh, "I2randomBytes: unknown random source type");
		return -1;
		/* UNREACHED */
	}
	return 0;
}
