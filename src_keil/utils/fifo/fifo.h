#ifndef FIFO_H
#define FIFO_H

#define FIFO_SIZE 255

typedef struct FIFO_ {
    char freeSpace;

    char readIndex;
    char writeIndex;

    int content[FIFO_SIZE];
} FIFO;

void initFIFO(FIFO* fifo);

void writeInFIFO(FIFO* fifo, int data);
int readFIFO(FIFO* fifo);

void emptyFIFO(FIFO *fifo);

#endif