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

/*
** This function generates <count> many random bytes and
** places them in the location pointed to by <ptr>. It is
** a responsibility of the caller to have allocated
** sufficient space.
*/
void
I2RandomBytes(char *ptr, int count)
{
	int i;
	long scale = (RAND_MAX / 1<<8);
	for (i = 0; i < count; i++)
		*(u_int8_t *)(ptr+i) = random()/scale; 
}
