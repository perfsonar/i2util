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
#ifndef	_i2_mach_dep_h_
#define	_i2_mach_dep_h_

extern
char	**I2GetSysErrList(
	int	*count
	);

#endif	/* _i2_mach_dep_h_ */
