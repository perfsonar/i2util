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
 *	File:		readpassphrase.h
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Wed Dec 10 15:32:58 MST 2003
 *
 *	Description:	
 *		pulled-in from freebsd.
 */
/*
 * Copyright (c) 2000 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _i2_readpassphrase_h_
#define _i2_readpassphrase_h_

#include <I2util/util.h>

#define I2RPP_ECHO_OFF    0x00		/* Turn off echo (default). */
#define I2RPP_ECHO_ON     0x01		/* Leave echo on. */
#define I2RPP_REQUIRE_TTY 0x02		/* Fail if there is no tty. */
#define I2RPP_FORCELOWER  0x04		/* Force input to lower case. */
#define I2RPP_FORCEUPPER  0x08		/* Force input to upper case. */
#define I2RPP_SEVENBIT    0x10		/* Strip the high bit from input. */


BEGIN_C_DECLS

char * I2ReadPassPhrase(const char *, char *, size_t, int);
char * I2ReadPassPhraseAlloc(
        const char  *prompt,
        int         flags,
        char        **lbuf,     /* memory pointer - realloc'd if needed */
        size_t      *lbuf_max   /* current len of lbuf */
        );

END_C_DECLS

#endif /* !_i2_readpassphrase_h_ */
