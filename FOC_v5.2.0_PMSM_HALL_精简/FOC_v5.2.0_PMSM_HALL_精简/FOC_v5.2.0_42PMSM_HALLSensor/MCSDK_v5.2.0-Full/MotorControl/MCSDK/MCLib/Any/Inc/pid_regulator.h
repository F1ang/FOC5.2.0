/**
  ******************************************************************************
  * @file    pid_regulator.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          PID reulator component of the Motor Control SDK.
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
  * @ingroup PIDRegulator
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PIDREGULATOR_H
#define __PIDREGULATOR_H

/* Includes ------------------------------------------------------------------*/
#include "mc_type.h"

/**
  * @brief Handle of a PID component
  *
  * @detail This structure stores all the parameters needed to perform a proportional,
  * integral and derivative regulation computation. It also stores configurable limits
  * in order to saturate the integral terms and the output value. This structure is
  * passed to each PID component function.
  */
typedef struct PID_Handle
{
  int16_t   hDefKpGain;           /**< Default @f$K_{pg}@f$ gain */
  int16_t   hDefKiGain;           /**< Default @f$K_{ig}@f$ gain */
  int16_t   hKpGain;              /**< @f$K_{pg}@f$ gain used by PID component */
  int16_t   hKiGain;              /**< @f$K_{ig}@f$ gain used by PID component */
  int32_t   wIntegralTerm;        /**< integral term */
  int32_t   wUpperIntegralLimit;  /**< Upper limit used to saturate the integral
                                       term given by @f$\frac{K_{ig}}{K_{id}} @f$ * integral of
                                       process variable error */
  int32_t   wLowerIntegralLimit;  /**< Lower limit used to saturate the integral
                                       term given by Ki / Ki divisor * integral of
                                       process variable error */
  int16_t   hUpperOutputLimit;    /**< Upper limit used to saturate the PI output */
  int16_t   hLowerOutputLimit;    /**< Lower limit used to saturate the PI output */
  uint16_t  hKpDivisor;           /**< Kp gain divisor, used in conjuction with
                                       Kp gain allows obtaining fractional values.
                                       If FULL_MISRA_C_COMPLIANCY is not defined
                                       the divisor is implemented through
                                       algebrical right shifts to speed up PI
                                       execution. Only in this case this parameter
                                       specifies the number of right shifts to be
                                       executed */
  uint16_t  hKiDivisor;           /**< Ki gain divisor, used in conjuction with
                                       Ki gain allows obtaining fractional values.
                                       If FULL_MISRA_C_COMPLIANCY is not defined
                                       the divisor is implemented through
                                       algebrical right shifts to speed up PI
                                       execution. Only in this case this parameter
                                       specifies the number of right shifts to be
                                       executed */
  uint16_t  hKpDivisorPOW2;       /**< Kp gain divisor expressed as power of 2.
                                       E.g. if gain divisor is 512 the value
                                       must be 9 as 2^9 = 512 */
  uint16_t  hKiDivisorPOW2;       /**< Ki gain divisor expressed as power of 2.
                                       E.g. if gain divisor is 512 the value
                                       must be 9 as 2^9 = 512 */
} PID_Handle_t;

/*
 * It initializes the handle
 */
void PID_HandleInit( PID_Handle_t * pHandle );

/*
 * It set a new value into the PI integral term
 */
void PID_SetIntegralTerm( PID_Handle_t * pHandle, int32_t wIntegralTermValue );

/* This function compute the output of a PI regulator sum of its
 * proportional and integralterms
 */
int16_t PI_Controller( PID_Handle_t * pHandle, int32_t wProcessVarError );



#endif /*__PIDREGULATOR_H*/

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
