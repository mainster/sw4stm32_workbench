#include "mdb_gpio.h"

/**< Declaration of all used GPIOs
 * Needs to be ordered like in typedef GPIO_NAME_t 
 */
MDB_GPIO_t  MDB_GPIO[] = {
  {LED_GREEN_A      ,GPIOG, GPIO_Pin_13, GPIO_Mode_OUT, GPIO_Speed_50MHz,  GPIO_OType_PP, GPIO_PuPd_NOPULL, RCC_AHB1Periph_GPIOG, GPIO_OFF},   
  {LED_RED_A        ,GPIOG, GPIO_Pin_14, GPIO_Mode_OUT, GPIO_Speed_50MHz,  GPIO_OType_PP, GPIO_PuPd_NOPULL, RCC_AHB1Periph_GPIOG, GPIO_OFF},   
  {BEAM_INTERRUPT   ,GPIOE, GPIO_Pin_10, GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, RCC_AHB1Periph_GPIOE, GPIO_OFF},   
  {TRIGGER_SRC      ,GPIOE, GPIO_Pin_12, GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, RCC_AHB1Periph_GPIOE, GPIO_OFF},   
  {DBG_TIMING_PE2   ,GPIOE, GPIO_Pin_2,  GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, RCC_AHB1Periph_GPIOE, GPIO_OFF},   
  {DBG_TIMING_PE4   ,GPIOE, GPIO_Pin_4,  GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, RCC_AHB1Periph_GPIOE, GPIO_OFF},   
  {DBG_TIMING_PE6   ,GPIOE, GPIO_Pin_6,  GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, RCC_AHB1Periph_GPIOE, GPIO_OFF},   
};


/**< LED toggeln
 */
void MDB_GPIO_BeamPin(MDB_GPIO_NAME_t name, MDB_GPIO_DRIVER_STATE_t newState) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = MDB_GPIO[name]._GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = MDB_GPIO[name]._GPIO_Speed;

    if (newState == GPIO_DRIVER_ON) {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    }
    else {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    }


    GPIO_Init(MDB_GPIO[name]._GPIO_PORT, &GPIO_InitStructure);
   
}


/**< Init all GPIOs listet in MDB_GPIO[]array
 */
void MDB_GPIO_Init(void) {
  GPIO_InitTypeDef  GPIO_InitStructure;
  MDB_GPIO_NAME_t   name;
  
  for(name=(MDB_GPIO_NAME_t)0; name < (MDB_GPIO_NAME_t)GPIO_COUNT; name++) {
    ///< Enable clock source
    RCC_AHB1PeriphClockCmd(MDB_GPIO[name]._GPIO_AHB_CLK, ENABLE);      

    GPIO_InitStructure.GPIO_Pin = MDB_GPIO[name]._GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = MDB_GPIO[name]._GPIO_Mode;
    GPIO_InitStructure.GPIO_OType = MDB_GPIO[name]._GPIO_OType;
    GPIO_InitStructure.GPIO_PuPd = MDB_GPIO[name]._GPIO_PuPd;
    GPIO_InitStructure.GPIO_Speed = MDB_GPIO[name]._GPIO_Speed;
    GPIO_Init(MDB_GPIO[name]._GPIO_PORT, &GPIO_InitStructure);

    // Default Wert einstellen
    if(MDB_GPIO[name]._GPIO_INIT==GPIO_OFF) {
      MDB_GPIO_Off(name);
    }
    else {
      MDB_GPIO_On(name);
    }
  }
}


/**< Enable GPIO 
 */
void MDB_GPIO_Off(MDB_GPIO_NAME_t name)
{
  MDB_GPIO[name]._GPIO_PORT->BSRRH = MDB_GPIO[name]._GPIO_PIN;
}

/**< Disable GPIO 
 */
void MDB_GPIO_On(MDB_GPIO_NAME_t name)
{
  MDB_GPIO[name]._GPIO_PORT->BSRRL = MDB_GPIO[name]._GPIO_PIN;
} 

/**< LED toggeln
 */
void MDB_GPIO_Toggle(MDB_GPIO_NAME_t name) {
  MDB_GPIO[name]._GPIO_PORT->ODR ^= MDB_GPIO[name]._GPIO_PIN;
}

//--------------------------------------------------------------
// LED ein- oder ausschalten
//--------------------------------------------------------------
void MDB_GPIO_Switch(MDB_GPIO_NAME_t name, MDB_GPIO_STATE_t newState)
{
  if(newState == GPIO_OFF) {
    MDB_GPIO_Off(name);
  }
  else {
    MDB_GPIO_On(name);
  }
}
