/*
 *      $Id$
 */
/************************************************************************
*									*
*			     Copyright (C)  2003			*
*				Internet2				*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		hex.h
 *
 *	Author:		Jeff W. Boote
 *			Internet2
 *
 *	Date:		Tue Dec 16 15:47:34 MST 2003
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
#ifndef	_i2_hex_h_
#define	_i2_hex_h_

#include <I2util/util.h>
#include <stdio.h>

/*
 * Function:	I2HexEncode
 *
 * takes 'nbytes' in 'bytes' and returns a hex representation of them in 'buff'.
 *
 * buff must be at least (nbytes*2)+1 in size or memory will overrun.
 */
extern void
I2HexEncode(
        char            *hexstr,
        const uint8_t   *bytes,
        size_t	        nbytes
        );

/*
 * Function:	I2HexDecode
 *
 * Decode hex chars into bytes. Return True on success, False on error.
 */
extern I2Boolean
I2HexDecode(
        const char  *hexstr,
        uint8_t     *bytes,
        size_t	    nbytes
        );

#endif	/* _i2_hex_h_ */
