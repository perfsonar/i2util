/*
**      $Id$
*/
/************************************************************************
*									*
*			     Copyright (C)  2002			*
*				Internet2				*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
**	File:		table.h
**
**	Author:		Anatoly Karp
**
**	Date:		Thu Apr 19 13:47:17  EDT 2002
**
**	Description:	Simple hash table - header file.
*/
#ifndef _I2util_table_h_
#define _I2util_table_h_

#include <I2util/util.h>

typedef struct I2Table *I2Table;

typedef u_int32_t	I2TableDataSizeT;
/*
** This type is used to represent keys and values in a hash.
*/
typedef struct {
             void		*dptr;
             I2TableDataSizeT	dsize;
} I2Datum;

typedef int (*I2HashCmpFunc)(
	I2Datum	x,
	I2Datum	y
	);

typedef u_int32_t (*I2HashFunc)(
	I2Datum	key
	);

/*
 * These are basic hash-manipulation functions.
 */

/*
 * The I2HashInit function is the initialization function for a new hash.
 * hint indicates a good guess as to the size of the hash.
 */
extern I2Table I2HashInit(
	I2ErrHandle		eh,
	int			hint,	/* guess on number of elements */
	I2HashCmpFunc		cmp,
	I2HashFunc		hash
	);

extern I2Boolean
I2HashFetch(
	I2Table	hash,
	I2Datum	key,
	I2Datum	*ret
	);

/* return 0 on success */
extern int I2HashStore(
	I2Table	table, 
	I2Datum	key, 
	I2Datum	value
	);

extern int
I2HashDelete(
	I2Table	table,
	I2Datum	key
	);

extern I2TableDataSizeT
I2HashNumEntries(
		I2Table	table
		);

extern void
I2HashClose(
	I2Table		table
	);

/*
 * This function will be called on every key/value pair in the hash as
 * long as the function returns true. The iteration terminates when this
 * function returns false.
 * The app_data passed into this function is the same one that is passed
 * into I2HashIterate.
 *
 * The hash_iterate_func currently has some limitations:
 * 	The only modification operation that may be done on the hash during
 * 	the iteration is delete. All others will produce errors.
 * 		(store/close)
 */
typedef I2Boolean (*I2HashIterateFunc)(
		I2Datum	key,
		I2Datum	value,
		void	*app_data
		);

extern void
I2HashIterate(
	I2Table			table,
	I2HashIterateFunc	ifunc,
	void			*app_data
	      );

/*
 * This example shows how a hash of string key/value
 * pairs can be printed using the I2HashIterate functionality:
 *
 * First define the fuction:
 *
 * I2Boolean hash_print(
 * 	const I2Datum	*key,
 * 	I2Datum		*value,
 * 	void		*app_data
 * 	)
 * {
 * 	FILE	*fp = (FILE*)app_data;
 *
 * 	fprintf(fp,"key=%s\tvalue=%s\n",key->dptr,value->dptr);
 *
 * 	return True;
 * }
 *
 * Then later in the code after storing hash values, to print the hash:
 *
 * I2HashIterate(table,hash_print,(void*)stdout);
 */

#endif
