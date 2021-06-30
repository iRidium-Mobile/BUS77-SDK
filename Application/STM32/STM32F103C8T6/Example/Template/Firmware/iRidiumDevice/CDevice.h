#ifndef _C_DEVICE_H_INCLUDED_
#define _C_DEVICE_H_INCLUDED_

// Включения
#include "CIridiumBusProtocol.h"

// Флаги для канала управления
#define CF_W   0x40                                // Признак изменения канала по PIN коду
#define CF_R   0x20                                // Признак чтения канала по PIN коду

// Данные канала обратной связи
typedef struct device_tag_s
{
   u32   m_u32ID;                                  // Идентификатор канала
   char* m_pszName;                                // Указатель на имя канала
   u8    m_u8Type;                                 // Тип значения канала обратной связи
   u64   m_u64Min;                                 // Минимальное значение
   u64   m_u64Max;                                 // Максимальное значение
   u64   m_u64Step;                                // Шаг
   char* m_pszDesc;                                // Указатель на описание канала
   u8    m_u8Flags;                                // Список флагов
} device_tag_t;

// Данные канала управления
typedef struct device_channel_s
{
   u32   m_u32ID;                                  // Идентификатор канала
   char* m_pszName;                                // Указатель на имя канала
   u8    m_u8Type;                                 // Тип значения канала обратной связи
   u64   m_u64Min;                                 // Минимальное значение
   u64   m_u64Max;                                 // Максимальное значение
   u64   m_u64Step;                                // Шаг
   char* m_pszDesc;                                // указатель на описание канала
   u8    m_u8Flags;                                // Список флагов
   u8    m_u8MaxVariables;                         // Максимальное количество глобальных переменных на канал управления
} device_channel_t;

//////////////////////////////////////////////////////////////////////////
// class CDevice
//////////////////////////////////////////////////////////////////////////
class CDevice : public CIridiumBusProtocol
{
public:
   // Конструктор/деструктор
   CDevice();
   virtual ~CDevice();

   //////////////////////////////////////////////////////////////////////////
   // Перегруженные методы для взаимодействия с протколом
   //////////////////////////////////////////////////////////////////////////
   // Отправка данных
   virtual bool SendPacket(bool in_bBroadcast, iridium_address_t in_Address, void* in_pBuffer, size_t in_stSize);

   // Настройка
   virtual void SetLID(u8 in_u8LID);
   // Сравнение HWID
   virtual bool CompareHWID(const char* in_pszHWID);
   // Проверка возможности операции
   virtual s8 CheckOperation(eIridiumOperation in_eType, u32 in_u32PIN, void* in_pData);
      
   // Установка/получение информации о найденом устройстве
   virtual bool GetSearchInfo(iridium_search_info_t& out_rInfo);

   // Получение подробной информации об устройстве
   virtual bool GetDeviceInfo(iridium_device_info_t& out_rInfo);

   // Работа с глобальными переменными
   virtual void SetVariable(u16 in_u16VariableID, u8 in_u8Type, universal_value_t& in_rValue, u8 in_u8Flags);
   virtual bool GetVariable(u16 in_u16VariableID, u8& out_rType, universal_value_t& out_rValue);
      
   // Работа с каналами обратной связи
   virtual size_t GetTags();
   virtual size_t GetTagIndex(u32 in_u32TagID);
   virtual size_t GetTagData(size_t in_stIndex, iridium_tag_info_t& out_pInfo, size_t in_stSize);
   virtual bool GetTagDescription(u32 in_u32TagID, iridium_tag_description_t& out_rDescription);
   virtual bool LinkTagAndVariable(u32 in_u32TagID, bool in_bOwner, u16 in_u16Variable);
   virtual bool SetTagValue(u32 in_u32TagID, u8 in_u8Type, universal_value_t& in_rValue, u8 in_u8Flags);

   // Работа с каналами управления
   virtual size_t GetChannels();
   virtual size_t GetChannelIndex(u32 in_u32ChannelID);
   virtual size_t GetChannelData(size_t in_stIndex, iridium_channel_info_t& out_pInfo);
   virtual bool GetChannelDescription(u32 in_u32ChannelID, iridium_channel_description_t& out_rDescription);
   virtual bool LinkChannelAndVariable(u32 in_u32ChannelID, u8 in_u8Variables, u16* in_pVariables);
   virtual bool SetChannelValue(u32 in_u32ChannelID, u8 in_u8Type, universal_value_t& in_rValue, u8 in_u8Flags);

   // Работа с потоками
   virtual u8 StreamOpen(const char* in_pszName, eIridiumStreamMode in_eMode);
   virtual void StreamOpenResult(const char* in_pszName, eIridiumStreamMode in_eMode, u8 in_u8StreamID);
   virtual size_t StreamBlock(u8 in_u8StreamID, u8 in_u8BlockID, size_t in_stSize, const void* in_pBuffer);
   virtual void StreamBlockResult(u8 in_u8StreamID, u8 in_u8BlockID, size_t in_stSize);
   virtual void StreamClose(u8 in_u8StreamID);
   
   // Smart API
   virtual bool GetSmartAPI(void*& out_rBuffer, size_t& out_rSize);
      
   //////////////////////////////////////////////////////////////////////////
   // Собственные методы
   //////////////////////////////////////////////////////////////////////////
   // Инициализация устройства
   void Setup();
   
   // Основной цикл
   void Loop();

private:
   void WorkInputs();
   void ChangeVariable(u16 in_u16Variable, u8 in_u8Type, universal_value_t& in_rValue);

   bool GetChannelValue(u32 in_u32ChannelID, u8& out_rType, universal_value_t& out_rValue);
   bool GetTagValue(u32 in_u32TagID, u8& out_rType, universal_value_t& out_rValue);

   // Обработка внешнего CAN порта
   void ReadFromExtCan();
   //void WriteToExtCan();

   // Данные входящего и исходящего буфера
   u8 m_aInBuffer[IRIDIUM_BUS_IN_BUFFER_SIZE];
   u8 m_aOutBuffer[IRIDIUM_BUS_OUT_BUFFER_SIZE];
};

extern "C" void iRidiumDevice_InitEEPROM(void);
extern "C" void iRidiumDevice_Setup(void);
extern "C" void iRidiumDevice_Loop(void);

#endif   // _C_DEVICE_H_INCLUDED_
