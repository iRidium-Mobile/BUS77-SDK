#ifndef __RXX_ANALOG_H
#define __RXX_ANALOG_H

#ifdef __cplusplus
extern "C" {
#endif 
  
#define max_adc_chanel          1
#define max_size_adc_per_chanel 1
#define max_average_point_2_chanel 1  

///конфигурация аналоговых входов.
#define   num_temp              16
#define   num_vref              17          
#define   num_volt              ADC_CHANNEL_8

#define  num_AIN1               0
#define  num_AIN2               2 
#define  num_AIN3               6
#define  num_AIN4               8 
  

  
  enum LinkChanAna
  {    
    ana_resist1W1=21,
    ana_resist1W2,
    ana_resist1W3,
    ana_resist1W4,
    ana_resist1,
    ana_resist4

  };
  typedef struct t_u32_u16  {
    uint16_t    hi;
//    uint16_t    low;
  }t_u32_u16, *  p_u32_u16 ;
    typedef struct t_max_min {
    float       min;
    float       max;
  }t_max_min;
  typedef struct  t_refer_chan_adc {
    uint8_t       conf_chan;
    int32_t      corrector;     
    void          (* func_math)(uint32_t, uint8_t); 

    uint8_t        flag_new_value;
    uint32_t       last;
    uint32_t      cnt_value;       
    uint32_t      value_ain;     
    float        f_value_ain;
    t_max_min     max_min;     
//    float       f_aver;
    uint32_t      aver_array[max_average_point_2_chanel];
  }t_refer_chan_adc;
 /* typedef struct t_refer_adc {
    uint8_t       cnt_ain;
    uint8_t       list_ain[12];

    int32_t      value_ain[max_adc_chanel];
    float        f_value_ain[max_adc_chanel+2];
  }t_refer_adc; */

  extern void RXXanalog_init(uint16_t val_sec);
  extern  t_refer_chan_adc * GetChanData(uint8_t chan);
  extern uint8_t GetNewValueAIN(uint8_t * tag, uint32_t * value);

//  extern t_refer_adc refer_adc;
  
  
  
  
  
  
  
#ifdef __cplusplus
}
#endif   
#endif