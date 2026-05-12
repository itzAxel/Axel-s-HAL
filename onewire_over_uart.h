#ifndef LL_ONEWIRE_OVER_UART_H
#define LL_ONEWIRE_OVER_UART_H

#include "main.h"

void OW_Callback(void);
uint8_t OW_Reset_Blocking(void);
void OW_SendByte_Async(uint8_t byte);
uint8_t OW_IsBusy(void);
void OW_Reset_Async(void);
uint8_t OW_GetResult(void);
uint8_t OW_GetResetStatus(void);

#endif //LL_ONEWIRE_OVER_UART_H