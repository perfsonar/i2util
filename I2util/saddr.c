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

#include <I2util/saddr.h>

/*
 * Function:	I2SockAddrEqual
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
	socklen_t		sa2_len,
	u_int32_t		chk_what
	)
{
	/*
	 * If the lengths are not equal - or the families are not the
	 * same - check if the v6 address is really an encoded v4 address.
	 * If it is - then re-call ourselves with the v4 version directly.
	 */
	if((sa1_len != sa2_len) || (sa1->sa_family != sa2->sa_family)){
#ifdef	AF_INET6
		struct sockaddr_in6	*v6addr;
		struct sockaddr_in	v4rec;
		struct sockaddr_in	*v4addr = &v4rec;

		/*
		 * check if sa1 is a mapped addr.
		 */
		v6addr = (struct sockaddr_in6*)sa1;
		if((sa1->sa_family==AF_INET6) &&
				IN6_IS_ADDR_V4MAPPED(&v6addr->sin6_addr)){
			memset(v4addr,0,sizeof(*v4addr));
#ifdef	HAVE_STRUCT_SOCKADDR_SA_LEN
			v4addr->sin_len = sizeof(*v4addr);
#endif
			v4addr->sin_family = AF_INET;
			memcpy(&v4addr->sin_addr.s_addr,
					&v6addr->sin6_addr.s6_addr[12],4);
			return I2SockAddrEqual((struct sockaddr*)v4addr,
					sizeof(*v4addr),
					sa2,sa2_len,chk_what);
		}

		v6addr = (struct sockaddr_in6*)sa2;
		if((sa2->sa_family==AF_INET6) &&
				IN6_IS_ADDR_V4MAPPED(&v6addr->sin6_addr)){
			memset(v4addr,0,sizeof(*v4addr));
#ifdef	HAVE_STRUCT_SOCKADDR_SA_LEN
			v4addr->sin_len = sizeof(*v4addr);
#endif
			v4addr->sin_family = AF_INET;
			memcpy(&v4addr->sin_addr.s_addr,
					&v6addr->sin6_addr.s6_addr[12],4);
			return I2SockAddrEqual(sa1,sa1_len,
					(struct sockaddr*)v4addr,
					sizeof(*v4addr),chk_what);
		}
#endif
		return 0;
	}

	switch(sa1->sa_family){
#ifdef	AF_INET6
	case AF_INET6:
		if((chk_what & I2SADDR_ADDR) &&
			(memcmp(&((struct sockaddr_in6*)sa1)->sin6_addr,
				&((struct sockaddr_in6*)sa2)->sin6_addr,
				sizeof(struct in6_addr)) != 0)){
			return 0;
		}

		if((chk_what & I2SADDR_PORT) &&
				(((struct sockaddr_in6*)sa1)->sin6_port !=
				((struct sockaddr_in6*)sa2)->sin6_port)){
			return 0;
		}

		/*
		 * TODO:Do I need to check scope? Won't for now...
		 */
		return 1;

		break;
#endif
	case AF_INET:
		if((chk_what & I2SADDR_ADDR) &&
				(memcmp(&((struct sockaddr_in*)sa1)->sin_addr,
				&((struct sockaddr_in*)sa2)->sin_addr,
				sizeof(struct in_addr)) != 0)){
			return 0;
		}

		if((chk_what & I2SADDR_PORT) &&
				(((struct sockaddr_in*)sa1)->sin_port !=
				((struct sockaddr_in*)sa2)->sin_port)){
			return 0;
		}

		return 1;

		break;

	default:
		return -1;
	}

	return -1;
}
