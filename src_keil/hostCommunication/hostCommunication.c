#include "stm32f10x.h"

#include "../utils/gpio/gpio.h"
#include "../utils/fifo/fifo.h"

#include "hostCommunication.h"

FIFO HOST_UART_COM_FIFO;

void initHostCom() {
    // Init FIFO
    initFIFO(&HOST_UART_COM_FIFO);

    // Configuration de PA2 and PA9 (TX) en mode Alternate Function Push-Pull avec une vitesse de 10 MHz
    initGpioA(2, 0b1011);

    // Configuration de PA3 and PA10 (RX) en mode Input
    initGpioA(3, 0b1000);
    GPIOA->ODR |= (1 << 3); // Activer la resistance de pull-up pour PA3 and PA10 (ODR3 = 1)

    // Enable UART2 to have serial connnection to the PC with USB
    initUart2();

    // Send init message
    hostSendString("SPI communication initialized", 29);
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
    USART2->BRR = 480;

    // Activer la transmission
    USART2->CR1 |= (1 << 3);

    // Activer la reception
    USART2->CR1 |= (1 << 2);

    // Enable UART2
    USART2->CR2 |= (0b11 << 12);
}

void hostSend(char data) {
    // Check if there is enough space in the FIFO
    if(HOST_UART_COM_FIFO.freeSpace < 1) return;
    writeInFIFO(&HOST_UART_COM_FIFO, data);
}

void hostSendString(char* data, int length) {
    // Check if there is enough space in the FIFO
    if(HOST_UART_COM_FIFO.freeSpace < length + 2) return;

    // Write start string character
    writeInFIFO(&HOST_UART_COM_FIFO, '$');

    // Write data
    for(int index = 0; index < length; index++) writeInFIFO(&HOST_UART_COM_FIFO, data[index]);

    // Write end line character
    writeInFIFO(&HOST_UART_COM_FIFO, '\n');
}