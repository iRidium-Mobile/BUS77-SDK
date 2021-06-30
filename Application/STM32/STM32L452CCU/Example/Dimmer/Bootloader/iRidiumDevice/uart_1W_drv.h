#ifndef __UART_1W_DRV_H
#define __UART_1W_DRV_H

#ifdef __cplusplus
extern "C" {
#endif 
  
  
extern void Drv_1W_Start(uint32_t addr);
//extern uint8_t GetValueFromTag1W1(uint8_t tag, uint64_t * value);  
//extern uint8_t GetNewValue1W1(uint8_t * tag, uint32_t * value); 
//extern uint8_t SetValueFromCtrl1W1(uint8_t tag, uint32_t * value);
//extern void WriteUidToEeprom(uint32_t addr);
//extern void ReadUidFromEeprom(uint32_t addr);
//extern uint8_t GetNewDSUID();

extern uint8_t GetUIDValue(uint8_t in_u8Line, uint8_t in_u8Num, uint64_t* out_pValue);
extern uint8_t GetTempValue(uint8_t in_u8Line, uint8_t in_u8Num, float* out_pValue);


#pragma pack(push, 1)
 typedef struct t_dev_1w {
   uint8_t      line;
   uint8_t      device;
 } t_dev_1w;  
   
 typedef struct t_family_code {
   uint8_t      code;
   const char*     refer;
 } t_family_code;
 
  #pragma pack(pop)

 extern  const t_family_code list_family_code[5];
 
#ifdef __cplusplus
}
#endif   
#endif