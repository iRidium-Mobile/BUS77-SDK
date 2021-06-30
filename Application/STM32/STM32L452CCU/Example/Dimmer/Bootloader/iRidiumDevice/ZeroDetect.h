#ifndef _ZERO_DETECT_H
#define _ZERO_DETECT_H



#ifdef __cplusplus
extern "C" {
#endif 
  
#define         pwm_chan                6  
#define         size_chan               2  
#define         size_sample_100hz       10        
#define         global_prescaler        39  
  
  
  typedef struct ZD_channel_dataset{
  uint32_t      array_sample[size_sample_100hz];  //array for series sample freq from ZD
  uint32_t      freq;  //average freq.
  uint32_t      count; //current sample
  const TIM_IC_InitTypeDef      *      chan1;
  const TIM_IC_InitTypeDef      *      chan2;
  uint32_t      InputTrigger;
  uint32_t      nameofchan;
  uint32_t      interrupt_nameofchan;
  uint32_t      flag_ZD_present;
  uint32_t      MINIMAL;
  TIM_HandleTypeDef * TIM;
  } ZD_channel_dataset, * pZD_channel_dataset;
  
  typedef struct ZD_dataset{
  ZD_channel_dataset    chan[size_chan];
  uint32_t              current_chan;  
  uint32_t              empty_sample;
  } ZD_dataset, * pZD_dataset;
  
  
  
  
#define         size_key                6
  
  

  
  typedef struct KEY_refer_dataset {
    TIM_HandleTypeDef   *       tim_KEY;
    uint32_t                    chan_KEY;
    uint32_t                    group;
  }KEY_refer_dataset;
    
     
  
  
  void ZeroDetect_Init( void);
  void PWM_Channel_Set(uint16_t percent_value, uint8_t chan);
  
  
 #define         quant_temp      10 
  
  
typedef struct TEMP_channel_dataset {
uint32_t  pwm;
uint32_t  last_position;
uint32_t  mode;       
uint32_t  delta;
uint32_t  aver;
int32_t   length_array[quant_temp];
uint32_t  array[quant_temp];
uint32_t  index;
 }TEMP_channel_dataset, * pTEMP_channel_dataset;
  
  
typedef struct TEMP_dataset {
TEMP_channel_dataset chan[pwm_chan];
}TEMP_dataset;
  
  
  #ifdef __cplusplus
}
#endif   
#endif