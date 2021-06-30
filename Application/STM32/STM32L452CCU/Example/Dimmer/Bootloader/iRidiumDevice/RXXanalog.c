#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_dma.h"
#include "stm32l4xx_hal_tim.h"
#include "stm32l4xx_hal_adc.h"
#include "RXXanalog.h"
#include <math.h>
#include "stdlib.h"
#include  "IridiumValue.h"

TIM_HandleTypeDef htim6;
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;



const uint8_t list[8]={num_volt,num_volt, num_volt,num_volt,num_temp,num_temp,num_vref,num_vref};





extern void HTIM_Display_Refresh(TIM_HandleTypeDef *htim);

void math_temp(uint32_t, uint8_t); 
void math_input_volt(uint32_t, uint8_t); 
void math_input_volt1Wx(uint32_t, uint8_t); 
void math_input_voltANx(uint32_t, uint8_t); 
void math_resistM(uint32_t, uint8_t);
void math_resistW(uint32_t, uint8_t);
void math_VREF(uint32_t, uint8_t);

t_refer_chan_adc refer_chan_adc[3] = 
{
  {num_vref,0,math_VREF},
  {num_volt,0,math_input_volt},
  {num_temp,0,math_temp},

};
/*
{num_AIN1,0,math_input_voltANx},
{num_AIN2,0,math_input_voltANx},
{num_AIN3,0,math_input_voltANx},
{num_AIN4,0,math_input_voltANx},
  {ana_resist1W1,0,math_resistW},
  {ana_resist1W2,0,math_resistW},
  {ana_resist1W3,0,math_resistW},
  {ana_resist1W4,0,math_resistW},
*/
uint16_t array_buf[42];
uint16_t ptr_array = 0;

#define cnt_sample (sizeof(refer_chan_adc[0].aver_array)/sizeof(refer_chan_adc[0].aver_array[0]))

/*__root*/ float Upower = 3.3;
/*__root*/ uint32_t intUpower = 3300;
/*__root */ uint32_t intScale = 1000;
//резистивный делитель для определения входного напряжения
const float Uinup=(10000 + 1000);
const float Uindn=4096*1000;
const float UinR = (10000 + 1000)/4096*1000;
//резистивный делитель для определения выходного напряжения ainN 
const float UaPup=(10000 + 1000);
const float UaPdn=4096*1000;
//резистивный делитель для определения напряжения 1Wx
const float Ucrup=(10000 + 15000);
const float Ucrdn=4096*15000;
//const float Ucrmux=(((float)10*3.3)*(1+(float)18000/(float)36000))/(float)4095;
//const float Ucrmux=(((float)10*3.3)*(1+(float)10000/(float)19100))/(float)4095;
int32_t VREF=4095*125;
t_max_min max_min[2]={{65535,0},{65535,0}};

//t_refer_adc refer_adc;
volatile uint16_t  buff_adc[sizeof(list)/sizeof(list[0])*max_size_adc_per_chanel*max_average_point_2_chanel];

void analog_math(void);
static void MX_ADC1_Init(void);
static void MX_TIM6_Init(void);



 
#define cnt_get_index sizeof(refer_chan_adc)/sizeof(refer_chan_adc[0])

uint8_t       get_index(uint8_t chan_num) {
  for (uint8_t i=0;i<cnt_get_index;i++) 
    if (chan_num==refer_chan_adc[i].conf_chan)
      return i;
  return 0xff;
}

t_refer_chan_adc * GetChanData(uint8_t chan){
  if (get_index(chan)==0xFF)
    return 0;
      else
        return &refer_chan_adc[get_index(chan)];
}

uint32_t gistrez(uint32_t step, uint32_t accur) {
  uint32_t current, last;
  current = refer_chan_adc[step].aver_array[0];
  last = refer_chan_adc[step].last;
  if ((current>  (last + (last >> accur))) || (current<  (last - (last >> accur))))
  { 
    refer_chan_adc[step].last = current;
    refer_chan_adc[step].flag_new_value = 1;
    return current;
  }
       return last;
};
void math_temp(uint32_t val, uint8_t step) {

  
  refer_chan_adc[step].value_ain = __HAL_ADC_CALC_TEMPERATURE(intUpower, (gistrez(step,7)>>7), LL_ADC_RESOLUTION_12B);

      
}; 
void math_input_volt(uint32_t val, uint8_t step){
refer_chan_adc[step].value_ain=gistrez(step,8) *11;
refer_chan_adc[step].value_ain = intUpower * (refer_chan_adc[step].value_ain>>11);
refer_chan_adc[step].value_ain = refer_chan_adc[step].value_ain >> 8;
refer_chan_adc[step].f_value_ain= (float)(refer_chan_adc[step].value_ain)/1000;
}; 



void math_input_voltANx(uint32_t val, uint8_t step){
if (refer_chan_adc[step].aver_array[0] > 12000)

refer_chan_adc[step].value_ain = gistrez(step,7) *intUpower;
else

refer_chan_adc[step].value_ain = gistrez(step,3) *intUpower;

refer_chan_adc[step].value_ain = refer_chan_adc[step].last *intUpower;
refer_chan_adc[step].value_ain >>=19;
refer_chan_adc[step].f_value_ain= (float)(refer_chan_adc[step].value_ain)/(1000);
}; 

void math_resistW(uint32_t val, uint8_t step){
  

uint8_t index = get_index(num_AIN1)+(step-get_index(ana_resist1W1));

uint32_t uUizm = refer_chan_adc[index].value_ain;
refer_chan_adc[step].flag_new_value = refer_chan_adc[index].flag_new_value;
int32_t iResult = 0;
if (uUizm >0)
{

if (uUizm < 1900)  
iResult = intUpower  -150;
else
iResult = intUpower  -100;
iResult =iResult -uUizm;
iResult = (int32_t)(uUizm*3300)/iResult;
if (iResult >=0) {
 refer_chan_adc[step].value_ain= iResult;
}
else
 refer_chan_adc[step].value_ain=  100000;
if (iResult > 100000)
  refer_chan_adc[step].value_ain=  100000;

refer_chan_adc[step].f_value_ain=(float) refer_chan_adc[step].value_ain/1000;
}
else 
{
refer_chan_adc[step].f_value_ain =  0;
}

}

void math_VREF(uint32_t val, uint8_t step){

intUpower = refer_chan_adc[step].value_ain = 2515968000/(gistrez(step,8) <<2);


}




void buff2value(uint32_t step) {
  uint32_t i;

 uint32_t start,stop;
 uint32_t position=0; 
 if (step==0)
 {start=0;
  stop=sizeof(buff_adc)/sizeof(buff_adc[0])/2;

 } else
 {
   //start=sizeof(buff_adc)/sizeof(buff_adc[0])/2;
   start=0;
   stop=sizeof(buff_adc)/sizeof(buff_adc[0]);
//   position = max_average_point_2_chanel/2;
   position=0;
   

 }

p_u32_u16 pdata;
 while (start<stop) 
 {
 for (i=0;i<sizeof(refer_chan_adc)/sizeof(refer_chan_adc[0]);i++) {
  //    refer_chan_adc[i].cnt_value = 0;

      refer_chan_adc[i].aver_array[position]=0;
   //    refer_chan_adc[i].idisper=0;
  }
   i=0;
 uint8_t index=0xff;

 
 index =  get_index(list[i]);

 
   while ((start<stop)&&(i<sizeof(list)/sizeof(list[0]))) 
   {
     
   pdata = (p_u32_u16)&buff_adc[start]; 
   if (i>0) {
     if (list[i-1]!=list[i])
        index=  get_index(list[i]);

   }
    if (index!=0xff)
    {

int32_t temp_val=pdata->hi;
      refer_chan_adc[index].aver_array[position] += pdata->hi;
      refer_chan_adc[index].cnt_value++;   
    }


     i++;
     start++;
   }
  for (i=0;i<sizeof(refer_chan_adc)/sizeof(refer_chan_adc[0]);i++) 
  if (refer_chan_adc[i].cnt_value != 0)
  { 
  
    refer_chan_adc[i].aver_array[position] =  
     ( refer_chan_adc[i].aver_array[position] << 7)  /refer_chan_adc[i].cnt_value; 
    refer_chan_adc[i].cnt_value=0;
  }
    
   position++;
 }
 if (step==1)
  for (i=0;i<sizeof(refer_chan_adc)/sizeof(refer_chan_adc[0]);i++) 
      if (refer_chan_adc[i].func_math!=0) 
      {
        refer_chan_adc[i].func_math(0,i);
      }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)  {
   if (hadc==&hadc1) {
 buff2value(0);

  } 
};

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  if (hadc==&hadc1) {
 buff2value(1);

  }
};

GPIO_TypeDef * numtoport(uint8_t chan) {
  switch (chan) {
  case 0: return GPIOA;
  case 1: return GPIOA;
  case 2: return GPIOA;
  case 3: return GPIOA;
  case 4: return GPIOA;
  case 5: return GPIOA;
  case 6: return GPIOA;
  case 7: return GPIOA;
  case 8: return GPIOB;
  case 9: return GPIOB;
  }
  return NULL;
}
uint16_t     numtopin(uint8_t chan) {
  switch (chan) {
  case 0: return GPIO_PIN_0;
  case 1: return GPIO_PIN_1;
  case 2: return GPIO_PIN_2;
  case 3: return GPIO_PIN_3;
  case 4: return GPIO_PIN_4;
  case 5: return GPIO_PIN_5;
  case 6: return GPIO_PIN_6;
  case 7: return GPIO_PIN_7;
  case 8: return GPIO_PIN_0;
  case 9: return GPIO_PIN_1;
  }
  return 0;
}
uint32_t numtoadcchan(uint8_t chan) {
  switch (chan) {
  case 0: return ADC_CHANNEL_0;
  case 1: return ADC_CHANNEL_1;
  case 2: return ADC_CHANNEL_2;
  case 3: return ADC_CHANNEL_3;
  case 4: return ADC_CHANNEL_4;
  case 5: return ADC_CHANNEL_5;
  case 6: return ADC_CHANNEL_6;
  case 7: return ADC_CHANNEL_7;
  case 8: return ADC_CHANNEL_8;
  case 9: return ADC_CHANNEL_9;
  case 16: return ADC_CHANNEL_16;
  case 17: return ADC_CHANNEL_17;
  }
  return ADC_CHANNEL_16;
}
void RXXanalog_init(uint16_t val_sec) {
 
  
  
  
  
  MX_ADC1_Init();
   MX_TIM6_Init();
 
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

  HAL_ADC_Start_DMA(&hadc1,(uint32_t *)&buff_adc[0],sizeof(list)/sizeof(list[0])*max_size_adc_per_chanel*max_average_point_2_chanel);
 // HAL_ADCEx_MultiModeStart_DMA(&hadc1,(uint32_t *)&buff_adc[0],sizeof(list)/sizeof(list[0])*max_size_adc_per_chanel*max_average_point_2_chanel);
   __HAL_DMA_DISABLE_IT(&hdma_adc1, DMA_IT_HT );

 HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID , &HTIM_Display_Refresh); 
  HAL_TIM_Base_Start_IT(&htim6);
    __HAL_DBGMCU_FREEZE_TIM6(); 

}




static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 9999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}





static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
   hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 8;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T6_TRGO;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}










uint32_t GetNewAin(universal_value_t *value) {
  uint32_t i;
  for (i = ana_resist1W1; i <= ana_resist1W4 ; i++) {
    if (get_index(i) != 0xff) 
      if  (refer_chan_adc[get_index(i)].flag_new_value != 0) {
      value->m_f32Value = refer_chan_adc[get_index(i)].f_value_ain;
      refer_chan_adc[get_index(i)].flag_new_value = 0;
      refer_chan_adc[get_index(num_AIN1)+(get_index(i)-get_index(ana_resist1W1))].flag_new_value=0;
      return i;
    }
  }
  return 0;
}
bool GetAIN(universal_value_t *value, u32 Param) {
  if (get_index(Param) != 0xff) {
    value->m_f32Value = refer_chan_adc[get_index(Param)].f_value_ain;
    return 1;
  }
  
  return 0;
}; //вернет в value значение (f32) канала с номеров в Param. Если return ==0, то устройства  нет
bool SetAIN(universal_value_t *value, u32 Param){
    if (get_index(Param) != 0xff) {
    refer_chan_adc[get_index(Param)].f_value_ain =  value->m_f32Value;
    return 1;
  }
  return 0;
};

