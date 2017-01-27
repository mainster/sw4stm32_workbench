#include "md_stm32f4_dac_waveform.h"

/**
 * @brief      DAC Channel SineWave Configuration
 *
 * @param      DACx          The DAC channel to use
 * @param[in]  NewStateDAC   The new state dac
 * @param[in]  NewStateDMA   The new state dma
 */
void DAC_CHx_DMA_SineWaveConfig(MD_DAC_Channel_t DACx,
								FunctionalState NewStateDAC,
								FunctionalState NewStateDMA) {
	uint32_t DAC_DHR12Rx_ADDRESS = 0;

	/* Initialization Structures */
	DMA_InitTypeDef DMA_Config;
	DAC_InitTypeDef DAC_Config;

	/* DAC Configuration */
	DAC_Config.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_Config.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_Config.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

	/* DAC Initialization */
	DAC_Init(DAC_DHR12Rx_ADDRESS, &DAC_Config);

	if (DACx == MD_DAC1) {
		/* FIXME */
		DAC_DHR12Rx_ADDRESS = DAC_DHR12R1_ADDRESS;
	}

	if (DACx == MD_DAC2) {
		DAC_DHR12Rx_ADDRESS = DAC_DHR12R2_ADDRESS;
	}

	/* Configure Direct Memory Access  */
	DMA_DeInit(DMA1_Stream5);
	DMA_Config.DMA_Channel = DMA_Channel_7;
	DMA_Config.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR12Rx_ADDRESS;
	/* FIXME */
	//  DMA_Config.DMA_Memory0BaseAddr = (uint32_t)&Sine12bit;
	DMA_Config.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_Config.DMA_BufferSize = 32;
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_Config.DMA_Mode = DMA_Mode_Circular;
	DMA_Config.DMA_Priority = DMA_Priority_High;
	DMA_Config.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Config.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_Config.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_Config.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5, &DMA_Config);

	/* Enable DMA1_Stream5 */
	DMA_Cmd(DMA1_Stream5, NewStateDMA);

	/* Enable DAC Channel x */
	DAC_Cmd(DACx, NewStateDAC);

	/* Enable DMA for DAC Channel2 */
	//  DAC_DMACmd(DAC_Channel_2, ENABLE);
}

/**
 * @brief  DAC Channel1 Escalator Configuration
 * @param  None
 * @retval None
 */
void DAC_CHx_EscalatorConfig(MD_DAC_Channel_t DACx) {
	uint32_t DAC_DHR8Rx_ADDRESS = 0;

	/* Initialization Structures */
	DMA_InitTypeDef DMA_Config;
	DAC_InitTypeDef DAC_Config;

	/* DAC Configuration */
	DAC_Config.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_Config.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_Config.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

	/* DAC Initialization */
	DAC_Init(DAC_DHR8Rx_ADDRESS, &DAC_Config);

	if (DACx == MD_DAC1) {
		DAC_DHR8Rx_ADDRESS = DAC_DHR8R1_ADDRESS;
	}

	if (DACx == MD_DAC2) {
		DAC_DHR8Rx_ADDRESS = DAC_DHR8R2_ADDRESS;
	}

	/* Configure Direct Memory Access  */
	DMA_DeInit(DMA1_Stream6);
	DMA_Config.DMA_Channel = DMA_Channel_7;
	/* DMA1_Stream6 channel7 configuration */
	DMA_Config.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR8Rx_ADDRESS;
	/* FIXME 	DMA_Config.DMA_Memory0BaseAddr = (uint32_t)&Escalator8bit; */
	DMA_Config.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_Config.DMA_BufferSize = 6;
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_Config.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_Config.DMA_Mode = DMA_Mode_Circular;
	DMA_Config.DMA_Priority = DMA_Priority_High;
	DMA_Config.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Config.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_Config.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_Config.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream6, &DMA_Config);

	//  /* Enable DMA1_Stream6 */
	//  DMA_Cmd(DMA1_Stream6, ENABLE);
	//
	//  /* Enable DAC Channel1 */
	//  DAC_Cmd(DAC_Channel_1, ENABLE);

	//  /* Enable DMA for DAC Channel1 */
	//  DAC_DMACmd(DAC_Channel_1, ENABLE);
}

/**
 * @brief  DAC Channel2 Triangle Configuration
 * @param  None
 * @retval None
 */
void DAC_CHx_TriangleConfig(MD_DAC_Channel_t DACx) {
	uint32_t DAC_DHR12Rx_ADDRESS = 0;

	/* Initialization Structure */
	DAC_InitTypeDef DAC_Config;

	/* DAC Configuration */
	DAC_Config.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_Config.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_Config.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

	/* DAC Initialization */
	DAC_Init(DAC_DHR12Rx_ADDRESS, &DAC_Config);

	if (DACx == MD_DAC1) {
		DAC_DHR12Rx_ADDRESS = DAC_DHR12R1_ADDRESS;
	}

	if (DACx == MD_DAC2) {
		DAC_DHR12Rx_ADDRESS = DAC_DHR12R2_ADDRESS;
	}

	/* DAC channelx Configuration */
	DAC_Config.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_Config.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
	DAC_Config.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1023;
	DAC_Config.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_2, &DAC_Config);

	/* Enable DAC Channel2 */
	DAC_Cmd(DAC_Channel_2, ENABLE);

	/* Set DAC channel2 DHR12RD register */
	DAC_SetChannel2Data(DAC_Align_12b_R, 0x100);
}

/**
 * @brief  DAC  Channel1 Noise Configuration
 * @param  None
 * @retval None
 */
//void DAC_CHx_NoiseConfig(MD_DAC_Channel_t DACx)
//{
//	/* DAC channel1 Configuration */
//	DACx->DAC_Trigger = DAC_Trigger_T6_TRGO;
//	DACx->DAC_WaveGeneration = DAC_WaveGeneration_Noise;
//	DACx->DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits10_0;
//	DACx->DAC_OutputBuffer = DAC_OutputBuffer_Enable;
//	DAC_Init(DAC_Channel_1, &DACx);
//
//	/* Enable DAC Channel1 */
//	DAC_Cmd(DAC_Channel_1, ENABLE);
//
//	/* Set DAC Channel1 DHR12L register */
//	DAC_SetChannel1Data(DAC_Align_12b_L, 0x7FF0);
//}

/**
 * @}
 */
