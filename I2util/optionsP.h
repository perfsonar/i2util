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
 *	File:		optionsP.h
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 14:41:50  2002
 *
 *	Description:	
 *
 *	This file manages a resource data base of valid command line 
 *	options. Valid options may be merged into the data base
 *	at any time and later extracted with their coresponding values
 *	as determined by the command line.
 *
 *
 *	Modified from code writen by John Clyne at UCAR...
 *
 *
 *	Based on code from UCAR DCS tools. Copyright information
 *	from UCAR follows:
 *
 *	Copyright 1997 University Corporation for Atmospheric Research,
 *	Scientific Computing Division.  All rights reserved.
 *
 *
 *	Permission to use, copy, modify and distribute this software
 *	and its	documentation for any academic, educational and
 *	scientific research purpose is hereby granted without fee,
 *	provided that the above copyright notice and this permission
 *	notice appear in all copies of this software and its
 *	documentation, and that the software is not sold and/or made
 *	the subject of any commercial activity.  Parties interested
 *	in commercial licensing should contact the copyright holder.
 */

#ifndef	_i2_optionsP_h_
#define	_i2_optionsP_h_

#include "options.h"

/*
 *	maximum length of a string passed to AToArgv() by ParseEnvOptions()
 */
#define	MAX_ATOARGV_STRING	1024

typedef	unsigned long	BitMask;

/*
 * 	maximum number of option tables
 */
#define	MAX_TBLS	1024

/*
 * 	initial malloc size for a small chunk of memory
 */
#define	SMALL_BLOCK	10

/*
 *      Same struct as I2OptDescRec except has a `default_value' field
 */
typedef	struct {
	const char      *option;/* name of option without preceeding '-' */
	int     	arg_count;	/* num args expected by option	*/
	char    	*value;		/* current val for the argument	*/
	char    	*default_value;	/* default val for the argument	*/
	const char      *help;		/* help string for option	*/
} OptRec_T;
	

typedef	struct	{
	OptRec_T	*opt_rec;	/* the option table	*/
	int		size; 		/* mem alloced to table */
	int		num;		/* num elements in table*/
	I2ErrHandle	eh;
	} OptTable;

#endif	/* _i2_optionsP_h_	*/
