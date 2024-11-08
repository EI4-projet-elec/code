#include "stm32f10x.h"

#include "../utils/utils.h"
#include "../fifo/fifo.h"

#include "hostCommunication.h"

FIFO uartFifo;

void initHostCom() {
    // Init FIFO
    initFIFO(&uartFifo, 500);

    // Configuration de PA2 and PA9 (TX) en mode Alternate Function Push-Pull avec une vitesse de 10 MHz
    initGpioA(2, 0b1011);
    GPIOA->ODR |= (1 << 2);

    // Configuration de PA3 and PA10 (RX) en mode Input
    initGpioA(3, 0b1000);

    // Activer la resistance de pull-up pour PA3 and PA10 (ODR3 = 1)
    GPIOA->ODR |= (1 << 3);

    // Enable UART2 to have serial connnection to the PC with USB
    initUart2();
}

void hostComUpdate() {
    if(hostSendAvailable()) {
        int data = readFIFO(&uartFifo);
        if(data) USART2->DR = data;
    }
}

void hostSend(char data) {
    if(!isSpaceAvailable(&uartFifo, 1)) return;
    writeInFIFO(&uartFifo, data);
}

void hostSendString(char* data, int length) {
    if(!isSpaceAvailable(&uartFifo, length + 3)) return;

    writeInFIFO(&uartFifo, '$');
    for(int index = 0; index < length; index++) writeInFIFO(&uartFifo, data[index]);
    writeInFIFO(&uartFifo, '\n');
    writeInFIFO(&uartFifo, '$');
}

static char hostSendAvailable() {
    if(USART2->SR & (1 << 7)) return 1;
    return 0;
}

// UART2 init
static void initUart2() {
    // Setup pin REMAP
    AFIO->MAPR &= ~(1 << 3);

    // Enable UART2 clock
    RCC->APB1ENR |= (1 << 17);

    // Reset CR1 register
    USART2->CR1 = 0;

    // Enable UART2
    USART2->CR1 |= (1 << 13);

    // Configurer le Baudrate
    USART2->BRR = 4800;

    // Activer la transmission
    USART2->CR1 |= (1 << 3);

    // Activer la reception
    USART2->CR1 |= (1 << 2);

    // Enable UART2
    USART2->CR2 |= (0b11 << 12);
}