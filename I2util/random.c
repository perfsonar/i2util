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

#define OWP_RAND_DEV_PATH "/dev/urandom"

/*
** This function generates <count> many random bytes and
** places them in the location pointed to by <ptr>. It is
** a responsibility of the caller to have allocated
** sufficient space. Returns 0 on success, and -1 on failure.
*/
int
I2RandomBytes(unsigned char *ptr, int count)
{
	static FILE* fp = NULL;

	if (!fp) {
		if ((fp = fopen(OWP_RAND_DEV_PATH, "rb")) == NULL) {
			perror("I2randomBytes: fopen() failed");
			return -1;
		}
	}

	/* Interpet short count as error. */
	if (fread(ptr, 1, count, fp) != (size_t)count)
		return -1;

	return 0;
}
