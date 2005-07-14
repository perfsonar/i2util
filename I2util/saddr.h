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
 *	File:		saddr.h
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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <I2util/util.h>

BEGIN_C_DECLS

#define	I2SADDR_ADDR	(0x1)
#define	I2SADDR_PORT	(I2SADDR_ADDR<<1)
#define	I2SADDR_ALL	(I2SADDR_ADDR|I2SADDR_PORT)

/*
 * I2SockAddrEqual
 * 	This function is used to compare to sockaddrs. chk_what is used
 * 	to determine which parts of the sockaddr to look at when comparing.
 * 	(The above #define's are used to indicate this.)
 *
 * returns:
 * >0: true
 * ==0: false
 * <0: error
 */
extern int
I2SockAddrEqual(
	const struct sockaddr	*sa1,
	socklen_t		sa1_len,
	const struct sockaddr	*sa2,
	socklen_t		sa2_len,
	uint32_t		chk_what
	);

/*
 * I2SockAddrIsLoopback
 * 	This function is used to determine if the given sockaddr is
 * 	a loopback. It currently supports IPv4/IPv6.
 *
 * returns:
 * >0: true
 * ==0: false
 * <0: error
 */
extern int
I2SockAddrIsLoopback(
	const struct sockaddr	*sa1,
	socklen_t		sa1_len
		);

END_C_DECLS
#endif	/*	_i2_saddr_h_	*/
