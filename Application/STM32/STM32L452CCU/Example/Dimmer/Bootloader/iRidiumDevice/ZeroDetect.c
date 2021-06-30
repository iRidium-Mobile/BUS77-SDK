#include "stm32l4xx_hal.h"
#include "ZeroDetect.h"
#include "stm32l4xx_hal_tim.h"

LPTIM_HandleTypeDef hlptim1;
LPTIM_HandleTypeDef hlptim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

static void MX_LPTIM1_Init(void);
static void MX_LPTIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM15_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);



const KEY_refer_dataset key_refer[size_key] = {
  {&htim2,TIM_CHANNEL_1,0},
  {&htim2,TIM_CHANNEL_2,0},
  {&htim1,TIM_CHANNEL_1,0},
  {&htim1,TIM_CHANNEL_2,1},
  {&htim1,TIM_CHANNEL_3,1},
  {&htim2,TIM_CHANNEL_3,1},
};

const TIM_IC_InitTypeDef  ch1Direct = {TIM_INPUTCHANNELPOLARITY_RISING,TIM_ICSELECTION_DIRECTTI, TIM_ICPSC_DIV1, 0}; 
const TIM_IC_InitTypeDef  ch1inDirect = {TIM_INPUTCHANNELPOLARITY_FALLING,TIM_ICSELECTION_INDIRECTTI, TIM_ICPSC_DIV1, 0}; 

const TIM_IC_InitTypeDef  ch2Direct = {TIM_INPUTCHANNELPOLARITY_RISING,TIM_ICSELECTION_DIRECTTI, TIM_ICPSC_DIV1, 0}; 
const TIM_IC_InitTypeDef  ch2inDirect = {TIM_INPUTCHANNELPOLARITY_FALLING,TIM_ICSELECTION_INDIRECTTI, TIM_ICPSC_DIV1, 0}; 

#define         default_freq    80000000/(global_prescaler+1)/100
ZD_dataset      zd_data = {{{{default_freq,default_freq,default_freq,default_freq,default_freq,default_freq,default_freq,default_freq,default_freq,default_freq},10000,0,&ch1inDirect,&ch2Direct,TIM_TS_TI2FP2,TIM_CHANNEL_2,HAL_TIM_ACTIVE_CHANNEL_2,0,0,&htim2},
                            {{default_freq,default_freq,default_freq,default_freq,default_freq,default_freq,default_freq,default_freq,default_freq,default_freq},10000,0,&ch1Direct,&ch2inDirect,TIM_TS_TI1FP1,TIM_CHANNEL_1,HAL_TIM_ACTIVE_CHANNEL_1,0,0,&htim1}},
                            0};

const uint32_t Alarm_Pin[pwm_chan]={GPIO_PIN_4,GPIO_PIN_13, GPIO_PIN_14,GPIO_PIN_15,GPIO_PIN_7,GPIO_PIN_5};
uint32_t        Alarm_status[pwm_chan] ={0};
TEMP_dataset   temp_data;

uint32_t  div_presc =0;


void ZeroDetect_Interrupt (TIM_HandleTypeDef *htim);
void ZeroDetect_Interrupt_Empty( TIM_HandleTypeDef *htim);
void HTIM_Reload( TIM_HandleTypeDef *htim);
/*
unsigned abs(int value) { 
  unsigned result = (value < 0) ? (unsigned)-value : (unsigned)value;
  return  result;
}
*/


static void TIM_TI1_ConfigInputStage(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICFilter)
{
  uint32_t tmpccmr1;
  uint32_t tmpccer;

  /* Disable the Channel 1: Reset the CC1E Bit */
  tmpccer = TIMx->CCER;
  TIMx->CCER &= ~TIM_CCER_CC1E;
  tmpccmr1 = TIMx->CCMR1;

  /* Set the filter */
  tmpccmr1 &= ~TIM_CCMR1_IC1F;
  tmpccmr1 |= (TIM_ICFilter << 4U);

  /* Select the Polarity and set the CC1E Bit */
  tmpccer &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
  tmpccer |= TIM_ICPolarity;

  /* Write to TIMx CCMR1 and CCER registers */
  TIMx->CCMR1 = tmpccmr1;
  TIMx->CCER = tmpccer;
}


static void TIM_TI2_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection,
                              uint32_t TIM_ICFilter)
{
  uint32_t tmpccmr1;
  uint32_t tmpccer;


  
  tmpccmr1 = TIMx->CCMR1;
  tmpccer = TIMx->CCER;


  /* Select the Polarity and set the CC2E Bit */
  tmpccer &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
  tmpccer |= ((TIM_ICPolarity << 4U) & (TIM_CCER_CC2P | TIM_CCER_CC2NP));

  /* Write to TIMx CCMR1 and CCER registers */
  TIMx->CCMR1 = tmpccmr1 ;
  TIMx->CCER = tmpccer;
}


static void TIM_TI2_ConfigInputStage(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICFilter)
{
  uint32_t tmpccmr1;
  uint32_t tmpccer;


  tmpccmr1 = TIMx->CCMR1;
  tmpccer = TIMx->CCER;

  /* Select the Polarity and set the CC2E Bit */
  tmpccer &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
  tmpccer |= (TIM_ICPolarity << 4U);

  /* Write to TIMx CCMR1 and CCER registers */
  TIMx->CCMR1 = tmpccmr1 ;
  TIMx->CCER = tmpccer;
}

static void TIM_TI3_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection,
                              uint32_t TIM_ICFilter)
{
  uint32_t tmpccmr2;
  uint32_t tmpccer;


  tmpccmr2 = TIMx->CCMR2;
  tmpccer = TIMx->CCER;

  /* Select the Polarity and set the CC3E Bit */
  tmpccer &= ~(TIM_CCER_CC3P | TIM_CCER_CC3NP);
  tmpccer |= ((TIM_ICPolarity << 8U) & (TIM_CCER_CC3P | TIM_CCER_CC3NP));

  /* Write to TIMx CCMR2 and CCER registers */
  TIMx->CCMR2 = tmpccmr2;
  TIMx->CCER = tmpccer;
}


static void TIM_TI4_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection,
                              uint32_t TIM_ICFilter)
{
  uint32_t tmpccmr2;
  uint32_t tmpccer;


  tmpccmr2 = TIMx->CCMR2;
  tmpccer = TIMx->CCER;

  /* Select the Polarity and set the CC4E Bit */
  tmpccer &= ~(TIM_CCER_CC4P | TIM_CCER_CC4NP);
  tmpccer |= ((TIM_ICPolarity << 12U) & (TIM_CCER_CC4P | TIM_CCER_CC4NP));

  /* Write to TIMx CCMR2 and CCER registers */
  TIMx->CCMR2 = tmpccmr2;
  TIMx->CCER = tmpccer ;
}

uint32_t add_mod(int32_t s1, int32_t s2, uint32_t mod) {
  int32_t temp = s1+s2;
  if (temp<0)
    return mod-1;
  if (temp >= mod)
    return 0;
  return temp;
}

uint32_t Temp_calc_average(int32_t * Data, uint32_t quant) {
  uint32_t q=0;
  uint32_t result = 0;
  for (uint32_t i=0; i < quant; i++) {
    if (* Data > 0) {
    result +=* Data;
    q++;
    }
    Data++;   
  }
  return result/q;
}
uint32_t Temp_calc_delta(int32_t * Data, uint32_t quant, uint32_t aver) {
    uint32_t result = 0;
    for (uint32_t i=0; i < quant; i++) {
        if (* Data > 0) {
          if (abs(aver - * Data)>result) 
          result = abs(aver - * Data);
        }
        Data++;
    }
    return result;
}

uint32_t Temp_add_value(pTEMP_channel_dataset chan_data, uint32_t new_value) {
     chan_data->array[chan_data->index]=new_value; //;
    
    chan_data->length_array[chan_data->index] =(int32_t) chan_data->array[chan_data->index] -
      (int32_t) chan_data->array[add_mod(chan_data->index, -1 , quant_temp)];
    if (chan_data->mode == 0) {
      if (++(chan_data->index) >= quant_temp)
      {
        chan_data->aver = Temp_calc_average(&chan_data->length_array[0], quant_temp);
        chan_data->delta = Temp_calc_delta(&chan_data->length_array[0], quant_temp, chan_data->aver);
        chan_data->index = 0;
        chan_data->mode =1;
        chan_data->last_position = chan_data->array[quant_temp-1];
        return chan_data->mode; //TIM_TI3_SetConfig(htim3.Instance, TIM_INPUTCHANNELPOLARITY_BOTHEDGE,TIM_ICSELECTION_DIRECTTI,10); 
      }
    }
    else {
     if (++(chan_data->index) >= quant_temp)
      {
        if (chan_data->array[0]> chan_data->last_position) {//проверка на переполнение счетчика
          if (((chan_data->last_position - chan_data->array[0])<(chan_data->aver+chan_data->delta)) &
              ((chan_data->last_position - chan_data->array[0])>(chan_data->aver-chan_data->delta))) { // следующая разница =PWM
                if (chan_data->length_array[1]>0)
                chan_data->pwm = chan_data->length_array[1]*100/chan_data->aver;
                else
                 if (chan_data->length_array[3]>0) 
                 chan_data->pwm = chan_data->length_array[3]*100/chan_data->aver; 
              }
          else
          { //текущая разница =PWM
            if (chan_data->length_array[2]>0)
            chan_data->pwm = chan_data->length_array[2]*100/chan_data->aver; 
            else
            if (chan_data->length_array[4]>0)  
            chan_data->pwm = chan_data->length_array[4]*100/chan_data->aver;  
          }
        }
        chan_data->index = 0;
        chan_data->mode =0;
        return chan_data->mode;
       // TIM_TI3_SetConfig(htim3.Instance, TIM_INPUTCHANNELPOLARITY_FALLING,TIM_ICSELECTION_DIRECTTI,10); 
      }
    }
 return chan_data->mode;
};

void ZeroDetect_Init( void) {
  
 for (uint32_t i =0; i<size_chan; i++)
   zd_data.chan[i].MINIMAL =  HAL_RCC_GetSysClockFreq()/(global_prescaler+1)/100;
 

      MX_TIM3_Init(); 
   HAL_TIM_RegisterCallback(&htim3, HAL_TIM_IC_CAPTURE_CB_ID , &ZeroDetect_Interrupt);
   HAL_TIM_RegisterCallback(&htim3, HAL_TIM_PERIOD_ELAPSED_CB_ID , &ZeroDetect_Interrupt_Empty);   
   HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
   HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);  
   HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_4);  

      MX_TIM15_Init(); 
   HAL_TIM_RegisterCallback(&htim15, HAL_TIM_IC_CAPTURE_CB_ID , &ZeroDetect_Interrupt);  
   HAL_TIM_IC_Start_IT(&htim15, TIM_CHANNEL_1);
   HAL_TIM_IC_Start_IT(&htim15, TIM_CHANNEL_2);  

      MX_TIM2_Init();
   HAL_TIM_RegisterCallback(&htim2, HAL_TIM_PERIOD_ELAPSED_CB_ID , &HTIM_Reload);   
   HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
   HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_3);   
   HAL_TIM_Base_Start_IT(&htim2);
   
      MX_TIM1_Init();
   HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_2);
   HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_3);
   HAL_TIM_RegisterCallback(&htim1, HAL_TIM_PERIOD_ELAPSED_CB_ID , &HTIM_Reload);   
   HAL_TIM_RegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID , &ZeroDetect_Interrupt);  
   HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_4);    
  HAL_TIM_Base_Start_IT(&htim1);
    
 HAL_TIM_Base_Start_IT(&htim3);
 HAL_TIM_Base_Start_IT(&htim15);

  MX_LPTIM1_Init();
  MX_LPTIM2_Init();  
  HAL_LPTIM_OnePulse_Start_IT(&hlptim1, 0x1100, 0x1000);
  HAL_LPTIM_OnePulse_Start_IT(&hlptim2, 0x1000, 0x1000);
}

void ZeroDetect_SetDelay(uint32_t channel, uint32_t delay) {
  LPTIM_HandleTypeDef * LPTIM;
  if (channel == 1)
        LPTIM = &hlptim1;
  else
        LPTIM = &hlptim2;
    HAL_LPTIM_SetOnce_Stop_IT(LPTIM);
    HAL_LPTIM_SetOnce_Start_IT(LPTIM, 0xFFFF, delay);
};

void ZeroDetect_CalcAverageFreq(pZD_channel_dataset data, uint32_t new_value) { 
  if (data->count >= size_sample_100hz)
    data->count = 0;
  if (new_value < data->MINIMAL)
    if (new_value +20 > data->MINIMAL)
      data->MINIMAL = new_value;
  
  data->array_sample[data->count] = new_value;
  uint32_t temp=0;                   
  for (uint32_t i = 0; i<size_sample_100hz; i++)
    temp +=data->array_sample[i];
  data->freq = temp /   size_sample_100hz / div_presc;    
  data->count++;
  data->flag_ZD_present = 0;
  
 
}

void ZeroDetect_SetActiveChan(pZD_channel_dataset data) {
  
  if (HAL_TIM_IC_ConfigChannel(&htim3, (TIM_IC_InitTypeDef *) data->chan1, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, (TIM_IC_InitTypeDef *) data->chan2, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = data->InputTrigger;
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
    TIM_CCxChannelCmd(htim3.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(htim3.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
    zd_data.empty_sample = 0;
}

void ZeroDetect_Interrupt (TIM_HandleTypeDef *htim) {
  
  if (htim == &htim3)
  if (htim->Channel == zd_data.chan[zd_data.current_chan].interrupt_nameofchan) {


  if (zd_data.empty_sample == 0) {
    zd_data.empty_sample = 1;
    return;
  }
  
  ZeroDetect_CalcAverageFreq(&zd_data.chan[zd_data.current_chan] , HAL_TIM_ReadCapturedValue(&htim3, zd_data.chan[zd_data.current_chan].nameofchan)+1);
 
  if (zd_data.chan[zd_data.current_chan].count >= size_sample_100hz) { 
    if (++zd_data.current_chan >= size_chan)
      zd_data.current_chan = 0;   
   ZeroDetect_SetActiveChan(&zd_data.chan[zd_data.current_chan]); 
  }
  }
  
  if (htim == &htim3)
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
   if (Temp_add_value(&temp_data.chan[4],HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_3)) == 0)
     TIM_TI3_SetConfig(htim3.Instance, TIM_INPUTCHANNELPOLARITY_FALLING,TIM_ICSELECTION_DIRECTTI,10); 
   else
     TIM_TI3_SetConfig(htim3.Instance, TIM_INPUTCHANNELPOLARITY_BOTHEDGE,TIM_ICSELECTION_DIRECTTI,10);  

  }
  
  if (htim == &htim3)
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
   if ( Temp_add_value(&temp_data.chan[5],HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_4))== 0)
     TIM_TI4_SetConfig(htim3.Instance, TIM_INPUTCHANNELPOLARITY_FALLING,TIM_ICSELECTION_DIRECTTI,10); 
   else
     TIM_TI4_SetConfig(htim3.Instance, TIM_INPUTCHANNELPOLARITY_BOTHEDGE,TIM_ICSELECTION_DIRECTTI,10);  
  }  

  if (htim == &htim15)
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
   if ( Temp_add_value(&temp_data.chan[1],HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1))== 0)
     TIM_TI1_SetConfig(htim->Instance, TIM_INPUTCHANNELPOLARITY_FALLING,TIM_ICSELECTION_DIRECTTI,10); 
   else
     TIM_TI1_SetConfig(htim->Instance, TIM_INPUTCHANNELPOLARITY_BOTHEDGE,TIM_ICSELECTION_DIRECTTI,10);  
  } 
  if (htim == &htim15)
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
   if ( Temp_add_value(&temp_data.chan[3],HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2))== 0)
     TIM_TI2_SetConfig(htim->Instance, TIM_INPUTCHANNELPOLARITY_FALLING,TIM_ICSELECTION_DIRECTTI,10); 
   else
     TIM_TI2_SetConfig(htim->Instance, TIM_INPUTCHANNELPOLARITY_BOTHEDGE,TIM_ICSELECTION_DIRECTTI,10);  
  } 
  if (htim == &htim1)
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
   if ( Temp_add_value(&temp_data.chan[0],HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_4))== 0)
     TIM_TI4_SetConfig(htim->Instance, TIM_INPUTCHANNELPOLARITY_FALLING,TIM_ICSELECTION_DIRECTTI,10); 
   else
     TIM_TI4_SetConfig(htim->Instance, TIM_INPUTCHANNELPOLARITY_BOTHEDGE,TIM_ICSELECTION_DIRECTTI,10);  
  } 
  
}

void HTIM_Reload( TIM_HandleTypeDef *htim) {

for (uint32_t i=0; i<size_chan; i++) {
  if (htim == zd_data.chan[i].TIM) 
    if (htim->Init.CounterMode == TIM_COUNTERMODE_CENTERALIGNED3)
  htim->Instance->ARR = zd_data.chan[i].MINIMAL/2;
    else
  htim->Instance->ARR = 0xFFFF;      
    break;
  }
    
}
void ZeroDetect_Interrupt_Empty( TIM_HandleTypeDef *htim) {
  
  if (++zd_data.chan[zd_data.current_chan].flag_ZD_present>3) {
    zd_data.chan[zd_data.current_chan].flag_ZD_present = 3;
        zd_data.chan[zd_data.current_chan].flag_ZD_present = 0;

      zd_data.chan[zd_data.current_chan].freq = 0;
    if (++zd_data.current_chan >= size_chan)
      zd_data.current_chan = 0;   
   ZeroDetect_SetActiveChan(&zd_data.chan[zd_data.current_chan]); 

  
  }
  if ((zd_data.chan[zd_data.current_chan].freq >11000) || (zd_data.chan[zd_data.current_chan].freq < 9000))
    if (zd_data.current_chan == 0) {
      set_slow_blink(0);
      set_slow_blink(1);
      set_slow_blink(2);
    }
  else
  {
      set_slow_blink(3);
      set_slow_blink(4);
      set_slow_blink(5);    
  }   
  
};

void PWM_Channel_Set(uint16_t percent_value, uint8_t chan) {
  if (chan >=size_key)
    return;
  if (percent_value >100)
    return;  
  uint32_t value;
  if (key_refer[chan].tim_KEY->Init.CounterMode == TIM_COUNTERMODE_CENTERALIGNED3) {
    value = zd_data.chan[key_refer[chan].group].MINIMAL*percent_value/200;
  }
  else {
    value = zd_data.chan[key_refer[chan].group].MINIMAL*percent_value/100;
  } 
  switch (key_refer[chan].chan_KEY) {
  case TIM_CHANNEL_1:
    key_refer[chan].tim_KEY->Instance->CCR1 = value;
    break;
  case TIM_CHANNEL_2:
    key_refer[chan].tim_KEY->Instance->CCR2 = value;
    break;
  case TIM_CHANNEL_3:
    key_refer[chan].tim_KEY->Instance->CCR3 = value;
    break;    
  }
  
  
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  for (uint32_t i=0; i<pwm_chan; i++)
    if (GPIO_Pin == Alarm_Pin[i]) {
      Alarm_status[i] = 1;
      set_series_blink(i);
      return;
    }
}


static void MX_LPTIM1_Init(void)
{

  /* USER CODE BEGIN LPTIM1_Init 0 */

  /* USER CODE END LPTIM1_Init 0 */

  /* USER CODE BEGIN LPTIM1_Init 1 */

  /* USER CODE END LPTIM1_Init 1 */
  hlptim1.Instance = LPTIM1;
  hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
  hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV128;
  hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_0;
  hlptim1.Init.Trigger.ActiveEdge = LPTIM_ACTIVEEDGE_RISING;
  hlptim1.Init.Trigger.SampleTime = LPTIM_TRIGSAMPLETIME_8TRANSITIONS;
  hlptim1.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_LOW;
  hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
  hlptim1.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
  hlptim1.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
  if (HAL_LPTIM_Init(&hlptim1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPTIM1_Init 2 */

  /* USER CODE END LPTIM1_Init 2 */

}

/**
  * @brief LPTIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPTIM2_Init(void)
{

  /* USER CODE BEGIN LPTIM2_Init 0 */

  /* USER CODE END LPTIM2_Init 0 */

  /* USER CODE BEGIN LPTIM2_Init 1 */

  /* USER CODE END LPTIM2_Init 1 */
  hlptim2.Instance = LPTIM2;
  hlptim2.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
  hlptim2.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV128;
  hlptim2.Init.Trigger.Source = LPTIM_TRIGSOURCE_0;
  hlptim2.Init.Trigger.ActiveEdge = LPTIM_ACTIVEEDGE_RISING;
  hlptim2.Init.Trigger.SampleTime = LPTIM_TRIGSAMPLETIME_8TRANSITIONS;
  hlptim2.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_LOW;
  hlptim2.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim2.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
  hlptim2.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
  hlptim2.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
  if (HAL_LPTIM_Init(&hlptim2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPTIM2_Init 2 */

  /* USER CODE END LPTIM2_Init 2 */

}
 
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = global_prescaler;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_ETRF;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
  sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim1, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 5200;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 10;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = global_prescaler;
  htim2.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED3;
  htim2.Init.Period = 0xFFFF;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_ETRF;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
  sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim2, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}


static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = global_prescaler;
  div_presc = HAL_RCC_GetSysClockFreq()/(global_prescaler+1)/100/10000;
 /*
  if (htim3.Init.Prescaler == 19) div_presc=4;
  if (htim3.Init.Prescaler == 39) div_presc=2;
  if (htim3.Init.Prescaler == 79) div_presc=1;
 */ 
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  ZeroDetect_SetActiveChan(&zd_data.chan[0]); 
  /* USER CODE BEGIN TIM3_Init 2 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};  
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* USER CODE END TIM3_Init 2 */

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 10;
    

  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }  
}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = global_prescaler;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 65535;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim15, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim15, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */

}
