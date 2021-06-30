#ifndef _COMMON_CAN_H_INCLUDED_
#define _COMMON_CAN_H_INCLUDED_

#include "stm32f1xx_hal.h"
#include "CCanPort.h"

bool CAN_Init(u8 in_u8Num, CAN_HandleTypeDef* in_pHandle, CCANPort* in_pPort);
bool CAN_SetFilter(u8 in_u8Num, u16 in_u16CanID, u8 in_u8Address);
void CAN_SendFrame(u8 in_u8Num);

#endif   // _COMMON_CAN_H_INCLUDED_
