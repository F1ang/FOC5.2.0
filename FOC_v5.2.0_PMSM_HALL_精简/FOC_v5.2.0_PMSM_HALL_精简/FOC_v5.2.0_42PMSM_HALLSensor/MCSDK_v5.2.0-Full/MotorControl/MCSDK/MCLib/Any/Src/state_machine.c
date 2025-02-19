/**
  ******************************************************************************
  * @file    state_machine.c
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file provides firmware functions that implement the features
  *          of the Motor Control State Machine component of the Motor Control SDK:
  *
  *           * Check that transition from one state to another is legal
  *           * Handle the fault processing
  *           * Provide accessor to State machine internal state
  *           * Provide accessor to error state
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
#include "state_machine.h"

/**
  * @brief  Initializes all the object variables, usually it has to be called
  *         once right after object creation.
  * @param pHandle pointer on the component instance to initialize.
  * @retval none.
  */
/**
  * 函数功能: 初始化
  * 输入参数: 
  * 返 回 值: 
  * 说    明: 
  */
void STM_Init( STM_Handle_t * pHandle )
{

  pHandle->bState         = IDLE;
  pHandle->hFaultNow      = MC_NO_FAULTS;
  pHandle->hFaultOccurred = MC_NO_FAULTS;
}

/**
  * @brief It submits the request for moving the state machine into the state
  *        specified by bState (FAULT_NOW and FAUL_OVER are not handled by this
  *        method). Accordingly with the current state, the command is really
  *        executed (state machine set to bState) or discarded (no state
  *        changes).
  *        If requested state can't be reached the return value is false and the
  *        MC_SW_ERROR is raised, but if requested state is IDLE_START,
  *        IDLE_ALIGNMENT or ANY_STOP, that corresponds with the user actions:
  *        Start Motor, Encoder Alignemnt and Stop Motor, the MC_SW_ERROR is
  *        not raised.
  * @param pHanlde pointer of type  STM_Handle_t.
  * @param bState New requested state
  * @retval bool It returns true if the state has been really set equal to
  *         bState, false if the requested state can't be reached
  */
/**
  * 函数功能: 设置下一步的状态
  * 输入参数: @bState 新的状态
  * 返 回 值: @bool true成功切换,false设置失败
  * 说    明: 如果进入错误进程MC_SW_ERROR,则表示状态机切换失败
  */
bool STM_NextState( STM_Handle_t * pHandle, State_t bState )
{
  bool bChangeState = false;
  State_t bCurrentState = pHandle->bState;
  State_t bNewState = bCurrentState;

  switch ( bCurrentState )
  {
    case IDLE:// 只能从IDLE -> IDLE_START 
      if ( bState == IDLE_START )

      {
        bNewState = bState;
        bChangeState = true;
      }
      break;
    

    case IDLE_START:// 从空闲中启动
      if ( ( bState == ANY_STOP ) || ( bState == CHARGE_BOOT_CAP ) ||
           ( bState == START ) || ( bState == OFFSET_CALIB ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case CHARGE_BOOT_CAP://自举电容充电
      if ( ( bState == OFFSET_CALIB ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case OFFSET_CALIB:// 电流校准偏移
      if ( ( bState == CLEAR ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case CLEAR: // 复位变量
      if ( ( bState == START ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case START:// 启动
      if ( ( bState == START_RUN ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case START_RUN:// 启动转入RUN
      if ( ( bState == RUN ) || ( bState == ANY_STOP ) )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case RUN:// 正常的RUN
      if ( bState == ANY_STOP )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case ANY_STOP:
      if ( bState == STOP )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case STOP:
      if ( bState == STOP_IDLE )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;

    case STOP_IDLE:
      if (  bState == IDLE  )
      {
        bNewState = bState;
        bChangeState = true;
      }
      break;
    default:
      break;
  }

  if ( bChangeState )
  {
    pHandle->bState = bNewState;
  }
  else
  {
    if ( !( ( bState == IDLE_START ) || ( bState == ANY_STOP ) ) )
    {
      /* If new state is not a user command START/STOP raise a software error */
      STM_FaultProcessing( pHandle, MC_SW_ERROR, 0u );
    }
  }

  return ( bChangeState );
}

/**
  * @brief It clocks both HW and SW faults processing and update the state
  *        machine accordingly with hSetErrors, hResetErrors and present state.
  *        Refer to State_t description for more information about fault states.
  * @param pHanlde pointer of type  STM_Handle_t
  * @param hSetErrors Bit field reporting faults currently present
  * @param hResetErrors Bit field reporting faults to be cleared
  * @retval State_t New state machine state after fault processing
  */
/**
  * 函数功能: 错误进程
  * 输入参数: @hSetErrors 错误状态类型 @hResetErrors清除错误警报
  * 返 回 值: 返回error是否已经被清除
  * 说    明: 判断当前存在错误没有被清除,或者是错误已经清除但是没有应答
  */
State_t STM_FaultProcessing( STM_Handle_t * pHandle, uint16_t hSetErrors, uint16_t
                             hResetErrors )
{
  State_t LocalState =  pHandle->bState;

  /* Set current errors */
  pHandle->hFaultNow = ( pHandle->hFaultNow | hSetErrors ) & ( ~hResetErrors );// 清除警报/或者置位警报
  pHandle->hFaultOccurred |= hSetErrors; // 记录出现的错误

  if ( LocalState == FAULT_NOW )             // 上一步已经处于错误状态,
  {
    if ( pHandle->hFaultNow == MC_NO_FAULTS )// 判断错误是否已经被清除
    {
      pHandle->bState = FAULT_OVER;          // 是则Fault_Over,等待应答处理
      LocalState = FAULT_OVER;
    }
  }
  else                                       // 上一步不是错误状态
  {                                           
    if ( pHandle->hFaultNow != MC_NO_FAULTS )// 调用这个函数也不是为了清除警报
    {                             
      pHandle->bState = FAULT_NOW;           // 就是当前发生了错误
      LocalState = FAULT_NOW;
    }
  }

  return ( LocalState );
}

/**
  * @brief  Returns the current state machine state
  * @param  pHanlde pointer of type  STM_Handle_t
  * @retval State_t Current state machine state
  */
/**
  * 函数功能: 获取状态机状态
  * 输入参数: @pHandle
  * 返 回 值: 无
  * 说    明: 无
  */
State_t STM_GetState( STM_Handle_t * pHandle )
{
  return ( pHandle->bState );
}



/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
