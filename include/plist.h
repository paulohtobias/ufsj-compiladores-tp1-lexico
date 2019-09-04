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
		plist = PLIST_REALLOC(plist, new_capacity * sizeof *list + sizeof plist->capacity + sizeof plist->length); \
		plist->capacity = new_capacity; \
		plist->data = list = (void *) plist + sizeof *plist; \
	} while(0);

#define plist_len(list) \
		(list != NULL ? ((plist_t *) __PLIST_L2P(list))->length : 0)

#define plist_append(list, item) \
	do { \
		plist_t *plist = NULL; \
		if (list == NULL) { \
			__PLIST_RESIZE(plist, list, PLIST_CAPACITY_INCREMENT); \
			plist->length = 0; \
		} else { \
			plist = __PLIST_L2P(list); \
\
			if (plist->length + 1 >= plist->capacity) { \
				__PLIST_RESIZE(plist, list, plist->capacity + PLIST_CAPACITY_INCREMENT); \
			} \
		} \
\
		list[plist->length++] = item; \
	} while(0); \

#define plist_free(list) \
	do{ \
		PLIST_FREE(__PLIST_L2P(list)); \
	} while(0);

#endif // PLIST_H
