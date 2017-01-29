///**
// * @file        console.c
// *
// * @date        29 Jan 2017
// * @author      Manuel Del Basso (mainster)
// * @email       manuel.delbasso@gmail.com
// *
// * @ide         System Workbench ac6 (eclipse stm32)
// * @stdperiph   STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
// * @license		GNU GPL v3
// *
//   @verbatim
//
//	Copyright (C) 2016	Manuel Del Basso
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//	@endverbatim
// *
// */
//#include "console.h"
//
//
///*
// *       @,!,# are newline indicators or line separators
// *
// *       @kp   :=   58
// *       #sin  :=   F F F F  d  d  A  A   A   A
// *       @w:   :=   0.0005452
// *     |______|____|________________|
// *     |  NC  | FS |      NV        |
// *
// *        3      2          16
// *
// *  => UART_BUFF_SIZE = 23 (newline + 21 + \0)
// *
// */
//
//
///*! \brief Fast console command parser
// *
// * Interface to a flat implemented signal generator based on lookup tables
// * Implemented waveforms:
// *
// * Sin/Cos:			Measurements
// *
// */
//
//
//
////void fastConsoleCase (volatile struct PID_DATA *pid) {
//#if  defined (KEIL_IDE)
//#pragma O0
//#elif  defined (__GNUC__)
//#pragma GCC push_options
//#pragma GCC optimize ("O0")
//#endif
//
//void fastConsoleCase (arm_pid_instance_f32 *pid) {
//	/**< private member declarations */
//	char sUart[UART_BUFF_SIZE],		///< string buffer to hold received input string
//	     sCmd[CMD_BUFF_SIZE],		///< string buffer to hold selected command as substring
//	     sVal[VALUE_BUFF_SIZE];     ///< string buffer to hold given value as substring
//	/**<
//	 * sVal holds the string representation of a received float value.
//	 * For useing atof() , a buffer of 4xsizeof(int8_t)+1 [--> float32_t] or 8xsizeof(int8_t) [--> double]
//	 * is necessary
//	 */
//
//	char *pCmd = sCmd;		        ///< pointer for buffer arrays
//	char *pVal = sVal;		        ///< pointer for buffer arrays
//
//	cmd_items_t cmd;
//	wav_items_t wav;
//	struct items *choice = NULL;
//	itemsw_t *choicew = NULL;
//
//	int i;
//
//
//	volatile uint8_t nChars = TM_USART_Gets( USART1, &sUart[0], UART_BUFF_SIZE);
//
//	/**< Check for serial data frames >= UART_BUFF_SIZE */
//	if (nChars < UART_BUFF_SIZE) {
//		MD_DISCO_LedOn(LED_RED);
//		memset(&sUart, 0, UART_BUFF_SIZE * sizeof(char));  ///< malloc the receive buffer
//		return;
//	}
//
//	MD_DISCO_LedOff(LED_RED);
//	MD_DISCO_LedOn(LED_GREEN);
//	ticks = 100000;   // 200k *10us = 2s
//
//	/**< malloc() if a frame received where sizeoff() >= UART_BUFF_SIZE */
//	memset(&sCmd, 0, CMD_BUFF_SIZE * sizeof(char));  ///< malloc command buffer
//	memset(&sVal, 0, VALUE_BUFF_SIZE * sizeof(char));   ///< malloc value buffer
//
//	/**< Check leading character via receive buffer
//	 *      @   frame indicator for pid related config/param.
//	 *      !   frame indicator for a misc command
//	 *      #   frame indicator for a signal generator command
//	 */
//	if ( sUart[0] == '@') {
//		/* pid related config/param */
//		strncpy( pCmd, &sUart[IDX_CMD], NC);		///< command substring
//		strncpy( pVal, &sUart[IDX_VAL], NV);		///< Value substring
//
//		/* Check for end of char coded float representation
//		 * numeric char:            0...9
//		 * decimal delimiter:       . or ,  !!!Take caution for forbidden thousands separator!!!
//		 * exponental indicator:    e or E
//		 * signdness:               -
//		 */
//		for (uint8_t k = 0; k <= NV; k++) {                 ///< Max NC numbers ...
//			if ( ((*pVal >= '0') && (*pVal <= '9')) || \
//			        (*pVal == '.') || (*pVal == ',')  || \
//			        (*pVal == 'e') || (*pVal == 'E') || \
//			        (*pVal == '-') ) {
//				pVal++;
//			}
//			else {
//				break;
//			}
//		}
//
//		*pVal = '\0';	              ///< place EOF marker
//		pVal = &sVal[0];
//
//		while ( *pCmd != '=' ) {    ///< check for end of command  indicator '='
//			pCmd++;
//		}
//		*(pCmd - 1) = '\0';		    ///< place EOS marker
//		pCmd = &sCmd[0];
//
//		/* parse sCmd buffer and enumerate the command/error */
//		for (i = 0, choice = NULL; i < sizeof items_list / sizeof (struct items); i++) {
//			if (strcasecmp(sCmd, items_list[i].name) == 0)
//			{
//				choice = items_list + i;
//				break;
//			}
//		}
//
//		cmd = choice ? choice->id : UNKNOWN;
//
//		char sValtemp1[] = {"-0.7e-09"};
//
//		/* FIXME */
//		volatile float fl1 __attribute__ ((unused)) = atof(&sValtemp1[0]);
//
//#ifdef  FLOAT_PARAMETERS
//		//            volatile float valueDecoded = (float)tmp/SLIDER_VAL_DEVIDER;
//		volatile float valueDecoded = atof( pVal );
//
//#elif   DOUBLE_PARAMETERS
//		double valueDecoded = (double)tmp / SLIDER_VAL_DEVIDER;
//#else
//		error "datatype?"
//#endif
//
//		switch (cmd) {
//		case KP:	{
//			pid->Kp = valueDecoded;
//#ifndef NO_FPU
//			arm_pid_init_f32(&PIDY, 0);
//#endif
//			printf("Kp % f set!\n", valueDecoded);
//		}
//		break;
//
//		case KI:	{
//			pid->Ki = valueDecoded;
//#ifndef NO_FPU
//			arm_pid_init_f32(&PIDY, 0);
//#endif
//			printf("Ki % f set!\n", valueDecoded);
//			//                            printf("Ki %s given\n", &sval[0]);
//		}
//		break;
//
//		case KD:	{
//			pid->Kd = valueDecoded;
//#ifndef NO_FPU
//			arm_pid_init_f32(&PIDY, 0);
//#endif
//			printf("Kd % f set!\n", valueDecoded);
//			//                         printf("Kd %s given\n", &sval);
//		}
//		break;
//
//		case W:		{
//			pidDataX.W_remf = valueDecoded;
//			pidDataY.W_remf = valueDecoded;
//			pidDataY.remoteCmdActive = 1;
//			printf("W = % g\n", pidDataY.W_remf);
//		}
//		break;
//
//		default:
//			printf("unknown choice\n");
//			break;
//		}
//
//
//	}
//	else {
//
//
//		/** A trailing ! char in a new Rx frame indicates that a misc command
//		 * follows...
//		 */
//		if (sUart[0] == '#') {   ///< True if signal generator command received
//
//			/**
//			 * Trailing # indicates a command frame for signal generation
//			 *
//			 * Frame Layout:
//			 *    c     waveform
//			 *    v     freq
//			 *    d  dutycycle
//			 *
//			 * # c c c : = v v v v  d  d
//			 * 0 1 2 3 4 5 6 7 8 9 10  11 12
//			 *
//			 *
//			 *
//			 * ------ Aus Qt software v8.3
//			 * Frame Layout:
//			 * cwaveform
//			 * vfreq
//			 * d dutycycle
//			 *
//			 * # c c c : = F F F F  d  d  A  A   A   A
//			 * 0 1 2 3 4 5 6 7 8 9 10  11 12 13  14  15
//			 *
//			 * Amplitude als char coded float
//			 *
//			 * # c c c : = F F F F  d  d  A ..16x.. A
//			 * 0 1 2 3 4 5 6 7 8 9 10  11 12 13  14  .... 27
//			 *
//			 */
//
//			//char  sWav[NC+1],    ///< string buffer to hold selected waveform as substring
//			//        sFreq[NV+1], ///< string buffer to hold given frequency as substring
//			//        sDuty[ND+1]; ///< string buffer to hold given duty cycle as substring
//
//			uint8_t NC = 3;      // index 0...3
//			//      uint8_t FS = 2;      // index 4...5 (fieldsep)
//			uint8_t NV = 4;      // index 6...9
//			uint8_t ND = 2;      // index 10...11
//			uint8_t NA = 16;     // index 12...15
//
//			char  sWav[NC + 1],  ///< string buffer to hold selected waveform as substring
//			      sFreq[NV + 1], ///< string buffer to hold given frequency as substring
//			      sDuty[ND + 1], ///< string buffer to hold given duty cycle as substring
//			      sAmp[16];     ///< string buffer to hold given amplitude as substring
//
//			//            char *pWav = sWav;
//			char *pDuty = sDuty;
//			char *pFreq = sFreq;
//			char *pAmp = sAmp;
//
//			strncpy(sWav,  &sUart[1], NC);   ///< waveform substring
//			strncpy(sFreq, &sUart[6],  4);      ///< frequency substring
//			strncpy(sDuty, &sUart[10], 2);      ///< duty cycle substring
//			strncpy(sAmp, &sUart[12], 16);      ///< peak amplitude substring
//
//
//			/* Check for end of char coded float representation
//			 * numeric char:            0...9
//			 * decimal delimiter:       . or ,  !!!Take caution for forbidden thousands separator!!!
//			 * exponental indicator:    e or E
//			 * signdness:               -
//			 */
//			for (uint8_t k = 0; k < NC; k++) { ///< check for end of value (padding) char ~
//				if ( *pCmd == RX_FRAME_PADDING_CHAR )
//					break;
//				pCmd++;
//			}
//			*(pCmd - 1) = '\0';   ///< place EOS marker
//			pCmd = &sCmd[0];
//
//			for (uint8_t k = 0; k < NV; k++) { ///< check for end of command  indicator '='
//				if ( *pFreq == RX_FRAME_PADDING_CHAR )
//					break;
//				pFreq++;
//			}
//			*pFreq = '\0';     ///< place EOS marker
//			pFreq = &sFreq[0];
//
//			for (uint8_t k = 0; k < ND; k++) { ///< check for end of command  indicator '='
//				if ( *pDuty == RX_FRAME_PADDING_CHAR )
//					break;
//				pDuty++;
//			}
//			*pDuty = '\0';     ///< place EOS marker
//			pDuty = &sDuty[0];
//
//			for (uint8_t k = 0; k <= NA; k++) {                 ///< Max NC numbers ...
//				if ( ((*pAmp >= '0') && (*pAmp <= '9')) || \
//				        (*pAmp == '.') || (*pAmp == ',')  || \
//				        (*pAmp == 'e') || (*pAmp == 'E') || \
//				        (*pAmp == '-') ) {
//					pAmp++;
//				}
//				else {
//					break;
//				}
//			}
//			*pAmp = '\0';   ///< place EOF marker
//			pAmp = &sWav[0];
//
//
//			/* parse sCmd buffer and enumerate the command/error */
//			for (i = 0, choicew = NULL; i < sizeof itemsw_list / sizeof (itemsw_t); i++) {
//				if (strcasecmp(sWav, itemsw_list[i].name) == 0)
//				{
//					choicew = itemsw_list + i;
//					break;
//				}
//			}
//
//			wav = choicew ? choicew->idw : NN;
//			//    /**
//			//     * Check for invalied chars in command string
//			//     *	Only [a-zA-Z] is threated as part of command string
//			//     *
//			//     */
//			//            while ( (*pWav >= 0x41) && (*pWav <= 0x5A) ||
//			//                            (*pWav >= 0x61) && (*pWav <= 0x7A) ) {
//			//                pWav++;
//			//            }
//
//			//    /**
//			//     * Same item structure used for PID-param and
//			//     * waveform generation commands! Don't forget
//			//     * to offset the for loop while while read in
//			//     * a waveform command.
//			//     *
//			//     */
//			//            for(i = 0, choicew = NULL; i < sizeof itemsw_list/sizeof (struct itemsw_t); i++)
//			//            {
//			//                if (strcasecmp(sWav, itemsw_list[i].name) == 0)
//			//                {
//			//                    choicew = itemsw_list + i;
//			//                    break;
//			//                }
//			//            }
//
//			//            wav = choicew ? choicew->idw : NN;
//			/* FIXME */
//			volatile float valueDecoded __attribute__ ((unused)) = atof( pVal );
//
//			g.waveForm 	= wav;
//			g.freq		= atoi(sFreq);
//			g.duty 		= atoi(sDuty);
//			g.ampl_f 	= (float)atof(sAmp);
//			g.ampl      = (int) g.ampl_f * 1000;
//
//			//g.ampl_f = (float)g.ampl / (float)AMPL_FLOAT_DIVISOR;
//
//			if (g.ampl_f > (float)AREF_IN_VOLT / 2)
//				g.ampl_f = (float)AREF_IN_VOLT / 2;
//			if (g.ampl_f < -(float)AREF_IN_VOLT / 2)
//				g.ampl_f = -(float)AREF_IN_VOLT / 2;
//			//            g.ampl_f = (float)g.ampl / (float)AMPL_FLOAT_DIVISOR;
//			//
//			//            if (g.ampl_f > (float)AREF_IN_VOLT/2)
//			//                g.ampl_f = (float)AREF_IN_VOLT/2;
//			//            if (g.ampl_f < -(float)AREF_IN_VOLT/2)
//			//                g.ampl_f = -(float)AREF_IN_VOLT/2;
//
//			/**
//			 *	Calculate the interval for lookup table index increment
//			 *	based on @SAMPLE_INTERVAL / TS, sizeof table and given
//			 *  waveform frequency.
//			 *
//			 *	g.freq  	[Hz]
//			 *	TS			[us]	* 10^(-6)
//			 *
//			 *  g.lookAt = g.freq/(TABLESIZE * TS * 10^(-6))
//			 *           = g.freq * 10^4 / TS
//			 */
//
//
//			//        g.lookAt = (short) 1/((double)TABLESIZE * TS * 10^(-6));
//			//		g.lookAt = (uint32_t) g.freq * pow(10,6) / (TABLESIZE * TS);
//			//        printf(" g.lookAt: %i", g.lookAt);
//
//			//        g.lookAt = g.lookAt / 100;
//			//        printf(" g.lookAt/100: %i", g.lookAt);
//			g.tctr = g.lookAt;
//
//			/**
//			 *  Precalculate a lookup table based on normed array
//			 *
//			 */
//			for (uint16_t k = 0; k < TABLESIZE; k++) {
//				calcTab[k] = (uint16_t) g.ampl * CosineN[k];
//			};
//
//			switch (wav) {
//			case COS:			{
//				printf("Cosine \n");
//				pSeq = &Cosine[0];
//				g.pBase = pSeq;
//				g.gen = EN;
//			}
//			break;
//
//			case TRIANG:		{
//				printf("Triangle \n");
//				//										pSeq = &Triangle[0];
//				/* 450/g.freq --> 100 Timer Ovf / periode */
//				triStruct.bottom = ((float)VA_BIAS - g.ampl_f / 2);
//				triStruct.top = ((float)VA_BIAS + g.ampl_f / 2);
//				triStruct.derivate = g.ampl_f / (float)100;
//				//                                            g.pBase = pSeq;
//				g.waveForm = TRIANG;
//				triStruct.triCtr = 0;
//				TM_Timer4_config(ENABLE, ENABLE, 450 / g.freq);
//				g.gen = EN;
//			}
//			break;
//
//			case SQUAREWAV:	{
//				printf("Squarewave\n");
//				TM_Timer4_config(ENABLE, ENABLE, 45000 / g.freq);
//				g.setpointSrc = REMOTE_INTERNAL_MIXED;
//				g.waveForm = SQUAREWAV;
//				g.gen = EN;
//				pSeq_f = &squarewaveBuff[0];
//			}
//			break;
//
//			case QUADRATIC:	{
//				printf("Quadratic\n");
//				pSeq = &Quadratic[0];
//				g.gen = EN;
//			}
//			break;
//
//			case I_QUADRATIC:	{
//				printf("Inv Quadratic\n");
//				pSeq = &I_Quadratic[0];
//				g.gen = EN;
//			}
//			break;
//
//			case SAWTOOTH:		{
//				printf("Sawtooth\n");
//				pSeq = &Sawtooth[0];
//				g.gen = EN;
//			}
//			break;
//
//			case CMD_REMOTE_SETPOINT: {
//				printf("CMD_REMOTE_SETPOINT\n");
//				g.setpointSrc = REMOTE_SETPOINT;
//			}
//			break;
//
//			case CMD_OPENLOOP_REMOTE: {
//				printf("CMD_OPENLOOP_REMOTE\n");
//				g.setpointSrc = REMOTE_OPENLOOP;
//			}
//			break;
//
//			case CMD_ANALOG_SETPOINT: {
//				printf("CMD_ANALOG_SETPOINT\n");
//				g.setpointSrc = ANALOG_SETPOINT;
//			}
//			break;
//
//			case CMD_INTERNAL_SETPOINT: {
//				printf("CMD_INTERNAL_SETPOINT\n");
//				g.setpointSrc = INTERNAL_SETPOINT;
//			}
//			break;
//
//			case CMD_TESTVECT: {
//				printf("CMD_TESTVECT\n");
//				//                                            g.setpointSrc = INTERNAL_SETPOINT;
//				dir = 1;
//				g.waveForm = CMD_TESTVECT;
//				TM_Timer4_config(ENABLE, ENABLE, 45000 / g.freq);
//			}
//			break;
//
//			case CMD_MATLAB_LINK: {
//				/* Matlab link integrieren
//				 * #mat:=1~~~0~1.6635~~~..  vector #1, beam off, xpos=1.6635
//				 * #mat:=2~~~0~1.2635~~~..
//				 * #mat:=3~~~1~-1.663~~~..  vector #1, beam on, xpos=-1.663
//				 */
//			}
//			break;
//
//			default: {
//				printf("unknown choice\n");
//				g.waveForm = NN;
//				g.duty 	= 999;
//				g.freq	= 999;
//				g.lookAt = 999;
//				g.pBase 	= pSeq;
//				g.tctr	= 999;
//				g.gen		= DIS;
//			}
//
//				/* Disable waveform generation if wgm-off command received */
//			}
//
//			//		g.pBase = (uint32_t)&pSeq;
//
//
//
//		}
//		else {                   ///< True if a misc command received
//			/**
//			 * Trailing ! indicates that a misc command frame was received
//			 *
//			 * Frame Layout:
//			 * 	c	command
//			 *
//			 * ! c c c c c c c c c c  c  c
//			 * 0 1 2 3 4 5 6 7 8 9 10 11 12
//			 *
//			 */
//#define MISC_CMD_LENGTH     35  // + trailing !
//			misc_items_t misc;
//			struct itemsm *choicem = NULL;
//
//			///< string buffer to hold given command substring
//			char sMisc[MISC_CMD_LENGTH + 1];
//			char *pMisc = sMisc;
//
//			strncpy(sMisc, 	&sUart[1],  MISC_CMD_LENGTH);	///< command substring
//
//			/**< Check for valied characters and substitute first unvalied char
//			 * by EOS char
//			 */
//
//			while ( ((*pMisc >= 'A') && (*pMisc <= 'Z')) || \
//			        ((*pMisc >= 'a') && (*pMisc <= 'z')) || \
//			        ((*pMisc >= '0') && (*pMisc <= '9')) || \
//			        ((*pMisc == '-') || (*pMisc == '_')) || \
//			        ((*pMisc == '.') || (*pMisc == ',')) || \
//			        ((*pMisc == 'e') || (*pMisc == 'E')) ) {
//
//				pMisc++;
//			}
//
//			*pMisc = '\0';				///< End of string
//
//
//			for (i = 0, choicem = NULL; i < sizeof itemsm_list / sizeof (struct itemsm); i++)
//			{
//				if (strcasecmp(sMisc, itemsm_list[i].name) == 0)
//				{
//					choicem = itemsm_list + i;
//					break;
//				}
//			}
//
//			misc = choicem ? choicem->idm : misc_NN;
//
//			g.miscReq 	= misc;
//
//			switch (misc) {
//			case misc_assOnOff_cmd:
//				break;
//			case misc_assUpperLim_cmd:
//				break;
//			case misc_assLowerLim_cmd:
//				break;
//			case misc_assTrippTime_cmd:
//				break;
//			case misc_assSaveVal_cmd:
//				break;
//
//			case misc_pid_init:
//				//                    pid_Init(KP_INIT, KI_INIT, KD_INIT, TF_INIT, TS, &pidDataY, NONBUFFERED);
//				resetPID();
//				break;
//			case misc_beamOff_cmd:
//				MDB_GPIO_BeamPin(BEAM_INTERRUPT, GPIO_DRIVER_OFF);
//				break;
//			case misc_beamOn_cmd:
//				MDB_GPIO_BeamPin(BEAM_INTERRUPT, GPIO_DRIVER_ON);
//				break;
//
//			default:
//				printf("function unknown\n");
//			}
//		}
//		memset(&sUart, 0, UART_BUFF_SIZE * sizeof(char));
//	}
//
//}
//#if  defined (KEIL_IDE)
//#pragma O2
//#elif  defined (__GNUC__)
//#pragma GCC pop_options
//#endif
//
//
//
