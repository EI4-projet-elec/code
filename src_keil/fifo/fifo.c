#include <stdlib.h>
#include "stm32f10x.h"

#include "fifo.h"

void initFIFO(FIFO* fifo, int size) {
    fifo->arraySize = size;
    fifo->freeSpace = size;
    fifo->readIndex = 0;
    fifo->writeIndex = 0;

    fifo->content = (char*) malloc(sizeof(char) * fifo->arraySize);
}

void writeInFIFO(FIFO *fifo, char data) {
    // Check if available space
    if(fifo->freeSpace > 0) {
        fifo->content[fifo->writeIndex ++] = data;

        // Loop if end is reached
        if(fifo->writeIndex == fifo->arraySize) fifo->writeIndex = 0;

        // Reduce available space
        fifo->freeSpace --;
    }
}

char readFIFO(FIFO *fifo) {
    // Check if some data is stored in the FIFO
    if(fifo->freeSpace < fifo->arraySize) {
        char data = fifo->content[fifo->readIndex ++];

        // Loop if end is reached
        if(fifo->readIndex == fifo->arraySize) fifo->readIndex = 0;

        // Increase space
        fifo->freeSpace ++;
        return data;
    } else return 0;
}

char isSpaceAvailable(FIFO *fifo, int space) {
    return (fifo->freeSpace - space > 0 ? 1 : 0);
}