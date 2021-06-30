#ifndef __HADR_DESCRIPTION_H
#define __HADR_DESCRIPTION_H

#include  "IridiumValue.h"

#ifdef __cplusplus
extern "C" {
#endif 
  
  typedef struct hd_chanel_s
  {
   u8                 m_u8Type;                           // ��� �������� ������ �������� �����
   const void*        m_pRange;                           // ��������� �� �����������, ������������ � ������� ��������
   u32                hd_param;                           //32-������ �������� ���������, ��� ������ �������   
   bool   (*GetParam) (universal_value_t *,u32);                //����� ������� ������� ������, ���� ������ ��� ����������- ������� false
   bool   (*SetParam) (universal_value_t *,u32);                //����� ������� ��������� ������, ���� ������ ��� ����������- ������� false
  }hd_chanel_t;  
  
  const f32   hdRangeTempSens1W[] =        {0xC25C0000,         0x42FA0000,          0.1         }; // {-55, +125, 0.1}
  const bool  hdRangeBool[]       =        {false,              true,                true        };
  const u32   hdRangeU32[]        =        {0,                  0xFFFFFFFF,          1           };
  const u64   hdRangeU64[]        =        {0,                  0xFFFFFFFFFFFFFFFF,  1           };
  
 //extern ��� ������� 
 //----------------------- ���������� ������ 
  extern bool GetParamDiscret(universal_value_t *value, u32 Param);
  extern bool SetParamDiscret(universal_value_t *value, u32 Param);
  
  // ������ ���������
  const   hd_chanel_t hd_discret[1] = {    {IVT_BOOL,hdRangeBool,0,&GetParamDiscret, &SetParamDiscret},
                                      };                                                               
 //  hd_discret[0].SetParam(&in_rValue,sys_led); ������ ���������� ��������� �����������
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
//------------------------ ����� 1W
  
  extern u32  concate1WParam(u32 line, u32 device);//� ����� u32 ���������� ����� ����� � ����� ���������� 1W ��� �������� Param ��� ������ ������� 1W
  
  extern bool GetUID1W(universal_value_t *value, u32 Param); //������ � value UID (u64) ������������� ����������. ���� value ==0, �� ����������  ���
  extern bool SetUID1W(universal_value_t *value, u32 Param); //�������� � ������� �������� UID ��������� value (u64) � ���������� ����������.
  
  extern bool GetTemp1W(universal_value_t *value, u32 Param); //������ � value (float) ��������� ���������� ����������� � ����������. 
  extern bool SetTemp1W(universal_value_t *value, u32 Param); //�������� � ������� �������� ����������� ��������� value (float) � ���������� ����������.  
  
  extern bool GetCntGood(universal_value_t *value, u32 Param); //������ � Value (u32)- ������� �������� �������� �������� ������� ��������� ����������� � ����������
  extern bool SetCntGood(universal_value_t *value, u32 Param);//�������� � ������� �������� ������� ������� ��������� value (float) � ���������� ����������. 
  
  extern bool GetCountDev(universal_value_t *value, u32 Param);//������ � Value ��������� ���������� ��������� ������������ ��������� �� ����� �����.
  extern bool SetCountDev(universal_value_t *value, u32 Param);//��������, ���������� false.
  
  extern bool GetTypeDev(universal_value_t *value, u32 Param);//������ � Value (u32)- ��� ����������, ������������� � �����
  extern bool SetTypeDev(universal_value_t *value, u32 Param);//��������, ���������� false.  
  
  extern bool GetMaxDev(universal_value_t *value, u32 Param);//������ ������������ ���-�� ��������� � ����� �����, ������� �������� - max_dev_OW �� ����� �����
  extern bool SetMaxDev(universal_value_t *value, u32 Param);//��������, ���������� false.
  
  extern const char* GetFamily( u32 Param);//�� ���� ���������� ���������� �������� �������� �� ������� list_family_code
    
  const hd_chanel_t hd_1wdev[6] = {  {IVT_U64,hdRangeU64,0,&GetUID1W, &SetUID1W},//0//UID ���������� �� 1W
                                    {IVT_F32,hdRangeTempSens1W,0,&GetTemp1W, &SetTemp1W},//1//������ � �����������, � ���������� �� 1W
                                    {IVT_U32,hdRangeU32,0,&GetCntGood, &SetCntGood},//2//���-�� �������� ������� � ����������� �� 1W
                                    {IVT_U32,hdRangeU32,0,&GetTypeDev, &SetTypeDev},//3//��� ���������� �� 1W
                                    {IVT_U32,hdRangeU32,0,&GetCountDev, &SetCountDev},//4//���-�� ��������� ��������� �� ��������� ����� �� ����� ����� 1W
                                    {IVT_U32,hdRangeU32,0,&GetMaxDev, &SetMaxDev},//5//����.���������� ��������� �� ����� ����� 1W
  };
  
  
  // hd_1wdev[1].GetParam(&in_rValue,concate1WParam(0,0))  - ������ �������� ����������� � 0-�� ������� �� 0-� �����
  
 //----------------------- ���������� �����
  
  extern bool GetAIN(universal_value_t *value, u32 Param); //������ � value �������� (f32) ������ � ������� � Param. ���� return ==0, �� ����������  ���
  extern bool SetAIN(universal_value_t *value, u32 Param); //�������� � ������� �������� ���������� �������  ��������� value (f32) � ���������� � Param ������.
  
  const hd_chanel_t hd_ain[1] = { {IVT_F32,hdRangeTempSens1W, 0, &GetAIN, &SetAIN}, };
  /*
    num_temp              16   -����������� ����
    num_vref              17          
    num_volt              9     -���������� ������� ����
    num_1W1AIN             1    - ���������� �� ����� 1W1
    num_1W2AIN             2    - ���������� �� ����� 1W2
    num_AIN1               3    - ���������� �� ����� AIN1
    num_AIN2               4    - ���������� �� ����� AIN2
    num_AIN3               5    - ���������� �� ����� AIN3
    num_AIN4               6    - ���������� �� ����� AIN4
  
    num_Vadj              8     - ���������� �� ������ ������� ���������
      ana_resist1W1=21,         - �������������, ������������ � 1W1
    ana_resist1W2,              - �������������, ������������ � 1W2
    ana_resist1,                - �������������, ������������ � AIN1        
    ana_resist2,                - �������������, ������������ � AIN2
    ana_resist3,                - �������������, ������������ � AIN3
    ana_resist4                 - �������������, ������������ � AIN4
  
  
    hd_ain[0].GetParam(&in_rValue,ana_resist1W2) - ������ �������� ������������� �� ����� 1W2
  */

  
 //-----------------------   



  
  
#ifdef __cplusplus
}
#endif   
#endif