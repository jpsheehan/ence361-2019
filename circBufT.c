// *******************************************************
// 
// circBufT.c
//
// Support for a circular buffer of uint32_t values on the 
//  Tiva processor.
// P.J. Bones UCECE
// Last modified:  8.3.2017
// 
// *******************************************************

#include <stdint.h>
#include "stdlib.h"
#include "circBufT.h"

// *******************************************************
// initCircBuf: Initialise the circBuf instance. Reset both indices to
// the start of the buffer.  Dynamically allocate and clear the the 
// memory and return a pointer for the data.  Return NULL if 
// allocation fails.
int32_t *
initCircBuf (circBuf_t *buffer, uint32_t size)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = size;
	buffer->data = 
        (int32_t *) calloc (size, sizeof(int32_t));
	return buffer->data;
}
   // Note use of calloc() to clear contents.

// *******************************************************
// writeCircBuf: insert entry at the current windex location,
// advance windex, modulo (buffer size).
void
writeCircBuf (circBuf_t *buffer, int32_t entry)
{
	buffer->data[buffer->windex] = entry;
	buffer->windex++;
	if (buffer->windex >= buffer->size)
	   buffer->windex = 0;
}

// *******************************************************
// readCircBuf: return entry at the current rindex location,
// advance rindex, modulo (buffer size). The function deos not check
// if reading has advanced ahead of writing.
int32_t
readCircBuf (circBuf_t *buffer)
{
    int32_t entry;
	
	entry = buffer->data[buffer->rindex];
	buffer->rindex++;
	if (buffer->rindex >= buffer->size)
	   buffer->rindex = 0;
    return entry;
}

// *******************************************************
// freeCircBuf: Releases the memory allocated to the buffer data,
// sets pointer to NULL and ohter fields to 0. The buffer can
// re-initialised by another call to initCircBuf().
void
freeCircBuf (circBuf_t * buffer)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = 0;
	free (buffer->data);
	buffer->data = NULL;
}

int32_t getSmallestCircBuf(circBuf_t* buffer)
{
    int i;
    int32_t smallest_value;
    for (i = 0; i < buffer->size; i++)
    {
        int32_t this_value = readCircBuf(buffer);
        if (i == 0 || this_value < smallest_value)
        {
            smallest_value = this_value;
        }
    }
    return smallest_value;
}

int32_t getLargestCircBuf(circBuf_t* buffer)
{
    int i;
    int32_t largest_value;
    for (i = 0; i < buffer->size; i++)
    {
        int32_t this_value = readCircBuf(buffer);
        if (i == 0 || this_value > largest_value)
        {
            largest_value = this_value;
        }
    }
    return largest_value;
}

int32_t getRangeCircBuf(circBuf_t* buffer)
{
    int32_t largest = getLargestCircBuf(buffer);
    int32_t smallest = getSmallestCircBuf(buffer);

    return largest - smallest;
}
