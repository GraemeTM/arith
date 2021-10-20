/*
 *               dynamicbuffer.c
 *
 *      Author: Noah Mendelsohn
 *
 *      Functions to manage a byte array of dynamic size.
 *      Each buffer has a used followed by an unused portion,
 *      either of which can be zero length. Together they
 *      fill the entire buffer.
 */

#include <stdlib.h>
#include <assert.h>
#include "dynamicbuffer.h"
/*
 *                allocateBuffer
 *
 *    Allocates space of INITIAL_BUFFER_SIZE
 *    then marks it all as unused.
 */

void allocateBuffer(DynamicBuffer *brp, size_t size)
{
	char *newbufp = malloc(size);
	assert(newbufp != NULL);
	brp->start = newbufp;
	brp->next = newbufp;
	brp->end = newbufp + size;
	return;
}

/*
 *                expandBuffer
 *
 *    Doubles the size of an existing buffer, copying
 *    any contents that are in use.
 */

void expandBuffer(DynamicBuffer *brp, size_t size)
{
	size_t oldusedsize = USED_LENGTH(*brp);
	char *newbufp = realloc(brp->start, size);
	assert(newbufp != NULL);
	brp->start = newbufp;
	brp->next = newbufp + oldusedsize;
	brp->end = newbufp + size;
	return;
}



/*
 *                addChartoBuffer
 *
 *
 *        Adds a single character to a buffer defined by
 *        a byte range. If necessary, the buffer is allocated
 *        or reallocated to make space for the character.
 */

void addChartoBuffer(DynamicBuffer *brp, const char c)
{
	/*
	 * If there's no space, allocate or extend the buffer
	 */
	if (brp -> start == NULL) {
		allocateBuffer(brp, INITIAL_DYNAMIC_BUFFER_SIZE);
	} else if (UNUSED_LENGTH(*brp) == 0) {
		expandBuffer(brp, BUFFER_LENGTH(*brp) * 2);
	}

	/*
	 * Store the character
	 */
	*(brp->next++) = c;

	return;
}
