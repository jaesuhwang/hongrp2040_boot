/*
 * uart.c
 *
 *  Created on: 2021. 7. 31.
 *      Author: baram
 */


#include "uart.h"
#include "qbuffer.h"


#if ( BOARD_ONLY_UART_USAGE == 0)
  #include "tusb.h"
#else   
  // #include "tusb.h"
#endif 

#include "pico/stdio/driver.h"

#ifdef _USE_HW_UART

#define UART_RX_BUF_LENGTH      1024

typedef enum
{
  UART_HW_TYPE_MCU,
  UART_HW_TYPE_USB,
} UartHwType_t;


typedef struct
{
  bool     is_open;
  uint32_t baud;
  UartHwType_t type;


  uint8_t  rx_buf[UART_RX_BUF_LENGTH];
  qbuffer_t qbuffer;

  uart_inst_t *p_uart; 
} uart_tbl_t;


static uart_tbl_t uart_tbl[UART_MAX_CH];

#if ( BOARD_ONLY_UART_USAGE == 0 )
  extern stdio_driver_t stdio_usb;
#endif

bool uartInit(void)
{
  for (int i=0; i<UART_MAX_CH; i++)
  {
    uart_tbl[i].is_open = false;
    uart_tbl[i].baud = 57600;
  }

  return true;
}

bool uartOpen(uint8_t ch, uint32_t baud)
{
  bool ret = false;


  switch(ch)
  {
    #if ( BOARD_ONLY_UART_USAGE == 0)
      case _DEF_UART1:
        uart_tbl[ch].type    = UART_HW_TYPE_USB;
        uart_tbl[ch].baud    = baud;      
        uart_tbl[ch].is_open = true;

        ret = true;
      break;
    #else 
      case _DEF_UART1:
          uart_tbl[ch].is_open = true;
          uart_tbl[ch].baud    = baud;
          uart_tbl[ch].p_uart  = uart0;

          uart_init(uart_tbl[ch].p_uart, baud);

          gpio_set_function(0, GPIO_FUNC_UART); // TX
          gpio_set_function(1, GPIO_FUNC_UART); // RX

          // Set UART flow control CTS/RTS, we don't want these, so turn them off
          uart_set_hw_flow(uart_tbl[ch].p_uart, false, false);

          // Set our data format
          uart_set_format(uart_tbl[ch].p_uart, 8, 1, UART_PARITY_NONE);

          // Turn On FIFO      
          uart_set_fifo_enabled(uart_tbl[ch].p_uart, true);

          uartFlush(ch);
          
          ret = true;
        break;
      // case _DEF_UART2:
      //     uart_tbl[ch].is_open = true;
      //     uart_tbl[ch].baud    = baud;
      //     uart_tbl[ch].p_uart  = uart1;

      //     uart_init(uart_tbl[ch].p_uart, baud);

      //     gpio_set_function(4, GPIO_FUNC_UART); // TX
      //     gpio_set_function(5, GPIO_FUNC_UART); // RX
      //     gpio_pull_up(9);  

      //     // Set UART flow control CTS/RTS, we don't want these, so turn them off
      //     uart_set_hw_flow(uart_tbl[ch].p_uart, false, false);

      //     // Set our data format
      //     uart_set_format(uart_tbl[ch].p_uart, 8, 1, UART_PARITY_NONE);

      //     // Turn On FIFO      
      //     uart_set_fifo_enabled(uart_tbl[ch].p_uart, true);

      //     uartFlush(ch);
          
      //     ret = true;
      //   break;
    #endif
  }

  return ret;
}

bool uartClose(uint8_t ch)
{
  return true;
}

uint32_t uartAvailable(uint8_t ch)
{
  uint32_t ret = 0;

  switch(ch)
  {
    #if ( BOARD_ONLY_UART_USAGE == 0)
      case _DEF_UART1:
        ret = tud_cdc_available();
        //ret = qbufferAvailable(&uart_tbl[ch].qbuffer);
        break;
    #else 
      case _DEF_UART1:
      // case _DEF_UART2:
        ret = uart_is_readable(uart_tbl[ch].p_uart);
        break;
    #endif
  }

  return ret;
}

bool uartFlush(uint8_t ch)
{
  uint32_t pre_time;

  pre_time = millis();
  while(uartAvailable(ch))
  {
    if (millis()-pre_time >= 10)
    {
      break;
    }
    uartRead(ch);
  }

  return true;
}

uint8_t uartRead(uint8_t ch)
{
  uint8_t ret = 0;

  switch(ch)
  {
    #if ( BOARD_ONLY_UART_USAGE == 0 )
      case _DEF_UART1:
        //qbufferRead(&uart_tbl[ch].qbuffer, &ret, 1);
        stdio_usb.in_chars((char *)&ret, 1);
        break;
    #else
      case _DEF_UART1:
      // case _DEF_UART2:
        ret = uart_getc(uart_tbl[ch].p_uart);
        break;
    #endif
  }

  return ret;
}

uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;

#if ( BOARD_ONLY_UART_USAGE == 1 )
  uint32_t pre_time;
  uint32_t tx_index;
#endif
  switch(ch)
  {
    #if ( BOARD_ONLY_UART_USAGE == 0 )
      case _DEF_UART1:
        //ret = (uint32_t)uwrite(uart_tbl[ch].p_huart, (void *)p_data, (int)length);
        stdio_usb.out_chars((char *)p_data, length);
        break;
    #else 
      case _DEF_UART1:
      // case _DEF_UART2:
        tx_index = 0;
        pre_time = millis();
        while(millis()-pre_time < 100)
        {
          if (uart_is_writable(uart_tbl[ch].p_uart))
          {
            uart_get_hw(uart_tbl[ch].p_uart)->dr = p_data[tx_index];
            tx_index++;
            if (tx_index >= length)
            {
              ret = length;
              break;
            }
          }
        }  
        break;    
    #endif
  }

  return ret;
}

uint32_t uartPrintf(uint8_t ch, const char *fmt, ...)
{
  char buf[256];
  va_list args;
  int len;
  uint32_t ret;

  va_start(args, fmt);
  len = vsnprintf(buf, 256, fmt, args);

  ret = uartWrite(ch, (uint8_t *)buf, len);

  va_end(args);


  return ret;
}

uint32_t uartGetBaud(uint8_t ch)
{
  uint32_t ret = 0;


  switch(ch)
  {
    case _DEF_UART1:
      ret = uart_tbl[ch].baud;
      break;
  }

  return ret;
}




#endif