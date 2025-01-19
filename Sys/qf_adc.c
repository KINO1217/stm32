#include "qf_adc.h"

/**
 * ADC信道 0-9 对应 PA0-PA7 PB0-PB1
 */

void QF_ADC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 最大14MHz

    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET)
        ;
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1))
        ;
}

u8 QF_ADC_GetV(u8 ch)
{
    uint32_t adc_value = 0;

    // 读取8次均值滤波
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);

    for (u8 i = 0; i < MEAN_CNT; i++) {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
            ;
        adc_value += ADC_GetConversionValue(ADC1);
    }

    adc_value /= MEAN_CNT;
    if (adc_value > 3960)
        adc_value = 3960;

    return adc_value / 40;
}
