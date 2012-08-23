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
 *	File:		mach_dep.h
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Wed Apr 24 13:57:41 EDT 2002
 *
 *	Description:	This module attempts to isolate all machine
 *			dependencies from platform to platform.
 *
 *		Based on code from UCAR DCS tools. Copyright information
 *		from UCAR follows:
 *
 *      Copyright 2012, University Corporation for Atmospheric Research.
 *
 *      This software may be used, subject to the terms of OSI's BSD-2 Clause
 *      License located at  http://www.opensource.org/licenses/bsd-license.php/
 */
#ifndef	_i2_mach_dep_h_
#define	_i2_mach_dep_h_

extern
const char	* const *I2GetSysErrList(
	int	*count
	);

#endif	/* _i2_mach_dep_h_ */
