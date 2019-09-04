/**
 * PList
 *
 * plist.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef PLIST_H
#define PLIST_H

#include <stddef.h>

// Custom capacity increment.
#ifndef PLIST_CAPACITY_INCREMENT
#define PLIST_CAPACITY_INCREMENT 10 // Arbitrary value.
#endif // PLIST_CAPACITY_INCREMENT

// Custom memory allocators.
#ifndef PLIST_MALLOC
#define PLIST_MALLOC malloc
#endif // PLIST_MALLOC

#ifndef PLIST_CALLOC
#define PLIST_CALLOC calloc
#endif // PLIST_CALLOC

#ifndef PLIST_REALLOC
#define PLIST_REALLOC realloc
#endif // PLIST_REALLOC

#ifndef PLIST_FREE
#define PLIST_FREE free
#endif // PLIST_FREE


typedef struct plist_t {
	size_t capacity;
	size_t length;

	void *data;
} plist_t;

#define __PLIST_L2P(list) (list != NULL ? (void *) list - sizeof(plist_t) : NULL)

#define __PLIST_RESIZE(plist, list, new_capacity) \
	do { \
		plist = PLIST_REALLOC(plist, new_capacity * sizeof *list + sizeof *plist); \
		plist->capacity = new_capacity; \
		plist->data = list = (void *) plist + sizeof *plist; \
	} while(0);

#define plist_create(list, initial_capacity) \
	do { \
		if (list == NULL) { \
			plist_t *plist = NULL; \
			__PLIST_RESIZE(plist, list, initial_capacity); \
			plist->length = 0; \
		} \
	} while(0);

#define plist_len(list) \
	(list != NULL ? ((plist_t *) __PLIST_L2P(list))->length : 0)

#define plist_cap(list) \
	(list != NULL ? ((plist_t *) __PLIST_L2P(list))->capacity : 0)

#define plist_insert(list, item, index) \
	do { \
		plist_t *plist = __PLIST_L2P(list); \
		if (list == NULL) { \
			plist_create(list, PLIST_CAPACITY_INCREMENT); \
			plist = __PLIST_L2P(list); \
		} else if (index >= plist->capacity) { \
			__PLIST_RESIZE(plist, list, plist->capacity + PLIST_CAPACITY_INCREMENT); \
		} \
\
		list[index] = item; \
		plist->length++; \
	} while(0);

#define plist_append(list, item) plist_insert(list, item, plist_len(list))

#define plist_free(list) PLIST_FREE(__PLIST_L2P(list))

#endif // PLIST_H
