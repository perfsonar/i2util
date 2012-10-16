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
 *    File:         hmac-sha1.c
 *
 *    Author:       Jeff W. Boote
 *                  Internet2
 *
 *    Date:         Sat Oct 07 12:35:24 MDT 2006
 *
 *    Description:    
 *    HMAC-SHA1 implementation. From rfc2104:
 * 2. Definition of HMAC
 *
 * The definition of HMAC requires a cryptographic hash function, which
 * we denote by H, and a secret key K. We assume H to be a cryptographic
 * hash function where data is hashed by iterating a basic compression
 * function on blocks of data.   We denote by B the byte-length of such
 * blocks (B=64 for all the above mentioned examples of hash functions),
 * and by L the byte-length of hash outputs (L=16 for MD5, L=20 for
 * SHA-1).  The authentication key K can be of any length up to B, the
 * block length of the hash function.  Applications that use keys longer
 * than B bytes will first hash the key using H and then use the
 * resultant L byte string as the actual key to HMAC. In any case the
 * minimal recommended length for K is L bytes (as the hash output
 * length). See section 3 for more information on keys.
 *
 * We define two fixed and different strings ipad and opad as follows
 * (the 'i' and 'o' are mnemonics for inner and outer):
 *
 *                 ipad = the byte 0x36 repeated B times
 *                opad = the byte 0x5C repeated B times.
 *
 * To compute HMAC over the data `text' we perform
 *
 *                  H(K XOR opad, H(K XOR ipad, text))
 *
 * Namely,
 *
 *  (1) append zeros to the end of K to create a B byte string
 *      (e.g., if K is of length 20 bytes and B=64, then K will be
 *       appended with 44 zero bytes 0x00)
 *  (2) XOR (bitwise exclusive-OR) the B byte string computed in step
 *      (1) with ipad
 *  (3) append the stream of data 'text' to the B byte string resulting
 *      from step (2)
 *  (4) apply H to the stream generated in step (3)
 *  (5) XOR (bitwise exclusive-OR) the B byte string computed in
 *      step (1) with opad
 *  (6) append the H result from step (4) to the B byte string
 *      resulting from step (5)
 *  (7) apply H to the stream generated in step (6) and output
 *      the result
 *
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
#include <I2util/hmac-sha1.h>
#include <I2util/sha1P.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 * The opad is computed using the key during init. It is not used until
 * 'final' after that. ipad is used to generate the 'inner' sha1 hash.
 * This is stored in sha1 until 'final' when the hash is generated, and then
 * the sha1 is reinitialized to generate the 'outer' hash value.
 */
struct I2HMACSha1ContextRec{
    I2ErrHandle     eh;
    I2Sha1Context   sha1;
    uint8_t         opad[I2SHA1_BLOCK_SIZE];
};

I2HMACSha1Context I2HMACSha1Alloc(
         I2ErrHandle    eh
         )
{
    I2HMACSha1Context   hmac = (I2HMACSha1Context)calloc(1,
            sizeof(struct I2HMACSha1ContextRec));

    if(hmac){
        hmac->sha1 = I2Sha1Alloc(eh);
    }

    if(!hmac || !hmac->sha1){
        I2ErrLogT(eh,LOG_ERR,errno,
                ": calloc(1,%d): %M",sizeof(struct I2HMACSha1ContextRec));
        return NULL;
    }

    hmac->eh = eh;

    return hmac;
}

void I2HMACSha1Free(
        I2HMACSha1Context   hmac
        )
{
    if(!hmac)
        return;

    I2Sha1Free(hmac->sha1);
    free(hmac);

    return;
}

void I2HMACSha1Init(
        I2HMACSha1Context   hmac,
        const uint8_t       *inkey,
        uint32_t            len
        )
{
    uint8_t     ipad[I2SHA1_BLOCK_SIZE];
    uint8_t     key[I2SHA1_BLOCK_SIZE];
    uint32_t    keylen;
    uint32_t    i;

    assert(hmac);
    assert(hmac->sha1);

    memset(key,0x00,I2SHA1_BLOCK_SIZE);
    memset(ipad,0x36,I2SHA1_BLOCK_SIZE);
    memset(hmac->opad,0x5c,I2SHA1_BLOCK_SIZE);

    /*
     * If the key is larger than the hash block size, then hash it and
     * use the hash value as the key.
     */
    if(len > I2SHA1_BLOCK_SIZE){
        I2Sha1(inkey,len,key);
        keylen = I2SHA1_DIGEST_SIZE;
    }
    else{
        memcpy(key,inkey,len);
        keylen = len;
    }

    /*
     * Now do the XOR for ipad and opad. opad will be used later - ipad
     * will be used as the first data to be hashed by sha1.
     */
    for(i=0; i< I2SHA1_BLOCK_SIZE; i++){
        ipad[i] ^= key[i];
        hmac->opad[i] ^= key[i];
    }

    I2Sha1Init(hmac->sha1);
    I2Sha1Append(hmac->sha1,ipad,I2SHA1_BLOCK_SIZE);

    return;
}

void I2HMACSha1Append(
        I2HMACSha1Context   hmac,
        const uint8_t   *txt,
        uint32_t        len
        )
{
    assert(hmac);
    assert(txt);
    assert(len);

    I2Sha1Append(hmac->sha1,txt,len);
}

void I2HMACSha1Finish(
        I2HMACSha1Context   hmac,
        uint8_t         *digest_ret /* I2HMAC_SHA1_DIGEST_SIZE */
        )
{
    uint8_t opad_digest[I2SHA1_DIGEST_SIZE];

    assert(hmac);

    /*
     * Get the inner-pad digest
     */
    I2Sha1Finish(hmac->sha1,opad_digest);

    /*
     * Reset the sha1 to use for computing the outer-pad digest
     */
    I2Sha1Init(hmac->sha1);
    I2Sha1Append(hmac->sha1,hmac->opad,I2SHA1_BLOCK_SIZE);
    I2Sha1Append(hmac->sha1,opad_digest,I2SHA1_DIGEST_SIZE);
    I2Sha1Finish(hmac->sha1,digest_ret);

    /*
     * clean heap
     */
    memset(opad_digest,0,I2SHA1_DIGEST_SIZE);

    return;
}

void I2HMACSha1(
        const uint8_t   *key,
        uint32_t        keylen,
        const uint8_t   *txt,
        uint32_t        txtlen,
        uint8_t         *digest_ret
        )
{
    /*
     * Don't really do an alloc - just create the record on the stack
     * (this means 
     */
    struct I2Sha1ContextRec     sha1rec;
    struct I2HMACSha1ContextRec hmacrec;
    I2HMACSha1Context           hmac;

    memset((void*)&sha1rec,0,sizeof(sha1rec));
    memset((void*)&hmacrec,0,sizeof(hmacrec));

    hmacrec.sha1 = &sha1rec;
    hmac = &hmacrec;

    I2HMACSha1Init(hmac,key,keylen);
    I2HMACSha1Append(hmac,txt,txtlen);
    I2HMACSha1Finish(hmac,digest_ret);

    return;
}
