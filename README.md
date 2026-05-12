# Axel-s-HAL
A simple handwritten HAL for STM32

> [!IMPORTANT]
This "HAL" is made for my own specific purposes (mostly learning the LL coding); it's __not__ universal like the original STM32 HAL

> Though it might be useful to see "what's going on under the hood."

TODO:
- [x] UART implementation
- [x] 1-Wire emulation through UART1 (Partially)
- [ ] I2C implementation
- [ ] SPI implementation
- [ ] SSD1306 LL library

UART:  
Designed for STM32F4  
Fixed mapping: UART1 is bound to DMA2 Stream 2 & 7  
Flash Consumption: ~800 bytes  

1-Wire:  
Designed for STM32F4  
Requires uart_driver.c to work properly  
Flash Consumption: ~60 bytes  

> [!NOTE]
> 1-Wire requires TX (Open-Drain) and RX to be tied together with a 4.7k pull-up resistor.  
> Or you may turn on Half-Duplex UART mode in STM32CubeMX when creating the project

# A brief description of the basic functions


## Synchronous UART transmission with interface selection

Function structure:
```c
void UART_Transmit(USART_TypeDef *port, uint8_t *data, uint32_t len);
```
Simple and fast way to work with UART if DMA is not needed  
Example:
```c
// main.c
#include "uart_driver.h"
UART_Transmit(USART1, "Hello", 5);
```

## Asynchronous UART transmission with hardcoded interface
Function structure:
```c
void UART1_Transmit_DMA(uint8_t *buffer, uint16_t size);
```
Example:
```c
// main.c
#include "uart_driver.h"
uint8_t Byte = 0x55;
UART1_Transmit_DMA(&Byte, 1);
```
You also can process the interrupts in stm32f4xx_it.c file:
```c
// stm32f4xx_it.c
#include "uart_driver.h"
void DMA2_Stream7_IRQHandler(void) // This callback is created by STM32CubeMX code generator
{
  if (LL_DMA_IsActiveFlag_TC7(DMA2)) {
    // Transfer Complete
    LL_DMA_ClearFlag_TC7(DMA2); // Clear the flag

    // while (!LL_USART_IsActiveFlag_TC(USART1)){} // Optional
    // LL_USART_DisableDMAReq_TX(USART1); // Optional
    UART1_DMA_TX_State = 1;
  }
  if (LL_DMA_IsActiveFlag_HT7(DMA2)) {
    // Half Transfer
    LL_DMA_ClearFlag_HT7(DMA2);
    UART1_DMA_TX_State = 2;
  }
}
```

## Asynchronous UART receive with hardcoded interface
Function structure:
```c
void UART1_Receive_DMA(uint8_t *buffer, uint16_t size);
```
Example:
```c
// main.c
#include "uart_driver.h"
uint8_t rx_buf = 0;
UART1_Receive_DMA(&rx_buf, 1);
```
You also can process the interrupts in stm32f4xx_it.c file:
```c
// stm32f4xx_it.c
#include "uart_driver.h"
void DMA2_Stream2_IRQHandler(void) // This callback is created by STM32CubeMX code generator
{
  if (LL_DMA_IsActiveFlag_TC2(DMA2)) {
    // Transfer Complete
    LL_DMA_ClearFlag_TC2(DMA2); // Clear the flag
    // LL_USART_DisableDMAReq_RX(USART1); // Optional
    UART1_DMA_RX_State = 1;
  }
  if (LL_DMA_IsActiveFlag_HT2(DMA2)) {
    // Half Transfer
    LL_DMA_ClearFlag_HT2(DMA2);
    UART1_DMA_RX_State = 2;
  }
}
```

## Changing the baud rate of the UART1 interface
Function structure:
```c
void UART1_SetBaudRate(uint32_t baud);
```
Note: The function recalculates the clock dividers itself; the frequency of PCLK is not hardcoded  
Note 2: The function uses 16x Oversampling all the time  
Example:
```c
// main.c
#include "uart_driver.h"
UART1_SetBaudRate(9600);
```
