/**
  ******************************************************************************
  * @file    mc_interface.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the features
  *          of the MC Interface component of the Motor Control SDK:
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
#include "mc_interface.h"


/* Private macros ------------------------------------------------------------*/

/* Functions -----------------------------------------------*/

/**
  * @brief  Initializes all the object variables, usually it has to be called
  *         once right after object creation. It is also used to assign the
  *         state machine object, the speed and torque controller, and the FOC
  *         drive object to be used by MC Interface.
  * @param  pHandle pointer on the component instance to initialize.
  * @param  pSTM the state machine object used by the MCI.
  * @param  pSTC the speed and torque controller used by the MCI.
  * @param  pFOCVars pointer to FOC vars to be used by MCI.
  * @retval none.
  */
/**
  * ��������: ������ƽӿڳ�ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void MCI_Init( MCI_Handle_t * pHandle, STM_Handle_t * pSTM, SpeednTorqCtrl_Handle_t * pSTC, pFOCVars_t pFOCVars )
{
  pHandle->pSTM     = pSTM;
  pHandle->pSTC     = pSTC;
  pHandle->pFOCVars = pFOCVars;

  /* Buffer related initialization */
  pHandle->lastCommand  = MCI_NOCOMMANDSYET;
  pHandle->hFinalSpeed  = 0;
  pHandle->hFinalTorque = 0;
  pHandle->hDurationms  = 0;
  pHandle->CommandState = MCI_BUFFER_EMPTY;
}

/**
  * @brief  This is a buffered command to set a motor speed ramp. This commands
  *         don't become active as soon as it is called but it will be executed
  *         when the pSTM state is START_RUN or RUN. User can check the status
  *         of the command calling the MCI_IsCommandAcknowledged method.
  * @param  pHandle Pointer on the component instance to operate on.
  * @param  hFinalSpeed is the value of mechanical rotor speed reference at the
  *         end of the ramp expressed in tenths of HZ.
  * @param  hDurationms the duration of the ramp expressed in milliseconds. It
  *         is possible to set 0 to perform an instantaneous change in the
  *         value.
  * @retval none.
  */
/**
  * ��������: ִ���ٶ�Ramp����ָ��
  * �������: @hFinalSpeed Ŀ���ٶ�  @hDurationms����ʱ��
  * �� �� ֵ: ��
  * ˵    ��: ��MCI_EXECSPEEDRAMPָ������ָ���,�ȴ�״̬��ΪSTART_RUN or RUN��ʱ��
  *           �ͻ�ִ��ָ��MCI_EXECSPEEDRAMP.
  */
void MCI_ExecSpeedRamp( MCI_Handle_t * pHandle,  int16_t hFinalSpeed, uint16_t hDurationms )
{
  pHandle->lastCommand  = MCI_EXECSPEEDRAMP;
  pHandle->hFinalSpeed  = hFinalSpeed;
  pHandle->hDurationms  = hDurationms;
  pHandle->CommandState = MCI_COMMAND_NOT_ALREADY_EXECUTED;
  pHandle->LastModalitySetByUser = STC_SPEED_MODE;
}
/**
  * @brief  This is a buffered command to set a motor torque ramp. This commands
  *         don't become active as soon as it is called but it will be executed
  *         when the pSTM state is START_RUN or RUN. User can check the status
  *         of the command calling the MCI_IsCommandAcknowledged method.
  * @param  pHandle Pointer on the component instance to work on.
  * @param  hFinalTorque is the value of motor torque reference at the end of
  *         the ramp. This value represents actually the Iq current expressed in
  *         digit.
  *         To convert current expressed in Amps to current expressed in digit
  *         is possible to use the formula:
  *         Current (digit) = [Current(Amp) * 65536 * Rshunt * Aop] / Vdd micro.
  * @param  hDurationms the duration of the ramp expressed in milliseconds. It
  *         is possible to set 0 to perform an instantaneous change in the
  *         value.
  * @retval none.
  */
void MCI_ExecTorqueRamp( MCI_Handle_t * pHandle,  int16_t hFinalTorque, uint16_t hDurationms )
{
  pHandle->lastCommand  = MCI_EXECTORQUERAMP;
  pHandle->hFinalTorque = hFinalTorque;
  pHandle->hDurationms  = hDurationms;
  pHandle->CommandState = MCI_COMMAND_NOT_ALREADY_EXECUTED;
  pHandle->LastModalitySetByUser = STC_TORQUE_MODE;
}

/**
  * @brief  This is a user command used to begin the start-up procedure.
  *         If the state machine is in IDLE state the command is executed
  *         instantaneously otherwise the command is discarded. User must take
  *         care of this possibility by checking the return value.
  *         Before calling MCI_StartMotor it is mandatory to execute one of
  *         these commands:\n
  *         MCI_ExecSpeedRamp\n
  *         MCI_ExecTorqueRamp\n
  *         MCI_SetCurrentReferences\n
  *         Otherwise the behaviour in run state will be unpredictable.\n
  *         <B>Note:</B> The MCI_StartMotor command is used just to begin the
  *         start-up procedure moving the state machine from IDLE state to
  *         IDLE_START. The command MCI_StartMotor is not blocking the execution
  *         of project until the motor is really running; to do this, the user
  *         have to check the state machine and verify that the RUN state (or
  *         any other state) has been reached.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval bool It returns true if the command is successfully executed
  *         otherwise it return false.
  */
/**
  * ��������: �������
  * �������: @pHandle
  * �� �� ֵ: @RetVal ָ��ִ�� �ɹ����
  * ˵    ��: ֻ����״̬��ΪIDLE��ʱ���������ִ����������,�������ö���.
  *           �ڵ���MCI_StartMotor֮ǰ�����ȵ�������ָ��֮һ:
  *                      MCI_ExecSpeedRamp\n
  *                      MCI_ExecTorqueRamp\n
  *                      MCI_SetCurrentReferences\n
  */ 
bool MCI_StartMotor( MCI_Handle_t * pHandle )
{
  bool RetVal = STM_NextState( pHandle->pSTM, IDLE_START );

  if ( RetVal == true )
  {
    pHandle->CommandState = MCI_COMMAND_NOT_ALREADY_EXECUTED;
  }

  return RetVal;
}

/**
  * @brief  This is a user command used to begin the stop motor procedure.
  *         If the state machine is in RUN or START states the command is
  *         executed instantaneously otherwise the command is discarded. User
  *         must take care of this possibility by checking the return value.\n
  *         <B>Note:</B> The MCI_StopMotor command is used just to begin the
  *         stop motor procedure moving the state machine to ANY_STOP.
  *         The command MCI_StopMotor is not blocking the execution of project
  *         until the motor is really stopped; to do this, the user have to
  *         check the state machine and verify that the IDLE state has been
  *         reached again.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval bool It returns true if the command is successfully executed
  *         otherwise it return false.
  */
/**
  * ��������: ����ANY_STOPָ��
  * �������: pHandle
  * �� �� ֵ: @bool ָ��ִ�гɹ����
  * ˵    ��: ֻ����״̬��ΪRUN or START��������ָ���,��������ָ��
  */
bool MCI_StopMotor( MCI_Handle_t * pHandle )
{
  return STM_NextState( pHandle->pSTM, ANY_STOP );
}




/**
  * @brief  This is usually a method managed by task. It must be called
  *         periodically in order to check the status of the related pSTM object
  *         and eventually to execute the buffered command if the condition
  *         occurs.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval none.
  */
/**
  * ��������: ִ�л����е�ָ��
  * �������: @pHandle
  * �� �� ֵ: ��
  * ˵    ��: �����Եĵ��øú�������ִ�п���ָ��
  */
void MCI_ExecBufferedCommands( MCI_Handle_t * pHandle )
{
  if ( pHandle != MC_NULL )
  {
    /* ��ǰ�����е�ָ��ִ��״̬:δִ��(��Ҫ��ִ��) */
    if ( pHandle->CommandState == MCI_COMMAND_NOT_ALREADY_EXECUTED )
    {
      bool commandHasBeenExecuted = false;
      switch ( pHandle->lastCommand )
      {
        case MCI_EXECSPEEDRAMP: // Speed Ramp
        {
          STC_SetControlMode( pHandle->pSTC, STC_SPEED_MODE );
          commandHasBeenExecuted = STC_ExecRamp( pHandle->pSTC, pHandle->hFinalSpeed, pHandle->hDurationms );
        }
        break;
        case MCI_EXECTORQUERAMP:// Ť�� Ramp
        {
          STC_SetControlMode( pHandle->pSTC, STC_TORQUE_MODE );
          commandHasBeenExecuted = STC_ExecRamp( pHandle->pSTC, pHandle->hFinalTorque, pHandle->hDurationms );
        }
        break;
        case MCI_SETCURRENTREFERENCES:// ���õ�ǰ��Ŀ��ֵ
        {
          pHandle->Iqdref =  pHandle->Iqdref;;
          commandHasBeenExecuted = true;
        }
        break;
        default:
          break;
      }
    /* ��ǰ�����е�ָ��ִ��״̬:��ִ�� */
      if ( commandHasBeenExecuted )
      {
        pHandle->CommandState = MCI_COMMAND_EXECUTED_SUCCESFULLY;
      }
      else
      {
        pHandle->CommandState = MCI_COMMAND_EXECUTED_UNSUCCESFULLY;
      }
    }
  }
}


/**
  * @brief  It returns information about the state of the related pSTM object.
  * @param  pHandle Pointer on the component instance to work on.
  * @retval State_t It returns the current state of the related pSTM object.
  */
/**
  * ��������: ��ȡ��ǰ״̬��״̬
  * �������: @pHandle
  * �� �� ֵ: @State_t ״̬��״̬
  * ˵    ��: ��
  */
State_t  MCI_GetSTMState( MCI_Handle_t * pHandle )
{
  return STM_GetState( pHandle->pSTM );
}



/************************ (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
