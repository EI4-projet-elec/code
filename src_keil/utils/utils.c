#include "stm32f10x.h"

#include "utils.h"

// Function to activate any GPIOA pin with specified mode
void initGpioA(const unsigned char num_bit, const char config) {
    unsigned char bit_ref = (num_bit * 4) & 31;

    // Activer l'horloge pour GPIOA : RCC->APB2ENR bit 2
    RCC->APB2ENR |= (1 << 2);

    if(num_bit < 8) {
        GPIOA->CRL &= ~(0xF << bit_ref);
        GPIOA->CRL |= (config << bit_ref);
    } else {
        GPIOA->CRH &= ~(0xF << bit_ref);
        GPIOA->CRH |= (config << bit_ref);
    }
}

// Function to activate any GPIOB pin with specified mode
void initGpioB(const unsigned char num_bit, const unsigned char config) {
    unsigned char bit_ref = (num_bit * 4) & 31;

    // Enable clock for GPIOB
    RCC->APB2ENR |= (1 << 3);

    if (num_bit < 8) {
        GPIOB->CRL &= ~(0xF << bit_ref);
        GPIOB->CRL |= (config << bit_ref);
    } else {
        GPIOB->CRH &= ~(0xF << bit_ref);
        GPIOB->CRH |= (config << bit_ref);
    }
}