#include "buffer.h"

void bufferInit(RingBuffer* buffer){
    int i;
    buffer->index = 0;
    for(i=0;i<RING_BUFFER_SIZE;i++){
        buffer->buf[i] = 0;
    }
}

void bufferPush(RingBuffer* buffer, int16_t num){
    buffer->buf[buffer->index] = num;
    if(buffer->index >= RING_BUFFER_SIZE - 1){
        buffer->index = 0;
    }else{
        buffer->index++;
    }
}

int16_t bufferGetAverage(RingBuffer* buffer){
    int32_t ave = 0;
    int i;
    for(i=0;i<RING_BUFFER_SIZE;i++){
        ave += buffer->buf[i];
    }
    return (int16_t)(ave / RING_BUFFER_SIZE);
}