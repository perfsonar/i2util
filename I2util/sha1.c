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
 *    File:         sha1.c
 *
 *    Author:       Jeff W. Boote
 *                  Internet2
 *
 *    Date:         Wed Sep 27 15:23:54 MDT 2006
 *
 *    Description:    
 *
 *    Public domain version of SHA-1 from 'entropy'. Modifying
 *    it as needed... (Will change to I2 symbols - and update for C99
 *    compliance over time.)
 */
/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 *
 * Test Vectors (from FIPS PUB 180-1)
 * "abc"
 *   A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
 * "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
 *   84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
 * A million repetitions of "a"
 *   34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
 */

/* $Id$ */
#define SHA1HANDSOFF        /* Copies data before messing with it. */

#include <I2util/sha1.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 * XXX I'm tempted to remove all of the loop unrolling nonsense...
 * (This kind of optimization is best done by a compiler!)
 * -jwb
 */
#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/*
 * blk0() and blk() perform the initial expand.
 * I got the idea of expanding during the round function from SSLeay
 */
#if BYTE_ORDER == LITTLE_ENDIAN
# define blk0(i) (block[i] = (rol(block[i],24)&0xFF00FF00) \
        |(rol(block[i],8)&0x00FF00FF))
#else
# define blk0(i) block[i]
#endif
#define blk(i) (block[i&15] = rol(block[(i+13)&15]^block[(i+8)&15] \
            ^block[(i+2)&15]^block[i&15],1))

/*
 * (R0+R1), R2, R3, R4 are the different operations (rounds) used in SHA1
 */
#define R0(v,w,x,y,z,i) \
    z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) \
    z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) \
    z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) \
    z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) \
    z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

/*
 * Hash a single 512-bit block. This is the core of the algorithm.
 */
static void sha1_transform(
        u_int32_t       state[5],
        const u_int8_t  buffer[I2SHA1_BLOCK_SIZE]
        )
{
    u_int32_t a, b, c, d, e;
    u_int32_t block[16];

    assert(buffer != 0);
    assert(state != 0);

    /* Copy buffer into integer array */
    (void)memcpy(block, buffer, I2SHA1_BLOCK_SIZE);

    /* Copy sha1->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
    R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
    R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
    R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
    R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);

    /* Add the working vars back into context.state[] */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;

    return;
}

struct I2Sha1ContextRec {
    I2ErrHandle eh;
    u_int32_t   state[5];
    u_int32_t   count[2];
    u_int8_t    buffer[I2SHA1_BLOCK_SIZE];
};

void
I2Sha1Free(
        I2Sha1Context   ctx
        )
{
    if(!ctx)
        return;

    free(ctx);

    return;
}

I2Sha1Context
I2Sha1Alloc(
        I2ErrHandle eh
        )
{
    I2Sha1Context   ctx = (I2Sha1Context)calloc(1,
            sizeof(struct I2Sha1ContextRec));

    if(!ctx){
        I2ErrLogT(eh,LOG_ERR,errno,
                ": calloc(1,%d): %M",sizeof(struct I2Sha1ContextRec));
        return NULL;
    }

    ctx->eh = eh;

    return ctx;
}



/*
 * sha1_init - Initialize new context
 */
void I2Sha1Init(
        I2Sha1Context   sha1
        )
{
    assert(sha1 != NULL);

    /* SHA1 initialization constants */
    sha1->state[0] = 0x67452301;
    sha1->state[1] = 0xEFCDAB89;
    sha1->state[2] = 0x98BADCFE;
    sha1->state[3] = 0x10325476;
    sha1->state[4] = 0xC3D2E1F0;
    sha1->count[0] = sha1->count[1] = 0;

    return;
}


/*
 * Run your data through this.
 * XXX - The bits/bytes translation makes this code more complicated than
 * it needs to be. Is there any reason not to just keep track in bytes
 * until the end?
 */
void I2Sha1Append(
        I2Sha1Context   sha1,
        const uint8_t   *data,
        uint32_t        len
        )
{
    u_int i, j;

    assert(sha1 != NULL);

    if (len <= 0)
        return;

    /*
     * Count is used to keep track of the 64 bit length
     * of message - in bits). See fips 180-1.
     * Overflow in count[0] is added into count[1].
     */
    j = sha1->count[0];
    if((sha1->count[0] += (len << 3)) < j){
        sha1->count[1] += ((len>>29)+1);
    }

    /*
     * If this 'len' fills a complete 512 bit block, then call transform on it.
     */
    j = (j >> 3) & 63;
    if ((j + len) > 63) {
        (void)memcpy(&sha1->buffer[j], data, (i = 64-j));
        sha1_transform(sha1->state, sha1->buffer);
        for ( /* */ ; i + 63 < len; i += 64){
            sha1_transform(sha1->state, &data[i]);
        }
        j = 0;
    } else {
        i = 0;
    }

    /*
     * copy the remaining bits of data that have not created a complete block
     * yet into the buffer so they can be processed on the next call, or
     * during Finish.
     */
    (void)memcpy(&sha1->buffer[j], &data[i], len - i);

    return;
}


/*
 * Add padding and return the message digest.
 */
void I2Sha1Finish(
        I2Sha1Context   sha1,
        uint8_t         *digest_ret
        )
{
    uint32_t i;
    uint8_t finalcount[8];

    assert(digest_ret != NULL);
    assert(sha1 != NULL);

    /*
     * Pull 64 bit count from sha1->count, and put it in finalcount
     */
    for (i = 0; i < 8; i++) {
        /* Endian independent */
        finalcount[i] = (uint8_t)((sha1->count[(i >= 4 ? 0 : 1)]
                                            >> ((3-(i & 3)) * 8) ) & 255);
    }

    /*
     * Append octal 200 (10000000) - End of message byte for SHA-1
     */
    I2Sha1Append(sha1, (const void *)"\200", 1);

    /*
     * Fill remaining block (except last 64 bits) with 0's
     */
    while ((sha1->count[0] & 504) != 448)
        I2Sha1Append(sha1, (const void *)"\0", 1);

    /*
     * Put 'count' in final 64 bits
     */
    I2Sha1Append(sha1, finalcount, 8);  /* Should cause a SHA1Transform() */

    /*
     * Return digest
     */
    for (i = 0; i < I2SHA1_DIGEST_SIZE; i++)
            digest_ret[i] = (u_int8_t)
                ((sha1->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);

    /* Wipe variables */
    memset(sha1, 0, sizeof(*sha1));

    return;
}

void I2Sha1(
        const uint8_t   *data,
        uint32_t        len,
        uint8_t         *digest_ret
        )
{
    /*
     * Don't really do an Alloc - just create the record on the stack
     */
    struct I2Sha1ContextRec sha1rec;
    I2Sha1Context           sha1 = &sha1rec;

    memset((void*)sha1,0,sizeof(*sha1));

    I2Sha1Init(sha1);
    I2Sha1Append(sha1,data,len);
    I2Sha1Finish(sha1,digest_ret);

    return;
}
