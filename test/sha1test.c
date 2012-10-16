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
 *    File:         sha1test.c
 *
 *    Author:       Jeff W. Boote
 *                  Internet2
 *
 *    Date:         Fri Sep 29 18:48:51 MDT 2006
 *
 *    Description:    
 *                  Use the test vectors from fips 180-1 to test the
 *                  sha-1 implementation.
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <I2util/sha1.h>

int
main(
        int     argc    __attribute__((unused)),
        char    **argv
    ) {
    char                *progname;
    I2LogImmediateAttr  ia;
    I2ErrHandle         eh;
    int                 status = 0;
    char   *in[] =
                    {"abc",
                    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
                    NULL,
                    NULL};
    char                *out[] = 
                    {"A9993E364706816ABA3E25717850C26C9CD0D89D",
                    "84983E441C3BD26EBAAE4AA1F95129E5E54670F1",
                    "34AA973CD4C4DAA4F61EEB2BDBAD27316534016F",
                    NULL};
    char            *ststr;
    uint8_t         digest[I2SHA1_DIGEST_SIZE];
    uint8_t         result[I2SHA1_DIGEST_SIZE];
    char            hd[(2*I2SHA1_DIGEST_SIZE)+1];

    ia.line_info = (I2NAME | I2MSG);
#ifndef        NDEBUG
    ia.line_info |= (I2LINE | I2FILE);
#endif
    ia.fp = stderr;

    progname = (progname = strrchr(argv[0], '/')) ? progname+1 : *argv;

    /*
     * Start an error logging session for reporing errors to the
     * standard error
     */
    eh = I2ErrOpen(progname, I2ErrLogImmediate, &ia, NULL, NULL);
    if(! eh) {
        fprintf(stderr, "%s : Couldn't init error module\n", progname);
        exit(1);
    }


    /*
     * First test vector
     */
    I2Sha1((uint8_t*)in[0],strlen(in[0]),result);

    I2HexDecode(out[0],digest,I2SHA1_DIGEST_SIZE);
    if(memcmp(digest,result,I2SHA1_DIGEST_SIZE)){
        status = 1;
        ststr = "FAILED";
    }
    else{
        ststr = "SUCCESS";
    }

    hd[2*I2SHA1_DIGEST_SIZE] = '\0';
    I2HexEncode(hd,result,I2SHA1_DIGEST_SIZE);

    fprintf(stdout,"%s: sha1(\"%s\") = %s\n",ststr,in[0],hd);

    /*
     * Second test vector
     */
    I2Sha1((uint8_t*)in[1],strlen(in[1]),result);

    I2HexDecode(out[1],digest,I2SHA1_DIGEST_SIZE);
    if(memcmp(digest,result,I2SHA1_DIGEST_SIZE)){
        status = 1;
        ststr = "FAILED";
    }
    else{
        ststr = "SUCCESS";
    }

    hd[2*I2SHA1_DIGEST_SIZE] = '\0';
    I2HexEncode(hd,result,I2SHA1_DIGEST_SIZE);

    fprintf(stdout,"%s: sha1(\"%s\") = %s\n",ststr,in[1],hd);

    /*
     * Third test vector
     * (1,000,000 "a"'s)
     */
    in[2] = malloc(1000000*sizeof(char));
    if(!in[2]){
        perror("Unable to malloc 1000000 char's for 3rd test vector");
        exit(1);
    }
    memset(in[2],'a',1000000);
    I2Sha1((uint8_t*)in[2],1000000,result);

    I2HexDecode(out[2],digest,I2SHA1_DIGEST_SIZE);
    if(memcmp(digest,result,I2SHA1_DIGEST_SIZE)){
        status = 1;
        ststr = "FAILED";
    }
    else{
        ststr = "SUCCESS";
    }

    hd[2*I2SHA1_DIGEST_SIZE] = '\0';
    I2HexEncode(hd,result,I2SHA1_DIGEST_SIZE);

    fprintf(stdout,"%s: sha1(\"%s\") = %s\n",ststr,"1000000 a\'s",hd);

    exit(status);
}
