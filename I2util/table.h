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

typedef struct I2table *I2table;

/*
** This type is used to represent keys and values in a hash.
*/
typedef struct {
             void	*dptr;
             int	dsize;
} I2datum;

/*
** This structure represents a single chain of key/value pairs.
*/
struct I2binding {
	struct I2binding	*link;
	const I2datum		*key;
	I2datum			*value;
};

/*
** This type is used to allow customized print-out procedure
** for keys/values having a specific internal structure.
*/
typedef void (*I2print_binding_func)(const struct I2binding *p, FILE* fp);

/*
** These are basic hash-manipulation functions.
*/
extern I2table I2hash_init(
		   I2ErrHandle eh,
		   int hint,
		   int cmp(const I2datum *x, const I2datum *y),
		   unsigned long hash(const I2datum *key),
		   void print_I2binding(const struct I2binding *p, FILE* fp)
		   );
extern I2datum* I2hash_fetch(I2table hash, const I2datum *key);
extern int I2hash_store(
		      I2table table, 
		      const I2datum *key, 
		      I2datum *value
		      );
extern void I2hash_print(I2table table, FILE* fp);
extern void I2hash_close(I2table *table);

/*
 * This function will be called on every key/value pair in the hash, as
 * long as it returns true, when it is passed in to the I2hash_iterate
 * function. The iteration terminates when this function returns false.
 * The app_data passed into this function is the same one that is passed
 * into the I2hash_iterate function.
 */
typedef I2Boolean (*I2hash_iterate_func)(
		const I2datum	*key,
		I2datum		*value,
		void		*app_data
		);
extern void
I2hash_iterate(
	I2table			table,
	I2hash_iterate_func	ifunc,
	void			*app_data
	      );

#endif
