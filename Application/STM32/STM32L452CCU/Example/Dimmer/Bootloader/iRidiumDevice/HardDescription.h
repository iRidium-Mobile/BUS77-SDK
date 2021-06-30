#ifndef __HADR_DESCRIPTION_H
#define __HADR_DESCRIPTION_H

#include  "IridiumValue.h"

#ifdef __cplusplus
extern "C" {
#endif 
  
  typedef struct hd_chanel_s
  {
   u8                 m_u8Type;                           // Тип значения канала обратной связи
   const void*        m_pRange;                           // Указатель на минимальное, максимальное и шаговое значение
   u32                hd_param;                           //32-битное значение параметра, для вызова функции   
   bool   (*GetParam) (universal_value_t *,u32);                //вызов функции статуса канала, Если ошибка или невозможно- возврат false
   bool   (*SetParam) (universal_value_t *,u32);                //вызов функции установки канала, Если ошибка или невозможно- возврат false
  }hd_chanel_t;  
  
  const f32   hdRangeTempSens1W[] =        {0xC25C0000,         0x42FA0000,          0.1         }; // {-55, +125, 0.1}
  const bool  hdRangeBool[]       =        {false,              true,                true        };
  const u32   hdRangeU32[]        =        {0,                  0xFFFFFFFF,          1           };
  const u64   hdRangeU64[]        =        {0,                  0xFFFFFFFFFFFFFFFF,  1           };
  
 //extern для функций 
 //----------------------- дискретные выходы 
  extern bool GetParamDiscret(universal_value_t *value, u32 Param);
  extern bool SetParamDiscret(universal_value_t *value, u32 Param);
  
  // вызовы дискретов
  const   hd_chanel_t hd_discret[1] = {    {IVT_BOOL,hdRangeBool,0,&GetParamDiscret, &SetParamDiscret},
                                      };                                                               
 //  hd_discret[0].SetParam(&in_rValue,sys_led); пример управления системным светодиодом
 /*
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
  */
//------------------------ линия 1W
  
  extern u32  concate1WParam(u32 line, u32 device);//В одном u32 объединяет номер линии и номер устройства 1W для значения Param при вызове функций 1W
  
  extern bool GetUID1W(universal_value_t *value, u32 Param); //вернет в value UID (u64) подключенного устройства. Если value ==0, то устройства  нет
  extern bool SetUID1W(universal_value_t *value, u32 Param); //заполнит в массиве значений UID значением value (u64) у указанного устройства.
  
  extern bool GetTemp1W(universal_value_t *value, u32 Param); //вернет в value (float) последнюю полученную температуру с устройства. 
  extern bool SetTemp1W(universal_value_t *value, u32 Param); //заполнит в массиве значений температуру значением value (float) у указанного устройства.  
  
  extern bool GetCntGood(universal_value_t *value, u32 Param); //вернет в Value (u32)- текущее значение счетчика успешных попыток получения температуры с устройства
  extern bool SetCntGood(universal_value_t *value, u32 Param);//заполнит в массиве значений счетчик попыток значением value (float) у указанного устройства. 
  
  extern bool GetCountDev(universal_value_t *value, u32 Param);//вернет в Value последнее количество найденных подключенных устройств на одной линии.
  extern bool SetCountDev(universal_value_t *value, u32 Param);//заглушка, возвращает false.
  
  extern bool GetTypeDev(universal_value_t *value, u32 Param);//вернет в Value (u32)- тип устройтсва, подключенного к линии
  extern bool SetTypeDev(universal_value_t *value, u32 Param);//заглушка, возвращает false.  
  
  extern bool GetMaxDev(universal_value_t *value, u32 Param);//вернет максимальное кол-во устройств в одной линии, которое возможно - max_dev_OW на одной линии
  extern bool SetMaxDev(universal_value_t *value, u32 Param);//заглушка, возвращает false.
  
  extern const char* GetFamily( u32 Param);//По типу устройства возвращает текствое описание из таблицы list_family_code
    
  const hd_chanel_t hd_1wdev[6] = {  {IVT_U64,hdRangeU64,0,&GetUID1W, &SetUID1W},//0//UID устройства на 1W
                                    {IVT_F32,hdRangeTempSens1W,0,&GetTemp1W, &SetTemp1W},//1//данные о температуре, с устройства на 1W
                                    {IVT_U32,hdRangeU32,0,&GetCntGood, &SetCntGood},//2//кол-во успешных обменов с устройством на 1W
                                    {IVT_U32,hdRangeU32,0,&GetTypeDev, &SetTypeDev},//3//Тип устройства на 1W
                                    {IVT_U32,hdRangeU32,0,&GetCountDev, &SetCountDev},//4//Кол-во найденных устройств за последний поиск на одной линии 1W
                                    {IVT_U32,hdRangeU32,0,&GetMaxDev, &SetMaxDev},//5//Макс.количество устройств на одной линии 1W
  };
  
  
  // hd_1wdev[1].GetParam(&in_rValue,concate1WParam(0,0))  - вернет значение температуры с 0-го датчика на 0-й линии
  
 //----------------------- аналоговые входы
  
  extern bool GetAIN(universal_value_t *value, u32 Param); //вернет в value значение (f32) канала с номеров в Param. Если return ==0, то устройства  нет
  extern bool SetAIN(universal_value_t *value, u32 Param); //заполнит в массиве значений аналоговых каналов  значением value (f32) у указанного в Param канала.
  
  const hd_chanel_t hd_ain[1] = { {IVT_F32,hdRangeTempSens1W, 0, &GetAIN, &SetAIN}, };
  /*
    num_temp              16   -температура чипа
    num_vref              17          
    num_volt              9     -напряжение питания чипа
    num_1W1AIN             1    - напряжение на входе 1W1
    num_1W2AIN             2    - напряжение на входе 1W2
    num_AIN1               3    - напряжение на входе AIN1
    num_AIN2               4    - напряжение на входе AIN2
    num_AIN3               5    - напряжение на входе AIN3
    num_AIN4               6    - напряжение на входе AIN4
  
    num_Vadj              8     - напряжение на выходе питания устройств
      ana_resist1W1=21,         - сопротивление, подключенное к 1W1
    ana_resist1W2,              - сопротивление, подключенное к 1W2
    ana_resist1,                - сопротивление, подключенное к AIN1        
    ana_resist2,                - сопротивление, подключенное к AIN2
    ana_resist3,                - сопротивление, подключенное к AIN3
    ana_resist4                 - сопротивление, подключенное к AIN4
  
  
    hd_ain[0].GetParam(&in_rValue,ana_resist1W2) - вернет величину сопротивления на входе 1W2
  */

  
 //-----------------------   



  
  
#ifdef __cplusplus
}
#endif   
#endif