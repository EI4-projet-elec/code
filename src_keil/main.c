#include "stdio.h"

#include "stm32f10x.h"

#include "utils/fifo/fifo.h"
#include "utils/gpio/gpio.h"

#include "hostCommunication/hostCommunication.h"
#include "rfidManager/rfid.h"
#include "toolsManager/toolsManager.h"

#include "main.h"

void init() {
    // Init host communication with USART
    initHostCom();

    // Init RFID manager
    initRFID();

    // Init tools manager
    initToolsManager();

    // Init oboard LED
    initGpioA(5, 0b0011);

    initGpioA(10, 0b1000);
    initGpioB(5, 0b1000);
}

void loop() {
    while(1) {
        // Host communication update
        if(USART2->SR & (1 << 7) && HOST_UART_COM_FIFO.freeSpace < FIFO_SIZE) USART2->DR = readFIFO(&HOST_UART_COM_FIFO);

        // RFID manager update
        rfidUpdate();

        // Tools manager update
        toolsManagerUpdate();

        // Copy PA10 to PA5
        if(GPIOA->IDR & (1 << 10) || GPIOB->IDR & (1 << 5)) GPIOA->ODR = (1 << 5);
        else GPIOA->ODR &= ~(1 << 5);
    }
}

int main(void) {
    init();
    loop();
}