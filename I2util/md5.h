/* MD5.H - header file for MD5C.C
 * $FreeBSD: src/sys/sys/md5.h,v 1.13 1999/12/29 04:24:44 peter Exp $
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

#ifndef _I2_md5_h_
#define _I2_md5_h_
#include <sys/cdefs.h>
#include <I2util/util.h>

/* MD5 context. */
typedef struct I2MD5Context {
  uint32_t state[4];	/* state (ABCD) */
  uint32_t count[2];	/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];	/* input buffer */
} I2MD5_CTX;


BEGIN_C_DECLS

void   I2MD5Init (I2MD5_CTX *);
void   I2MD5Update (I2MD5_CTX *, const unsigned char *, unsigned int);
void   I2MD5Pad (I2MD5_CTX *);
void   I2MD5Final (unsigned char [16], I2MD5_CTX *);
char * I2MD5End(I2MD5_CTX *, char *);
char * I2MD5File(const char *, char *);
char * I2MD5Data(const unsigned char *, unsigned int, char *);

END_C_DECLS

#endif /* _I2_md5_h_ */
