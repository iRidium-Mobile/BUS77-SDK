#ifndef __RXX_DIGITALIO_H
#define __RXX_DIGITALIO_H


#ifdef __cplusplus
extern "C" {
#endif 
#define max_discrete            5  
#define PIN_BUTTON      GPIO_PIN_3
  
typedef enum hd_discret_pin_e {
  sys_led = 0,
  rgb1_red,
  rgb1_green,
  rgb1_blue,
  rgb2_red,
  rgb2_green,
  rgb2_blue,
  pwr_relay1,
  pwr_relay2,
  relay3,
  relay4
} hd_discret_pin_e; 
  
  
#pragma pack(push, 1)
    typedef struct hd_discret_pin_s {
  GPIO_TypeDef*         GPIOx;
  uint16_t              GPIO_Pin;
  GPIO_PinState         PinSET;
  GPIO_PinState         PinRESET;
  uint32_t              PinMODE;
} hd_discret_pin_t, * hd_discret_pin_p;
  

  typedef struct t_u8_event_discret {
   uint8_t           value;
   uint8_t           tag;
  }t_u8_event_discret;

  typedef struct t_u32_event_discret {
   uint32_t           value;
   uint8_t            tag;
  } t_u32_event_discret; 

  typedef struct t_refer_discret {
    uint16_t                    pin_mask;
    t_u8_event_discret          pressed;
    t_u32_event_discret         cnt_rise;
    t_u32_event_discret         cnt_fall;
    t_u32_event_discret         cnt_rise_fall;
    t_u8_event_discret          trig_fall;
    t_u8_event_discret          trig_rise;
    t_u8_event_discret          long_press;
    uint16_t                    cnt_wait;
    uint16_t                    new_value;
  } t_refer_discret;  

  typedef struct t_refer_rele {
    uint8_t            tag[4];
   uint8_t      new_value[4]; 
  } t_refer_rele ;
  #pragma pack(pop)

  
extern   const   hd_discret_pin_t  hd_discret_pin[];
  
  extern volatile t_refer_discret refer_discret[max_discrete];   
  extern void RXXdigitalIO_init(uint8_t sec_wait);   

  
  
  
  
#ifdef __cplusplus
}
#endif   
#endif
