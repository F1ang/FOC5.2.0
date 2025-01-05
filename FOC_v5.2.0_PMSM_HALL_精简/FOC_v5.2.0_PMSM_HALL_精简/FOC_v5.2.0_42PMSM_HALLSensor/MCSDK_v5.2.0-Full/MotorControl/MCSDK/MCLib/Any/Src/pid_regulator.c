/**
 ******************************************************************************
 * @file    pid_regulator.c
 * @author  Motor Control SDK Team, ST Microelectronics
 * @brief   This file provides firmware functions that implement the following features
 *          of the PID regulator component of the Motor Control SDK:
 *
 *           * proportional, integral and derivative computation funcions
 *           * read and write gain functions
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
#include "pid_regulator.h"

/**
 * @brief  It initializes the handle
 * @param  pHandle: handler of the current instance of the PID component
 * @retval None
 */
/**
  * ��������: ��ʼ��PID����
  * �������: @pHandle
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void PID_HandleInit( PID_Handle_t * pHandle )
{
  pHandle->hKpGain =  pHandle->hDefKpGain;
  pHandle->hKiGain =  pHandle->hDefKiGain;
  pHandle->wIntegralTerm = 0x00000000UL;
}


/**
 * @brief  It set a new value into the PI integral term
 * pHandle: handler of the current instance of the PID component
 * @param  wIntegralTermValue: new integral term value
 * @retval None
 */
void PID_SetIntegralTerm( PID_Handle_t * pHandle, int32_t wIntegralTermValue )
{
  pHandle->wIntegralTerm = wIntegralTermValue;

  return;
}

/**
 * @brief  This function compute the output of a PI regulator sum of its
 *         proportional and integral terms
 * @param  pHandle: handler of the current instance of the PID component
 * @param  wProcessVarError: current process variable error, intended as the reference
 *         value minus the present process variable value
 * @retval computed PI output
 */
/**
  * ��������: PID������
  * �������: @wProcessVarError ���ֵ
  * �� �� ֵ: PID�㷨���
  * ˵    ��: 
  */
int16_t PI_Controller( PID_Handle_t * pHandle, int32_t wProcessVarError )
{
  int32_t wProportional_Term, wIntegral_Term, wOutput_32, wIntegral_sum_temp;
  int32_t wDischarge = 0;
  int16_t hUpperOutputLimit = pHandle->hUpperOutputLimit;
  int16_t hLowerOutputLimit = pHandle->hLowerOutputLimit;

  /* Proportional term computation*/
  wProportional_Term = pHandle->hKpGain * wProcessVarError;// ������

  /* Integral term computation */
  if ( pHandle->hKiGain == 0 ) // ������
  {
    pHandle->wIntegralTerm = 0;
  }
  else
  {
    wIntegral_Term = pHandle->hKiGain * wProcessVarError; // Ki * error
    wIntegral_sum_temp = pHandle->wIntegralTerm + wIntegral_Term;// �ۼ� error

    /* �з����������������,����������ӽ���Ǹ���.˵���Ѿ������ */
    if ( wIntegral_sum_temp < 0 ) // ��ͬ��(uint32_t) wIntegral_sum_temp > 32767
    {
      if ( pHandle->wIntegralTerm > 0 )
      {
        if ( wIntegral_Term > 0 )
        {
          wIntegral_sum_temp = INT32_MAX;
        }
      }
    }
    else
    {
      if ( pHandle->wIntegralTerm < 0 )
      {
        if ( wIntegral_Term < 0 )
        {
          wIntegral_sum_temp = -INT32_MAX;
        }
      }
    }

    /* �������޺����޴��� */
    if ( wIntegral_sum_temp > pHandle->wUpperIntegralLimit )
    {
      pHandle->wIntegralTerm = pHandle->wUpperIntegralLimit;
    }
    else if ( wIntegral_sum_temp < pHandle->wLowerIntegralLimit )
    {
      pHandle->wIntegralTerm = pHandle->wLowerIntegralLimit;
    }
    else
    {
      pHandle->wIntegralTerm = wIntegral_sum_temp;
    }
  }
  
  /* ����� = Kp*e + Ki*SUM(e)) 
   * Kp,Ki����������������ʽ���渡����,�������������޶��˷�ĸ��2���ݴη�,
   *  ����������nλ������� 2^n
   */
  wOutput_32 = ( wProportional_Term >> pHandle->hKpDivisorPOW2 ) + ( pHandle->wIntegralTerm >> pHandle->hKiDivisorPOW2 );

  /* �������������,����������Ƶ�һ�����ۼӵ�������,������һ������ */
  if ( wOutput_32 > hUpperOutputLimit )
  {

    wDischarge = hUpperOutputLimit - wOutput_32;
    wOutput_32 = hUpperOutputLimit;
  }
  else if ( wOutput_32 < hLowerOutputLimit )
  {

    wDischarge = hLowerOutputLimit - wOutput_32;
    wOutput_32 = hLowerOutputLimit;
  }

  pHandle->wIntegralTerm += wDischarge;

  return ( ( int16_t )( wOutput_32 ) );
}

/************************ (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
