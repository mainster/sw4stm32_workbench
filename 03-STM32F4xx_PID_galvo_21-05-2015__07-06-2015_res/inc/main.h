/**
 *	Keil project for XY-GalvoScanner
 *  29-04-2015
 *
 *
 *	@author		Manuel Del Basso
 *	@email		Manuel.DelBasso@googlemail.com  
 *	@ide		Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */
#ifndef __MAIN_H
#define __MAIN_H 100

#include "defines.h"
#include "attributes.h"
#include "mdb_gpio.h"
#include "pid.h"


/* Acess macros for actual ADC values 
 * ToDo:    Maybe this macros needs to be atomic! 
 */

//extern __IO int16_t ADC_MultiConvBuff[5];
//extern __IO float ADC_fBuff[5];

#define POS_X_FLOAT         ADC_fBuff[INDEX_Px]	        /* actual position */
#define POS_Y_FLOAT         ADC_fBuff[INDEX_Py]	        /* actual position */
#define SETPOINT_Y_FLOAT	ADC_fBuff[INDEX_W]	        /* position we want to have */

extern float pidErrBuff[2];        /* PID error buffer*/
extern float toPlantBuff[2];        /* PID output buffer*/


#define pidErr_x    pidErrBuff[0]
#define pidErr_y    pidErrBuff[1]
#define toPlant_x   toPlantBuff[0]
#define toPlant_y   toPlantBuff[1]



typedef enum {
    DB_ADC1_DUAL,
    DB_ADC2_DUAL,
    DB_ADC3_DUAL
} DB_ADCx_DUAL_t;

typedef enum {
    _FLOAT_,
    _INT_,
    _UINT_ 
} structType;

typedef enum {
    FALSE = 0,
    TRUE,
    DNI        ///< Do Not Interpret
} tribool_state_t;

/* ================================================================================ */
enum cmd_items { 
	KP, KI, KD, W, UNKNOWN
};		///< Enumeration for switch-by-string casees

/* ================================================================================ */
static struct items									///< Struct for switch by string casees  
{
  char *name;
  enum cmd_items id;
} 
items_list[] = {
  { "kp:", KP },
  { "ki:", KI },
  { "kd:", KD },
  { "w::", W  },
};

// ==============================================================
//            wave command items struct
// ==============================================================
enum wav_items { 							///< Enumeration for switch-by-string casees 
	NN, 										///< unknown wave form received		
	EN, 										///< Generator enable
	DIS,										///< Generator disable
	COS, TRIANG, SQUAREWAV, 			///< Usable waveforms
	QUADRATIC, I_QUADRATIC, SAWTOOTH,	
    CMD_REMOTE_SETPOINT, 
    CMD_OPENLOOP_REMOTE, 
    CMD_ANALOG_SETPOINT,
    CMD_INTERNAL_SETPOINT,
    CMD_REFRESH_RATE,
    CMD_TESTVECT,
    
    /* CMD_MATLAB_LINK 
     *
     * # c c c : = F F F F  d  d  A ..16x.. A 
     * #mat:=25~~0~
     *
     */
    CMD_MATLAB_LINK
	/**< Quadratic: testsignal zum vergleich mit ݢersichtsblatt "Was f𲠍
	  Regler notwendig um quadratischen sollwertverl嶦en noch...."
	  (siehe Vorlesung H. Brunner) 
	*/
};

/* ================================================================================ */
static struct itemsw								///< Struct for switch-by-string casees  
{
  char *name;
  enum wav_items idw;
} 	
itemsw_list[] = {
	{ "sin" , COS },
	{ "tri" , TRIANG },
	{ "rec" , SQUAREWAV },
	{ "s2 " , QUADRATIC },
	{ "is2" , I_QUADRATIC },
	{ "saw" , SAWTOOTH},
	{ "rem" , CMD_REMOTE_SETPOINT},
	{ "orm" , CMD_OPENLOOP_REMOTE},
	{ "ast" , CMD_ANALOG_SETPOINT},
	{ "ist" , CMD_INTERNAL_SETPOINT},
	{ "rfr" , CMD_REFRESH_RATE},
	{ "vec" , CMD_TESTVECT},
	{ "mat" , CMD_MATLAB_LINK},
    
};


// ==============================================================
//            MISC command items struct
// ==============================================================
enum misc_items { 
    misc_NN,
    misc_none,
	misc_pid_init, 
    misc_pid_Controller, 
    misc_pid_Reset_Integrator,
    misc_update_pid_data,
    misc_assOnOff_cmd,          ///< @enum  AutoSafetyShutdown Enable/Disable cmd
    misc_assUpperLim_cmd,       ///< @enum  AutoSafetyShutdown set Upper Limit command
    misc_assLowerLim_cmd,       ///< @enum  AutoSafetyShutdown set Lower Limit command
    misc_assTrippTime_cmd,      ///< @enum  AutoSafetyShutdown set Tripping time command
    misc_assSaveVal_cmd,        ///< @enum  AutoSafetyShutdown set Safe value command
    misc_beamOn_cmd,            ///< @enum  Beam enable disable command enumeration
    misc_beamOff_cmd,           ///< @enum  Beam enable disable command enumeration
    };		///< Enumeration for switch-by-string casees 

/* ================================================================================ */
static struct itemsm    ///< Struct for switch-by-string casees  
{
  char *name;
  enum misc_items idm;
} 	
itemsm_list[] = {
  { "pid_init",             misc_pid_init },
  { "pid_Controller",       misc_pid_Controller },
  { "pid_Reset_Integrator", misc_pid_Reset_Integrator },
  { "update_pid_data" ,     misc_update_pid_data  },
  { "safetyEnable" ,        misc_assOnOff_cmd  },
  { "safetyUpperLim" ,      misc_assUpperLim_cmd  },
  { "safetyLowerLim" ,      misc_assLowerLim_cmd  },
  { "safetyTripp" ,         misc_assTrippTime_cmd  },
  { "safetyVal" ,           misc_assSaveVal_cmd  },
  { "EnableBeam" ,          misc_beamOn_cmd  },
  { "DisableBeam" ,         misc_beamOff_cmd  },
};

/* ================================================================================ */
typedef struct {
    int16_t lowerLim;
    int16_t upperLim;
} dac_lim_t;

/* ================================================================================ */
///**< typedef for different setpoint source select */
//typedef enum {
//    INTERNAL_SETPOINT,          /**< Setpoint signal generated by software... */
//    ANALOG_SETPOINT,            /**< Setpoint by analog waveform, converted by I_L adc line yet */
//    REMOTE_SETPOINT,            /**< Setpoint commands/Signal over serial interface, NO internal setpoint sources */
//    REMOTE_OPENLOOP,             /**< No active PID - Feed remote setpoint to hardware */
//    REMOTE_INTERNAL_MIXED       /**< Iternal setpoint + remote setpoint */
//} setpoint_src_t;
/**< typedef for different setpoint source select */

#pragma GCC push_options
#pragma GCC optimize ("O0")
enum setpoint_src_t{
    INTERNAL_SETPOINT,          /**< Setpoint signal generated by software... */
    ANALOG_SETPOINT,            /**< Setpoint by analog waveform, converted by I_L adc line yet */
    REMOTE_SETPOINT,            /**< Setpoint commands/Signal over serial interface, NO internal setpoint sources */
    REMOTE_OPENLOOP,             /**< No active PID - Feed remote setpoint to hardware */
    REMOTE_INTERNAL_MIXED       /**< Iternal setpoint + remote setpoint */
};



// ==============================================================
//            Global state struct
// ==============================================================
struct global {
	enum 	wav_items waveForm;	/**< Holds enum for selected wave form. */
	enum 	wav_items gen;      /**< This is the signal generator enable/disable param. */
	
	uint32_t  	lookAt;			/**< Integer multiple of @SAMPLE_TIME. USe it as interval 
                                        counter for lookup table index increments. */
	uint16_t  	freq;           /**< Frequency for waveform generation [kHz]. */
	uint16_t  	tctr;           /**< lookup sequence counter. */
	uint16_t   ampl;           /**< Holds the given value of peak amplitude. */
	int16_t	*pBase;         /**< Base address holder for look up ptr. */
	uint16_t	duty;           /**< Duty parameter (uint16_t). */
	float       dutyCyc;        /**< Duty cycle for waveform generation [%]. */	
    dac_lim_t   dacHw[2];       /**< DAC1/DAC2 range values to implement hardware protection  */
	enum    setpoint_src_t     setpointSrc;
	enum 	misc_items miscReq; /**< Holds the misc command requested by serial interface
                                  * @NOTE   clear this member to misc_none at the end of the misc cmd */  
    uint16_t    refresh;       /**< refresh rate in ms for mainloop */
    float        ampl_f;        /**< float amplitude for generated waveform */
    MDB_GPIO_STATE_t      beamEnabled;    /**< On Off Flag for Laser beam */
};  

//struct triangWave {
//    uint16_t    triCtr;
//    float    bottom;     /**< (top-bottom)*n Timer ovf's => Amplitude */     
//    float    top;
//    float    derivate;   /**< derivative to increase V-DAC by Amplitude after n Timer Ovf's */
//};

typedef enum {
    ASS_CHARGING_INTEGRATOR,
    ASS_DISCHARGING_INTEGRATOR,
    ASS_STATIONARY_INTEGRATOR
} ass_int_state_t;

// ==============================================================
//            Auto safestate system ASS
// ==============================================================
struct autoSaveSystem {
    float   lowerVal;    /**< [V] Lower value ADC_f less than this value triggers the ass*/
    float   upperVal;    /**< [V] Upper value ADC_f greater than this value triggers the ass*/
    float   safeVal;     /**< [V] If ass tripped, this output value becomes active*/
    double  integrator;  /**< runtime integrator - holds the "amount" of overcurrent - states */
    float   tripTime;    /**< [ms] Time constant, used to change tripping characteristics at runtime */
    uint8_t tripped;    /**< tripped != 0 if tripping is active */
    uint8_t ack;        /**< acknowlage tripped stat */
    ass_int_state_t state;
    uint8_t stateCtr;
};
/* ================================================================================ */

typedef enum {
    BEAM_CTRL_SOURCE_GLOBAL,
    BEAM_CTRL_SOURCE_MANUAL
} beamCtrlSource_t;

/* ================================================================================ */
extern struct global g;
extern struct autoSaveSystem ass;
extern struct triangWave triStruct;
extern float setpoint_tgl;

    #pragma GCC pop_options
    
#endif
