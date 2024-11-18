#ifndef HOST_COMMUNICATION_H
#define HOST_COMMUNICATION_H

#include "../utils/fifo/fifo.h"

extern FIFO HOST_UART_COM_FIFO;

void initHostCom();

void hostSend(char data);
void hostSendString(char* data, int length);

static void initUart2();

#endif