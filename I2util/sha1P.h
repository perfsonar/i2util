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
 *    File:         sha1P.h
 *
 *    Author:       Jeff W. Boote
 *                  Internet2
 *
 *    Date:         Sun Oct 08 03:15:21 MDT 2006
 *
 *    Description:    
 */
#ifndef I2SHA1P_H
#define I2SHA1P_H

#include <I2util/sha1.h>

BEGIN_C_DECLS

struct I2Sha1ContextRec {
    I2ErrHandle eh;
    u_int32_t   state[5];
    u_int32_t   count[2];
    u_int8_t    buffer[I2SHA1_BLOCK_SIZE];
};

END_C_DECLS

#endif /* I2SHA1P_H */
