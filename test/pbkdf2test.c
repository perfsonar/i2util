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
 *      sure things are consistent. I have not been able to find
 *      pbkdf2-hmac-sha1 test vectors!
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <I2util/pbkdf2.h>
#include <I2util/hmac-sha1.h>

int
main(
        int     argc    __attribute__((unused)),
        char    **argv
    ) {
    char                *progname;
    I2LogImmediateAttr  ia;
    I2ErrHandle         eh;
    int                 status = 0;
    char            *ststr="SUCCESS";
                    /* size of aeskey XXX:use #define for it! */
    char            *out[] = {
        "4136F73441E3128DF2752C876F763FD5"
    };
    uint8_t         verify[16];
    uint8_t         result[16];
    char            hd[(2*16)+1];

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
    if( (I2pbkdf2(I2HMACSha1,(uint32_t)I2SHA1_DIGEST_SIZE,(uint8_t *)"password",8,(uint8_t *)"salty",5,1024,16,result))){
        perror("I2pbkdf2: ");
        exit(1);
    }

    I2HexDecode(out[0],verify,16);
    if(memcmp(verify,result,16)){
        status = 1;
        ststr = "FAILED";
    }
    else{
        ststr = "SUCCESS";
    }

    hd[2*16] = '\0';
    I2HexEncode(hd,result,16);

    fprintf(stdout,"%s: pbkdf2(pw=\"%s\",s=\"%s\",c=\"%d\") = %s\n",ststr,"password","salty",1024,hd);


    exit(status);
}
