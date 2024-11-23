#include "stm32f10x.h"

#include "../utils/gpio/gpio.h"

#include "toolsManager.h"

char freq = 25;
char duty = 20; // 20 to 0
int counter = 0;

char mode = 0;

void initToolsManager() {
    // Init timer 4
    initTimer4();

    // Init GPIO
    initGpioA(6, 0b0011);
}

void toolsManagerUpdate() {
    if(counter >= duty && counter < freq) {
        GPIOA -> ODR |= (1 << 6);
    } else if(counter >= freq) {
        GPIOA -> ODR &= ~(1 << 6);

        counter = 0;
    }

    if(GPIOB->IDR & (1 << 5)) mode = 1;
    else mode = 0;

    if(mode) {
        GPIOA -> ODR |= (1 << 5);
        duty = 20;
    } else {
        GPIOA -> ODR &= ~(1 << 5);
        duty = 0;
    }
}

// Function to enable STM32F103RB TIM4 timer
void initTimer4() {
    // Enable TIMER 4 clock
    RCC -> APB1ENR |= (1 << 2);

    // Set OPM (one pulse mode) to 0 to never stop the TIMER, Docs 338
    TIM4 -> CR1 &= ~(1 << 3);
    
    // Configure counting in forward mode, Docs 338: 0: Counter used as upcounter
    TIM4 -> CR1 &= ~(1 << 4);
        
    // Set pre-scaler to reduce frequency to 1Mhz
    TIM4 -> PSC = 71;
    
    // Reduce frequency to 125Khz
    TIM4 -> ARR = 99;

    // Enable interrupt on update event
    TIM4 -> DIER |= (1 << 0);
    
    // Start timer
    TIM4 -> CR1 |= (1 << 0);

    // Register interrupt
    NVIC_EnableIRQ(TIM4_IRQn);
}

// Interrupt handler for TIM4
void TIM4_IRQHandler() {
    if(TIM4 -> SR & (1 << 0)) {
        // Clear interrupt flag
        TIM4 -> SR &= ~(1 << 0);

        counter++;
    }
}