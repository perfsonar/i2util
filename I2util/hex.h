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
 *	File:		hex.h
 *
 *	Author:		Jeff W. Boote
 *			Internet2
 *
 *	Date:		Tue Dec 16 15:47:34 MST 2003
 *
 *	Description:	
 */
#ifndef	_i2_hex_h_
#define	_i2_hex_h_

#include <stdio.h>
#include <I2util/util.h>

/*
 * Function:	I2HexEncode
 *
 * takes 'nbytes' in 'bytes' and returns a hex representation of them in 'buff'.
 *
 * buff must be at least (nbytes*2)+1 in size or memory will overrun.
 */
extern void
I2HexEncode(
	char		*buff,
	u_int8_t	*bytes,
	unsigned int	nbytes
	);

/*
 * Function:	I2HexDecode
 *
 * Decode hex chars into bytes. Return True on success, False on error.
 */
extern I2Boolean
I2HexDecode(
	char		*buff,
	u_int8_t	*bytes,
	unsigned int	nbytes
	);

#endif	/* _i2_hex_h_ */
