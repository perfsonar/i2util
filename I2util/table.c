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
**	File:		table.c
**
**	Author:		Anatoly Karp
**
**	Date:		Thu Apr 19 13:47:17  EDT 2002
**
**	Description:	Simple hash table - implementation.
*/
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "table.h"

/* Types used to define a hash table. */
struct I2table {
	I2ErrHandle	eh;
	int size;
	int (*cmp)(const I2datum *x, const I2datum *y);
	unsigned long (*hash)(const I2datum *key);
	int length;
	struct I2binding  **buckets;
	I2print_binding_func print_binding;
};

#ifndef	MIN
#define MIN(x,y)	((x<y)?x:y)
#endif

/* Static functions (used by default unless specified). */
static int 
cmpatom(const I2datum *x, const I2datum *y)
{
	/* return x != y; */
	assert(x);
	assert(y);
	return (!(x->dsize == y->dsize) ||
			bcmp(x->dptr, y->dptr, MIN(x->dsize,y->dsize)));
}

static unsigned long
hashatom(const I2datum *key)
{
	return (unsigned long)key->dptr>>2;
}

static void
simple_print_binding(const struct I2binding *p, FILE* fp)
{
	fprintf(fp, "the value for key %s is %s\n", 
		p->key->dptr, p->value->dptr);
}

I2table 
I2hash_init(
	    I2ErrHandle eh,
	    int hint,
	    int cmp(const I2datum *x, const I2datum *y),
	    unsigned long hash(const I2datum *key),
	    void print_binding(const struct I2binding *p, FILE* fp)
)
{
	I2table table;
	int i;
	static int primes[] = { 509, 509, 1021, 2053, 4093, 8191, 16381,
	32771, 65521, INT_MAX };
	
	assert(hint >= 0);
	for (i = 1; primes[i] < hint; i++)
		;
	table = (void *)malloc(sizeof(*table) + 
			       primes[i-1]*sizeof(table->buckets[0]));
	if (table == NULL){
		I2ErrLogP(eh, ENOMEM, "FATAL: malloc for hash table");
		
		exit(1);
	}

	table->eh = eh;
	table->size = primes[i-1];
	table->cmp = cmp? cmp : cmpatom;
	table->hash = hash ? hash : hashatom;
	table->print_binding = print_binding ? 
		print_binding : simple_print_binding;
	table->buckets = (struct I2binding **)(table + 1);
	for (i = 0; i < table->size; i++)
		table->buckets[i] = NULL;
	table->length = 0;
	return table;
}

void
I2hash_close(I2table *table)
{
	assert(table && *table);
	if ((*table)->length > 0){
		int i;
		struct I2binding *p, *q;
		for (i = 0; i < (*table)->size; i++)
			for (p = (*table)->buckets[i]; p; p = q){
				q = p->link;
				if (p)
					free(p);
			}
	}
	if (*table)
		free(*table);
}

int 
I2hash_store(I2table table, const I2datum *key, I2datum *value)
{
	int i;
	struct I2binding *p;

	assert(table);
	assert(key);

	/* Search table for key. */
	i = (*table->hash)(key)%table->size;
	for (p = table->buckets[i]; p; p = p->link){
		if ((*table->cmp)(key, p->key) == 0)
			break;
	}

	if (p == NULL){ /* not found */
		p = (void *)malloc(sizeof(*p));
		if (p == NULL){
			I2ErrLogP(table->eh,ENOMEM,
					"FATAL: malloc for hash table");
			return -1;
		}
		p->key = key;
		p->link = table->buckets[i];
		table->buckets[i] = p;
		table->length++;
	}
	p->value = value;
	return 0;
}

I2datum *
I2hash_fetch(I2table table, const I2datum *key){
	int i;
	struct I2binding *p;
	I2datum ret;
	ret.dsize = 0;
	ret.dptr = NULL;

	assert(table);
	assert(key);

	/* Search table for key. */
	i = (*table->hash)(key)%table->size;
	for (p = table->buckets[i]; p; p = p->link){
		if ((*table->cmp)(key, p->key) == 0)
			break;
	}
	
	return p ? (p->value) : NULL;
}

void
I2hash_print(I2table table, FILE* fp)
{
	int i;
	struct I2binding *p;
	
	assert(table);

	for (i = 0; i < table->size; i++)
		for (p = table->buckets[i]; p; p = p->link)
			table->print_binding(p, fp);
}

void
I2hash_iterate(
	I2table			table,
	I2hash_iterate_func	ifunc,
	void			*app_data
	      )
{
	int i;
	struct I2binding *p;
	
	assert(table);
	assert(ifunc);

	for (i = 0; i < table->size; i++)
		for (p = table->buckets[i]; p; p = p->link){
			if(!((*ifunc)(p->key,p->value,app_data)))
				return;
		}
}
