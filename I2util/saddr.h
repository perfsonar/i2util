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
 *	Date:		Tue Nov 26 11:09:26 MST 2002
 *
 *	Description:	
 */
#ifndef	_i2_saddr_h_
#define	_i2_saddr_h_

#include <I2util/util.h>
#include <sys/socket.h>

BEGIN_C_DECLS

extern int
I2SockAddrEqual(
	const struct sockaddr	*sa1,
	socklen_t		sa1_len,
	const struct sockaddr	*sa2,
	socklen_t		sa2_len
	);

END_C_DECLS
#endif	/*	_i2_saddr_h_	*/
