#include "my_adc.h"

void vAdcInit(u8 channel);
void vAdcGet(u8 channel, float *v);
static u8 baseInit = 0;

const c_my_adc my_adc = {vAdcInit,vAdcGet};

void vAdcInit(u8 channel)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /**初始化GPIO端口*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if(channel == MY_ADC_0)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if(channel == MY_ADC_1)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if(channel == MY_ADC_2)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if(channel == MY_ADC_3)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if(channel == MY_ADC_4)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if(channel == MY_ADC_5)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if(channel == MY_ADC_6)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if(channel == MY_ADC_7)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if(channel == MY_ADC_8)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    else if(channel == MY_ADC_9)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    
    if(baseInit == 0)
    {
        /**设置ADC分频因子*/
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);

        /**初始化ADC参数*/
        ADC_InitTypeDef ADC_InitStructure;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
        ADC_InitStructure.ADC_NbrOfChannel = 1;
        ADC_Init(ADC1, &ADC_InitStructure);

        /**使能ADC*/
        ADC_Cmd(ADC1, ENABLE);

        /**ADC校准*/
        ADC_ResetCalibration(ADC1);
        while(ADC_GetCalibrationStatus(ADC1));
        ADC_StartCalibration(ADC1);
        while(ADC_GetCalibrationStatus(ADC1));

        baseInit = 1;
    }
}

void vAdcGet(u8 channel, float *v)
{
    u16 temp = 0;

    /**设置转换通道*/
    if(channel == MY_ADC_0)
        ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
    else if(channel == MY_ADC_1)
        ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_55Cycles5);
    else if(channel == MY_ADC_2)
        ADC_RegularChannelConfig(ADC1,ADC_Channel_2,1,ADC_SampleTime_55Cycles5);
    else if(channel == MY_ADC_3)
        ADC_RegularChannelConfig(ADC1,ADC_Channel_3,1,ADC_SampleTime_55Cycles5);
    else if(channel == MY_ADC_4)
        ADC_RegularChannelConfig(ADC1,ADC_Channel_4,1,ADC_SampleTime_55Cycles5);
    else if(channel == MY_ADC_5)
        ADC_RegularChannelConfig(ADC1,ADC_Channel_5,1,ADC_SampleTime_55Cycles5);
    else if(channel == MY_ADC_6)
        ADC_RegularChannelConfig(ADC1,ADC_Channel_6,1,ADC_SampleTime_55Cycles5);
    else if(channel == MY_ADC_7)
        ADC_RegularChannelConfig(ADC1,ADC_Channel_7,1,ADC_SampleTime_55Cycles5);
    else if(channel == MY_ADC_8)
        ADC_RegularChannelConfig(ADC1,ADC_Channel_8,1,ADC_SampleTime_55Cycles5);
    else if(channel == MY_ADC_9)
        ADC_RegularChannelConfig(ADC1,ADC_Channel_9,1,ADC_SampleTime_55Cycles5);

    /**软件触发转换并获取值*/
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    temp = ADC_GetConversionValue(ADC1);

    /**计算电压值*/
    *v = (float)3.3 * temp / 4095;
}
