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
**                      Jeff W. Boote
**
**	Date:		Thu Apr 19 13:47:17  EDT 2002
**
**	Description:	Simple hash table - implementation.
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
#include "table.h"

#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/*
 * this type is used to hold a single key/value pair.
 */
typedef struct I2BindingRec I2BindingRec, *I2Binding;
struct I2BindingRec{
	I2Datum		key;
	I2Datum		value;
	I2Boolean	delete;
	I2Binding	next;
};

/* Types used to define a hash table. */
struct I2Table {
	I2ErrHandle		eh;
	I2TableDataSizeT	size;
	size_t			hint;
	I2HashCmpFunc		cmp;
	I2HashFunc		hash;
	I2TableDataSizeT	length;
	I2Binding		*buckets;
	I2Binding		freelist;
	I2Binding		*alist;
	size_t			num_alist;
	size_t			size_alist;
	I2Boolean		in_iterate;
	I2TableDataSizeT	delete_nodes;
};

/* Static functions (used by default unless specified). */
static int 
cmpatom(
	I2Datum	x,
	I2Datum y
	)
{
	/* return x != y; */
	return (!(x.dsize == y.dsize) ||
			memcmp(x.dptr, y.dptr, x.dsize));
}

static uint32_t
hashatom(
	I2Datum	key
	)
{
	unsigned long i;
	unsigned char *ptr = (unsigned char *)(key.dptr);
	unsigned long ret = 0;
	for (i = 0; i < key.dsize; i++, ptr++)
		ret += *ptr;
	return ret;
}

static int
alloc_freelist(
		I2Table	table
		)
{
	I2Binding	t;
	unsigned int    i;

	if(table->num_alist <= table->size_alist){
		I2Binding	*alist;
		if(!(alist = realloc(table->alist,sizeof(I2Binding)*
					(table->size_alist+table->hint) ))){
			I2ErrLogP(table->eh,errno,"WARNING: realloc(): %M");
			return -1;
		}
		table->size_alist += table->hint;
		table->alist = alist;
	}

	if(!(t = calloc(sizeof(I2BindingRec),table->hint))){
		I2ErrLogP(table->eh,errno,"WARNING: calloc(): %M");
		return -1;
	}

	table->alist[table->num_alist++] = t;

	for(i=0;i<table->hint;i++){
		t[i].next = table->freelist;
		table->freelist = &t[i];
	}

	return 0;
}

static I2Binding
alloc_binding(
		I2Table	table
		)
{
	I2Binding	node;

	if(!table->freelist && (alloc_freelist(table) != 0)){
		return NULL;
	}

	node = table->freelist;
	table->freelist = node->next;
	node->next = NULL;

	return node;
}

static void
free_binding(
		I2Table		table,
		I2Binding	node
		)
{
	node->next = table->freelist;
	table->freelist = node;

	return;
}

I2TableDataSizeT
I2HashNumEntries(
		I2Table	table
		)
{
	if(table->delete_nodes > table->length){
		I2ErrLogP(table->eh,0,
				"WARNING: I2HashNumEntries - table invalid!");
		return 0;
	}

	return table->length - table->delete_nodes;
}

I2Table 
I2HashInit(
	I2ErrHandle	eh,
	size_t		hint,
	int		cmp(I2Datum x, I2Datum y),
	uint32_t	hash(I2Datum key)
	)
{
	I2Table table;
	unsigned int i;
	unsigned int primes[] = {31, 67, 127, 251, 509, 1021, 2053, 4093, 8191,
							16381, 32771, 65521};
	
	for(i=I2Number(primes)-1;
			(i>0) && (primes[i] > hint);i--);

	table = (void *)calloc(1,sizeof(*table));
	if(!table){
		I2ErrLogP(eh, ENOMEM, "FATAL: calloc for hash table");
		return NULL;
	}
	table->buckets = malloc(primes[i]*sizeof(table->buckets[0]));
	if(!table->buckets){
		I2ErrLogP(eh, ENOMEM, "FATAL: malloc for hash buckets");
		goto error;
	}
	memset(table->buckets,0,primes[i]*sizeof(table->buckets[0]));

	table->eh = eh;
	table->size = primes[i];
	table->hint = (hint) ? hint : primes[i];
	table->cmp = cmp ? cmp : cmpatom;
	table->hash = hash ? hash : hashatom;
	table->length = 0;

	table->freelist = NULL;
	table->alist = NULL;
	table->num_alist = table->size_alist = 0;

	table->in_iterate=False;
	table->delete_nodes=0;

	if(alloc_freelist(table) != 0){
		goto error;
	}

	return table;

error:
	if(table->buckets){
		free(table->buckets);
	}

	if(table->size_alist){
		for(i=0;i<table->num_alist;i++){
			free(table->alist[i]);
		}
		free(table->alist);
	}
	free(table);

	return NULL;
}

void
I2HashClose(
	I2Table	table
	)
{
	unsigned int    i;

	assert(table);
	assert(!table->in_iterate);

	free(table->buckets);

	if(table->size_alist){
		for(i=0;i<table->num_alist;i++){
			free(table->alist[i]);
		}
		free(table->alist);
	}

	free(table);

	return;
}

int 
I2HashDelete(
	I2Table	table,
	I2Datum	key
	)
{
	I2TableDataSizeT    i;
	I2Binding	    *p;
	I2Binding	    q;

	assert(table);

	/* Search table for key. */
	i = (*table->hash)(key)%table->size;
	for (p = &table->buckets[i]; *p; p = &(*p)->next){
		if (!(*p)->delete && ((*table->cmp)(key, (*p)->key) == 0)){
			break;
		}
	}

	if (!*p) /* not found */
		return -1;

	if(table->in_iterate){
		(*p)->delete = True;
		table->delete_nodes++;
		return 0;
	}

	q = *p;
	*p = q->next;
	free_binding(table,q);
	table->length--;

	return 0;
}

/*
** Save a key/value in the hash. Return 0 on success, and -1 on failure.
*/
int 
I2HashStore(
	I2Table	table,
	I2Datum	key,
	I2Datum	value
	)
{
	I2TableDataSizeT	i;
	I2Binding		q;

	assert(table);

	assert(!table->in_iterate);

	i=0;
	i=~i;
	if(table->size == i){
		I2ErrLogP(table->eh,0,"FATAL: hash table full");
		return -1;
	}

	/* Search table for key. */
	i = (*table->hash)(key)%table->size;
	for (q = table->buckets[i]; q; q = q->next){
		if ((*table->cmp)(key, q->key) == 0)
			break;
	}

	if (q == NULL){ /* not found */
		q = alloc_binding(table);
		if (q == NULL){
			return -1;
		}
		q->key = key;
		q->delete = False;
		q->next = table->buckets[i];
		table->buckets[i] = q;
		table->length++;
	}
	q->value = value;
	return 0;
}

/*
** Look up the value corresponding to a given key. Returns
** the value datum on success, or NULL on failure.
*/
I2Boolean
I2HashFetch(
	I2Table	table,
	I2Datum	key,
	I2Datum	*ret
	)
{
	I2TableDataSizeT    i;
	I2Binding	    p;

	assert(table);
	assert(ret);

	/* Search table for key. */
	i = (*table->hash)(key)%table->size;

	for (p = table->buckets[i]; p; p = p->next){
		if (!p->delete && ((*table->cmp)(key, p->key) == 0)){
			break;
		}
	}

	if(!p)
		return False;

	*ret = p->value;
	return True;
}

void
I2HashIterate(
	I2Table			table,
	I2HashIterateFunc	ifunc,
	void			*app_data
	      )
{
	I2TableDataSizeT	i;
	I2Binding		*p;
	I2Binding		q;
	
	assert(table);
	assert(!table->in_iterate);
	assert(ifunc);

	table->in_iterate = True;
	table->delete_nodes = 0;
	for (i = 0; i < table->size; i++){
		for (q = table->buckets[i]; q; q = q->next){
			if(q->delete){
				continue;
			}
			if(!((*ifunc)(q->key,q->value,app_data)))
				goto done_iterate;
		}
	}
done_iterate:

	/*
	 * Now delete any nodes that were removed during the iterate.
	 */
	for (i = 0;((i < table->size)&&(table->delete_nodes > 0)); i++){
		p = &table->buckets[i];
		while(*p && (table->delete_nodes > 0)){
			if((*p)->delete){
				q = *p;
				*p = q->next;
				free_binding(table,q);
				table->delete_nodes--;
				table->length--;
			}
			else{
				p = &(*p)->next;
			}
		}
	}
	table->in_iterate = False;
}

void
I2HashClean(
	I2Table			table
	      )
{
	I2TableDataSizeT	i;
	I2Binding		*p;
	I2Binding		q;

	assert(table);
	assert(!table->in_iterate);

	for (i = 0;((i < table->size) && (table->length > 0)); i++){
		p = &table->buckets[i];
		while(*p){
			q = *p;
			*p = q->next;
			free_binding(table,q);
			table->length--;
		}
	}

	assert(table->length == 0);

	return;
}
