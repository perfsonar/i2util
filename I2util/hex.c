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
 *	File:		hex.c
 *
 *	Author:		Jeff W. Boote
 *			Internet2
 *
 *	Date:		Tue Dec 16 15:45:00 MST 2003
 *
 *	Description:	
 */
#include <ctype.h>
#include <I2util/util.h>

/*
 * buff must be at least (nbytes*2) +1 long or memory will be over-run.
 */
void
I2HexEncode(
	char		*buff,
	u_int8_t	*bytes,
	unsigned int	nbytes
	)
{
	char		hex[]="0123456789abcdef";
	unsigned int	i;

	for(i=0;i<nbytes;i++){
		*buff++ = hex[*bytes >> 4];
		*buff++ = hex[*bytes++ & 0x0f];
	}
	*buff = '\0';
}

/*
 * Function:	I2HexDecode
 *
 * Description:	
 * 	Decode hex chars into bytes. Return True on success, False on error.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
I2Boolean
I2HexDecode(
	char		*buff,
	u_int8_t	*bytes,
	unsigned int	nbytes
	)
{
	char		hex[]="0123456789abcdef";
	unsigned int	i,j,offset;
	char		a;

	for(i=0;i<nbytes;i++,bytes++){
		*bytes = 0;
		for(j=0;(*buff != '\0')&&(j<2);j++,buff++){
			a = tolower(*buff);
			for(offset=0;offset<sizeof(hex);offset++){
				if(a == hex[offset]){
					*bytes |= offset;
					if(!j)
						*bytes <<= 4;
					goto byteset;
				}
			}
			return False;
byteset:
			;
		}
	}

	return True;
}

