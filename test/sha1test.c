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
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <I2util/util.h>

int
main(
        int     argc    __attribute__((unused)),
        char    **argv
    ) {
    char                *progname;
    I2LogImmediateAttr  ia;
    I2ErrHandle         eh;

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

    uint8_t digest[20];
    char    hd[41];

    I2Sha1("abc",3,digest);
    hd[40] = '\0';
    I2HexEncode(hd,digest,20);
    fprintf(stdout,"sha1(\"abc\") = %s\n",hd);

    exit(0);
}
