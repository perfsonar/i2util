/*
 *      $Id$
 */
/************************************************************************
*									*
*			     Copyright (C)  2003			*
*				Internet2				*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		conf.h
 *
 *	Author:		Jeff W. Boote
 *			Internet2
 *
 *	Date:		Tue Sep 16 14:28:48 MDT 2003
 *
 *	Description:	
 */
#ifndef	_i2_conf_h_
#define	_i2_conf_h_

#include <I2util/util.h>

#include <stdio.h>

/*
 * Growth increment for linebuffer.
 */
#define I2LINEBUFINC	(120)

/*
 * same charactors isspace checks for - useful for strtok splits
 * of whitespace.
 */
#define I2WSPACESET   "\t\n\v\f\r "

/*
 * Maximum "identity"/"username" charactor length
 */
#define I2MAXIDENTITYLEN	(16)

/*
 * Length of a key in bytes.
 */
#define	I2KEYLEN		(16)

extern int
I2GetConfLine(
	I2ErrHandle	eh,
	FILE		*fp,
	int		rc,
	char		**lbuf,
	size_t		*lbuf_max
	);

extern int
I2ReadConfVar(
	FILE	*fp,
	int	rc,
	char	*key,
	char	*val,
	size_t	max,
	char	**lbuf,
	size_t	*lbuf_max
	);

/*
 * lbuf will contain an undisturbed copy of the last line to be parsed in
 * the file. (It is the callers responsibility to free the memory pointed
 * to by lbuf after calling this function.)
 *
 */
extern int
I2ParseKeyFile(
	I2ErrHandle	eh,
	FILE		*fp,
	int		rc,
	char		**lbuf,
	size_t		*lbuf_max,
	FILE		*tofp,		/* copy 'to' file, or null */
	const char	*id_query,
	char		*id_ret,	/* [I2MAXIDENTITYLEN+1] or null */
	uint8_t	*key_ret	/* [I2KEYLEN] or null */
	);

extern int
I2WriteKeyLine(
	I2ErrHandle	eh,
	FILE		*fp,
	const char	*id,		/* no more than [I2MAXIDENTITYLEN+1] */
	const uint8_t	*key		/* [I2KEYLEN] */
	);

typedef uint64_t	I2numT;

/*
 * These two functions are destructive to the passed in string.
 * StrToNum interprets k/m/g as base 10 numbers,
 * StrToBytes interprets k == 1024 etc...
 */
extern int
I2StrToNum(
	I2numT		*numret,
	char		*str
	  );

extern int
I2StrToByte(
	I2numT		*numret,
	char		*str
	  );

#endif	/* _i2_conf_h_ */
