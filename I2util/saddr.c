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
#include <assert.h>
#include <I2util/saddr.h>

#include <string.h>

/*
 * These first functions are used to copy sockaddr structs in and out of
 * a union designed to make the rest of the code more ISO C99 compliant.
 * Specifically - type punning (aliasing) is much more limited than it
 * used to be to allow for more optimization. (Of course it specifically
 * de-optimizes my code since I now have to memcpy things instead of
 * modifying them in place. YUCK!)
 */

/*
 * Function:    I2SockAddrToSockUnion
 *
 * Description:    
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
I2SockUnion *
I2SockAddrToSockUnion(
        const struct sockaddr   *sa,
        socklen_t               sa_len,
        I2SockUnion             *sau_mem
        )
{
    assert(sau_mem);
    assert(sa);

    memset(sau_mem,0,sizeof(I2SockUnion));

    switch(sa->sa_family){
#ifdef	AF_INET6
        case AF_INET6:
            if(sa_len < sizeof(struct sockaddr_in6))
                return NULL;

            memcpy(&sau_mem->sin6,sa,sa_len);

            break;
#endif
        case AF_INET:
            if(sa_len < sizeof(struct sockaddr_in))
                return NULL;

            memcpy(&sau_mem->sin,sa,sa_len);

            break;

        default:
            return NULL;
    }

    return sau_mem;
}

/*
 * Function:    I2SockUnionToSockAddr
 *
 * Description:    
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
struct sockaddr *
I2SockUnionToSockAddr(
        const I2SockUnion   *sau,
        socklen_t           *sa_len_in_out,
        struct sockaddr     *sa_mem
        )
{
    assert(sau);
    assert(sa_mem);

    switch(sau->sa.sa_family){
#ifdef	AF_INET6
        case AF_INET6:
            if(*sa_len_in_out < sizeof(struct sockaddr_in6))
                return NULL;
            *sa_len_in_out = sizeof(struct sockaddr_in6);


            break;
#endif
        case AF_INET:
            if(*sa_len_in_out < sizeof(struct sockaddr_in))
                return NULL;
            *sa_len_in_out = sizeof(struct sockaddr_in);

            break;

        default:
            return NULL;
    }

    memcpy(sa_mem,&sau->sas,*sa_len_in_out);

    return sa_mem;

}

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
	uint32_t		chk_what
	)
{
    I2SockUnion sau1_mem, sau2_mem;
    I2SockUnion *sau1, *sau2;

	/*
	 * If the lengths are not equal - or the families are not the
	 * same - check if the v6 address is really an encoded v4 address.
	 * If it is - then re-call ourselves with the v4 version directly.
	 */
	if((sa1_len != sa2_len) || (sa1->sa_family != sa2->sa_family)){
#ifdef	AF_INET6
		struct sockaddr_in  v4rec;

		/*
		 * check if sa1 is a mapped addr.
		 */

                /* copy sa1 into the union */
                if( !(sau1 = I2SockAddrToSockUnion(sa1,sa1_len,&sau1_mem))){
                    return -1;
                }

		if((sau1->sa.sa_family==AF_INET6) &&
				IN6_IS_ADDR_V4MAPPED(&sau1->sin6.sin6_addr)){
			memset(&v4rec,0,sizeof(v4rec));
#ifdef	HAVE_STRUCT_SOCKADDR_SA_LEN
			v4rec.sin_len = sizeof(v4rec);
#endif
			v4rec.sin_family = AF_INET;
			memcpy(&v4rec.sin_addr.s_addr,
					&sau1->sin6.sin6_addr.s6_addr[12],4);
                        v4rec.sin_port = sau1->sin6.sin6_port;
			return I2SockAddrEqual((struct sockaddr*)&v4rec,
					sizeof(v4rec),
					sa2,sa2_len,chk_what);
		}

                /* copy sa1 into the union */
                if( !(sau2 = I2SockAddrToSockUnion(sa2,sa2_len,&sau2_mem))){
                    return -1;
                }

		if((sau2->sa.sa_family==AF_INET6) &&
				IN6_IS_ADDR_V4MAPPED(&sau2->sin6.sin6_addr)){
			memset(&v4rec,0,sizeof(v4rec));
#ifdef	HAVE_STRUCT_SOCKADDR_SA_LEN
			v4rec.sin_len = sizeof(v4rec);
#endif
			v4rec.sin_family = AF_INET;
			memcpy(&v4rec.sin_addr.s_addr,
					&sau2->sin6.sin6_addr.s6_addr[12],4);
                        v4rec.sin_port = sau2->sin6.sin6_port;
			return I2SockAddrEqual(sa1,sa1_len,
					(struct sockaddr*)&v4rec,
					sizeof(v4rec),chk_what);
		}
#endif
		return 0;
	}

        /* copy to union to enable punning */
        if( !(sau1 = I2SockAddrToSockUnion(sa1,sa1_len,&sau1_mem)) ||
                !(sau2 = I2SockAddrToSockUnion(sa2,sa2_len,&sau2_mem))){
            return -1;
        }

	switch(sau1->sa.sa_family){
#ifdef	AF_INET6
	case AF_INET6:
		if((chk_what & I2SADDR_ADDR) &&
			(memcmp(&sau1->sin6.sin6_addr,&sau2->sin6.sin6_addr,
				sizeof(struct in6_addr)) != 0)){
			return 0;
		}

		if((chk_what & I2SADDR_PORT) &&
				(sau1->sin6.sin6_port != sau2->sin6.sin6_port)){
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
				(memcmp(&sau1->sin.sin_addr,&sau2->sin.sin_addr,
				sizeof(struct in_addr)) != 0)){
			return 0;
		}

		if((chk_what & I2SADDR_PORT) &&
				(sau1->sin.sin_port != sau2->sin.sin_port)){
			return 0;
		}

		return 1;

		break;

	default:
		return -1;
	}

	return -1;
}

/*
 * Function:	I2SockAddrIsLoopback
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
I2SockAddrIsLoopback(
	const struct sockaddr	*sa,
	socklen_t		sa_len
	)
{
    I2SockUnion sau_mem;
    I2SockUnion *sau;

    if( !(sau = I2SockAddrToSockUnion(sa,sa_len,&sau_mem))){
        return -1;
    }


    switch(sau->sa.sa_family){
#ifdef	AF_INET6
        case AF_INET6:
            if (IN6_IS_ADDR_V4MAPPED(&sau->sin6.sin6_addr)){
                struct sockaddr_in  v4rec;

                memset(&v4rec,0,sizeof(v4rec));
#ifdef	HAVE_STRUCT_SOCKADDR_SA_LEN
                v4rec.sin_len = sizeof(v4rec);
#endif
                v4rec.sin_family = AF_INET;
                memcpy(&v4rec.sin_addr.s_addr,
                        &sau->sin6.sin6_addr.s6_addr[12],4);
                v4rec.sin_port = sau->sin6.sin6_port;

                return I2SockAddrIsLoopback(&sau->sa,sizeof(v4rec));
            }

            return IN6_IS_ADDR_LOOPBACK(&sau->sin6.sin6_addr);

            break;
#endif
        case AF_INET:
            return (sau->sin.sin_addr.s_addr == htonl(INADDR_LOOPBACK));

            break;

        default:
            return -1;
    }

    return 0;
}
