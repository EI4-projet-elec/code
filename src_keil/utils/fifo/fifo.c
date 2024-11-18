#include <stdlib.h>

#include "stm32f10x.h"

#include "fifo.h"

void initFIFO(FIFO* fifo) {
    fifo->freeSpace = FIFO_SIZE;
    
    fifo->readIndex = 0;
    fifo->writeIndex = 0;
}

void writeInFIFO(FIFO *fifo, int data) {
    // Check if available space
    fifo->content[fifo->writeIndex ++] = data;

    // Loop if end is reached
    if(fifo->writeIndex == FIFO_SIZE) fifo->writeIndex = 0;

    // Reduce available space
    fifo->freeSpace --;
}

int readFIFO(FIFO *fifo) {
    // Check if some data is stored in the FIFO
    int data = fifo->content[fifo->readIndex ++];

    // Loop if end is reached
    if(fifo->readIndex == FIFO_SIZE) fifo->readIndex = 0;

    // Increase space
    fifo->freeSpace ++;

    return data;
}

void emptyFIFO(FIFO *fifo) {
    fifo->freeSpace = FIFO_SIZE;
    
    fifo->readIndex = 0;
    fifo->writeIndex = 0;
}