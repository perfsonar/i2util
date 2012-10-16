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
 *	File:		io.h
 *
 *	Author:		Jeff W. Boote
 *			Internet2
 *
 *	Date:		Fri Aug 09 12:53:52 MDT 2002
 *
 *	Description:	
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */
#ifndef	_i2_io_h_
#define	_i2_io_h_

#include <I2util/util.h>
#include <fcntl.h>

BEGIN_C_DECLS

extern ssize_t
I2Readni(
	int	fd,
	void	*vptr,
	size_t	n,
	int	*retn_on_intr	/* pointer so it can be modified via signals */
	);

extern ssize_t
I2Readn(
	int	fd,
	void	*vptr,
	size_t	n
	);

extern ssize_t
I2Writeni(
	int		fd,
	const void	*vptr,
	size_t		n,
	int		*retn_on_intr
	);

extern ssize_t
I2Writen(
	int		fd,
	const void	*vptr,
	size_t		n
	);

extern int
I2CopyFile(
        I2ErrHandle eh,
        int         tofd,
        int         fromfd,
        size_t      len
        );
END_C_DECLS
#endif	/*	_i2_io_h_	*/
