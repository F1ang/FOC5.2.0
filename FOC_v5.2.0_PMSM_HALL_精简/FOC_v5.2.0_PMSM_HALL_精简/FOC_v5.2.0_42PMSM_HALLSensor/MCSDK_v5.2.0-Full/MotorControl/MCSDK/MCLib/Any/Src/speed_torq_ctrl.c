/*
  ******************************************************************************
  * @file    speed_torq_ctrl.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the following features
  *          of the Speed & Torque Control component of the Motor Control SDK.
  *
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

/* Includes ------------------------------------------------------------------*/
#include "speed_torq_ctrl.h"

/**
  * @brief  Initializes all the object variables, usually it has to be called
  *         once right after object creation.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @param  oPI the PI object used as controller for the speed regulation.
  *         It can be equal to MC_NULL if the STC is initialized in torque mode
  *         and it will never be configured in speed mode.
  * @param  oSPD the speed sensor used to perform the speed regulation.
  *         It can be equal to MC_NULL if the STC is used only in torque
  *         mode.
  * @retval none.
  */
/**
  * ��������: �ٶ�Ť�ؿ��Ƴ�ʼ��
  * �������: ������ƾ��
  * �� �� ֵ: ��
  * ˵    ��: ��ʼ������
  */
void STC_Init( SpeednTorqCtrl_Handle_t * pHandle, PID_Handle_t * pPI, SpeednPosFdbk_Handle_t * SPD_Handle )
{

  pHandle->PISpeed = pPI;
  pHandle->SPD = SPD_Handle;
  pHandle->Mode = pHandle->ModeDefault;
  pHandle->SpeedRef01HzExt = ( int32_t )pHandle->MecSpeedRef01HzDefault * 65536;
  pHandle->TorqueRef = ( int32_t )pHandle->TorqueRefDefault * 65536;
  pHandle->TargetFinal = 0;
  pHandle->RampRemainingStep = 0u;
  pHandle->IncDecAmount = 0;
}


/**
  * @brief It returns the speed sensor utilized by the FOC.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval SpeednPosFdbk_Handle_t speed sensor utilized by the FOC.
  */
  
/**
  * ��������: ��ȡ�ٶȴ���������
  * �������: @pHandle �ٶȴ��������ƾ��
  * �� �� ֵ: ��
  * ˵    ��: ������ٶ�ģʽ,��PID�㷨�еĻ��������
  */
SpeednPosFdbk_Handle_t * STC_GetSpeedSensor( SpeednTorqCtrl_Handle_t * pHandle )
{
  return ( pHandle->SPD );
}

/**
  * @brief  It should be called before each motor restart. If STC is set in
            speed mode, this method resets the integral term of speed regulator.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval none.
  */
/**
  * ��������: ��ձ���
  * �������: @pHandle �ٶȴ��������ƾ��
  * �� �� ֵ: ��
  * ˵    ��: ������ٶ�ģʽ,��PID�㷨�еĻ��������
  */
void STC_Clear( SpeednTorqCtrl_Handle_t * pHandle )
{
  if ( pHandle->Mode == STC_SPEED_MODE )
  {
    PID_SetIntegralTerm( pHandle->PISpeed, 0 );
  }
}


/**
  * ��������: ��ȡŤ��Ŀ��ֵ
  * �������: @pHandle
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
int16_t STC_GetTorqueRef( SpeednTorqCtrl_Handle_t * pHandle )
{
  return ( ( int16_t )( pHandle->TorqueRef / 65536 ) );
}

/**
  * @brief  Set the modality of the speed and torque controller. Two modality
  *         are available Torque mode and Speed mode.
  *         In Torque mode is possible to set directly the motor torque
  *         reference or execute a motor torque ramp. This value represents
  *         actually the Iq current reference expressed in digit.
  *         In Speed mode is possible to set the mechanical rotor speed
  *         reference or execute a speed ramp. The required motor torque is
  *         automatically calculated by the STC.
  *         This command interrupts the execution of any previous ramp command
  *         maintaining the last value of Iq.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @param  bMode modality of STC. It can be one of these two settings:
  *         STC_TORQUE_MODE to enable the Torque mode or STC_SPEED_MODE to
  *         enable the Speed mode.
  * @retval none
  */
void STC_SetControlMode( SpeednTorqCtrl_Handle_t * pHandle, STC_Modality_t bMode )
{
  pHandle->Mode = bMode;
  pHandle->RampRemainingStep = 0u; /* Interrupts previous ramp. */
}

/**
  * @brief  Starts the execution of a ramp using new target and duration. This
  *         command interrupts the execution of any previous ramp command.
  *         The generated ramp will be in the modality previously set by
  *         STC_SetControlMode method.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @param  hTargetFinal final value of command. This is different accordingly
  *         the STC modality.
  *         If STC is in Torque mode hTargetFinal is the value of motor torque
  *         reference at the end of the ramp. This value represents actually the
  *         Iq current expressed in digit.
  *         To convert current expressed in Amps to current expressed in digit
  *         is possible to use the formula:
  *         Current(digit) = [Current(Amp) * 65536 * Rshunt * Aop]  /  Vdd micro
  *         If STC is in Speed mode hTargetFinal is the value of mechanical
  *         rotor speed reference at the end of the ramp expressed in tenths of
  *         HZ.
  * @param  hDurationms the duration of the ramp expressed in milliseconds. It
  *         is possible to set 0 to perform an instantaneous change in the value.
  * @retval bool It return false if the absolute value of hTargetFinal is out of
  *         the boundary of the application (Above max application speed or max
  *         application torque or below min application speed depending on
  *         current modality of TSC) in this case the command is ignored and the
  *         previous ramp is not interrupted, otherwise it returns true.
  */
bool STC_ExecRamp( SpeednTorqCtrl_Handle_t * pHandle, int16_t hTargetFinal, uint32_t hDurationms )
{
  bool AllowedRange = true;
  uint32_t wAux;
  int32_t wAux1;
  int16_t hCurrentReference;

  /* Check if the hTargetFinal is out of the bound of application. */
  if ( pHandle->Mode == STC_TORQUE_MODE ) // Ť��ģʽ
  {
    hCurrentReference = STC_GetTorqueRef( pHandle );
    /* ���Ť����ֵ��Χ */
    if ( ( int32_t )hTargetFinal > ( int32_t )pHandle->MaxPositiveTorque )
    {
      AllowedRange = false;
    }
    if ( ( int32_t )hTargetFinal < ( int32_t )pHandle->MinNegativeTorque )
    {
      AllowedRange = false;
    }
  }
  else  // �ٶ�ģʽ
  {
    hCurrentReference = ( int16_t )( pHandle->SpeedRef01HzExt >> 16 );
    /* ����ٶ���ֵ��Χ */
    if ( ( int32_t )hTargetFinal > ( int32_t )pHandle->MaxAppPositiveMecSpeed01Hz )
    {
      AllowedRange = false;
    }
    else if ( hTargetFinal < pHandle->MinAppNegativeMecSpeed01Hz )
    {
      AllowedRange = false;
    }
    else if ( ( int32_t )hTargetFinal < ( int32_t )pHandle->MinAppPositiveMecSpeed01Hz )
    {
      if ( hTargetFinal > pHandle->MaxAppNegativeMecSpeed01Hz )
      {
        AllowedRange = false;
      }
    }
  }

  if ( AllowedRange == true )
  {
    /* Interrupts the execution of any previous ramp command */
    if ( hDurationms == 0u )// ʱ������Ϊ0.��ֱ������
    {
      if ( pHandle->Mode == STC_SPEED_MODE )
      {
        pHandle->SpeedRef01HzExt = ( int32_t )hTargetFinal * 65536;
      }
      else
      {
        pHandle->TorqueRef = ( int32_t )hTargetFinal * 65536;
      }
      pHandle->RampRemainingStep = 0u;
      pHandle->IncDecAmount = 0;
    }
    else
    {
      /* Store the hTargetFinal to be applied in the last step */
      pHandle->TargetFinal = hTargetFinal;

      /* Compute the (wRampRemainingStep) number of steps remaining to complete
      the ramp. */
      /* ����ʱ��(ms),�������ʱֵ(����),���Ƕ���STC������֮��ﵽĿ��ת�� */
      wAux = ( uint32_t )hDurationms * ( uint32_t )pHandle->STCFrequencyHz;
      wAux /= 1000u;
      pHandle->RampRemainingStep = wAux;
      pHandle->RampRemainingStep++;

      /* Compute the increment/decrement amount (wIncDecAmount) to be applied to
      the reference value at each CalcTorqueReference. */
      /* �ٶȲ�(Ť�ز�) / ����,�൱�ڼ��ٶ�,����ʱ��Ƭ�ķ�ʽ�������ٶ�ֵ  */
      wAux1 = ( ( int32_t )hTargetFinal - ( int32_t )hCurrentReference ) * 65536;
      wAux1 /= ( int32_t )pHandle->RampRemainingStep;
      pHandle->IncDecAmount = wAux1;
    }
  }

  return AllowedRange;
}


/**
  * @brief  It is used to compute the new value of motor torque reference. It
  *         must be called at fixed time equal to hSTCFrequencyHz. It is called
  *         passing as parameter the speed sensor used to perform the speed
  *         regulation.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval int16_t motor torque reference. This value represents actually the
  *         Iq current expressed in digit.
  *         To convert current expressed in Amps to current expressed in digit
  *         is possible to use the formula:
  *         Current(digit) = [Current(Amp) * 65536 * Rshunt * Aop]  /  Vdd micro
  */
/**
  * ��������: ���ݿ���ģʽ�����Ramp�����е�Ŀ��ֵ
  * �������: @pHandle 
  * �� �� ֵ: Ť��Ŀ��ֵ �����������
  * ˵    ��: ������(Amp)ת������������(digit)        Current(digit) = [Current(Amp) * 65536 * Rshunt * Aop]  /  Vdd micro
  */
int16_t STC_CalcTorqueReference( SpeednTorqCtrl_Handle_t * pHandle )
{
  int32_t wCurrentReference;
  int16_t hTorqueReference = 0;
  int16_t hMeasuredSpeed;
  int16_t hTargetSpeed;
  int16_t hError;

  /* ����ģʽ���� */
  if ( pHandle->Mode == STC_TORQUE_MODE )
  {
    wCurrentReference = pHandle->TorqueRef;
  }
  else
  {
    wCurrentReference = pHandle->SpeedRef01HzExt;
  }

  /* ���ݿ���ģʽ����Ŀ��ֵ. */
  if ( pHandle->RampRemainingStep > 1u )
  {
    /* Increment/decrement the reference value. */
    wCurrentReference += pHandle->IncDecAmount;

    /* Decrement the number of remaining steps */
    pHandle->RampRemainingStep--;
  }
  else if ( pHandle->RampRemainingStep == 1u ) // ���һ��
  {
    /* �趨���յ�Ŀ��ֵ. */
    wCurrentReference = ( int32_t )pHandle->TargetFinal * 65536;
    pHandle->RampRemainingStep = 0u;
  }

  /* �ٶ�ģʽ����PI������֮��õ�������Ŀ��ֵ */
  if ( pHandle->Mode == STC_SPEED_MODE )
  {
    /* Run the speed control loop */

    /* Compute speed error */
    hTargetSpeed = ( int16_t )( wCurrentReference / 65536 );
    hMeasuredSpeed = SPD_GetAvrgMecSpeed01Hz( pHandle->SPD );
    hError = hTargetSpeed - hMeasuredSpeed;
    hTorqueReference = PI_Controller( pHandle->PISpeed, ( int32_t )hError );

    pHandle->SpeedRef01HzExt = wCurrentReference;
    pHandle->TorqueRef = ( int32_t )hTorqueReference * 65536;
  }
  else/* Ť��ģʽ����ֱ��ʹ��Ŀ��ֵ */
  {
    pHandle->TorqueRef = wCurrentReference;
    hTorqueReference = ( int16_t )( wCurrentReference / 65536 );
  }

  return hTorqueReference;
}

/**
  * @brief  Get the Default mechanical rotor speed reference expressed in tenths
  *         of HZ.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval int16_t It returns the Default mechanical rotor speed. reference
  *         expressed in tenths of HZ.
  */
/**
  * ��������: ��ȡĬ�ϵĻ�еת��
  * �������: @pHandle
  * �� �� ֵ: ��
  * ˵    ��: ��еת��Ĭ��ֵ(0.1Hz)
  */
int16_t STC_GetMecSpeedRef01HzDefault( SpeednTorqCtrl_Handle_t * pHandle )
{
  return pHandle->MecSpeedRef01HzDefault;
}


/**
  * @brief It returns the default values of Iqdref.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval default values of Iqdref.
  */
/**
  * ��������: ��ȡĬ�ϵ�Iqֵ
  * �������: @pHandle
  * �� �� ֵ: Iq,IdĬ��ֵ
  * ˵    ��: 
  */
Curr_Components STC_GetDefaultIqdref( SpeednTorqCtrl_Handle_t * pHandle )
{
  Curr_Components IqdRefDefault;
  IqdRefDefault.qI_Component1 = pHandle->TorqueRefDefault;
  IqdRefDefault.qI_Component2 = pHandle->IdrefDefault;
  return IqdRefDefault;
}

/**
  * @brief  Force the speed reference to the curren speed. It is used
  *         at the START_RUN state to initialize the speed reference.
  * @param  pHandle: handler of the current instance of the SpeednTorqCtrl component
  * @retval none
  */
void STC_ForceSpeedReferenceToCurrentSpeed( SpeednTorqCtrl_Handle_t * pHandle )
{
  pHandle->SpeedRef01HzExt = ( int32_t )SPD_GetAvrgMecSpeed01Hz( pHandle->SPD ) * ( int32_t )65536;
}

/************************ (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
