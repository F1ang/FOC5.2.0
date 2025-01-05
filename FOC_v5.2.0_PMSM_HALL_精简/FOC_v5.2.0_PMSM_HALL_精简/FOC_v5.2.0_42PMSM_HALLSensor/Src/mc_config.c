/**
  ******************************************************************************
  * @file    mc_config.c 
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   Motor Control Subsystem components configuration and handler structures.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
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
#include "main.h"
#include "parameters_conversion.h"
#include "mc_parameters.h"
#include "mc_config.h"


#define OFFCALIBRWAIT_MS     0 // ���1��������У׼֮ǰ�ĵȴ�ʱ��,ʹ�ú����޸�
#define OFFCALIBRWAIT_MS2    0 // ���2��������У׼֮ǰ�ĵȴ�ʱ��, 

/**
  * @brief  PI / PID Speed loop parameters Motor 1
  */
  
/* ���1��PI�ٶȻ����� */
PID_Handle_t PIDSpeedHandle_M1 =
{
  .hDefKpGain          = (int16_t)PID_SPEED_KP_DEFAULT,       /* Ĭ��Kp ,���ڳ�ʼ������ */
  .hDefKiGain          = (int16_t)PID_SPEED_KI_DEFAULT,       /* Ĭ��Ki ,���ڳ�ʼ������ */
  
  .wUpperIntegralLimit = (int32_t)IQMAX * (int32_t)SP_KIDIV,  /*!< ���ֱ������� */
  .wLowerIntegralLimit = -(int32_t)IQMAX * (int32_t)SP_KIDIV, /*!< ���ֱ�������  */
  .hUpperOutputLimit   = (int16_t)IQMAX,                      /*!< PI�����������*/
  .hLowerOutputLimit   = -(int16_t)IQMAX,                     /*!< PI�����������*/
  .hKpDivisor          = (uint16_t)SP_KPDIV,                  /*!< Kp�ķ�ĸ,�ó������渡���� */
  .hKiDivisor          = (uint16_t)SP_KIDIV,                  /*!< Ki�ķ�ĸ,�ó������渡���� */
  .hKpDivisorPOW2      = (uint16_t)SP_KPDIV_LOG,              /*!< Kp�ķ�ĸ(2��ָ��)E.g. ��� SP_KPDIV = 512 => SP_KPDIV_LOG = 9 ,2^9 = 512 */
  .hKiDivisorPOW2      = (uint16_t)SP_KIDIV_LOG,              /*!< Ki�ķ�ĸ(2��ָ��)*/

};

/**
  * @brief  PI / PID Iq loop parameters Motor 1
  */
/* ���1��Iq PID������ */

PID_Handle_t PIDIqHandle_M1 =
{
  .hDefKpGain          = (int16_t)PID_TORQUE_KP_DEFAULT,   /* Ĭ��Kp ,���ڳ�ʼ������ */
  .hDefKiGain          = (int16_t)PID_TORQUE_KI_DEFAULT,   /* Ĭ��Ki ,���ڳ�ʼ������ */

  .wUpperIntegralLimit = (int32_t)INT16_MAX * TF_KIDIV,    /*!< ���ֱ������� */
  .wLowerIntegralLimit = (int32_t)-INT16_MAX * TF_KIDIV,   /*!< ���ֱ�������  */
  .hUpperOutputLimit       = INT16_MAX,                    /*!< PI�����������*/
  .hLowerOutputLimit       = -INT16_MAX,                   /*!< PI�����������*/
  .hKpDivisor          = (uint16_t)TF_KPDIV,               /*!< Kp�ķ�ĸ,�ó������渡���� */
  .hKiDivisor          = (uint16_t)TF_KIDIV,               /*!< Ki�ķ�ĸ,�ó������渡���� */
  .hKpDivisorPOW2      = (uint16_t)TF_KPDIV_LOG,           /*!< Kp�ķ�ĸ(2��ָ��)E.g. ��� SP_KPDIV = 512 => SP_KPDIV_LOG = 9 ,2^9 = 512 */
  .hKiDivisorPOW2      = (uint16_t)TF_KIDIV_LOG,           /*!< Ki�ķ�ĸ(2��ָ��)*/

};

/**
  * @brief  PI / PID Id loop parameters Motor 1
  */
/* ���1��Id PID������ */
PID_Handle_t PIDIdHandle_M1 =
{
  .hDefKpGain          = (int16_t)PID_FLUX_KP_DEFAULT,    /* Ĭ��Kp ,���ڳ�ʼ������ */
  .hDefKiGain          = (int16_t)PID_FLUX_KI_DEFAULT,    /* Ĭ��Ki ,���ڳ�ʼ������ */

  .wUpperIntegralLimit = (int32_t)INT16_MAX * TF_KIDIV,   /*!< ���ֱ������� */
  .wLowerIntegralLimit = (int32_t)-INT16_MAX * TF_KIDIV,  /*!< ���ֱ�������  */
  .hUpperOutputLimit       = INT16_MAX,                   /*!< PI�����������*/
  .hLowerOutputLimit       = -INT16_MAX,                  /*!< PI�����������*/
  .hKpDivisor          = (uint16_t)TF_KPDIV,              /*!< Kp�ķ�ĸ,�ó������渡���� */
  .hKiDivisor          = (uint16_t)TF_KIDIV,              /*!< Ki�ķ�ĸ,�ó������渡���� */
  .hKpDivisorPOW2      = (uint16_t)TF_KPDIV_LOG,          /*!< Kp�ķ�ĸ(2��ָ��)E.g. ��� SP_KPDIV = 512 => SP_KPDIV_LOG = 9 ,2^9 = 512 */
  .hKiDivisorPOW2      = (uint16_t)TF_KIDIV_LOG,          /*!< Ki�ķ�ĸ(2��ָ��)*/

};

/**
  * @brief  SpeednTorque Controller parameters Motor 1
  */
/* ���1���ٶ�Ť�ؿ����� ���� */
SpeednTorqCtrl_Handle_t SpeednTorqCtrlM1 =
{
  .STCFrequencyHz =           		MEDIUM_FREQUENCY_TASK_RATE, 	    /*!< �û�����STC_CalcTorqueReference����Ť��Ŀ��ֵ��Ƶ��,��λ��hz(500Hz) */
  .MaxAppPositiveMecSpeed01Hz =	(uint16_t)(MAX_APPLICATION_SPEED/6),/*!< ����Ӧ��ת��,��λ��0.1Hz(0.1RPS). 3000RPM/6  */
  .MinAppPositiveMecSpeed01Hz =	(uint16_t)(MIN_APPLICATION_SPEED/6),/*!< ��С��Ӧ��ת��,��λ��0.1Hz(0.1RPS).*/
  .MaxAppNegativeMecSpeed01Hz =	(int16_t)(-MIN_APPLICATION_SPEED/6),/*!< ��С��Ӧ��ת��,��λ��0.1Hz(0.1RPS).*/
  .MinAppNegativeMecSpeed01Hz =	(int16_t)(-MAX_APPLICATION_SPEED/6),/*!< ����Ӧ��ת��,��λ��0.1Hz(0.1RPS). -3000RPM/6  */
  .MaxPositiveTorque =				(int16_t)NOMINAL_CURRENT,		          /*!< ����Ť��,Iq����������ֵ */
  .MinNegativeTorque =				-(int16_t)NOMINAL_CURRENT,            /*!< ��С��Ť��,Iq����������ֵ */
  .ModeDefault =					DEFAULT_CONTROL_MODE,                     /*!< Ĭ�ϵĿ���ģʽ,�ٶ�ģʽ.*/
  .MecSpeedRef01HzDefault =		(int16_t)(DEFAULT_TARGET_SPEED_RPM/6),/*!< Ĭ�ϵ�Ŀ��ת��,��λ��0.1Hz.*/
  .TorqueRefDefault =				(int16_t)DEFAULT_TORQUE_COMPONENT,      /*!< Ĭ�ϵ�Ť��Ŀ��ֵ,Iq����������ֵ */
  .IdrefDefault =					(int16_t)DEFAULT_FLUX_COMPONENT,          /*!< Ĭ�ϵ�IdĿ��ֵ.*/
};

PWMC_R3_F4_Handle_t PWM_Handle_M1=
{
  { /* ����ָ�뺯��ʵ����,ָ��ʵ������(���ڹ淶�ӿڱ�׼) */
    .pFctGetPhaseCurrents              = &R3F4XX_GetPhaseCurrents,    
    .pFctSwitchOffPwm                  = &R3F4XX_SwitchOffPWM,             
    .pFctSwitchOnPwm                   = &R3F4XX_SwitchOnPWM,              
    .pFctCurrReadingCalib              = &R3F4XX_CurrentReadingCalibration,/* ������ȡУ׼ */
    .pFctTurnOnLowSides                = &R3F4XX_TurnOnLowSides,          
    .pFctSetADCSampPointSect1          = &R3F4XX_SetADCSampPointSect1,   /* ����1��ADC������ */  
    .pFctSetADCSampPointSect2          = &R3F4XX_SetADCSampPointSect2,   /* ����2��ADC������ */    
    .pFctSetADCSampPointSect3          = &R3F4XX_SetADCSampPointSect3,   /* ����3��ADC������ */    
    .pFctSetADCSampPointSect4          = &R3F4XX_SetADCSampPointSect4,   /* ����4��ADC������ */    
    .pFctSetADCSampPointSect5          = &R3F4XX_SetADCSampPointSect5,   /* ����5��ADC������ */    
    .pFctSetADCSampPointSect6          = &R3F4XX_SetADCSampPointSect6,   /* ����6��ADC������ */    
    /*  */
    .hT_Sqrt3 = (PWM_PERIOD_CYCLES*SQRT3FACTOR)/16384u,   /*!< (T*16384*sqrt(3)*2)/16384 ϵ��,��������SVPWM */
    .hSector = 0,     /*!< �ռ��������� ��� */
    .hCntPhA = 0,     /*!< A������PWMͨ���ıȽ�ֵ */
    .hCntPhB = 0,     /*!< B������PWMͨ���ıȽ�ֵ */
    .hCntPhC = 0,     /*!< C������PWMͨ���ıȽ�ֵ */
    .bTurnOnLowSidesAction = false, /*!< true if TurnOnLowSides action is active,false otherwise. */
    .hOffCalibrWaitTimeCounter = 0, /*!< �ڵ����������У׼֮ǰ�ȴ��̶���ʱ��ļ����� */
    .bMotor = 0,     /*!< ������ */

    .hPWMperiod          = PWM_PERIOD_CYCLES,     /*!<  PWM ����ֵ: hPWMPeriod = Timer Fclk / Fpwm 168M/18k= 9333  */
    .hOffCalibrWaitTicks = (uint16_t)((SYS_TICK_FREQUENCY * OFFCALIBRWAIT_MS)/ 1000),/*!< �ڵ����������У׼֮ǰ�ȴ��̶���ʱ�� */

  },
  .wPhaseAOffset = 0,   /*!< A�������AD����ƫ��ֵ */
  .wPhaseBOffset = 0,   /*!< B�������AD����ƫ��ֵ */
  .wPhaseCOffset = 0,   /*!< C�������AD����ƫ��ֵ */
  .wADC1Channel = 0,    /*!< ADC1�Ĳ���ͨ��,��������ʱʵʱ�޸Ĳ���ͨ��  */
  .wADC2Channel = 0,    /*!< ADC1�Ĳ���ͨ��,��������ʱʵʱ�޸Ĳ���ͨ�� */
  .Half_PWMPeriod = PWM_PERIOD_CYCLES/2u,  /* ��PWM����ֵ 9333/2 = 4666*/
  .bSoFOC = 0,  /*!< �����������0,��TIM_UP�ж�����λ,�����ִ��FOC�㷨��֮��,����1��,��˵��FOC�㷨ռ���ʹ��� */
  .bIndex = 0,  /*!< ����Ŀ¼ */
  .wADCTriggerSet = 0,   /*!< �洢ADC->CR2��ֵ,�����޸Ĵ������� */
  .wADCTriggerUnSet = 0, /*!< �洢ADC->CR2��ֵ,�����޸Ĵ������� */
  .pParams_str = &R3_F4_ParamsM1,/*!< ������Ʋ���*/

};

/**
  * @brief  SpeedNPosition sensor parameters Motor 1 - HALL
  */
/* �����ӿڲ��� */
HALL_Handle_t HALL_M1 =
{
  ._Super = {/* �̳�SpeednPosFdbk_Handle_t,���ڷ��ʸ��� */
    .bElToMecRatio                     =	POLE_PAIR_NUM, /*!< ����ת����еת�Ǻ͵�Ƕ�,������ */
    .hMaxReliableMecSpeed01Hz          =	(uint16_t)(1.15*MAX_APPLICATION_SPEED/6), /*!< �����Чת��,��λ��0.1Hz */
    .hMinReliableMecSpeed01Hz          =	(uint16_t)(MIN_APPLICATION_SPEED/6),/*!< ��С��Чת��,��λ��0.1Hz*/
    .bMaximumSpeedErrorsNumber         =	MEAS_ERRORS_BEFORE_FAULTS,          /*!< ��������������.*/
    .hMaxReliableMecAccel01HzP         =	65535,                              /*!< ���ļ��ٶ�,��λ��0.1HzP(ÿ�ٶȲ������ڵ��ٶȱ仯ֵ) */
    .hMeasurementFrequency             =	TF_REGULATION_RATE,                 /*!< ���������Ƕȵ�Ƶ��,ͬ�����ڽ��ٶȵ�λ��0.1Hzת����dpp */
  }, 
  /* SW Settings */
  .SensorPlacement     = HALL_SENSORS_PLACEMENT, /*!< �����������ڷ�λ�� DEGREES_120 or DEGREES_60.*/
  .PhaseShift          = (int16_t)(HALL_PHASE_SHIFT * 65536/360), /*!< ͬ����Ƕ�(s16degree) */

  .SpeedSamplingFreqHz = MEDIUM_FREQUENCY_TASK_RATE, /*!< �������ٶȵ�Ƶ��Frequency (Hz) 500 */

  .SpeedBufferSize     = HALL_AVERAGING_FIFO_DEPTH,  /*!< �ٶȻ��� ����С��18.*/

  /* HW Settings */
 .TIMClockFreq       = HALL_TIM_CLK,           /*!< ��ʱ��ʱ��Ƶ�� (Hz) */

 .TIMx                = HALL_TIM5,             /*!< TIM5 */

 .H1Port             =  M1_HALL_H1_GPIO_Port,  /*!< HALL sensor H1 channel GPIO input port */
 .H1Pin              =  M1_HALL_H1_Pin,        /*!< HALL sensor H1 channel GPIO output pin */
 .H2Port             =  M1_HALL_H2_GPIO_Port,  /*!< HALL sensor H2 channel GPIO input port */
 .H2Pin              =  M1_HALL_H2_Pin,        /*!< HALL sensor H2 channel GPIO output pin */
 .H3Port             =  M1_HALL_H3_GPIO_Port,  /*!< HALL sensor H3 channel GPIO input port */
 .H3Pin              =  M1_HALL_H3_Pin,        /*!< HALL sensor H3 channel GPIO output pin */												 
};


/**
  * @brief  CircleLimitation Component parameters Motor 1 - Base Component
  */
/* Բ���� */
CircleLimitation_Handle_t CircleLimitationM1 =
{
  .MaxModule          = MAX_MODULE,      /*!< ���ĵ���ֵ,root(Vd^2+Vq^2) <= MAX_MODULE = 32767*97% */
  .Circle_limit_table = MMITABLE,        /*!< ������ */
  .Start_index        = START_INDEX, 		 /*!< ������ʼֵ*/
};


/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/

