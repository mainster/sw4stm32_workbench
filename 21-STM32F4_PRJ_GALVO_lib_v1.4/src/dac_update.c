#include "main.h"
#include "tools.h"
#include "dac_update.h"
#include "adc_dac_dma.h"


static struct autoSaveSystem ass;
//static DAC_WritePointer_t (*DAC_SecureSetDualChanSigned) (int16_t, int16_t);
DAC_WritePointer_t (*DAC_SecureSetDualChanSigned) (int16_t, int16_t);

/**
 * @brief   Safely update actuator control signals
 * Safely means in terms of missplaced or oscillating controller outputs.
 * The analog watchdog peripheral takes care about "out of normal range" 
 * events. If a converted position signal is out of range, the watchdog 
 * handler starts time integration and after "out of range" integrator 
 * reaches a defineable limit, the watchdog takes the system to into
 * "Tripped" state.
 * Therefor a global error flag becomes true AND the function pointer
 * DAC_SetDualChanSigned() that points to a wraper function, gots to be
 * replaced by a pointer that points to DAC_SetDualChanSigned_Tripped().
 * In the DAC_SetDualChanSigned() function, that is pointed to in tripped 
 * state, only outputs a zero level DAC-Signal to take external hardware 
 * in a safe state.
 */
int updateActuator_f(float I_set_x, float I_set_y) {
    int toPlant_intBuff[2] = { 0, 0 };
    int *toPlant_int = &toPlant_intBuff[0];
    
    if (!ass.tripped) {     ///< only if ass state is NOT tripped

        /**< decode the float values to an integral type */
        *toPlant_int     = decode_toInt(I_set_x);
        *(toPlant_int+1) = decode_toInt(I_set_y);

        /**
         * Check decoded values to be in DAC output range. Clipping 
         * would be necessary 
         */
        for (uint8_t k=0; k<2; k++) {
            if (*(toPlant_int+k) > UPPER_DAC_LIMIT_SIGNED)
                    *(toPlant_int+k) = UPPER_DAC_LIMIT_SIGNED;
            if (*(toPlant_int+k) < LOWER_DAC_LIMIT_SIGNED)
                    *(toPlant_int+k) = LOWER_DAC_LIMIT_SIGNED;
        }
    } 
    else{   
    /**<><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>    
       <>  This branch eror-handles an ass integrator_full event i.e.<>
       <><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>    
       <>                    FUSE TRIPPED                            <>
       <><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<> */  
       /* All safety related actions are done by the watchdog handler */ 
    }

   /**
    * @brief    Return argument toPlant_int is of type int16_t because 
    *           it holds the --un--biased DAC output register values...
    *           This is a function pointer!
    * Casting from (float32_t) toPlant to int16_t type introduces 
    * rounding errors which couldn't be prevented! 
    */
    DAC_SecureSetDualChanSigned( *toPlant_int, *(toPlant_int+1));  
        
    return 0;
}

