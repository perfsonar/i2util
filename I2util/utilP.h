/*
 *      $Id$
 */
/************************************************************************
*									*
*			     Copyright (C)  2007			*
*				Internet2				*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		utilP.h
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Fri Mar 30 16:33:03 MDT 2007
 *
 *	Description:	
 *			I2 Utility library private header. (Public header
 *			can't include autoconf stuff.)
 */
#ifndef	_I2_utilP_h_
#define	_I2_utilP_h_

#include <I2util/config.h>
#include <I2util/util.h>

#ifndef	HAVE___ATTRIBUTE__
#define	__attribute__(x)
#endif

/*
 * Portablility sanity checkes.
 */
#if    !HAVE_GETADDRINFO || !HAVE_SOCKET
#error    Missing needed networking capabilities! (getaddrinfo and socket)
#endif

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

#endif	/*	_I2_utilP_h_	*/
