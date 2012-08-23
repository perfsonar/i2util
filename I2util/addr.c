/*
 ** ex: set tabstop=4 ai expandtab softtabstop=4 shiftwidth=4:
 **      $Id$
 */
/************************************************************************
 *                                                                       *
 *                       Copyright (C)  2005                             *
 *                           Internet2                                   *
 *                       All Rights Reserved                             *
 *                                                                       *
 ************************************************************************/
/*
 **    File:        addr.c
 **
 **    Author:      Jeff W. Boote
 **
 **    Date:        Tue Dec 20 11:55:21 MST 2005
 **
 **    Description:    
 **                 Address abstraction API. (Everybody needs one, right?)
 */

#include <I2util/utilP.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/*
 * Data structures
 */
typedef struct I2AddrRec I2AddrRec;

struct I2AddrRec{
    I2ErrHandle      eh;

    I2Boolean       node_set;
    char            node[NI_MAXHOST+1];

    I2Boolean       port_set;
    uint16_t        port_value;
    char            port[NI_MAXSERV+1];

    I2Boolean       ai_free;    /* free ai list directly...*/
    struct addrinfo *ai;

    struct sockaddr *saddr;
    socklen_t       saddrlen;
    int             so_type;    /* socktype saddr works with    */
    int             so_protocol;    /* protocol saddr works with    */
    I2Boolean       passive;

    I2Boolean       fd_user;
    int             fd;
};

static void
_I2AddrClearAI(
        I2Addr  addr
        )
{
    if(!addr)
        return;

    if(addr->ai){
        if(!addr->ai_free){
            freeaddrinfo(addr->ai);
        }else{
            struct addrinfo    *ai, *next;

            ai = addr->ai;
            while(ai){
                next = ai->ai_next;

                if(ai->ai_addr) free(ai->ai_addr);
                if(ai->ai_canonname) free(ai->ai_canonname);
                free(ai);

                ai = next;
            }
        }
    }

    addr->ai = NULL;
    addr->saddr = NULL;
    addr->saddrlen = 0;
    addr->ai_free = False;

    return;
}

void
I2AddrFree(
        I2Addr    addr
        )
{
    if(!addr)
        return;

    _I2AddrClearAI(addr);

    if((addr->fd >= 0) && !addr->fd_user){
        if(close(addr->fd) < 0){
            I2ErrLogT(addr->eh,LOG_WARNING,
                    errno,":close(%d)",addr->fd);
        }
    }

    free(addr);

    return;
}

static I2Addr
_I2AddrAlloc(
        I2ErrHandle eh
        )
{
    I2Addr    addr = calloc(1,sizeof(struct I2AddrRec));

    if(!addr){
        I2ErrLogT(eh,LOG_ERR,I2EUNKNOWN,
                ": calloc(1,%d): %M",sizeof(struct I2AddrRec));
        return NULL;
    }

    addr->eh = eh;

    addr->node_set = 0;
    strncpy(addr->node,"unknown",sizeof(addr->node));
    addr->port_set = addr->port_value = 0;
    strncpy(addr->port,"unknown",sizeof(addr->port));
    addr->ai_free = 0;
    addr->ai = NULL;

    addr->saddr = NULL;
    addr->saddrlen = 0;

    addr->fd_user = 0;
    addr->fd= -1;

    return addr;
}

static struct addrinfo*
CopyAddrRec(
        I2ErrHandle eh,
        const struct addrinfo   *src
        )
{
    struct addrinfo    *dst = calloc(1,sizeof(struct addrinfo));

    if(!dst){
        I2ErrLogT(eh,LOG_ERR,errno,
                ": calloc(1,sizeof(struct addrinfo))");
        return NULL;
    }

    *dst = *src;

    if(src->ai_addr){
        dst->ai_addr = malloc(src->ai_addrlen);
        if(!dst->ai_addr){
            I2ErrLogT(eh,LOG_ERR,errno,
                    "malloc(%u): %s",src->ai_addrlen,
                    strerror(errno));
            free(dst);
            return NULL;
        }
        memcpy(dst->ai_addr,src->ai_addr,src->ai_addrlen);
        dst->ai_addrlen = src->ai_addrlen;
    }
    else
        dst->ai_addrlen = 0;

    if(src->ai_canonname){
        int    len = strlen(src->ai_canonname);

        if(len > NI_MAXHOST){
            I2ErrLogT(eh,LOG_WARNING,
                    I2EUNKNOWN,
                    ":Invalid canonname!");
            dst->ai_canonname = NULL;
        }else{
            dst->ai_canonname = malloc(sizeof(char)*(len+1));
            if(!dst->ai_canonname){
                I2ErrLogT(eh,LOG_WARNING,
                        errno,": malloc(sizeof(%d)",len+1);
                dst->ai_canonname = NULL;
            }else
                strcpy(dst->ai_canonname,src->ai_canonname);
        }
    }

    dst->ai_next = NULL;

    return dst;
}

I2Addr
I2AddrCopy(
        I2Addr from
        )
{
    I2Addr          to;
    struct addrinfo **aip;
    struct addrinfo *ai;

    if(!from)
        return NULL;

    if( !(to = _I2AddrAlloc(from->eh)))
        return NULL;

    if(from->node_set){
        strncpy(to->node,from->node,sizeof(to->node));
        to->node_set = True;
    }

    if(from->port_set){
        strncpy(to->port,from->port,sizeof(to->port));
        to->port_value = from->port_value;
        to->port_set = True;
    }

    aip = &to->ai;
    ai = from->ai;

    while(ai){
        to->ai_free = 1;
        *aip = CopyAddrRec(from->eh,ai);
        if(!*aip){
            I2AddrFree(to);
            return NULL;
        }
        if(ai->ai_addr == from->saddr){
            to->saddr = (*aip)->ai_addr;
            to->saddrlen = (*aip)->ai_addrlen;
        }

        aip = &(*aip)->ai_next;
        ai = ai->ai_next;
    }

    to->fd = from->fd;

    if(to->fd > -1)
        to->fd_user = True;

    return to;
}

I2Addr
I2AddrByNode(
        I2ErrHandle eh,
        const char    *node
        )
{
    I2Addr      addr;
    char        buff[NI_MAXHOST+1];
    const char  *nptr=node;
    char        *pptr=NULL;
    char        *s1,*s2;

    if(!node)
        return NULL;

    if(!(addr=_I2AddrAlloc(eh)))
        return NULL;

    strncpy(buff,node,sizeof(buff));

    /*
     * Pull off port if specified. If syntax doesn't match URL like
     * node:port - ipv6( [node]:port) - then just assume whole string
     * is nodename and let getaddrinfo report problems later.
     * (This service syntax is specified by rfc2396 and rfc2732.)
     */

    /*
     * First try ipv6 syntax since it is more restrictive.
     */
    if( (s1 = strchr(buff,'['))){
        s1++;
        if(strchr(s1,'[')) goto NOPORT;
        if(!(s2 = strchr(s1,']'))) goto NOPORT;
        *s2++='\0';
        if(strchr(s2,']')) goto NOPORT;
        if(*s2++ != ':') goto NOPORT;
        nptr = s1;
        pptr = s2;
    }
    /*
     * Now try ipv4 style.
     */
    else if( (s1 = strchr(buff,':'))){
        *s1++='\0';
        /* second ':' indicates a v6 address with no port specified */
        if(strchr(s1,':')) goto NOPORT;
        nptr = buff;
        pptr = s1;
    }


NOPORT:
    /*
     * Set hostname if it was specified.
     */
    if(nptr && strlen(nptr)){
        strncpy(addr->node,nptr,sizeof(addr->node));
        addr->node_set = 1;
    }

    if(pptr && strlen(pptr)){
        int     tint;
        char    *tstr=NULL;

        tint = strtol(pptr,&tstr,10);
        if(!tstr || (pptr == tstr) || (tint < 0) || (tint > (int)0xffff)){
            addr->port_set = addr->port_value = 0;
        }
        else{
            addr->port_set = True;
            addr->port_value = (uint16_t)tint;
            strncpy(addr->port,pptr,sizeof(addr->port));
        }
    }

    return addr;
}

I2Addr
I2AddrByWildcard(
        I2ErrHandle eh,
        int         socktype,
        const char  *servname
        )
{
    struct addrinfo *ai=NULL;
    struct addrinfo hints;
    I2Addr          addr;
    int             ai_err;


    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = socktype;
    hints.ai_flags = AI_PASSIVE;

    if( (ai_err = getaddrinfo(NULL,servname,&hints,&ai)!=0)
            || !ai){
        I2ErrLogT(eh,LOG_ERR,I2EUNKNOWN,
                "getaddrinfo(): %s",gai_strerror(ai_err));
        return NULL;
    }

    if( !(addr = _I2AddrAlloc(eh))){
        freeaddrinfo(ai);
        return NULL;
    }

    addr->ai = ai;
    addr->passive = True;

    return addr;
}

static void
_I2AddrSetNodePort(
        I2Addr  addr
        )
{
    int     gai;
    char    *pptr=NULL;
    char    *tstr=NULL;
    int     tint;

    if(!addr->saddr || (addr->node_set && addr->port_set)){
        return;
    }

    if(addr->saddr->sa_family == AF_UNIX){
        strncpy(addr->node,"unixsock",sizeof(addr->node));
        strncpy(addr->port,"unnamed",sizeof(addr->port));

        addr->node_set = True;
        addr->port_set = True;
        addr->port_value = 0;
    }
    else{
        if( (gai = getnameinfo(addr->saddr,addr->saddrlen,
                        addr->node,sizeof(addr->node),
                        addr->port,sizeof(addr->port),
                        NI_NUMERICSERV)) != 0){
            I2ErrLogT(addr->eh,LOG_WARNING,I2EUNKNOWN,
                    "getnameinfo(): %s",gai_strerror(gai));
            strncpy(addr->node,"unknown",sizeof(addr->node));
            strncpy(addr->port,"unknown",sizeof(addr->port));
        }
        else{

            addr->node_set = True;

            pptr = addr->port;
            tstr = NULL;
            tint = strtol(pptr,&tstr,10);
            if(!tstr || (pptr == tstr) || (tint < 0) || (tint > (int)0xffff)){
                addr->port_set = addr->port_value = 0;
            }
            else{
                addr->port_set = True;
                addr->port_value = (uint16_t)tint;
            }
        }
    }

    return;
}

/*
 * Function:    I2AddrBySAddr
 *
 * Description:    
 *     Construct an I2Addr record given a sockaddr struct.
 *
 * In Args:    
 *              Set socktype == 0 if it doesn't matter. (but realize
 *              this is here because the saddr will probably by used
 *              to create a socket...
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
I2Addr
I2AddrBySAddr(
        I2ErrHandle eh,
        struct sockaddr *saddr,
        socklen_t       saddrlen,
        int             socktype,
        int             protocol
        )
{
    I2Addr               addr;
    struct addrinfo     *ai=NULL;
    struct sockaddr_in  v4addr;

    if(!saddr){
        return NULL;
    }

    switch(saddr->sa_family){
#ifdef    AF_INET6
        struct sockaddr_in6    v6addr;

        case AF_INET6:
        if(saddrlen < sizeof(v6addr)){
            I2ErrLogT(eh,LOG_ERR,EINVAL,"invalid saddrlen for addr family");
            return NULL;
        }

        /*
         * If this is a mapped addr - create a sockaddr_in
         * instead of the sockaddr_in6. (This is so addr
         * matching will work in other parts of the code, and
         * users of v4 will not be confused by security limits
         * on v6 addresses causing problems.)
         */
        memcpy(&v6addr,saddr,sizeof(v6addr));
        if(IN6_IS_ADDR_V4MAPPED(&v6addr.sin6_addr)){
            memset(&v4addr,0,sizeof(v4addr));
#ifdef    HAVE_STRUCT_SOCKADDR_SA_LEN
            v4addr.sin_len = sizeof(v4addr);
#endif
            v4addr.sin_family = AF_INET;
            v4addr.sin_port = v6addr.sin6_port;
            memcpy(&v4addr.sin_addr.s_addr,
                    &v6addr.sin6_addr.s6_addr[12],4);
            saddr = (struct sockaddr*)&v4addr;
            saddrlen = sizeof(v4addr);
        }
        break;
#endif

        /* fall through */
        case AF_INET:
        case AF_UNIX:
        break;

        default:
        I2ErrLogT(eh,LOG_ERR,EINVAL,"Invalid addr family");
        return NULL;
        break;
    }

    if(!(addr = _I2AddrAlloc(eh)))
        return NULL;

    if(!(ai = calloc(1,sizeof(struct addrinfo)))){
        I2ErrLogT(addr->eh,LOG_ERR,I2EUNKNOWN,
                "malloc():%s",strerror(errno));
        (void)I2AddrFree(addr);
        return NULL;
    }

    if(!(addr->saddr = calloc(1,saddrlen))){
        I2ErrLogT(addr->eh,LOG_ERR,I2EUNKNOWN,
                "malloc():%s",strerror(errno));
        (void)I2AddrFree(addr);
        (void)free(ai);
        return NULL;
    }
    memcpy(addr->saddr,saddr,saddrlen);
    ai->ai_addr = addr->saddr;
    addr->saddrlen = saddrlen;
    ai->ai_addrlen = saddrlen;

    ai->ai_flags = 0;
    ai->ai_family = saddr->sa_family;
    ai->ai_socktype = socktype;
    ai->ai_protocol = protocol;
    ai->ai_canonname = NULL;
    ai->ai_next = NULL;

    addr->ai = ai;
    addr->ai_free = True;
    addr->so_type = socktype;
    addr->so_protocol = protocol;

    _I2AddrSetNodePort(addr);

    return addr;
}

static I2Addr
ByAnySockFD(
        I2ErrHandle     eh,
        int             fd,
        I2Boolean       close_on_free,
        struct sockaddr *saddr,
        socklen_t       saddrlen)
{
    int                     so_type;
    socklen_t               so_typesize = sizeof(so_type);
    I2Addr                  addr;

    /*
     * *BSD getsockname/getpeername returns 0 size for AF_UNIX.
     * fake a sockaddr to describe this.
     */
    if(!saddrlen){
        saddr->sa_family = AF_UNIX;
        /*
         * Set the size of this "fake" sockaddr to include
         * the sa_family member. (and possibly the sa_len member)
         */
        saddrlen = (char*)&saddr->sa_family - (char*)saddr;
        saddrlen += sizeof(saddr->sa_family);
#ifdef    HAVE_STRUCT_SOCKADDR_SA_LEN
        if(saddrlen <= (socklen_t)((char*)&saddr->sa_len - (char*)saddr)){
            saddrlen = (char*)&saddr->sa_len - (char*)saddr;
            saddrlen += sizeof(saddr->sa_len);
        }
        saddr->sa_len = saddrlen;
#endif
    }

    if(getsockopt(fd,SOL_SOCKET,SO_TYPE,
                (void*)&so_type,&so_typesize) != 0){
        I2ErrLogT(eh,LOG_ERR,errno,"getsockopt(): %M");
        return NULL;
    }

    addr = I2AddrBySAddr(eh,saddr,saddrlen,so_type,0);
    if(!addr)
        return NULL;

    addr->fd_user = !close_on_free;
    addr->fd = fd;

    return addr;
}

I2Addr
I2AddrBySockFD(
        I2ErrHandle eh,
        int         fd,
        I2Boolean   close_on_free
        )
{
    struct sockaddr_storage sbuff;
    socklen_t               saddrlen = sizeof(sbuff);

    if(getpeername(fd,(void*)&sbuff,&saddrlen) != 0){
        I2ErrLogT(eh,LOG_ERR,errno,"getpeername(): %M");
        return NULL;
    }

    return ByAnySockFD(eh,fd,close_on_free,(struct sockaddr *)&sbuff,saddrlen);
}

I2Addr
I2AddrByLocalSockFD(
        I2ErrHandle eh,
        int         fd,
        I2Boolean   close_on_free
        )
{
    struct sockaddr_storage sbuff;
    socklen_t               saddrlen = sizeof(sbuff);

    if(getsockname(fd,(void*)&sbuff,&saddrlen) != 0){
        I2ErrLogT(eh,LOG_ERR,errno,"getsockname(): %M");
        return NULL;
    }

    return ByAnySockFD(eh,fd,close_on_free,(struct sockaddr *)&sbuff,saddrlen);
}


/*
 * Function:    I2AddrSetSAddr
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
 *              Does not copy the memory and does not directly free it.
 *              (It is expected that this memory points at the saddr
 *              portion of one of the getaddrinfo structures returned
 *              from I2AddrAddrInfo.)
 */
I2Boolean
I2AddrSetSAddr(
        I2Addr addr,
        struct sockaddr *saddr,
        socklen_t       saddr_len
        )
{
    if(addr->fd > -1){
        I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                "I2AddrSetSAddr: Addr already associated with socket: %M");
        return False;
    }

    if(!addr->ai){
        I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                "I2AddrSetSAddr: AddrInfo not set: %M");
        return False;
    }

    addr->saddr = saddr;
    addr->saddrlen = saddr_len;

    /*
     * reset node/port based on new saddr
     */
    addr->node_set = False;
    addr->port_set = addr->port_value = 0;
    _I2AddrSetNodePort(addr);

    return True;
}

/*
 * Function:    I2AddrSetFD
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
I2Boolean
I2AddrSetFD(
        I2Addr      addr,
        int         fd,
        I2Boolean   close_on_free
        )
{
    if(!addr)
        return False;

    if((fd > -1) && (fd != addr->fd) && (addr->fd > -1)){
        I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                "I2AddrSetFD: Addr already associated with socket: %M");
        return False;
    }

    addr->fd = fd;
    addr->fd_user = !close_on_free;

    return True;
}

/*
 * Function:    I2AddrSetPort
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
I2Boolean
I2AddrSetPort(
        I2Addr     addr,
        uint16_t   p
        )
{
    uint16_t    sp;

    if(!addr)
        return False;

    if(addr->fd > -1){
        I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                "I2AddrSetPort: Addr already associated with socket: %M");
        return False;
    }

    /*
     * If saddr is already set - than modify the port.
     */
    if(addr->saddr){
        I2SockUnion sau_mem;
        I2SockUnion *sau;

        if( !(sau = I2SockAddrToSockUnion(addr->saddr,
                        addr->saddrlen,&sau_mem))){
            I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                    "I2AddrSetPort: Unable to decode sockaddr");
            return False;
        }

        /*
         * decode v4 and v6 sockaddrs.
         */
        sp = htons(p);
        switch(sau->sa.sa_family){
#ifdef    AF_INET6

            case AF_INET6:
            sau->sin6.sin6_port = sp;
            break;
#endif
            case AF_INET:
            sau->sin.sin_port = sp;
            break;
            default:
            I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                    "I2AddrSetPort: Invalid address family");
            return False;
        }

        if( !I2SockUnionToSockAddr(sau,&addr->saddrlen,addr->saddr)){
            return False;
        }
    }

    snprintf(addr->port,sizeof(addr->port),"%u",p);
    addr->port_set = True;
    addr->port_value = p;

    return True;
}

/*
 * Function:    I2AddrPort
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
uint16_t
I2AddrPort(
        I2Addr     addr
        )
{

    if(!addr)
        return 0;

    return addr->port_value;
}

/*
 * Function:    I2AddrSetProtocol
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
I2Boolean
I2AddrSetProtocol(
        I2Addr  addr,
        int     protocol
        )
{
    if(!addr)
        return False;

    if(addr->so_protocol == protocol)
        return True;

    /*
     * If so_protocol was "unset" before, then this is a simple assignment
     * even for an associated socket.
     */
    if(!addr->so_protocol){
        addr->so_protocol = protocol;
        return True;
    }

    /*
     * Otherwise, make sure this assignment is happening *before* actual
     * association with a socket/saddr.
     */
    if(addr->fd > -1){
        I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                "I2AddrSetProtocol: Addr already associated with socket: %M");
        return False;
    }
    if(addr->saddr){
        I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                "I2AddrSetProtocol: Addr already associated with saddr: %M");
        return False;
    }

    _I2AddrClearAI(addr);

    addr->so_protocol = protocol;

    return True;
}

/*
 * Function:    I2AddrProtocol
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
int
I2AddrProtocol(
        I2Addr  addr
        )
{
    if(!addr)
        return 0;

    return addr->so_protocol;
}

/*
 * Function:    I2AddrSetSocktype
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
I2Boolean
I2AddrSetSocktype(
        I2Addr     addr,
        int         so_type
        )
{
    if(!addr)
        return False;

    if(addr->so_type == so_type)
        return True;

    /*
     * If so_type was "unset" before, then this is a simple assignment
     * even for an associated socket.
     */
    if(!addr->so_type){
        addr->so_type = so_type;
        return True;
    }

    if(addr->fd > -1){
        I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                "I2AddrSetSocktype: Addr already associated with socket: %M");
        return False;
    }
    if(addr->saddr){
        I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                "I2AddrSetSocktype: Addr already associated with saddr: %M");
        return False;
    }

    _I2AddrClearAI(addr);

    addr->so_type = so_type;

    return True;
}

/*
 * Function:    I2AddrSocktype
 *
 * Description:    
 *          Retrieves so_socktype.
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
int
I2AddrSocktype(
        I2Addr  addr
        )
{
    if(!addr)
        return 0;

    return addr->so_type;
}

/*
 * Function:    I2AddrSetPassive
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
I2Boolean
I2AddrSetPassive(
        I2Addr     addr,
        I2Boolean  passive
        )
{
    if(!addr)
        return False;

    if(addr->passive == passive)
        return True;

    if(addr->fd > -1){
        I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                "I2AddrSetPassive: Addr already associated with socket: %M");
        return False;
    }
    if(addr->saddr){
        I2ErrLogT(addr->eh,LOG_ERR,EINVAL,
                "I2AddrSetPassive: Addr already associated with saddr: %M");
        return False;
    }

    _I2AddrClearAI(addr);

    addr->passive = passive;

    return True;
}

/*
 * Function:    I2AddrFD
 *
 * Description:    
 *      Retrieves the fd associated with the I2Addr.
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
int
I2AddrFD(
        I2Addr addr
        )
{
    if(!addr || (addr->fd < 0))
        return -1;

    return addr->fd;
}

/*
 * Function:    I2AddrSAddr
 *
 * Description:    
 *          This function retrieves the sockaddr associated with the given
 *          I2Addr. (If the saddr has not been set, it returns null.)
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
I2AddrSAddr(
        I2Addr      addr,
        socklen_t   *saddr_len
        )
{
    socklen_t   len_mem;
    socklen_t   *len = &len_mem;

    if(saddr_len){
        len = saddr_len;
    }

    if(addr && addr->saddr){
        *len = addr->saddrlen;
        return addr->saddr;
    }

    *len = 0;
    return NULL;
}

/*
 * Function:    I2AddrNodeName
 *
 * Description:    
 *              This function gets a char* node name for a given I2Addr.
 *              The len parameter is an in/out parameter.
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
char *
I2AddrNodeName(
        I2Addr addr,
        char    *buf,
        size_t  *len
        )
{
    int i;

    assert(buf);
    assert(len);
    assert(*len > 0);

    if(!addr){
        goto bail;
    }

    if(!addr->node_set){
        _I2AddrSetNodePort(addr);
    }

    if(addr->node_set){
        *len = MIN(*len,sizeof(addr->node));
        strncpy(buf,addr->node,*len);

        for(i = 0; i < strlen(buf); i++) {
             if (buf[i] == '%') {
                 buf[i] = '\0';
                 *len = i;
             }
        }

        return buf;
    }

bail:
    *len = 0;
    buf[0] = '\0';
    return NULL;
}

/*
 * Function:    I2AddrServName
 *
 * Description:    
 *              This function gets a char* service name for a given I2Addr.
 *              The len parameter is an in/out parameter.
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
char *
I2AddrServName(
        I2Addr addr,
        char    *buf,
        size_t  *len
        )
{
    assert(buf);
    assert(len);
    assert(*len > 0);

    if(!addr){
        goto bail;
    }

    if(!addr->port_set){
        _I2AddrSetNodePort(addr);
    }

    if(addr->port_set){
        *len = MIN(*len,sizeof(addr->port));
        strncpy(buf,addr->port,*len);
        return buf;
    }

bail:
    *len = 0;
    buf[0] = '\0';
    return NULL;
}

/*
 * Function:    I2AddrNodeServName
 *
 * Description:    
 *              This function gets a char* complete IP:port name for a given
 *              I2Addr. The len parameter is an in/out parameter.
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
char *
I2AddrNodeServName(
        I2Addr addr,
        char    *buf,
        size_t  *len
        )
{
    size_t  newlen;

    assert(buf);
    assert(len);
    assert(*len > 0);

    if(!addr){
        goto bail;
    }

    if(!addr->node_set || !addr->port_set){
        _I2AddrSetNodePort(addr);
    }

    if(!addr->node_set || !addr->port_set){
        goto bail;
    }

    newlen = strlen("[]:") + strlen(addr->node) + strlen(addr->port);
    *len = MIN(*len,newlen);

    strncpy(buf,"[",*len);
    strncat(buf,addr->node,*len);
    strncat(buf,"]:",*len);
    strncat(buf,addr->port,*len);

    return buf;

bail:
    *len = 0;
    buf[0] = '\0';
    return NULL;
}

/*
 * Function:    I2AddrAddrInfo
 *
 * Description:    
 *
 * In Args:    
 *              def_node:   only used if internal node is not set
 *              def_serv:   only used if internal port not set
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
struct addrinfo
*I2AddrAddrInfo(
        I2Addr     addr,
        char        *def_node,
        char        *def_serv
        )
{
    struct addrinfo hints;
    char            *host=NULL;
    char            *port=NULL;
    int             gai;

    if(!addr){
        return NULL;
    }

    if(addr->ai)
        return addr->ai;

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;

    if(addr->so_type){
        hints.ai_socktype = addr->so_type;
    }
    else{
        hints.ai_socktype = SOCK_STREAM;
    }

    if(addr->so_protocol){
        hints.ai_protocol = addr->so_protocol;
    }

    if(addr->passive){
        hints.ai_flags = AI_PASSIVE;
    }

    if(addr->node_set && (strncmp(addr->node,"unknown",sizeof(addr->node)))){
        host = addr->node;
    }
    else if(def_node){
        host = def_node;
    }

    if(addr->port_set && (strncmp(addr->port,"unknown",sizeof(addr->port)))){
        port = addr->port;
    }
    else if(def_serv){
        port = def_serv;
    }

    /*
     * Some implementations allow this and give you a bogus connection
     * to localhost. I prefer the quick fail.
     */
    if(!host && !addr->passive){
        return NULL;
    }

    if(((gai = getaddrinfo(host,port,&hints,&addr->ai)) != 0) || !addr->ai){
        I2ErrLogT(addr->eh,LOG_ERR,I2EUNKNOWN,"getaddrinfo(): %s",
                gai_strerror(gai));
        return NULL;
    }
    addr->ai_free = 0;

    return addr->ai;
}

/*
 * Function:    I2AddrSockLen
 *
 * Description:    
 *              Return socket address length (for use in calling accept etc...)
 *              or 0 if it doesn't refer to a socket yet.
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
extern socklen_t
I2AddrSockLen(
        I2Addr    addr
        )
{
    if(addr && addr->saddr){
        return addr->saddrlen;
    }
    return 0;
}

/*
 * TODO: optimize byte swapping by using BYTE_ORDER macro
 * i.e.
 * #if (BYTE_ORDER == BIG_ENDIAN)
 *  do_nothing
 * #else
 *  do_this
 * #endif
 */

/*
 * Deal with network ordering of 64 bit int's.
 */
uint64_t 
I2htonll(
        uint64_t    h64
      )
{
    uint64_t    n64=0;
    uint32_t    l32;
    uint32_t    h32;
    uint8_t     *t8;

    /* Use t8 to byte address the n64 */
    t8 = (uint8_t *)&n64;

    /* set low-order bytes */
    l32 = (uint32_t)(h64 & 0xFFFFFFFFUL);
    l32 = htonl(l32);

    /* set high-order bytes */
    h64 >>=32;
    h32 = (uint32_t)(h64 & 0xFFFFFFFFUL);
    h32 = htonl(h32);

    memcpy(&t8[0],&h32,4);
    memcpy(&t8[4],&l32,4);

    return n64;
}

uint64_t 
I2ntohll(
        uint64_t    n64
      )
{
    uint64_t    h64;
    char        *t8 = (char *)&n64;
    uint32_t    t32;

    /* High order bytes */
    memcpy(&t32,&t8[0],4);
    h64 = ntohl(t32);
    h64 <<= 32;

    /* Low order bytes */
    memcpy(&t32,&t8[4],4);
    h64 |= ntohl(t32);

    return h64;
}

/*
 * Function:    I2AddrIsLoopback
 *
 * Description:    
 *     Check whether an address is for a loopback interface.
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 *     1 if true, 0 if false
 * Side Effect:    
 */
int
I2AddrIsLoopback(
        I2Addr    addr
        )
{
    struct sockaddr *saddr;
    socklen_t       saddrlen;

    if(!(saddr = I2AddrSAddr(addr,&saddrlen))){
        return 0;
    }

    return I2SockAddrIsLoopback(saddr, saddrlen);
}
