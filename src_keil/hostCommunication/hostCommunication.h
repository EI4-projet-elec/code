#ifndef HOST_COMMUNICATION_H
#define HOST_COMMUNICATION_H

void initHostCom();

void hostComUpdate();

void hostSend(char data);
void hostSendString(char* data, int length);

static char hostSendAvailable();

static void initUart2();

#endif