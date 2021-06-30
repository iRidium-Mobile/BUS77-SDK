#ifndef __LINK_CHAN_H
#define __LINK_CHAN_H

#ifdef __cplusplus
extern "C" {
#endif 
  
  
#define R0
//#define R5  
//#define R10  

#define onwire_val_1w1
#define onwire_val_1w2   
//#define onwire_save   
  
//#define short_but
#define long_but
#define trigup_but
//#define trigdonw_but
#define countup_but
//#define countdown_but
//#define countupdown_but
  
  
#define short_1w1
#define long_1w1
#define trigup_1w1
//#define trigdonw_1w1
#define countup_1w1
//#define countdown_1w1
//#define countupdown_1w1
  
   
#define short_1w2
#define long_1w2
#define trigup_1w2
//#define trigdonw_1w2
#define countup_1w2
//#define countdown_1w2
/*#define countupdown_1w2
  
*/
  
#define short_an1
#define long_an1
//#define trigup_an1
//#define trigdonw_an1
#define countup_an1
//#define countdown_an1
//#define countupdown_an1
  
#define short_an2
//#define long_an2
//#define trigup_an2
//#define trigdonw_an2
#define countup_an2
//#define countdown_an2
//#define countupdown_an2
  
  enum LinkChanTag
  {
    tg_lnk_VOLT = 1,
    tg_lnk_TEMP,
    tg_lnk_AU,
    tg_lnk_AI,
    tg_lnk_R,
     
#ifdef short_1w1     
    tg_lnk_1W1S,
#endif
#ifdef  long_1w1   
    tg_lnk_1W1L,
#endif
#ifdef  trigup_1w1    
    tg_lnk_1W1TR,
#endif
#ifdef trigdonw_1w1 
    tg_lnk_1W1TF,
#endif
#ifdef  countup_1w1
    tg_lnk_1W1CR,
#endif
#ifdef countdown_1w1 
    tg_lnk_1W1CL,
#endif
#ifdef countupdown_1w1 
    tg_lnk_1W1CRL,
#endif
    
#ifdef  short_1w2    
    tg_lnk_1W2S,
#endif
#ifdef long_1w2 
    tg_lnk_1W2L,
#endif
#ifdef trigup_1w2 
    tg_lnk_1W2TR,
#endif
#ifdef trigdonw_1w2 
    tg_lnk_1W2TF,
#endif
#ifdef countup_1w2 
    tg_lnk_1W2CR,
#endif
#ifdef countdown_1w2 
    tg_lnk_1W2CL,
#endif
#ifdef countupdown_1w2 
    tg_lnk_1W2CRL,
#endif
    
#ifdef  short_an1    
    tg_lnk_AN1S,
#endif
#ifdef  long_an1 
    tg_lnk_AN1L,
#endif
#ifdef  trigup_an1
    tg_lnk_AN1TR,
#endif
#ifdef  trigdonw_an1 
    tg_lnk_AN1TF,
#endif
#ifdef countup_an1 
    tg_lnk_AN1CR,
#endif
#ifdef countdown_an1 
    tg_lnk_AN1CL,
#endif
#ifdef countupdown_an1 
    tg_lnk_AN1CRL,
#endif
    
#ifdef  short_an2 
    tg_lnk_AN2S,
#endif
#ifdef long_an2 
    tg_lnk_AN2L,
#endif
#ifdef trigup_an2 
    tg_lnk_AN2TR,
#endif
#ifdef  trigdonw_an2 
    tg_lnk_AN2TF,
#endif
#ifdef countup_an2 
    tg_lnk_AN2CR,
#endif
#ifdef countdown_an2 
    tg_lnk_AN2CL,
#endif
#ifdef  countupdown_an2
    tg_lnk_AN2CRL,
#endif
    
#ifdef   short_but   
    tg_lnk_BUTS,
#endif
#ifdef   long_but
    tg_lnk_BUTL,
#endif
#ifdef  trigup_but
    tg_lnk_BUTTR,
#endif
#ifdef  trigdonw_but 
    tg_lnk_BUTTF,
#endif
#ifdef countup_but 
    tg_lnk_BUTCR,
#endif
#ifdef  countdown_but 
    tg_lnk_BUTCL,
#endif
#ifdef  countupdown_but
    tg_lnk_BUTCRL,
#endif
    
    tg_lnk_LED,
    
    
#ifdef  R5    
    tg_lnk_RELAY1,
    tg_lnk_RELAY2,
#endif
#ifdef  R10
    tg_lnk_RELAY1,   
#endif
    tg_lnk_EN5V,
    tg_lnk_24V,
    tg_lnk_ENPUP1W3,
    
#ifdef  onwire_val_1w1  
    tg_lnk_1w1_1_id,
    tg_lnk_1w1_1_value,
    tg_lnk_1w1_2_id,
    tg_lnk_1w1_2_value,
    tg_lnk_1w1_3_id,
    tg_lnk_1w1_3_value,
    tg_lnk_1w1_4_id, 
    tg_lnk_1w1_4_value,
#endif
#ifdef  onwire_val_1w2     
    tg_lnk_1w2_1_id,
    tg_lnk_1w2_1_value,
    tg_lnk_1w2_2_id,
    tg_lnk_1w2_2_value,
    tg_lnk_1w2_3_id,
    tg_lnk_1w2_3_value,
    tg_lnk_1w2_4_id,
    tg_lnk_1w2_4_value,
#endif
    
    maxEnumLinkChanTag,
  };
#define n_press                 1
#define n_cnt_rise              2
#define n_cnt_fall              4
#define n_cnt_rise_fall         8
#define n_trig_fall             16
#define n_trig_rise             32
#define n_long_press            64
  
  enum eDeviceChannel
  {
    DC_NAME = 1,
#if defined(long_but)||defined(long_1w1)||defined(long_1w2)||defined(long_an1)||defined(long_an2)    
    DC_MODE_1,
#endif    
    DC_LED,
#ifdef   onwire_save
    DC_FIND,
#endif
#ifdef  R5  
    DC_RELAY1,
    DC_RELAY2,
#endif
#ifdef R10
    DC_RELAY1,
#endif
    DC_EN5V,
    DC_EN24V,
    DC_ENPUP1W3,
    DC_Enum_Max,
  };
  enum eDeviceChannel_reserve
  {  start_ = DC_Enum_Max+1,
#ifdef R0
    DC_RELAY1,
    DC_RELAY2,
#endif
 #ifdef R10
    DC_RELAY2,
#endif
  };
  enum  LinkChanTag_reserv {
    start = maxEnumLinkChanTag+1,
 #ifndef short_1w1     
    tg_lnk_1W1S,
#endif
#ifndef  long_1w1   
    tg_lnk_1W1L,
#endif
#ifndef  trigup_1w1    
    tg_lnk_1W1TR,
#endif
#ifndef trigdonw_1w1 
    tg_lnk_1W1TF,
#endif
#ifndef  countup_1w1
    tg_lnk_1W1CR,
#endif
#ifndef countdown_1w1 
    tg_lnk_1W1CL,
#endif
#ifndef countupdown_1w1 
    tg_lnk_1W1CRL,
#endif
    
#ifndef  short_1w2    
    tg_lnk_1W2S,
#endif
#ifndef long_1w2 
    tg_lnk_1W2L,
#endif
#ifndef trigup_1w2 
    tg_lnk_1W2TR,
#endif
#ifndef trigdonw_1w2 
    tg_lnk_1W2TF,
#endif
#ifndef countup_1w2 
    tg_lnk_1W2CR,
#endif
#ifndef countdown_1w2 
    tg_lnk_1W2CL,
#endif
#ifndef countupdown_1w2 
    tg_lnk_1W2CRL,
#endif
    
#ifndef  short_an1    
    tg_lnk_AN1S,
#endif
#ifndef  long_an1 
    tg_lnk_AN1L,
#endif
#ifndef  trigup_an1
    tg_lnk_AN1TR,
#endif
#ifndef  trigdonw_an1 
    tg_lnk_AN1TF,
#endif
#ifndef countup_an1 
    tg_lnk_AN1CR,
#endif
#ifndef countdown_an1 
    tg_lnk_AN1CL,
#endif
#ifndef countupdown_an1 
    tg_lnk_AN1CRL,
#endif
    
#ifndef  short_an2 
    tg_lnk_AN2S,
#endif
#ifndef long_an2 
    tg_lnk_AN2L,
#endif
#ifndef trigup_an2 
    tg_lnk_AN2TR,
#endif
#ifndef  trigdonw_an2 
    tg_lnk_AN2TF,
#endif
#ifndef countup_an2 
    tg_lnk_AN2CR,
#endif
#ifndef countdown_an2 
    tg_lnk_AN2CL,
#endif
#ifndef  countupdown_an2
    tg_lnk_AN2CRL,
#endif
    
#ifndef   short_but   
    tg_lnk_BUTS,
#endif
#ifndef   long_but
    tg_lnk_BUTL,
#endif
#ifndef  trigup_but
    tg_lnk_BUTTR,
#endif
#ifndef  trigdonw_but 
    tg_lnk_BUTTF,
#endif
#ifndef countup_but 
    tg_lnk_BUTCR,
#endif
#ifndef  countdown_but 
    tg_lnk_BUTCL,
#endif
#ifndef  countupdown_but
    tg_lnk_BUTCRL,
#endif  
#ifdef R0
    tg_lnk_RELAY1,    
    tg_lnk_RELAY2,
#endif
#ifdef R10    
    tg_lnk_RELAY2,
#endif    
  };
  
#ifdef __cplusplus
}
#endif   
#endif
