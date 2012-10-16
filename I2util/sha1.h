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
 *    File:         sha1.h
 *
 *    Author:       Jeff W. Boote   <boote@internet2.edu>
 *                  Internet2
 *
 *    Date:         Wed Sep 27 15:27:47 MDT 2006
 *
 *    Description:    
 *
 *    Public domain version of SHA-1 from 'entropy'. Modifying
 *    it as needed... (Will change to I2 symbols - and update for C99
 *    compliance, and COMMENT.)
 *
 *    Also - changed API to make sha1 context opaque.
 *
 *    Not aiming for a "complete" sha1 here - just enough to support the
 *    hmac-sha1 needed for owamp.
 *
 *		The Internet2 copyright only applies to changes made
 *		to this code. The original copyright follows.
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
/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 */

/* $Id$ */
#ifndef I2SHA1_H
#define I2SHA1_H

#include <I2util/util.h>

#define I2SHA1_DIGEST_SIZE      20
#define I2SHA1_BLOCK_SIZE       64

BEGIN_C_DECLS

struct I2Sha1ContextRec;
typedef struct I2Sha1ContextRec *I2Sha1Context;

extern I2Sha1Context I2Sha1Alloc(
        I2ErrHandle eh
        );
extern void I2Sha1Free(
        I2Sha1Context   sha1
        );
extern void I2Sha1Init(
        I2Sha1Context   sha1
        );
extern void I2Sha1Append(
        I2Sha1Context   sha1,
        const uint8_t   *data,
        uint32_t        len
        );
extern void I2Sha1Finish(
        I2Sha1Context   sha1,
        uint8_t         *digest_ret /* I2SHA1_DIGEST_SIZE */
        );
extern void I2Sha1(
        const uint8_t   *data,
        uint32_t        len,
        uint8_t         *digest_ret
        );
        
END_C_DECLS

#endif /* I2SHA1_H */
