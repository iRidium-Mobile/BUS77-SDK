#ifndef _C_DEVICE_H_INCLUDED_
#define _C_DEVICE_H_INCLUDED_

// Включения
#include "CIridiumBusProtocol.h"
#include "NVRAMCommon.h"

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

   // Работа с каналами обратной связи
   virtual size_t GetTags();

   // Работа с каналами управления
   virtual size_t GetChannels();

   // Работа с потоками
   virtual u8 StreamOpen(const char* in_pszName, eIridiumStreamMode in_eMode);
   virtual void StreamOpenResult(const char* in_pszName, eIridiumStreamMode in_eMode, u8 in_u8StreamID);
   virtual size_t StreamBlock(u8 in_u8StreamID, u8 in_u8BlockID, size_t in_stSize, const void* in_pBuffer);
   virtual void StreamBlockResult(u8 in_u8StreamID, u8 in_u8BlockID, size_t in_stSize);
   virtual void StreamClose(u8 in_u8StreamID);

   //////////////////////////////////////////////////////////////////////////
   // Собственные методы
   //////////////////////////////////////////////////////////////////////////
   // Инициализация устройства
   virtual void Setup();

   // Основной цикл
   virtual void Loop();
   void WorkInputs();
   void AddPacket(void* in_pBuffer, size_t in_stSize)
      { m_InBuffer.Add(in_pBuffer, in_stSize);}
private:
   
   void ChangeChannel(u8 in_u8Channel, eIridiumValueType in_eType, universal_value_t& in_rValue);
   void ChangeVariable(u16 in_u16Variable, u8 in_u8Type, universal_value_t& in_rValue);

   bool GetChannelValue(u32 in_u32ChannelID, u8& out_rType, universal_value_t& out_rValue);
   bool GetTagValue(u32 in_u32TagID, u8& out_rType, universal_value_t& out_rValue);

   u8 m_aInBuffer[IRIDIUM_BUS_IN_BUFFER_SIZE];
   u8 m_aOutBuffer[IRIDIUM_BUS_OUT_BUFFER_SIZE];
};

//////////////////////////////////////////////////////////////////////////
// Связывание C и CPP кода
//////////////////////////////////////////////////////////////////////////

// Процедуры для связи С и С++ файлов
extern "C" nvram_common_t* iRidiumDevice_InitNVRAM(void);
extern "C" void iRidiumDevice_Setup(void);
extern "C" void iRidiumDevice_Loop(void);

#endif   // _C_DEVICE_H_INCLUDED_
