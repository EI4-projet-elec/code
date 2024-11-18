#ifndef RFID_H
#define RFID_H

#include "../utils/fifo/fifo.h"

#define DATA_LENGTH 255

#define TAU_MIN 25
#define TAU_MAX 70

#define TAU2_MIN 80
#define TAU2_MAX 140


typedef enum DecodeStateMachine_ {
    sm_SYNC = 0x0,
    sm_DETECT_START,
    sm_ACQUIRE,
} DecodeStateMachine;

extern FIFO IC_times;

void initRFID();

void rfidUpdate();

static void initIimer2InputCapture();
static void initTimer3PWM();

#endif