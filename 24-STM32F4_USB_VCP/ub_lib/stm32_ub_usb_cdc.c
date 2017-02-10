//--------------------------------------------------------------
// File     : stm32_ub_usb_cdc.c
// Datum    : 16.06.2013
// Version  : 0.1
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : GPIO, MISC
// Funktion : USB-COM-Verbindung per USB-OTG-Port am Discovery
//
// Hinweis  : auf dem PC muss der Treiber von ST
//            "VirtualComportDriver" installiert sein (V:1.3.1)
//            nur dann wird beim Verbinden ein VirtuellerComPort
//            im Ger�temanager angezeigt
//
//            Beim Discovery-Modul funktioniert nur der
//            USB-FullSpeed-Mode (USB-High-Speed geht nicht)
//--------------------------------------------------------------
//              PA8   -> USB_OTG_SOF (wird nicht benutzt)
//              PA9   -> USB_OTG_VBUS
//              PA10  -> USB_OTG_ID
//              PA11  -> USB_OTG_DM
//              PA12  -> USB_OTG_DP
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_usb_cdc.h"

//--------------------------------------------------------------
// Globale Variabeln
//--------------------------------------------------------------
USB_OTG_CORE_HANDLE  USB_OTG_dev;





//--------------------------------------------------------------
// Init vom USB-OTG-Port als CDC-Device
// (Virtueller ComPort)
//--------------------------------------------------------------
void UB_USB_CDC_Init(USB_OTG_CORE_ID_TypeDef coreID)
{
  USB_CDC_STATUS=USB_CDC_DETACHED;
  USBD_Init(&USB_OTG_dev, coreID, &USR_desc, &USBD_CDC_cb, &USR_cb);
}

//--------------------------------------------------------------
// auslesen vom Status der USB-Schnittstelle
// Return_wert :
//  -> USB_CDC_NO_INIT   , USB-Schnittstelle noch nicht initialisiert
//  -> USB_CDC_DETACHED  , USB-Verbindung getrennt
//  -> USB_CDC_CONNECTED , USB-Verbindung hergestellt
//--------------------------------------------------------------
USB_CDC_STATUS_t UB_USB_CDC_GetStatus(void)
{
  return(USB_CDC_STATUS);
}


//--------------------------------------------------------------
// Sendet Daten per USB-OTG-Schnittstelle
// Return_wert :
//  -> ERROR   , wenn gesendet wurde
//  -> SUCCESS , wenn nicht gesendet wurde
//--------------------------------------------------------------
ErrorStatus UB_USB_CDC_SendData(char *ptr, uint16_t cnt)
{
  uint16_t n;

  if(USB_CDC_STATUS!=USB_CDC_CONNECTED) {
  // senden nur, wenn Verbindung hergestellt ist
    return(ERROR);
  }

  // alle Daten senden
  for(n=0;n<cnt;n++) {
    UB_VCP_DataTx(*ptr);
    ptr++;
  }

  return(SUCCESS);
}



//--------------------------------------------------------------
// Daten per USB-OTG-Schnittstelle empfangen
// (der Empfang wird per Interrupt abgehandelt)
// diese Funktion muss zyklisch gepollt werden
// Return Wert :
//   0 = nichts vorhanden
//  >0 = Anzahl der empfangenen Zeichen
//--------------------------------------------------------------
uint16_t UB_USB_CDC_ReceiveData(char *ptr)
{
  uint16_t check;

  if(USB_CDC_STATUS!=USB_CDC_CONNECTED) {
    // empfangen nur, wenn Verbindung hergestellt ist
    return(0);
  }

  check=UB_VCP_DataRx(ptr);
  if(check==0) {
    ptr[0]=0x00;
    return(0);
  }

  return(check);
}
