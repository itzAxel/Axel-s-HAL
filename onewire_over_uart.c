#include "onewire_over_uart.h"
#include "uart_driver.h"

typedef enum {
    OW_IDLE,
    OW_BUSY,
    OW_COMPLETE,
    OW_FAULT
} OW_Status_t;

volatile OW_Status_t ow_status = OW_IDLE;
static const uint8_t ResetByte = 0xF0; // This will save exactly 1 byte of SRAM
static uint8_t ow_buffer_rx[8];
static uint8_t ow_buffer_tx[8];
static uint8_t byte_res = 0;

// You have to call this function on TC interrupt in DMA2_Stream2_IRQHandler located in stm32f4xx_it.c
void OW_Callback(void) {
    ow_status = OW_COMPLETE;
}

uint8_t OW_Reset_Blocking(void) {
    static uint8_t Received = 0;

    if (ow_status == OW_BUSY) {return 0;} // These checks can be commented if you want to control everything yourself
    ow_status = OW_BUSY;
    UART1_SetBaudRate(9600);
    UART1_Receive_DMA(&Received, 1);
    UART1_Transmit_DMA((uint8_t*)&ResetByte, 1); // The function expects non-const, so it has to be converted
    LL_mDelay(5);
    ow_status = OW_IDLE;

    return Received;
}

void OW_SendByte_Async(uint8_t byte) {
    if (ow_status == OW_BUSY) {return;}
    ow_status = OW_BUSY;

    LSB_Encode(byte, ow_buffer_tx);
    UART1_SetBaudRate(115200);
    UART1_Receive_DMA(ow_buffer_rx, 8);
    UART1_Transmit_DMA(ow_buffer_tx, 8);
}

uint8_t OW_IsBusy(void) {
    return (ow_status == OW_BUSY);
}

void OW_Reset_Async(void) {
    if (ow_status == OW_BUSY) {return;}
    ow_status = OW_BUSY;

    UART1_SetBaudRate(9600);
    UART1_Receive_DMA(&byte_res, 1);
    UART1_Transmit_DMA((uint8_t*)&ResetByte, 1);
}

uint8_t OW_GetResult(void) {
    ow_status = OW_IDLE;
    return LSB_Decode(ow_buffer_rx);
}

uint8_t OW_GetResetStatus(void) {
    ow_status = OW_IDLE;
    return byte_res;
}