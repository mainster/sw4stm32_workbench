//--------------------------------------------------------------
// File     : main.c
// Datum    : 16.06.2013
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : CMSIS_BOOT, M4_CMSIS_CORE
// Funktion : USB to UART Bridge
//              Die CPU kann per USB-OTG-Buchse
//              an einen PC angeschlossen werden
//              (Virtueller COM-Port)
//              Alle eingehenden Daten vom PC werden an einer
//              UART der CPU ausgegeben und alle eingehenden
//              Daten an der UART werden an den PC gesendet
// Hinweis  : Diese zwei Files muessen auf 8MHz stehen
//              "cmsis_boot/stm32f4xx.h"
//              "cmsis_boot/system_stm32f4xx.c"
//
// Librarys : Die eingesetzen Library für USB-CDC und UART
//            sind abgeänderte Versionen und tragen daher
//            eine Alpha-Versions-Nr (V:0.1)
//
// Portpins :
//              PA8   -> USB_OTG_SOF (wird nicht benutzt)
//              PA9   -> USB_OTG_VBUS
//              PA10  -> USB_OTG_ID
//              PA11  -> USB_OTG_DM
//              PA12  -> USB_OTG_DP
//
//              PC10  -> UART_TX  [115200 Baud, 8N1]
//              PC11  -> UART_RX
//--------------------------------------------------------------

#include "main.h"
#include "stm32_ub_usb_cdc.h"
#include "stm32_ub_uart.h"

int main(void)
{
  USB_CDC_STATUS_t check_usb;
  uint16_t rx_usb;
  uint16_t rx_uart;

  char string_buf[RX_BUF_SIZE];

  SystemInit(); // Quarz Einstellungen aktivieren

  // init vom CDC
  UB_USB_CDC_Init();

  // init vom UART
  UB_Uart_Init();

  while(1)
  {
	  // test ob USB mit PC verbunden ist
	  check_usb=UB_USB_CDC_GetStatus();
	  if(check_usb==USB_CDC_CONNECTED) {
		  // USB -> UART
		  rx_usb=UB_USB_CDC_ReceiveData(string_buf);
		  if(rx_usb>0) {
			  // wenn Daten per USB empfangen wurden
			  // -> alle Daten per UART senden
			  UB_Uart_SendData(COM3,string_buf,rx_usb);
		  }

		  // UART -> USB
		  rx_uart=UB_Uart_ReceiveData(COM3,string_buf);
		  if(rx_uart>0) {
			  // wenn Daten per UART empfangen wurden
			  // -> alle Daten per USB senden
			  UB_USB_CDC_SendData(string_buf,rx_uart);
		  }
	  }
  }
}

