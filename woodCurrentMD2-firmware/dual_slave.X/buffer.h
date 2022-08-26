#ifndef BUFFER_H
#define	BUFFER_H

#include <stdint.h>

#define RING_BUFFER_SIZE    10

typedef struct{
    int16_t buf[RING_BUFFER_SIZE];
    int index;
}RingBuffer;

void bufferInit(RingBuffer* buffer);
void bufferPush(RingBuffer* buffer, int16_t num);
int16_t bufferGetAverage(RingBuffer* buffer);

#endif	/* BUFFER_H */