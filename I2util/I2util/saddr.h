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
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */
#ifndef	_i2_saddr_h_
#define	_i2_saddr_h_

#include <I2util/util.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


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

/*
 * This union is used to remove questionable (struct sockaddr*) type punning.
 *
 * If the socket API is ported to the host OS properly - the alignment of
 * all the elements should match between these at least sa and (sin or sin6)
 * - but I believe this is more C99 compliant.
 *
 * This union needs to contain each sockaddr_XXX type that is used
 * in the saddr.c and addr.c functions.
 */
typedef union _I2SockUnion {
    struct sockaddr         sa;
    struct sockaddr_in      sin;
    struct sockaddr_in6     sin6;
    struct sockaddr_storage sas;
} I2SockUnion;

/*
 * I2SockAddrToSockUnion
 *  This function is used to allow all other functions to deal with
 *  sockaddrs through a union therefore removing 'aliasing' issues
 *  introduced with C99.
 */
extern I2SockUnion *
I2SockAddrToSockUnion(
        const struct sockaddr   *sa,
        socklen_t               sa_len,
        I2SockUnion             *sau_mem
        );

/*
 * I2SockUnionToSockAddr
 *  This function is used to return a sockaddr union as a simple sockaddr
 *  when returning the content to the user.
 */
extern struct sockaddr *
I2SockUnionToSockAddr(
        const I2SockUnion   *sau,
        socklen_t           *sa_len_in_out,
        struct sockaddr     *sa_mem
        );

END_C_DECLS
#endif	/*	_i2_saddr_h_	*/
