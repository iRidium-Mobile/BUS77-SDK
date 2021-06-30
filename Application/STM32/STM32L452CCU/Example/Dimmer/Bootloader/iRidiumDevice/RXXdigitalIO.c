#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_tim.h"
#include "RXXdigitalIO.h"
//#include "LinkChan.h"
#include  "IridiumValue.h"

#define mask_button     0x800


  const   hd_discret_pin_t  hd_discret_pin[11]= {
    {GPIOB,GPIO_PIN_2,GPIO_PIN_SET,GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP },//sys_led,
 
 /*   {GPIOB,GPIO_PIN_12,GPIO_PIN_RESET,GPIO_PIN_SET,GPIO_MODE_OUTPUT_OD }, //rgb1_red,
    {GPIOB,GPIO_PIN_13,GPIO_PIN_RESET,GPIO_PIN_SET,GPIO_MODE_OUTPUT_OD },   //rgb1_green,
    {GPIOB,GPIO_PIN_14,GPIO_PIN_RESET,GPIO_PIN_SET,GPIO_MODE_OUTPUT_OD },   //rgb1_blue,
     
    {GPIOB,GPIO_PIN_3,GPIO_PIN_RESET,GPIO_PIN_SET,GPIO_MODE_OUTPUT_OD },  //rgb2_red,
    {GPIOB,GPIO_PIN_5,GPIO_PIN_RESET,GPIO_PIN_SET,GPIO_MODE_OUTPUT_OD },  //rgb2_green,
    {GPIOA,GPIO_PIN_0,GPIO_PIN_RESET,GPIO_PIN_SET,GPIO_MODE_OUTPUT_OD },   //rgb2_blue,
*/
    {GPIOA,GPIO_PIN_7,GPIO_PIN_SET,GPIO_PIN_RESET,GPIO_MODE_OUTPUT_PP },   //pwr_relay1,
    {GPIOB,GPIO_PIN_15,GPIO_PIN_SET,GPIO_PIN_RESET,GPIO_MODE_OUTPUT_PP }, //pwr_relay2,
    {GPIOB,GPIO_PIN_6,GPIO_PIN_SET,GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP },  //relay3,
    {GPIOB,GPIO_PIN_7,GPIO_PIN_SET,GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP }, //relay4
  };

void RXXdigitalIO_init(uint8_t sec_wait){
  
//инициализация дискретных пинов по таблице
  GPIO_InitTypeDef GPIO_InitStruct;
  for ( uint32_t i=0; i<(sizeof(hd_discret_pin)/sizeof(hd_discret_pin_t)); i++) {
   HAL_GPIO_WritePin(hd_discret_pin[i].GPIOx,hd_discret_pin[i].GPIO_Pin ,hd_discret_pin[i].PinRESET);
  GPIO_InitStruct.Pin = hd_discret_pin[i].GPIO_Pin;
  GPIO_InitStruct.Mode = hd_discret_pin[i].PinMODE;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(hd_discret_pin[i].GPIOx, &GPIO_InitStruct); 
  }  
}
bool GetParamDiscret(universal_value_t *value, u32 Param) {
  if (Param< (sizeof(hd_discret_pin)/sizeof(hd_discret_pin_t))){
  }
  else
    return 0;
  return 0;
};
bool SetParamDiscret(universal_value_t *value, u32 Param) {
 if (Param< (sizeof(hd_discret_pin)/sizeof(hd_discret_pin_t))){
   
 HAL_GPIO_WritePin(hd_discret_pin[Param].GPIOx,hd_discret_pin[Param].GPIO_Pin ,
                   (value->m_bValue == 1) ?   hd_discret_pin[Param].PinSET:hd_discret_pin[Param].PinRESET);  
 return 1; 
 }
  else
    return 0;
  return 0;
};