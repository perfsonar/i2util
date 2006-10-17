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
 *    File:         pbkdf2.c
 *
 *    Author:       Jeff W. Boote
 *                  Internet2
 *
 *    Date:         Fri Oct 13 03:37:53 MDT 2006
 *
 *    Description:    
 *
 *    Implementation of PBKDF2 from RFC2898:
 *
 *
 *    5.2 PBKDF2
 *    
 *       PBKDF2 applies a pseudorandom function (see Appendix B.1 for an
 *       example) to derive keys. The length of the derived key is essentially
 *       unbounded. (However, the maximum effective search space for the
 *       derived key may be limited by the structure of the underlying
 *       pseudorandom function. See Appendix B.1 for further discussion.)
 *       PBKDF2 is recommended for new applications.
 *    
 *       PBKDF2 (P, S, c, dkLen)
 *    
 *       Options:        PRF        underlying pseudorandom function (hLen
 *                                  denotes the length in octets of the
 *                                  pseudorandom function output)
 *    
 *       Input:          P          password, an octet string
 *                       S          salt, an octet string
 *                       c          iteration count, a positive integer
 *                       dkLen      intended length in octets of the derived
 *                                  key, a positive integer, at most
 *                                  (2^32 - 1) * hLen
 *    
 *       Output:         DK         derived key, a dkLen-octet string
 *    
 *       Steps:
 *    
 *          1. If dkLen > (2^32 - 1) * hLen, output "derived key too long" and
 *             stop.
 *    
 *          2. Let l be the number of hLen-octet blocks in the derived key,
 *             rounding up, and let r be the number of octets in the last
 *             block:
 *    
 *                       l = CEIL (dkLen / hLen) ,
 *                       r = dkLen - (l - 1) * hLen .
 *    
 *             Here, CEIL (x) is the "ceiling" function, i.e. the smallest
 *             integer greater than, or equal to, x.
 *    
 *          3. For each block of the derived key apply the function F defined
 *             below to the password P, the salt S, the iteration count c, and
 *             the block index to compute the block:
 *    
 *                       T_1 = F (P, S, c, 1) ,
 *                       T_2 = F (P, S, c, 2) ,
 *                       ...
 *                       T_l = F (P, S, c, l) ,
 *    
 *             where the function F is defined as the exclusive-or sum of the
 *             first c iterates of the underlying pseudorandom function PRF
 *             applied to the password P and the concatenation of the salt S
 *             and the block index i:
 *    
 *    
 *                       F (P, S, c, i) = U_1 \xor U_2 \xor ... \xor U_c
 *    
 *             where
 *    
 *                       U_1 = PRF (P, S || INT (i)) ,
 *                       U_2 = PRF (P, U_1) ,
 *                       ...
 *                       U_c = PRF (P, U_{c-1}) .
 *    
 *             Here, INT (i) is a four-octet encoding of the integer i, most
 *             significant octet first.
 *    
 *          4. Concatenate the blocks and extract the first dkLen octets to
 *             produce a derived key DK:
 *    
 *                       DK = T_1 || T_2 ||  ...  || T_l<0..r-1>
 *    
 *          5. Output the derived key DK.
 *    
 *       Note. The construction of the function F follows a "belt-and-
 *       suspenders" approach. The iterates U_i are computed recursively to
 *       remove a degree of parallelism from an opponent; they are exclusive-
 *       ored together to reduce concerns about the recursion degenerating
 *       into a small set of values.
 *    
 */
#include <I2util/pbkdf2.h>

#include <stdlib.h>
#include <string.h>

/*
 * Function:    F
 *
 * Description:    
 *              Used in step 3 to compute each block of derived key.
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
static void F(
        I2prf       prf,
        uint32_t    prf_hlen,
        uint8_t     *pw,
        uint32_t    pwlen,
        uint8_t     *salt,
        uint32_t    saltlen,
        uint32_t    count,
        uint32_t    i,
        uint8_t     *buffer,
        uint8_t     *u
        )
{
    uint8_t     ival[4];
    uint32_t    j,k;

    /*
     * U_1 = PRF(P, concat(S, INT(i)))
     */
    ival[0] = (i >> 24) & 0xff;
    ival[1] = (i >> 16) & 0xff;
    ival[2] = (i >> 8) & 0xff;
    ival[3] = i & 0xff;

    /*
     * Input of U_1 prf is concat(S, INT(i)), Output hmac put in buffer
     */
    memcpy(buffer,salt,saltlen);
    memcpy(buffer+saltlen,ival,4);
    (*prf)(pw,pwlen,buffer,saltlen+4,buffer);

    /*
     * Initialize return u with U_1
     */
    memcpy(u,buffer,prf_hlen);

    /*
     * U_2..U_{count}
     */
    for(j=2; j<=count; j++){
        /*
         * Input of U_{j} prf is U_{j-1} or buffer,
         * Output is put back in buffer for the next time around and is
         * immediately used to xor into the F result.
         */
        (*prf)(pw,pwlen,buffer,prf_hlen,buffer);
        for(k=0; k<prf_hlen; k++){
            u[k] ^= buffer[k];
        }
    }

    return;
}

/*
 * Function:    I2pbkdf2
 *
 * Description:    
 *          Implementation of pbkdf2 algorithm from RFC 2898.
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
int I2pbkdf2(
        I2prf       prf,
        uint32_t    prf_hlen,
        uint8_t     *pw,
        uint32_t    pwlen,
        uint8_t     *salt,
        uint32_t    saltlen,
        uint32_t    count,
        uint32_t    dklen,
        uint8_t     *dk_ret
        )
{
    int         rc = 0;
    uint32_t    l,r;
    uint32_t    i;
    uint8_t     *tmpbuff=NULL; /* Intermediate memspace for F */
    uint8_t     *outbuff=NULL; /* Results of F iteration */
    uint8_t     *out;

    /*
     * Make sure derived key is reasonably small with relation to the
     * selected PRF. (Step 1)
     * (This is a no-op. It is not possible for uint32_t to be
     * larger than 0xffffffff. Leaving here def'd out in case
     * the type constrants are changed.)
     */
#if 0
    if((dklen / prf_hlen) > 0xffffffff){
        rc = EINVAL;
        goto end;
    }
#endif

    /*
     * Allocate needed memory for complete algorithm at this level to
     * catch errors early.
     */
    if( !(tmpbuff = calloc(MAX(saltlen,prf_hlen)+4,sizeof(uint8_t))) ||
            !(outbuff = calloc(prf_hlen,sizeof(uint8_t)))){
        rc = ENOMEM;
        goto end;
    }

    /*
     * Step 2
     *  Let l = number of complete blocks
     *  Let r = number of bytes in any final incomplete block
     */
    l = dklen / prf_hlen;
    r = dklen % prf_hlen;


    /*
     * Step 3 and 4
     * 3) iterate over all complete blocks to compute complete prf_hlen blocks
     * 4) increment 'out' to concatenate each resulting T_{i}.
     *
     * (Passing pointer directly into dk_ret, to minimize copies.)
     */
    for(i=0; i<l; i++){
        out = dk_ret + (i * prf_hlen);
        F(prf,prf_hlen,pw,pwlen,salt,saltlen,count,i+1,tmpbuff,out);
    }

    /*
     * do any last incomplete block
     * - outbuff used to hold complete prf_hlen bytes, but only
     * - truncated result is used in dk_ret
     *
     * (Have to copy result because algorithm works on complete prf_hlen
     * blocks - and dk_ret is not long enough to hold.)
     */
    if(r){
        F(prf,prf_hlen,pw,pwlen,salt,saltlen,count,l+1,tmpbuff,outbuff);
        out = dk_ret + (l * prf_hlen);
        memcpy(out,outbuff,r);
    }

    /*
     * Step 5 - Wrote T_{i} results directly to dk_ret, so just clean-up
     * and return.
     */
        
end:
    if(tmpbuff) free(tmpbuff);
    if(outbuff) free(outbuff);

    if(rc){
        errno = rc;
        return -1;
    }

    return 0;
}
