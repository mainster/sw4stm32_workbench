//--------------------------------------------------------------
// File     : stm32_ub_uart.c
// Datum    : 16.06.2013
// Version  : 0.1
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : GPIO, USART, MISC
// Funktion : UART (RS232) In und OUT
//            Receive wird per Interrupt behandelt
//
// Hinweis  : mögliche Pinbelegungen
//            UART1 : TX:[PB6] RX:[PB7]  (PA9 und PA10 sind von USB belegt)
//            UART2 : TX:[PA2,PD5] RX:[PA3,PD6]
//            UART3 : TX:[PB10,PC10,PD8] RX:[PB11,PC11,PD9]
//            UART4 : TX:[PA0,PC10] RX:[PA1,PC11]
//            UART5 : TX:[PC12] RX:[PD2]
//            UART6 : TX:[PC6,PG14] RX:[PC7,PG9]
//
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_uart.h"




//--------------------------------------------------------------
// Definition aller UARTs
// Reihenfolge wie bei UART_NAME_t
//--------------------------------------------------------------
UART_t UART[] = {
// Name, Clock               , AF-UART      ,UART  , Baud, Interrupt
  {COM3,RCC_APB1Periph_USART3,GPIO_AF_USART3,USART3,115200,USART3_IRQn, // UART3 mit 115200 Baud
// PORT , PIN      , Clock              , Source
  {GPIOC,GPIO_Pin_10,RCC_AHB1Periph_GPIOC,GPIO_PinSource10},  // TX an PC10
  {GPIOC,GPIO_Pin_11,RCC_AHB1Periph_GPIOC,GPIO_PinSource11}}, // RX an PC11
};





//--------------------------------------------------------------
// init aller UARTs
//--------------------------------------------------------------
void UB_Uart_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  UART_NAME_t nr;

  for(nr=0;nr<UART_ANZ;nr++) {

    // Clock enable der TX und RX Pins
    RCC_AHB1PeriphClockCmd(UART[nr].TX.CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(UART[nr].RX.CLK, ENABLE);

    // Clock enable der UART
    if((UART[nr].UART==USART1) || (UART[nr].UART==USART6)) {
      RCC_APB2PeriphClockCmd(UART[nr].CLK, ENABLE);
    }
    else {
      RCC_APB1PeriphClockCmd(UART[nr].CLK, ENABLE);
    }

    // UART Alternative-Funktions mit den IO-Pins verbinden
    GPIO_PinAFConfig(UART[nr].TX.PORT,UART[nr].TX.SOURCE,UART[nr].AF);
    GPIO_PinAFConfig(UART[nr].RX.PORT,UART[nr].RX.SOURCE,UART[nr].AF);

    // UART als Alternative-Funktion mit PushPull
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    // TX-Pin
    GPIO_InitStructure.GPIO_Pin = UART[nr].TX.PIN;
    GPIO_Init(UART[nr].TX.PORT, &GPIO_InitStructure);
    // RX-Pin
    GPIO_InitStructure.GPIO_Pin =  UART[nr].RX.PIN;
    GPIO_Init(UART[nr].RX.PORT, &GPIO_InitStructure);

    // Oversampling
    USART_OverSampling8Cmd(UART[nr].UART, ENABLE);

    // init mit Baudrate, 8Databits, 1Stopbit, keine Parität, kein RTS+CTS
    USART_InitStructure.USART_BaudRate = UART[nr].BAUD;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART[nr].UART, &USART_InitStructure);

    // UART enable
    USART_Cmd(UART[nr].UART, ENABLE);

    // RX-Interrupt enable
    USART_ITConfig(UART[nr].UART, USART_IT_RXNE, ENABLE);

    // enable UART Interrupt-Vector
    NVIC_InitStructure.NVIC_IRQChannel = UART[nr].INT;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // RX-Puffer vorbereiten
    UART_RX[nr].rx_buffer[0]=RX_END_CHR;
    UART_RX[nr].wr_ptr=0;
    UART_RX[nr].status=UART_RX_EMPTY;
  }
}

//--------------------------------------------------------------
// ein Byte per UART senden
//--------------------------------------------------------------
void UB_Uart_SendByte(UART_NAME_t uart, uint16_t wert)
{
  // warten bis altes Byte gesendet wurde
  while (USART_GetFlagStatus(UART[uart].UART, USART_FLAG_TXE) == RESET);
  USART_SendData(UART[uart].UART, wert);
}

//--------------------------------------------------------------
// Daten per UART senden
//--------------------------------------------------------------
void UB_Uart_SendData(UART_NAME_t uart, char *ptr, uint16_t cnt)
{
  uint16_t n;

  // alle Daten senden
  for(n=0;n<cnt;n++) {
    UB_Uart_SendByte(uart,*ptr);
    ptr++;
  }
}

//--------------------------------------------------------------
// Daten per UART empfangen
// (der Empfang wird per Interrupt abgehandelt)
// diese Funktion muss zyklisch gepollt werden
// Return Wert :
//   0 = nichts vorhanden
//  >0 = Anzahl der empfangenen Zeichen
//--------------------------------------------------------------
uint16_t UB_Uart_ReceiveData(UART_NAME_t uart, char *ptr)
{
  uint16_t ret_wert=0;
  uint8_t n,wert;

  if(UART_RX[uart].wr_ptr==0) return(0);

  ret_wert=UART_RX[uart].wr_ptr;

  for(n=0;n<UART_RX[uart].wr_ptr;n++) {
	  wert=UART_RX[uart].rx_buffer[n];
	  ptr[n]=wert;
  }
  UART_RX[uart].wr_ptr=0;

  return(ret_wert);
}


//--------------------------------------------------------------
// interne Funktion
// speichern des empfangenen Zeichens im Puffer
//--------------------------------------------------------------
void P_UART_Receive(UART_NAME_t uart, uint16_t wert)
{
  uint8_t n;

  if(UART_RX[uart].wr_ptr<RX_BUF_SIZE) {
    // Byte im Puffer speichern
	n=UART_RX[uart].wr_ptr;
    UART_RX[uart].rx_buffer[n]=wert;
    UART_RX[uart].wr_ptr++;
  }
  else {
    // wenn Puffer voll ist
    UART_RX[uart].status=UART_RX_FULL;
  }
}


//--------------------------------------------------------------
// interne Funktion
// UART-Interrupt-Funktion
// Interrupt-Nr muss übergeben werden
//--------------------------------------------------------------
void P_UART_RX_INT(uint8_t int_nr, uint16_t wert)
{
  UART_NAME_t nr;

  // den passenden Eintrag suchen
  for(nr=0;nr<UART_ANZ;nr++) {
    if(UART[nr].INT==int_nr) {
      // eintrag gefunden, Byte speichern
      P_UART_Receive(nr,wert);
      break;
    }
  }
}


//--------------------------------------------------------------
// UART1-Interrupt
//--------------------------------------------------------------
void USART1_IRQHandler(void) {
  uint16_t wert;

  if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
    // wenn ein Byte im Empfangspuffer steht
    wert=USART_ReceiveData(USART1);
    // Byte speichern
    P_UART_RX_INT(USART1_IRQn,wert);
  }
}

//--------------------------------------------------------------
// UART2-Interrupt
//--------------------------------------------------------------
void USART2_IRQHandler(void) {
  uint16_t wert;

  if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
    // wenn ein Byte im Empfangspuffer steht
    wert=USART_ReceiveData(USART2);
    // Byte speichern
    P_UART_RX_INT(USART2_IRQn,wert);
  }
}


//--------------------------------------------------------------
// UART3-Interrupt
//--------------------------------------------------------------
void USART3_IRQHandler(void) {
  uint16_t wert;

  if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
    // wenn ein Byte im Empfangspuffer steht
    wert=USART_ReceiveData(USART3);
    // Byte speichern
    P_UART_RX_INT(USART3_IRQn,wert);
  }
}

//--------------------------------------------------------------
// UART4-Interrupt
//--------------------------------------------------------------
void USART4_IRQHandler(void) {
  uint16_t wert;

  if (USART_GetITStatus(UART4, USART_IT_RXNE) == SET) {
    // wenn ein Byte im Empfangspuffer steht
    wert=USART_ReceiveData(UART4);
    // Byte speichern
    P_UART_RX_INT(UART4_IRQn,wert);
  }
}

//--------------------------------------------------------------
// UART5-Interrupt
//--------------------------------------------------------------
void USART5_IRQHandler(void) {
  uint16_t wert;

  if (USART_GetITStatus(UART5, USART_IT_RXNE) == SET) {
    // wenn ein Byte im Empfangspuffer steht
    wert=USART_ReceiveData(UART5);
    // Byte speichern
    P_UART_RX_INT(UART5_IRQn,wert);
  }
}

//--------------------------------------------------------------
// UART6-Interrupt
//--------------------------------------------------------------
void USART6_IRQHandler(void) {
  uint16_t wert;

  if (USART_GetITStatus(USART6, USART_IT_RXNE) == SET) {
    // wenn ein Byte im Empfangspuffer steht
    wert=USART_ReceiveData(USART6);
    // Byte speichern
    P_UART_RX_INT(USART6_IRQn,wert);
  }
}
