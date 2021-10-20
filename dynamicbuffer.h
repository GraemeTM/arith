#ifndef  DYNAMICBUFFER_H_INCLUDED
#define  DYNAMICBUFFER_H_INCLUDED

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


#include <assert.h>

#define INITIAL_DYNAMIC_BUFFER_SIZE 200


typedef struct dynamicbuffer {
	char *start;             /* first byte in range          */
	char *next;              /* first unused byte in range   */
	char *end;               /* first byte past end of range */
} DynamicBuffer;

#define BUFFER_LENGTH(br) ((br).end - (br).start )
#define UNUSED_LENGTH(br) ((br).end - (br).next )
#define USED_LENGTH(br) ((br).next - (br).start )

void allocateBuffer(DynamicBuffer *brp, size_t size);
void addChartoBuffer(DynamicBuffer *brp, const char c);
void expandBuffer(DynamicBuffer *brp, size_t size);


#endif
