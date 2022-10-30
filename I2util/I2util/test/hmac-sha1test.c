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
 *    File:         hmac-sha1test.c
 *
 *    Author:       Jeff W. Boote
 *                  Internet2
 *
 *    Date:         Fri Sep 29 18:48:51 MDT 2006
 *
 *    Description:    
 *                  Use the test vectors from RFC2202 to test the
 *                  sha-1 implementation:
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

#include <I2util/hmac-sha1.h>

int status = 0;

static void DoTestCase(
        int         test_case,
        uint8_t     *key,
        uint32_t    keylen,
        uint8_t     *txt,
        uint32_t    txtlen,
        char        *dtxt,
        char        *datadesc)
{
    uint8_t         digest[I2SHA1_DIGEST_SIZE];
    uint8_t         result[I2SHA1_DIGEST_SIZE];
    char            hd[(2*I2SHA1_DIGEST_SIZE)+1];
    char            *ststr;

    I2HMACSha1(key,keylen,txt,txtlen,result);

    I2HexDecode(dtxt,digest,I2SHA1_DIGEST_SIZE);
    if(memcmp(digest,result,I2SHA1_DIGEST_SIZE)){
        status = 1;
        ststr = "FAILED";
    }
    else{
        ststr = "SUCCESS";
    }

    hd[2*I2SHA1_DIGEST_SIZE] = '\0';
    I2HexEncode(hd,result,I2SHA1_DIGEST_SIZE);

    fprintf(stdout,"%d: %s: hmac-sha1(\"%s\") = %s\n",test_case,ststr,
            ((datadesc)? datadesc: (char*)txt),hd);

    return;
}

int
main(
        int     argc    __attribute__((unused)),
        char    **argv
    ) {
    char                *progname;
    I2LogImmediateAttr  ia;
    I2ErrHandle         eh;
    uint8_t         key[80];
    uint8_t         txt[74];

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
 *
 *    3. Test Cases for HMAC-SHA-1
 *    
 */

/*
 *    test_case =     1
 *    key =           0x0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b
 *    key_len =       20
 *    data =          "Hi There"
 *    data_len =      8
 *    digest =        0xb617318655057264e28bc0b6fb378c8ef146be00
 */
    memset(key,0x0b,20);
    strcpy((char*)txt,"Hi There");
    DoTestCase(1,key,20,txt,8,
            "b617318655057264e28bc0b6fb378c8ef146be00",NULL);

/*
 *    test_case =     2
 *    key =           "Jefe"
 *    key_len =       4
 *    data =          "what do ya want for nothing?"
 *    data_len =      28
 *    digest =        0xeffcdf6ae5eb2fa2d27416d5f184df9c259a7c79
 */
    strcpy((char*)key,"Jefe");
    strcpy((char*)txt,"what do ya want for nothing?");
    DoTestCase(2,key,4,txt,28,"effcdf6ae5eb2fa2d27416d5f184df9c259a7c79",NULL);

/*
 *    test_case =     3
 *    key =           0xaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
 *    key_len =       20
 *    data =          0xdd repeated 50 times
 *    data_len =      50
 *    digest =        0x125d7342b9ac11cd91a39af48aa17b4f63f175d3
 */
    memset(key,0xaa,20);
    memset(txt,0xdd,50);
    DoTestCase(3,key,20,txt,50,
            "125d7342b9ac11cd91a39af48aa17b4f63f175d3","0xdd * 50");

/*
 *    test_case =     4
 *    key =           0x0102030405060708090a0b0c0d0e0f10111213141516171819
 *    key_len =       25
 *    data =          0xcd repeated 50 times
 *    data_len =      50
 *    digest =        0x4c9007f4026250c6bc8414f9bf50c86c2d7235da
 */
    I2HexDecode("0102030405060708090a0b0c0d0e0f10111213141516171819",key,25);
    memset(txt,0xcd,50);
    DoTestCase(4,key,25,txt,50,"4c9007f4026250c6bc8414f9bf50c86c2d7235da",
            "0xcd * 50");

/*
 *    test_case =     5
 *    key =           0x0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c
 *    key_len =       20
 *    data =          "Test With Truncation"
 *    data_len =      20
 *    digest =        0x4c1a03424b55e07fe7f27be1d58bb9324a9a5a04
 *    digest-96 =     0x4c1a03424b55e07fe7f27be1
 */
    memset(key,0x0c,20);
    strcpy((char *)txt,"Test With Truncation");
    DoTestCase(5,key,20,txt,20,"4c1a03424b55e07fe7f27be1d58bb9324a9a5a04",NULL);

/*
 *    
 *    test_case =     6
 *    key =           0xaa repeated 80 times
 *    key_len =       80
 *    data =          "Test Using Larger Than Block-Size Key - Hash Key First"
 *    data_len =      54
 *    digest =        0xaa4ae5e15272d00e95705637ce8a3b55ed402112
 */
    memset(key,0xaa,80);
    strcpy((char *)txt,"Test Using Larger Than Block-Size Key - Hash Key First");
    DoTestCase(6,key,80,txt,54,"aa4ae5e15272d00e95705637ce8a3b55ed402112",NULL);

/*
 *    
 *    test_case =     7
 *    key =           0xaa repeated 80 times
 *    key_len =       80
 *    data =          "Test Using Larger Than Block-Size Key and Larger
 *                    Than One Block-Size Data"
 *    data_len =      73
 *    digest =        0xe8e99d0f45237d786d6bbaa7965c7808bbff1a91
 */
    memset(key,0xaa,80);
    strcpy((char *)txt,
"Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data");
    DoTestCase(7,key,80,txt,73,"e8e99d0f45237d786d6bbaa7965c7808bbff1a91",NULL);

    exit(status);
}
