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
 *	File:		io.h
 *
 *	Author:		Jeff W. Boote
 *			Internet2
 *
 *	Date:		Fri Aug 09 12:53:52 MDT 2002
 *
 *	Description:	
 */
#ifndef	_i2_io_h_
#define	_i2_io_h_

#include <fcntl.h>
#include <I2util/util.h>

/*
** Robust low-level IO functions - out of Stevens. Read or write
** the given number of bytes. Returns -1 on error. No short
** count is possible.
*/

/*
 * TODO: Add timeout values for read's and write's. We don't want to wait
 * as long as kernel defaults - timeout is specified in the context.
 */

BEGIN_C_DECLS

extern ssize_t			       /* Read "n" bytes from a descriptor. */
I2Readn(int fd, void *vptr, size_t n);

extern ssize_t				/* Write "n" bytes to a descriptor. */
I2Writen(int fd, const void *vptr, size_t n);

END_C_DECLS
#endif	/*	_i2_io_h_	*/
