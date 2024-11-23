#include "stm32f10x.h"

#include "../utils/fifo/fifo.h"
#include "../utils/gpio/gpio.h"
#include "../hostCommunication/hostCommunication.h"

#include "espCommunication.h"

FIFO esp;

char lastButtonState = 0;
char buttonPressed = 0;

void initEspCom() {
    // Init USART3
    init_USART3();

    // Init FIFO
    initFIFO(&esp);

    // Init push button
    configPC13();
}

void espComUpdate() {
    // Read push button
    char currentState = (GPIOC->IDR & (1 << 13)) >> 13;
    if(currentState != lastButtonState && (currentState == 0)) buttonPressed = 1;
    lastButtonState = currentState;

    if(buttonPressed) {
        hostSendString("SENDING MAIL", 12);

        char message[] = "sendMail";
        for(int i = 0; i < 8; i++) writeInFIFO(&esp, message[i]);

        buttonPressed = 0;
    }

    // Send message to ESP
    if(esp.freeSpace < FIFO_SIZE) {
        if(USART3 -> SR & (1 << 7)) {
            char data = readFIFO(&esp);
            USART3->DR = data;
        }
    }
}

// USART
void init_USART3(void){
    RCC->APB1ENR |= (1<<18);

    // PB10 (TX)/PB11 (RX)
    initGpioB(10, 0x9);
    initGpioB(11, 0x4);

    // 9600 bps
    USART3->BRR = 3750;

    // 8 bits de donnees, 1 bit de stop
    USART3->CR1 |= (1<<13);
    USART3->CR1 &=~(1<<12);
    USART3->CR1 |= (1 << 3);
    USART3->CR1 |= (1 << 2);

    USART3->CR2 &=~(3<<12);
}

void configPC13(void){
	RCC->APB2ENR |= (1 << 4);

	GPIOC->CRH &= ~((1 << 20) | (1 << 21));
	GPIOC->CRH &= ~(1 << 23);
	GPIOC->CRH |= (1 << 22);
}