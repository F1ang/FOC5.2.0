
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "motorcontrol.h"
#include "ui_task.h" 
#include "key/bsp_key.h"

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;

/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM8_Init(void);
static void MX_NVIC_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                

/* Private function prototypes -----------------------------------------------*/
extern void test_RampCtrl(void);// �����ú���,��FOC��

/**
  * ��������: main���������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
int main(void)
{
  /* MCU Configuration----------------------------------------------------------*/

  /* ��λ��������,��ʼ��Flash �ӿں�ϵͳ�δ�ʱ��. */
  HAL_Init();

  /* ����ϵͳʱ�� */
  SystemClock_Config();

  /* ��ʼ���������� */
  KEY_GPIO_Init(); // ���а�����ʼ��Ϊ��ѯģʽ
  MX_GPIO_Init();  // KEY1Ϊ�ж�ģʽ,��������/ֹͣ���
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_MotorControl_Init();

  /* ��ʼ���ж� */
  MX_NVIC_Init();

  
  /* Infinite loop */
  HAL_Delay(500);
  MC_StartMotor1();
  HAL_Delay(2000);
  UI_SPDRampCtrl(2000,1000);// 1000RPM, 1000ms
  HAL_Delay(2000);
//  MC_StopMotor1();
  while (1)
  {
    if(KEY5_StateRead() == KEY_DOWN)
    {
      UI_SPDRampCtrl(-1000,1000);     // �ٶȿ���
      MC_StartMotor1();
    }
    if(KEY4_StateRead() == KEY_DOWN)
    {
      MC_StopMotor1();
    }
  }
}

/**
  * ��������: ϵͳʱ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* ����PLL �õ�CPU��ϵͳʱ��,168MHz */
  /* 8M����  ( (8/4) * 168 ) / 2 = 168MHz  PLLQʵ����û��ʹ�õ�*/
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /**����AHB,APB����ʱ��Ƶ�ʺͶ�ʱ��ʱ��Ƶ��
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
  /* Enables the Clock Security System */
  HAL_RCC_EnableCSS();

  /**Configure the Systick interrupt time */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  /*  ѡ��HCLKΪ�δ�ʱ����ʱ�� 168MHz */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* �ж����ȼ� */
  HAL_NVIC_SetPriority(SysTick_IRQn, 4, 0);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* ADC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(ADC_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(ADC_IRQn);
  /* TIM8_UP_TIM13_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
  /* TIM5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM5_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM5_IRQn);

  /* EXTI0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(Start_Stop_EXTI_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(Start_Stop_EXTI_IRQn);
}

/**
  * ��������: ADC�����ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ʹ��CH6,CH8,CH9Ϊע��ͨ��(A6,B0,B1�������������),ʹ�ò���������ģʽ
              CH10,CH13Ϊ����ͨ��(C0,C3��ѹ,�¶Ȳ�������),ʹ��ɨ��ģʽ
  */
static void MX_ADC1_Init(void)
{

  ADC_InjectionConfTypeDef sConfigInjected;

  /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
     ����ADC��ȫ������,����ʱ��,�ֱ���,���ݶ��뷽��,ת��ͨ������
  */
  hadc1.Instance                   = ADC1;
  hadc1.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;     //ʱ��=84/4 = 21MHz 
  hadc1.Init.Resolution            = ADC_RESOLUTION_12B;           // 12λ�ֱ���(ת��ʱ��15��ʱ������)
  hadc1.Init.ScanConvMode          = ENABLE;                       // ɨ��ģʽ   ��
  hadc1.Init.ContinuousConvMode    = DISABLE;                      // ��������   ��
  hadc1.Init.DiscontinuousConvMode = DISABLE;                      // ���������� ��
  hadc1.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;// ���ⲿ����
  hadc1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;           // �������
  hadc1.Init.DataAlign             = ADC_DATAALIGN_LEFT;           // �����
  hadc1.Init.NbrOfConversion       = 2;                            // ת��ͨ�� 2
  hadc1.Init.DMAContinuousRequests = DISABLE;                      // DMA�������� ��
  hadc1.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;          // ����ת����ɱ��
  HAL_ADC_Init(&hadc1);


  /* ����ע��ͨ���Ĳ���˳��Ͳ���ʱ�� */
  sConfigInjected.InjectedChannel           = ADC_CHANNEL_6;// CH6
  sConfigInjected.InjectedRank              = 1; // ����˳��     
  sConfigInjected.InjectedNbrOfConversion   = 3; // �ܵ�ת��ͨ������ 
  sConfigInjected.InjectedSamplingTime      = ADC_SAMPLETIME_3CYCLES;// ����ʱ��,3������,����ת��ʱ����15������
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONVEDGE_RISING; // �ⲿ�ź������ش���(ָADC���ⲿ)
  sConfigInjected.ExternalTrigInjecConv     = ADC_EXTERNALTRIGINJECCONV_T8_CC4;     // ����Դ:TIM8 CH4�ıȽ��¼�
  sConfigInjected.AutoInjectedConv          = DISABLE;        // �Զ�ע�� ��
  sConfigInjected.InjectedDiscontinuousConvMode = ENABLE;     // ���������� ��
  sConfigInjected.InjectedOffset            = 0;              // ����ƫ��ֵ
  HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);


  /* Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_8;// CH8 
  sConfigInjected.InjectedRank    = 2;            // ����˳��//����������CH6һ��
  HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);

  /**Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_9;// CH9 
  sConfigInjected.InjectedRank = 3;               // ����˳��//����������CH6һ��
  HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);

}

/**
  * ��������: ADC�����ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ʹ��CH6,CH8,CH9Ϊע��ͨ��(A6,B0,B1�������������),ʹ�ò���������ģʽ         
  */
static void MX_ADC2_Init(void)
{

  ADC_InjectionConfTypeDef sConfigInjected;

  /* ����ADC��ȫ������,����ʱ��,�ֱ���,���ݶ��뷽��,ת��ͨ������ */
  hadc2.Instance                   = ADC2;
  hadc2.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;       //ʱ��=84/4 = 21MHz 
  hadc2.Init.Resolution            = ADC_RESOLUTION_12B;             // 12λ�ֱ���(ת��ʱ��15��ʱ������)
  hadc2.Init.ScanConvMode          = ENABLE;                         // ɨ��ģʽ   ��
  hadc2.Init.ContinuousConvMode    = DISABLE;                        // ��������   ��
  hadc2.Init.DiscontinuousConvMode = DISABLE;                        // ���������� ��
  hadc2.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;  // ���ⲿ����
  hadc2.Init.ExternalTrigConv      = ADC_SOFTWARE_START;             // �������
  hadc2.Init.DataAlign             = ADC_DATAALIGN_LEFT;             // �����
  hadc2.Init.NbrOfConversion       = 1;                              // ת��ͨ�� 1
  hadc2.Init.DMAContinuousRequests = DISABLE;                        // DMA�������� ��
  hadc2.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;            // ����ת����ɱ��
  HAL_ADC_Init(&hadc2);

  /* ����ע��ͨ���Ĳ���˳��Ͳ���ʱ�� */
  sConfigInjected.InjectedChannel           = ADC_CHANNEL_6;
  sConfigInjected.InjectedRank              = 1;
  sConfigInjected.InjectedNbrOfConversion   = 3;
  sConfigInjected.InjectedSamplingTime      = ADC_SAMPLETIME_3CYCLES;              // ����ʱ��,3������,����ת��ʱ����15������
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONVEDGE_RISING;// �ⲿ�ź������ش���(ָADC���ⲿ)
  sConfigInjected.ExternalTrigInjecConv     = ADC_EXTERNALTRIGINJECCONV_T8_CC4;    // ����Դ:TIM8 CH4�ıȽ��¼�
  sConfigInjected.AutoInjectedConv          = DISABLE;        // �Զ�ע�� ��
  sConfigInjected.InjectedDiscontinuousConvMode = ENABLE;     // ���������� ��
  sConfigInjected.InjectedOffset            = 0;
  HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);

  /* ����ע��ͨ��CH8�Ĳ���˳�� */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_8;
  sConfigInjected.InjectedRank = 2;
  HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);


  /* ����ע��ͨ��CH9�Ĳ���˳�� */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_9;
  sConfigInjected.InjectedRank = 3;
  HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);


}

/**
  * ��������: TIIM5����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��TIM5����Ϊ����������ģʽ
  */static void MX_TIM5_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_HallSensor_InitTypeDef sConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim5.Instance           = TIM5;                   //
  htim5.Init.Prescaler     = 0;                      //
  htim5.Init.CounterMode   = TIM_COUNTERMODE_UP;     // ���ϼ���
  htim5.Init.Period        = M1_HALL_TIM_PERIOD;     //
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim5);


  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig);


  sConfig.IC1Polarity       = TIM_ICPOLARITY_RISING;
  sConfig.IC1Prescaler      = TIM_ICPSC_DIV1;
  sConfig.IC1Filter         = M1_HALL_IC_FILTER;
  sConfig.Commutation_Delay = 0;                ///���ӳ�
  HAL_TIMEx_HallSensor_Init(&htim5, &sConfig);
  

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC2REF;//TIM5 _OC2 �������
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig);


}

/**
  * ��������: TIM8��ʼ������
  * �������: �� 
  * �� �� ֵ: �� 
  * ˵    ��: ��
  */static void MX_TIM8_Init(void)
{

  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

  htim8.Instance               = TIM8;                           // 
  htim8.Init.Prescaler         = ((TIM_CLOCK_DIVIDER) - 1);      // ���Ķ���ģʽ1,�ݼ�������ʱ��Żᴥ���Ƚ��ж��¼�
  htim8.Init.CounterMode       = TIM_COUNTERMODE_CENTERALIGNED1; //168,000,000 / 16,000 = 10500 ,16kHz
  htim8.Init.Period            = ((PWM_PERIOD_CYCLES) / 2);      // 84MHz������������
  htim8.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV2;         // 1 ���������ж�һ��
  htim8.Init.RepetitionCounter = (REP_COUNTER);                  // �ظ�������1
  HAL_TIM_Base_Init(&htim8);
  

  HAL_TIM_PWM_Init(&htim8);

  sSlaveConfig.SlaveMode    = TIM_SLAVEMODE_TRIGGER;  // ��ģʽ
  sSlaveConfig.InputTrigger = TIM_TS_ITR1;            // ����Դ ITR1 ���ӵ�TIM2��TRGO 
  HAL_TIM_SlaveConfigSynchronization(&htim8, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;              // Ĭ��ֵ
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE; // ����ģʽ ��
  HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig);

  /* PWM���ͨ������ */
  sConfigOC.OCMode       = TIM_OCMODE_PWM1;           // PWM1ģʽ,�ȸߵ�ƽ�ٵ͵�ƽ
  sConfigOC.Pulse        = 0;                         
  sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;       // �������
  sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;      // 
  sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;        // ����ģʽ ��
  sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;     // ����״̬ RESET
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  ;HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1);


  HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2);// CH2

  HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3);// CH3

  sConfigOC.OCMode = TIM_OCMODE_PWM2;// PWM2ģʽ,�ȵ͵�ƽ�ٸߵ�ƽ
  sConfigOC.Pulse = (((PWM_PERIOD_CYCLES) / 2) - (HTMIN));// �Ƚ�ֵ,����ADC������ʱ�̵�
  HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4);
  
  /* ɲ������������ */
  sBreakDeadTimeConfig.OffStateRunMode  = TIM_OSSR_ENABLE;   // OSSR ��
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;   // OSSI ��
  sBreakDeadTimeConfig.LockLevel        = TIM_LOCKLEVEL_1;   // LOCK Level 1  ��BDTR.DTG/BKE/BKP/AOE , CR2.OISx/OISxN ִ��д����
  sBreakDeadTimeConfig.DeadTime         = ((DEAD_TIME_COUNTS) / 2);// ����ʱ��
  sBreakDeadTimeConfig.BreakState       = TIM_BREAK_DISABLE; // ɲ�� ��
  sBreakDeadTimeConfig.BreakPolarity    = TIM_BREAKPOLARITY_LOW;
  sBreakDeadTimeConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;//AOE �� 
  HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig);

  HAL_TIM_MspPostInit(&htim8);

}

/**
  * ��������: ���ų�ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��������/ֹͣ��ť,��ʹ��GPIOʱ��
  */
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();  

  /*Configure GPIO pin : Start_Stop_Pin */
  GPIO_InitStruct.Pin = Start_Stop_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Start_Stop_GPIO_Port, &GPIO_InitStruct);

}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
