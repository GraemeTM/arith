/*******************************************************************
 *                       readaline
 *
 *     Author: Noah Mendelsohn
 *
 *     Reads a line into a newly malloc'd buffer
 *
 *     Parameters:
 *         inputfd - open file descriptor from which to read
 *         datapp   - pointer to pointer to buffer
 *                   (*datapp is set by readaline -- see below)
 *
 *     Preconditions:
 *     Input stream is open and positioned either at eof or
 *     at first character of next line.
 *
 *     Postcondition:
 *     File is positioned after last character read in line
 *     (that will likely be a \n except in the case of a final
 *     line not terminated with a \n.
 *
 *     feof(inputfd) is set if attempt was made to read
 *     past last line. Callers should in general not check eof,
 *     but instead should loop until a NULL buffer pointer is returned.
 *
 *     This function raises a Hanson-style assertion if
 *     ferror becomes true.
 *
 *     Return values:
 *         function return: size of data read or 0 if EOF and no data
 *         *datapp:  pointer to malloc'd data buffer (if data was read)
 *                  NULL (if readaline called when already EOF)
 *                  Caller must in all case  free *datapp unless
 *                  it is NULL.
 *
 *******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "readaline.h"
#include "dynamicbuffer.h"

size_t readaline(FILE *inputfd, char **datapp, int len)
{
	DynamicBuffer line;
	int c;
	int count = 0;

	/*
	 * Check supplied arguments and initialize
	 */
        assert(inputfd != NULL);
	assert(datapp != NULL);
	assert(!ferror(inputfd));

	line.start = NULL;
	line.next = NULL;
	line.end = NULL;

	/*
	 * If at EOF, return immediately
	 */
	if (feof(inputfd)) {
		*datapp = NULL;
		return 0;
	}

	/*
	 * Loop reading characters until done. Allocate or expand
	 * line buffer as needed. Note: c == EOF for errors as
	 * well as end of line.
	 */
	for (c = getc(inputfd); (c != EOF) && count < len && (c != '\n'); c = getc(inputfd)) {
		addChartoBuffer(&line, c);
	}

	/*
	 * Hanson exception (CRE) if file could not be read
	 */
	assert(!ferror(inputfd));

	/*
	 * If newline we still have to add it to the buffer
	 */
	if (c == '\n') {
		addChartoBuffer(&line, '\n');
	}

	/*
	 * Give data buffer to caller and return
	 */
	if (feof(inputfd) && USED_LENGTH(line) == 0) {
		*datapp = NULL;
	} else {

		*datapp = line.start;
	}
	return USED_LENGTH(line);
}
