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

#define T I2table
typedef struct T *T;

/*
** This type is used to represent keys and values in a hash.
*/
typedef struct {
             char *dptr;
             int dsize;
} I2datum;

/*
** This structure represents a single chain of key/value pairs.
*/
struct I2binding {
	struct I2binding *link;
	const I2datum *key;
	I2datum *value;
};

/*
** This type is used to allow customized print-out procedure
** for keys/values having a specific internal structure.
*/
typedef void (*I2print_binding_func)(const struct I2binding *p, FILE* fp);

/*
** These are basic hash-manipulation functions.
*/
extern T I2hash_init(
		   I2ErrHandle eh,
		   int hint,
		   int cmp(const I2datum *x, const I2datum *y),
		   unsigned long hash(const I2datum *key),
		   void print_I2binding(const struct I2binding *p, FILE* fp)
		   );
extern I2datum* I2hash_fetch(T hash, const I2datum *key);
extern int I2hash_store(
		      T table, 
		      const I2datum *key, 
		      I2datum *value
		      );
extern void I2hash_print(T table, FILE* fp);
extern void I2hash_close(T *table);

#undef T
#endif
