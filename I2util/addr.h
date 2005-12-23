/*
 * ex: set tabstop=4 ai expandtab softtabstop=4 shiftwidth=4:
 * -*- mode: c-basic-indent: 4; tab-width: 4; indent-tabls-mode: nil -*-
 *      $Id$
 */
/************************************************************************
 *                                                                      *
 *                           Copyright (C)  2005                        *
 *                               Internet2                              *
 *                           All Rights Reserved                        *
 *                                                                      *
 ************************************************************************/
/*
 **    File:         addr.h
 **
 **    Author:       Jeff W. Boote
 **
 **    Date:         Tue Dec 20 12:04:12 MST 2005
 **
 **    Description:    
 *              Abstraction layer for addresses. Useful for keeping
 *              network address code localized to one part of the
 *              code.
 */
#ifndef    I2ADDR_H
#define    I2ADDR_H

/*
 * Portablility sanity checkes.
 */
#if    !HAVE_GETADDRINFO || !HAVE_SOCKET
#error    Missing needed networking capabilities! (getaddrinfo and socket)
#endif

#include <sys/types.h>
#include <sys/socket.h>

typedef struct I2AddrRec    *I2Addr;

/*
 * The I2AddrBy* functions are used to allow the addr API to more
 * adequately manage the memory associated with the many different ways
 * of specifying an address - and to provide a uniform way to specify an
 * address.
 *
 * These functions return NULL on failure. (They call the error handler
 * to specify the reason.)
 */
extern I2Addr
I2AddrByNode(
        I2ErrHandle eh,
        const char  *node    /* dns or valid char representation of addr */
        );

extern I2Addr
I2AddrByWildcard(
        I2ErrHandle eh,
        int         socktype,
        const char  *servname
        );

extern I2Addr
I2AddrBySAddr(
        I2ErrHandle     eh,
        struct sockaddr *saddr,
        socklen_t       saddr_len,
        int             socktype,
        int             protocol
        );

/*
 * Copies an I2Addr. If the src I2Addr represents a connected socket,
 * the copy will as well. However, as with the AddrBy*FD functions
 * free'ing the copy I2Addr will not close the fd.
 */
extern I2Addr
I2AddrCopy(
        I2Addr  src
        );

/*
 * Return the address for the remote side of a socket connection
 * (getpeername)
 * Only copies the address portion of the I2Addr record, does not
 * represent the actual connected socket.
 */
extern I2Addr
I2AddrBySockFD(
        I2ErrHandle eh,
        int         fd, /* fd must be an already connected socket */
        I2Boolean   close_on_free
        );

/*
 * Return the address for the local side of a socket connection
 * (getsockname)
 * Only copies the address portion of the I2Addr record, does not
 * represent the actual connected socket.
 */
extern I2Addr
I2AddrByLocalSockFD(
        I2ErrHandle eh,
        int         fd,    /* fd must be an already connected socket */
        I2Boolean   close_on_free
        );

/*
 * Addr access functions.
 * The set functions are only valid *before* a real socket is associated
 * with the Addr. So, the internal fd cannot be set upon entrance.
 */
extern I2Boolean
I2AddrSetSAddr(
        I2Addr          addr,
        struct sockaddr *saddr,
        socklen_t       saddr_len
        );

extern I2Boolean
I2AddrSetFD(
        I2Addr      addr,
        int         fd,
        I2Boolean   close_on_free
        );

extern I2Boolean
I2AddrSetPort(
        I2Addr     addr,
        uint16_t   port
        );

extern uint16_t
I2AddrPort(
        I2Addr  addr
        );

extern I2Boolean
I2AddrSetSocktype(
        I2Addr  addr,
        int     so_type
        );

extern int
I2AddrSocktype(
        I2Addr  addr
        );

extern I2Boolean
I2AddrSetProtocol(
        I2Addr  addr,
        int     protocol
        );

extern int
I2AddrProtocol(
        I2Addr  addr
        );

extern I2Boolean
I2AddrSetPassive(
        I2Addr      addr,
        I2Boolean   passive
        );

extern struct addrinfo
*I2AddrAddrInfo(
        I2Addr  addr,
        char    *def_node,
        char    *def_serv
        );

extern struct sockaddr
*I2AddrSAddr(
        I2Addr          addr,
        socklen_t       *saddr_len
        );

extern char *
I2AddrNodeName(
        I2Addr  addr,
        char    *buf,
        size_t  *len    /* in/out parameter for buf len */
        );

extern char *
I2AddrServName(
        I2Addr  addr,
        char    *buf,
        size_t  *len    /* in/out parameter for buf len */
        );

/*
 * return FD for given I2Addr or -1 if it doesn't refer to a socket yet.
 */
extern int
I2AddrFD(
        I2Addr    addr
        );

/*
 * return socket address length (for use in calling accept etc...)
 * or 0 if it doesn't refer to a socket yet.
 */
extern socklen_t
I2AddrSockLen(
        I2Addr    addr
        );

extern void
I2AddrFree(
        I2Addr    addr
        );

#ifndef htonll
#define htonll(x)   I2htonll(x)
#endif
#ifndef ntohll
#define ntohll(x)   I2ntohll(x)
#endif

extern uint64_t
I2htonll(
        uint64_t    h64
      );
extern uint64_t
I2ntohll(
        uint64_t    n64
      );

#endif    /* I2ADDR_H */
