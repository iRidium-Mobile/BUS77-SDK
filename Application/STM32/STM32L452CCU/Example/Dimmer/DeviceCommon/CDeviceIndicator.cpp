#include "CDeviceIndicator.h"
#include "main.h"

/**
   Получение времени
   на входе    :  *
   на выходе   :  текущее значение времени
*/
u32 CDeviceIndicator::GetTime()
{
   return HAL_GetTick();
}

/**
   Получение времени
   на входе    :  in_u8Color  - цвет индикатора
                  in_bState   - значение индикатора
   на выходе   :  *
*/
void CDeviceIndicator::Set(u8 in_u8Color, bool in_bState)
{
   GPIO_PinState l_State = GPIO_PIN_SET;
   
   l_State = in_bState ? GPIO_PIN_SET : GPIO_PIN_RESET;
   HAL_GPIO_WritePin(Onboard_LED_GPIO_Port, Onboard_LED_Pin, l_State);
}
