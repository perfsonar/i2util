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
 *	File:		options.h
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 14:36:59  2002
 *
 *	Description:	
 *		Modified from code writen by John Clyne at UCAR...
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

#ifndef	_i2_options_h_
#define	_i2_options_h_

#include <stdio.h>
#include <sys/types.h>

#include <stdarg.h>
#include <DCSTools/tools.h>

BEGIN_C_DECLS


/*
 *	structure for describing a valid option to buildOptionTable
 */
typedef	struct	_I2OptDescRec {
	const char	*option;/* name of option without preceeding '-' */
	int	arg_count;	/* num args expected by option		*/
	char	*value;		/* default value for the argument	*/
	const char	*help;	/* help string for option		*/
	} I2OptDescRec;

/*
 *	structure for returning the value of an option
 */
typedef	struct	_I2Option {
	char		*option_name;	/* the options name		*/

			/* 
			 * option type converter	
			 */
	int		(*type_conv)(
			I2ErrHandle eh, const char *from, void *to
			);

	void		*offset;		/* offset of return address	*/ 
	int		size;		/* size of option in bytes	*/
	} I2Option;

typedef	struct	_I2EnvOpt {
	char	*option;		/* option name			*/
	char	*env_var;		/* coresponding enviroment var	*/
	} I2EnvOpt;

typedef	struct	I2Dimension2D_ {
	int	nx, ny;
	} I2Dimension2D_T;

extern	int	I2CvtToInt(
	I2ErrHandle	eh,
	const char	*from,
	void		*to
);

extern	int	I2CvtToFloat(
	I2ErrHandle	eh,
	const char	*from,
	void		*to
);

extern	int	I2CvtToChar(
	I2ErrHandle	eh,
	const char	*from,
	void		*to
);

extern	int	I2CvtToBoolean(
	I2ErrHandle	eh,
	const char	*from,
	void		*to
);

extern	int	I2CvtToString(
	I2ErrHandle	eh,
	const char	*from,
	void		*to
);

extern	int	I2CvtToDimension2D(
	I2ErrHandle	eh,
	const char	*from,
	void		*to
);

extern	int	I2OpenOptionTbl(
	I2ErrHandle eh
);

extern	int	I2CloseOptionTbl(
	int	od
);

extern	int	I2GetOptions(
	int	od,
	const I2Option	*options
);

extern	int	I2LoadOptionTable(
	int	od,
	const I2OptDescRec	*optd
);

extern	void	I2RemoveOptions(
	int	od,
	const I2OptDescRec	*optd
);

extern	int	I2ParseOptionTable(
	int			od,
	int			*argc,
	char			**argv,
	const I2OptDescRec	*optds
);

#if	NOT
extern	int	I2ParseEnvOptions(
	int			od,
	const I2EnvOpt		*envv,
	const I2OptDescRec	*optds
);
#endif

extern	void	I2PrintOptionHelp(
	int	od,
	FILE	*fp
);

END_C_DECLS

#endif	/* _i2_options_h_	*/
