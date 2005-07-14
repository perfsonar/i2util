/*
 *      $Id$
 */
/************************************************************************
*									*
*			     Copyright (C)  2002			*
*				Internet2				*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		util.h
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 10:11:16  2002
 *
 *	Description:	
 *			I2 Utility library. Currently contains:
 *				* error logging
 *				* command-line parsing
 */
#ifndef	_I2_util_h_
#define	_I2_util_h_
#include <inttypes.h>
#include <I2util/config.h>

#ifndef	HAVE___ATTRIBUTE__
#define	__attribute__(x)
#endif

typedef unsigned int    I2Boolean;

#ifndef	False
#define	False	(0)
#endif
#ifndef	True
#define	True	!False
#endif

#ifdef	__cplusplus
#define	BEGIN_C_DECLS	extern "C" {
#define	END_C_DECLS	}
#else
#define	BEGIN_C_DECLS
#define	END_C_DECLS
#endif

#define	I2Number(arr)	(sizeof(arr)/sizeof(arr[0]))

#include <I2util/Pthread.h>
#include <I2util/errlog.h>
#include <I2util/options.h>
#include <I2util/table.h>
#include <I2util/random.h>
#include <I2util/io.h>
#include <I2util/saddr.h>
#include <I2util/md5.h>
#include <I2util/readpassphrase.h>
#include <I2util/hex.h>
#include <I2util/conf.h>

#endif	/*	_I2_util_h_	*/
