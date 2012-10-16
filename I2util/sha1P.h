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
#ifndef I2SHA1P_H
#define I2SHA1P_H

#include <I2util/sha1.h>

BEGIN_C_DECLS

struct I2Sha1ContextRec {
    I2ErrHandle eh;
    uint32_t   	state[5];
    uint32_t   	count[2];
    uint8_t    	buffer[I2SHA1_BLOCK_SIZE];
};

END_C_DECLS

#endif /* I2SHA1P_H */
