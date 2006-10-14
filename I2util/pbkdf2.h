/*
 * ex: set tabstop=4 ai expandtab softtabstop=4 shiftwidth=4:
 * -*- mode: c-basic-indent: 4; tab-width: 4; indent-tabls-mode: nil -*-
 *      $Id$
 */
/************************************************************************
*                                                                       *
*                           Copyright (C)  2006                         *
*                               Internet2                               *
*                           All Rights Reserved                         *
*                                                                       *
************************************************************************/
/*
 *    File:         pbkdf2.h
 *
 *    Author:       Jeff W. Boote
 *                  Internet2
 *
 *    Date:         Fri Oct 13 03:12:00 MDT 2006
 *
 *    Description:    
 */
#ifndef I2PBKDF2_H
#define I2PBKDF2_H

#include <I2util/util.h>

BEGIN_C_DECLS

/*
 * Type for pseudorandom functions - signature matches I2HMACSha1
 * (return 0 on success)
 */
typedef void (*I2prf)(
        const uint8_t   *key,
        uint32_t        keylen,
        const uint8_t   *txt,
        uint32_t        txtlen,
        uint8_t         *digest_ret
        );

/*
 * Implementation of pbkdf2 algorithm.
 * Returns 0 on success. errno set on failure. Possible errors are:
 *  ENOMEM (Unable to allocate buffer space for algorithm.)
 *  EINVAL (dklen too long)
 */
extern int I2pbkdf2(
        I2prf       prf,
        uint32_t    prf_hlen,
        uint8_t     *pw,
        uint32_t    pwlen,
        uint8_t     *salt,
        uint32_t    saltlen,
        uint32_t    count,
        uint32_t    dklen,
        uint8_t     *dk_ret
        );

END_C_DECLS

#endif /* I2PBKDF2_H */
