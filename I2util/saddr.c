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
 *	File:		saddr.c
 *
 *	Author:		Jeff W. Boote
 *			Internet2
 *
 *	Date:		Tue Nov 26 07:45:48 MST 2002
 *
 *	Description:	
 *
 *	Generic socket functions used here to abstract away addr family
 *	differences.
 */
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

/*
 * Function:	I2CmpSockAddr
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * 	<0	: error/unsupported addr family
 * 	0	: false
 * 	>0	: true
 * Side Effect:	
 */
int
I2SockAddrEqual(
	const struct sockaddr	*sa1,
	socklen_t		sa1_len,
	const struct sockaddr	*sa2,
	socklen_t		sa2_len
	)
{
	if(sa1_len != sa2_len)
		return 0;
	if(sa1->sa_family != sa2->sa_family)
		return 0;

	switch(sa1->sa_family){
#ifdef	AF_INET6
	case AF_INET6:
		if(memcmp(&((struct sockaddr_in6*)sa1)->sin6_addr,
				&((struct sockaddr_in6*)sa2)->sin6_addr,
				sizeof(struct in6_addr)) != 0)
			return 0;

		if( ((struct sockaddr_in6*)sa1)->sin6_port !=
				((struct sockaddr_in6*)sa2)->sin6_port)
			return 0;

		/*
		 * TODO:Do I need to check scope? Won't for now...
		 */
		return 1;

		break;
#endif
	case AF_INET:
		if(memcmp(&((struct sockaddr_in*)sa1)->sin_addr,
				&((struct sockaddr_in*)sa2)->sin_addr,
				sizeof(struct in_addr)) != 0)
			return 0;

		if( ((struct sockaddr_in*)sa1)->sin_port !=
				((struct sockaddr_in*)sa2)->sin_port)
			return 0;

		return 1;

		break;

	default:
		return -1;
	}

	return -1;
}
