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
 *	File:		errlogimmediate.h
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 14:44:22  2002
 *
 *	Description:	This file defines an "immediate-mode" logging
 *			function to be used as the `log_func' argument
 *			to the I2OpenErr() function.
 *
 *			Modified from code writen by John Clyne at UCAR...
 *
 *
 *		Based on code from UCAR DCS tools. Copyright information
 *		from UCAR follows:
 *
 *		Copyright 1997 University Corporation for Atmospheric Research,
 *		Scientific Computing Division.  All rights reserved.
 *
 *			
 *		Permission to use, copy, modify and distribute this software
 *		and its	documentation for any academic, educational and
 *		scientific research purpose is hereby granted without fee,
 *		provided that the above copyright notice and this permission
 *		notice appear in all copies of this software and its
 *		documentation, and that the software is not sold and/or made
 *		the subject of any commercial activity.  Parties interested
 *		in commercial licensing should contact the copyright holder.
 */
#ifndef	_i2_errlogimmediate_h_
#define	_i2_errlogimmediate_h_

#include <stdio.h>

BEGIN_C_DECLS


typedef struct  I2LogImmediateAttr_ {
	FILE		*fp;
	unsigned long	line_info;
	const char	*tformat;
	} I2LogImmediateAttr;

extern void	I2ErrLogImmediate(
	struct I2ErrLogEvent	*ev,
	void			*arg,
	void			**data	/* not used	*/
);

END_C_DECLS

#endif	/* _i2_errlogimmediate_h_	*/
