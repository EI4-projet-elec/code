#include "stm32f10x.h"
#include "stdio.h"

#include "utils/utils.h"
#include "hostCommunication/hostCommunication.h"

#include "main.h"

// Function to enable STM32F103RB input capture on TIM2 chanel 1
void initIimer2InputCapture(void) {
    // 1. Enable TIM2
    RCC->APB1ENR |= (1 << 0);

    // 2. Configure PA0 in alternate function input mode (TIM2_CH1)
    initGpioA(0, 0b0100);

    // 3. Configure TIM2 count frequency
    TIM2->PSC = 0; // Prescaler to get 125 kHz (72 MHz / 576)
    TIM2->ARR = 0xFFFF; // Auto-reload to count up to maximum value

    // 4. Configure channel 1 for Input Capture
    TIM2->CCMR1 = 0; // Clear CCMR1 register

    TIM2->CCMR1 |= (1 << 0); // CC1S = 01
    TIM2->CCMR1 |= (0b0000 << 4); // IC1F = 0001

    // 5. Configure captures on rising edges
    TIM2->CCER &= ~0b1; 

    // 6. Enable Input Capture on channel 1
    TIM2->CCER |= 0b1;

    // 7. Enable interrupts for channel 1
    TIM2->DIER |= 0b1 << 1;

    // Reset Timer 2 CR1
    TIM2->CR1 = 0;

    // Enable Timer 2
    TIM2->CR1 |= 0b1;

    // Clear interrupt flag
    TIM2->SR &= ~(0b1 << 1); 

    // Configure NVIC interrupt for TIM2
    NVIC_EnableIRQ(TIM2_IRQn);
}

enum DecodeStateMachine_ {
    SYNC = 0x0,
    DETECT_START,
    ACQUIRE,
} DecodeStateMachine;
char state = SYNC;

// const int TAU_MIN = 22, TAU_MAX = 32;
// const int TAU2_MIN = 55, TAU2_MAX = 65;

void TIM2_IRQHandler(void) {
    // Capture detected
    if(TIM2->SR & (0b1 << 1)) {
        int deltaTime = TIM2->CCR1;

        // Send string with captured delta time value
        char buffer[22]; for(int i = 0; i < 22; i++) buffer[i] = 0;
        sprintf(buffer, "Delta time: %d", deltaTime);
        hostSendString(buffer, 22);

        // if(state == SYNC) {
        //     if(deltaTime >= TAU2_MIN && deltaTime <= TAU2_MAX) {
        //         state = ACQUIRE;

        //         index = 0;
        //         for(unsigned int i = 0; i < DATA_LENGTH; i++) data[i] = 0;

        //         hostSendString("SYNCED", 4);
        //     }
        // }

        // Reset counter value
        TIM2->CNT = 0;

        // Invert the polarity to capture the next opposite edge
        TIM2->CCER ^= (0b1 << 1);

        // Clear interrupt flag
        TIM2->SR &= ~(0b1 << 1); 
    }
}

void init() {
    // Init host communication with USART
    initHostCom();  

    // Init Timer 2 in input capture mode
    initIimer2InputCapture();
}

void loop() {
    while(1) {
        hostComUpdate();
    }
}

int main(void) {
    init();
    loop();
}