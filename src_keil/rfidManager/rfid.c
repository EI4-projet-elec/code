#include "stdio.h"

#include "stm32f10x.h"

#include "rfid.h"

#include "../utils/gpio/gpio.h"
#include "../utils/fifo/fifo.h"

#include "../hostCommunication/hostCommunication.h"

FIFO IC_times;

char state = sm_SYNC;
char read = 0;
char startCounter = 0;

char index = 0;
char data[DATA_LENGTH];

void initRFID() {
    // Init Timer 2 in input capture mode
    initIimer2InputCapture();

    // Init Timer 3 in PWM mode
    initTimer3PWM();

    // Init vars
    initFIFO(&IC_times);
}

// Function to enable STM32F103RB input capture on TIM2 chanel 1 and 2
void initIimer2InputCapture() {
    // 1. Enable TIM2 clock
    RCC->APB1ENR |= (1 << 0);

    // 2. Configure PA0 in alternate function input mode (TIM2_TI1)
    initGpioA(0, 0b0100);

    // 3. Configure TIM2 count frequency
    TIM2->PSC = 360; // Prescaler to get 125 kHz (72 MHz / 576)
    TIM2->ARR = 0xFFFF; // Auto-reload to count up to maximum value

    // 4. Configure channel 1 and 2 for Input Capture
    TIM2->CCMR1 = 0; // Clear CCMR1 register
    TIM2->CCMR1 |= (1 << 0); // CC1S = 01
    TIM2->CCMR1 |= (1 << 9); // CC2S = 01

    // Use fast enable for channel 2
    TIM2->CCMR1 |= (1 << 12); // IC2F = 0000

    // 6. Enable Input Capture on channel 1
    TIM2->CCER = 0; // Clear CCER register
    TIM2->CCER |= 0b1; // Enable channel 1
    TIM2->CCER |= (0b1 << 4); // Enable channel 2
    TIM2->CCER |= (0b1 << 5); // Set channel 2 detection on falling edge

    // 7. Enable interrupts for channel 1 and 2
    TIM2->DIER |= (0b1 << 1);
    TIM2->DIER |= (0b1 << 2);

    // Reset Timer 2 CR1
    TIM2->CR1 = 0;
    TIM2->CR1 |= 0b1; // Enable Timer 2

    // Clear interrupt flag
    TIM2->SR &= ~(0b1 << 1); 

    // Configure NVIC interrupt for TIM2
    NVIC_EnableIRQ(TIM2_IRQn);
}

// Function to enable STM32F103RB PWM on TIM3 chanel 1
void initTimer3PWM() {
    // Enable TIMER 3 clock
    RCC->APB1ENR |= (1 << 1);

    initGpioA(6, 0b1011);

    // Remap TIM3 outputs, Docs 185: 00: No remap (CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1)
    AFIO -> MAPR &= ~(0b11 << 10);

    // Set OPM (one pulse mode) to 0 to never stop the TIMER, Docs 338
    TIM3 -> CR1 &= ~(1 << 3);
    
    // Configure counting in forward mode, Docs 338: 0: Counter used as upcounter
    TIM3 -> CR1 &= ~(1 << 4);
        
    // Set pre-scaler to reduce frequency to 1Mhz
    TIM3 -> PSC = 71;
    
    // Reduce frequency to 125Khz
    TIM3 -> ARR = 7;

    // Compare / capture CC2 chanel output mode, Docs 413: 00: CC2 channel is configured as output
    TIM3 -> CCMR1 &= ~(0b11 << 8);
    
    // Compare / capture CC1 chanel compare mode, Docs 414: 110: PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT < TIMx_CCR1 else inactive
    TIM3 -> CCMR1 |= (0b11 << 5);	
    TIM3 -> CCMR1 &= ~(1 << 4);
    
    // Enable chanel 1, output PWM signal on PA6, cf. MAPR ligne 14
    TIM3 -> CCER |= (1 << 0);

    // Duty cycle
    TIM3 -> CCR1 = 4;
    
    // Start timer
    TIM3 -> CR1 |= (1 << 0);
}

void TIM2_IRQHandler(void) {
    if(TIM2->SR & (0b1 << 1)) { // Capture detected on CH1
        writeInFIFO(&IC_times, TIM2->CCR1);
        writeInFIFO(&IC_times, 1);

        TIM2->SR &= ~(0b1 << 1); // Clear interrupt flag
        TIM2->CNT = 0; // Reset counter value
    } else if(TIM2->SR & (0b1 << 2)) { // Capture detected on CH2
        writeInFIFO(&IC_times, TIM2->CCR2);
        writeInFIFO(&IC_times, 0);

        TIM2->SR &= ~(0b1 << 2); // Clear interrupt flag
        TIM2->CNT = 0; // Reset counter value
    }
}

void rfidUpdate() {
    if(IC_times.freeSpace < FIFO_SIZE) {
        int deltaTime = readFIFO(&IC_times);
        char edge = readFIFO(&IC_times);

        char value = NULL;

        if(deltaTime >= TAU2_MAX) {
            state = sm_SYNC;
        } else if(state == sm_SYNC) {
            if(deltaTime >= TAU2_MIN && deltaTime <= TAU2_MAX) {
                state = sm_DETECT_START;

                startCounter = 0;
                read = 0;
                index = 0;

                hostSendString("SYNCED", 6);
            }
        } else {
            // Read value
            if((deltaTime >= TAU2_MIN && deltaTime <= TAU2_MAX) || (read && deltaTime >= TAU_MIN && deltaTime <= TAU_MAX)) {
                if(edge) value = 0;
                else value = 1;

                // Count consecutive 0s
                if(value == 0) startCounter++;
                else startCounter = 0;

                if(state == sm_DETECT_START) {
                    if(startCounter >= 9) {
                        state = sm_ACQUIRE;
                        startCounter = 0;

                        hostSendString("START DETECTED", 14);
                    }
                } else if(state == sm_ACQUIRE) {
                    if(startCounter >= 9) {
                        state = sm_SYNC;
                        startCounter = 0;

                        hostSendString("DATA ACQUIRED", 13);

                        // Send data
                        hostSendString("Card code:", 10);

                        char charac = 0; char j = 0;
                        for(int i = 0; i < index; i++) {
                            charac |= data[i] << (7 - j ++);

                            if(j == 8) {
                                hostSend(charac);

                                j = 0;
                                charac = 0;
                            }
                        }
                        hostSendString("", 0);

                        // Reset data
                        emptyFIFO(&IC_times);
                    } else {
                        data[index] = value;
                        index ++;

                        if(index >= DATA_LENGTH) hostSendString("DATA FULL", 9);
                    }
                }   

                read = 0;
            } else if(deltaTime >= TAU_MIN && deltaTime <= TAU_MAX) read = 1;
            else state = sm_SYNC;
        }
    }
}