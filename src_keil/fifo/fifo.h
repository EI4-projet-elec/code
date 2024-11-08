#ifndef FIFO_H
#define FIFO_H

typedef struct FIFO_ {
    int arraySize;
    int freeSpace;

    int readIndex;
    int writeIndex;

    char* content;
} FIFO;

void initFIFO(FIFO* fifo, int size);
void writeInFIFO(FIFO *fifo, char data);
char readFIFO(FIFO *fifo);
char isSpaceAvailable(FIFO *fifo, int space);

#endif