//--------------------------------------------------------------
// File     : stm32_ub_uart.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_UART_H
#define __STM32F4_UB_UART_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"



//--------------------------------------------------------------
// Liste aller UARTs
// (keine Nummer doppelt und von 0 beginnend)
//--------------------------------------------------------------
typedef enum
{
  COM3 = 0  // COM3 (TX=PC10, RX=PC11)
}UART_NAME_t;

#define  UART_ANZ   1 // Anzahl von UART_NAME_t


//--------------------------------------------------------------
// Endekennung beim Senden
//--------------------------------------------------------------
typedef enum {
  UART_NONE = 0,  // keine Endekennung
  UART_LFCR,      // LineFeed + CarriageReturn (0x0A,0x0D)
  UART_CRLF,      // CarriageReturn + LineFeed (0x0D,0x0A)
  UART_LF,        // nur LineFeed (0x0A)
  UART_CR         // nur CarriageReturn (0x0D)
}UART_LASTBYTE_t;


//--------------------------------------------------------------
// Status beim Empfangen
//--------------------------------------------------------------
typedef enum {
  UART_RX_EMPTY = 0,  // nichts empfangen
  UART_RX_READY,      // es steht was im Empfangspuffer
  UART_RX_FULL        // RX-Puffer ist voll
}UART_RXSTATUS_t;


//--------------------------------------------------------------
// Struktur eines UART-Pins
//--------------------------------------------------------------
typedef struct {
  GPIO_TypeDef* PORT;     // Port
  const uint16_t PIN;     // Pin
  const uint32_t CLK;     // Clock
  const uint8_t SOURCE;   // Source
}UART_PIN_t;

//--------------------------------------------------------------
// Struktur eines UARTs
//--------------------------------------------------------------
typedef struct {
  UART_NAME_t UART_NAME;    // Name
  const uint32_t CLK;       // Clock
  const uint8_t AF;         // AF
  USART_TypeDef* UART;      // UART
  const uint32_t BAUD;      // Baudrate
  const uint8_t INT;        // Interrupt
  UART_PIN_t TX;            // TX-Pin
  UART_PIN_t RX;            // RX-Pin
}UART_t;


//--------------------------------------------------------------
// Defines fuer das Empfangen
//--------------------------------------------------------------
#define  RX_BUF_SIZE   50    // Grösse vom RX-Puffer in Bytes
#define  RX_FIRST_CHR  0x20  // erstes erlaubte Zeichen (Ascii-Wert)
#define  RX_LAST_CHR   0x7E  // letztes erlaubt Zeichen (Ascii-Wert)
#define  RX_END_CHR    0x0D  // Endekennung (Ascii-Wert)


//--------------------------------------------------------------
// Struktur für UART_RX
//--------------------------------------------------------------
typedef struct {
  char rx_buffer[RX_BUF_SIZE]; // RX-Puffer
  uint8_t wr_ptr;              // Schreib Pointer
  UART_RXSTATUS_t status;      // RX-Status
}UART_RX_t;
UART_RX_t UART_RX[UART_ANZ];


//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_Uart_Init(void);
void UB_Uart_SendByte(UART_NAME_t uart, uint16_t wert);
void UB_Uart_SendData(UART_NAME_t uart, char *ptr, uint16_t cnt);
uint16_t UB_Uart_ReceiveData(UART_NAME_t uart, char *ptr);



//--------------------------------------------------------------
#endif // __STM32F4_UB_UART_H
