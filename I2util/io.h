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

BEGIN_C_DECLS

extern ssize_t
I2Readni(
	int	fd,
	void	*vptr,
	size_t	n,
	int	*retn_on_intr	/* pointer so it can be modified via signals */
	);

extern ssize_t
I2Readn(
	int	fd,
	void	*vptr,
	size_t	n
	);

extern ssize_t
I2Writeni(
	int		fd,
	const void	*vptr,
	size_t		n,
	int		*retn_on_intr
	);

extern ssize_t
I2Writen(
	int		fd,
	const void	*vptr,
	size_t		n
	);

END_C_DECLS
#endif	/*	_i2_io_h_	*/
