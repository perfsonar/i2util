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
#ifndef	_I2_tools_h_
#define	_I2_tools_h_

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

#include <I2util/errlog.h>
#include <I2util/options.h>

#endif	/*	_I2_tools_h_	*/
