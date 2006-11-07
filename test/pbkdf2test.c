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
 *    File:         pbkdftest.c
 *
 *    Author:       Jeff W. Boote
 *                  Internet2
 *
 *    Date:         Fri Oct 13 19:53:35 MDT 2006
 *
 *    Description:    
 *      Current test vectors are bogus. I created them - but they will
 *      at least let me test on multiple hosts architectures to make
 *      sure things are consistent.
 *      Test vectors from RFC 3962.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <I2util/pbkdf2.h>
#include <I2util/hmac-sha1.h>

int status = 0;

#ifndef tvalsub
#define tvalsub(a, b)                   \
    do {                                \
        (a)->tv_sec -= (b)->tv_sec;     \
        (a)->tv_usec -= (b)->tv_usec;   \
        if((a)->tv_usec < 0){           \
            (a)->tv_sec--;              \
            (a)->tv_usec += 1000000;    \
        }                               \
    } while(0)
#endif

static void DoTestCase(
        int         test_case,
        char        *pw,
        uint32_t    pwlen,
        char        *salt,
        uint32_t    saltlen,
        uint32_t    count,
        char        *dk,
        uint32_t    dklen,
        char        *pwbuff  /* "nice" printable pw */
        )
{
    /*
     * Largest dk supported currently is 256-bits (32 bytes)
     */
    uint8_t         verify[32];
    uint8_t         result[32];
    char            hd[(2*32)+1];
    char            *ststr;
    struct timeval  start;
    struct timeval  end;
    double          exectime;

    assert(dklen <= 32);

    memset(&start,0,sizeof(start));
    memset(&end,0,sizeof(end));

    /*
     * First test vector
     */
    gettimeofday(&start,NULL);
    if( (I2pbkdf2(I2HMACSha1,(uint32_t)I2SHA1_DIGEST_SIZE,(uint8_t *)pw,pwlen,
                    (uint8_t *)salt,saltlen,count,dklen,result))){
        perror("I2pbkdf2: ");
        exit(1);
    }
    gettimeofday(&end,NULL);
    tvalsub(&end,&start);
    exectime = end.tv_usec / 1000000.0;
    exectime += end.tv_sec;

    if(dk){
        I2HexDecode(dk,verify,dklen);
        if(memcmp(verify,result,dklen)){
            status = 1;
            ststr = "FAILED";
        }
        else{
            ststr = "SUCCESS";
        }

        hd[2*dklen] = '\0';
        I2HexEncode(hd,result,dklen);
    }
    else{
        ststr = "UNCHECKED";
        strcpy(hd,"\'N/A\'");
    }

    fprintf(stdout,
            "%d: %s: pbkdf2(pw=\"%s\",s=\"%s\",c=%d,dklen=%d) = %s (%g sec)\n",
            test_case,ststr,pwbuff?pwbuff:pw,salt,count,dklen,hd,exectime);

    return;
}

int
main(
        int     argc    __attribute__((unused)),
        char    **argv  __attribute__((unused))
    ) {
    int     i=1;
    char    saltbuff[8];
    char    passbuff[65];

    /*
     * Use test vectors from RFC 3962 to test pbkdf2 impl.
     */

    /*
     *  Iteration count = 1
     *  Pass phrase = "password"
     *  Salt = "ATHENA.MIT.EDUraeburn"
     *  128-bit PBKDF2 output:
     *      cd ed b5 28 1b b2 f8 01 56 5a 11 22 b2 56 35 15
     *  128-bit AES key:
     *      42 26 3c 6e 89 f4 fc 28 b8 df 68 ee 09 79 9f 15
     *  256-bit PBKDF2 output:
     *      cd ed b5 28 1b b2 f8 01 56 5a 11 22 b2 56 35 15
     *      0a d1 f7 a0 4b b9 f3 a3 33 ec c0 e2 e1 f7 08 37
     *  256-bit AES key:
     *      fe 69 7b 52 bc 0d 3c e1 44 32 ba 03 6a 92 e6 5b
     *      bb 52 28 09 90 a2 fa 27 88 39 98 d7 2a f3 01 61
     */
    DoTestCase(i++,"password",8,"ATHENA.MIT.EDUraeburn",21,1,
            "cdedb5281bb2f801565a1122b2563515",16,NULL);
    DoTestCase(i++,"password",8,"ATHENA.MIT.EDUraeburn",21,1,
            "cdedb5281bb2f801565a1122b25635150ad1f7a04bb9f3a333ecc0e2e1f70837",
            32,NULL);

    /*
     *
     *  Iteration count = 2
     *  Pass phrase = "password"
     *  Salt="ATHENA.MIT.EDUraeburn"
     *  128-bit PBKDF2 output:
     *      01 db ee 7f 4a 9e 24 3e 98 8b 62 c7 3c da 93 5d
     *  128-bit AES key:
     *      c6 51 bf 29 e2 30 0a c2 7f a4 69 d6 93 bd da 13
     *  256-bit PBKDF2 output:
     *      01 db ee 7f 4a 9e 24 3e 98 8b 62 c7 3c da 93 5d
     *      a0 53 78 b9 32 44 ec 8f 48 a9 9e 61 ad 79 9d 86
     *  256-bit AES key:
     *      a2 e1 6d 16 b3 60 69 c1 35 d5 e9 d2 e2 5f 89 61
     *      02 68 56 18 b9 59 14 b4 67 c6 76 22 22 58 24 ff
     */
    DoTestCase(i++,"password",8,"ATHENA.MIT.EDUraeburn",21,2,
            "01dbee7f4a9e243e988b62c73cda935d",16,NULL);
    DoTestCase(i++,"password",8,"ATHENA.MIT.EDUraeburn",21,2,
            "01dbee7f4a9e243e988b62c73cda935da05378b93244ec8f48a99e61ad799d86",
            32,NULL);

    /*
     *  Iteration count = 1200
     *  Pass phrase = "password"
     *  Salt = "ATHENA.MIT.EDUraeburn"
     *  128-bit PBKDF2 output:
     *      5c 08 eb 61 fd f7 1e 4e 4e c3 cf 6b a1 f5 51 2b
     *  128-bit AES key:
     *      4c 01 cd 46 d6 32 d0 1e 6d be 23 0a 01 ed 64 2a
     *  256-bit PBKDF2 output:
     *      5c 08 eb 61 fd f7 1e 4e 4e c3 cf 6b a1 f5 51 2b
     *      a7 e5 2d db c5 e5 14 2f 70 8a 31 e2 e6 2b 1e 13
     *  256-bit AES key:
     *      55 a6 ac 74 0a d1 7b 48 46 94 10 51 e1 e8 b0 a7
     *      54 8d 93 b0 ab 30 a8 bc 3f f1 62 80 38 2b 8c 2a
     */
    DoTestCase(i++,"password",8,"ATHENA.MIT.EDUraeburn",21,1200,
            "5c08eb61fdf71e4e4ec3cf6ba1f5512b",
            16,NULL);
    DoTestCase(i++,"password",8,"ATHENA.MIT.EDUraeburn",21,1200,
            "5c08eb61fdf71e4e4ec3cf6ba1f5512ba7e52ddbc5e5142f708a31e2e62b1e13",
            32,NULL);

    /*
     *  Iteration count = 5
     *  Pass phrase = "password"
     *  Salt=0x1234567878563412
     *  128-bit PBKDF2 output:
     *      d1 da a7 86 15 f2 87 e6 a1 c8 b1 20 d7 06 2a 49
     *  128-bit AES key:
     *      e9 b2 3d 52 27 37 47 dd 5c 35 cb 55 be 61 9d 8e
     *  256-bit PBKDF2 output:
     *      d1 da a7 86 15 f2 87 e6 a1 c8 b1 20 d7 06 2a 49
     *      3f 98 d2 03 e6 be 49 a6 ad f4 fa 57 4b 6e 64 ee
     *  256-bit AES key:
     *      97 a4 e7 86 be 20 d8 1a 38 2d 5e bc 96 d5 90 9c
     *      ab cd ad c8 7c a4 8f 57 45 04 15 9f 16 c3 6e 31
     *  (This test is based on values given in [PECMS].)
     */
    I2HexDecode("1234567878563412",(uint8_t *)saltbuff,8);
    DoTestCase(i++,"password",8,saltbuff,8,5,
            "d1daa78615f287e6a1c8b120d7062a49",
            16,NULL);
    DoTestCase(i++,"password",8,saltbuff,8,5,
            "d1daa78615f287e6a1c8b120d7062a493f98d203e6be49a6adf4fa574b6e64ee",
            32,NULL);

    /*
     *  Iteration count = 1200
     *  Pass phrase = (64 characters)
     *    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
     *  Salt="pass phrase equals block size"
     *  128-bit PBKDF2 output:
     *      13 9c 30 c0 96 6b c3 2b a5 5f db f2 12 53 0a c9
     *  128-bit AES key:
     *      59 d1 bb 78 9a 82 8b 1a a5 4e f9 c2 88 3f 69 ed
     *  256-bit PBKDF2 output:
     *      13 9c 30 c0 96 6b c3 2b a5 5f db f2 12 53 0a c9
     *      c5 ec 59 f1 a4 52 f5 cc 9a d9 40 fe a0 59 8e d1
     *  256-bit AES key:
     *      89 ad ee 36 08 db 8b c7 1f 1b fb fe 45 94 86 b0
     *      56 18 b7 0c ba e2 20 92 53 4e 56 c5 53 ba 4b 34
     */
    memset(passbuff,'X',64);
    DoTestCase(i++,passbuff,64,"pass phrase equals block size",29,1200,
            "139c30c0966bc32ba55fdbf212530ac9",
            16,"\'X\' (64 times)");
    DoTestCase(i++,passbuff,64,"pass phrase equals block size",29,1200,
            "139c30c0966bc32ba55fdbf212530ac9c5ec59f1a452f5cc9ad940fea0598ed1",
            32,"\'X\' (64 times)");

    /*   
     *  Iteration count = 1200
     *  Pass phrase = (65 characters)
     *    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
     *  Salt = "pass phrase exceeds block size"
     *  128-bit PBKDF2 output:
     *      9c ca d6 d4 68 77 0c d5 1b 10 e6 a6 87 21 be 61
     *  128-bit AES key:
     *      cb 80 05 dc 5f 90 17 9a 7f 02 10 4c 00 18 75 1d
     *  256-bit PBKDF2 output:
     *      9c ca d6 d4 68 77 0c d5 1b 10 e6 a6 87 21 be 61
     *      1a 8b 4d 28 26 01 db 3b 36 be 92 46 91 5e c8 2a
     *  256-bit AES key:
     *      d7 8c 5c 9c b8 72 a8 c9 da d4 69 7f 0b b5 b2 d2
     *      14 96 c8 2b eb 2c ae da 21 12 fc ee a0 57 40 1b
     */
    memset(passbuff,'X',65);
    DoTestCase(i++,passbuff,65,"pass phrase exceeds block size",30,1200,
            "9ccad6d468770cd51b10e6a68721be61",
            16,"\'X\' (65 times)");
    DoTestCase(i++,passbuff,65,"pass phrase exceeds block size",30,1200,
            "9ccad6d468770cd51b10e6a68721be611a8b4d282601db3b36be9246915ec82a",
            32,"\'X\' (65 times)");

    /*
     *  Iteration count = 50
     *  Pass phrase = g-clef (0xf09d849e)
     *  Salt = "EXAMPLE.COMpianist"
     *  128-bit PBKDF2 output:
     *      6b 9c f2 6d 45 45 5a 43 a5 b8 bb 27 6a 40 3b 39
     *  128-bit AES key:
     *      f1 49 c1 f2 e1 54 a7 34 52 d4 3e 7f e6 2a 56 e5
     *  256-bit PBKDF2 output:
     *      6b 9c f2 6d 45 45 5a 43 a5 b8 bb 27 6a 40 3b 39
     *      e7 fe 37 a0 c4 1e 02 c2 81 ff 30 69 e1 e9 4f 52
     *  256-bit AES key:
     *      4b 6d 98 39 f8 44 06 df 1f 09 cc 16 6d b4 b8 3c
     *      57 18 48 b7 84 a3 d6 bd c3 46 58 9a 3e 39 3f 9e
     */
    I2HexDecode("f09d849e",(uint8_t *)passbuff,4);
    passbuff[4]='\0';
    DoTestCase(i++,passbuff,4,"EXAMPLE.COMpianist",18,50,
            "6b9cf26d45455a43a5b8bb276a403b39",
            16,"g-clef (0xf09d849e)");
    DoTestCase(i++,passbuff,4,"EXAMPLE.COMpianist",18,50,
            "6b9cf26d45455a43a5b8bb276a403b39e7fe37a0c41e02c281ff3069e1e94f52",
            32,"g-clef (0xf09d849e)");

    /*
     * Remaining test cases are not part of RFC 3962. They were put in place
     * simply to test how long it would take for some larger values
     * of count.
     */

    /*
     *  Iteration count = 2048
     *  Pass phrase = "how long?"
     *  Salt = "garlic"
     */
    memset(passbuff,'X',64);
    DoTestCase(i++,passbuff,64,"pass phrase equals block size",29,2048,
            NULL,
            16,"\'X\' (64 times)");

    exit(status);
}
