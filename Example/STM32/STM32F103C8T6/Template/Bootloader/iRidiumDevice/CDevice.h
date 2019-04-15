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
   char* m_pszDesc;                                // указатель на описание канала
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
   virtual bool SendPacket(void* in_pBuffer, size_t in_stSize);

   // Блокирование/разблокирование входящего буфера (данные методы предназначены для микроконтроллеров
   // если микроконтроллер получает данные из шины по прерываниям)
   static u8 LockInBuffer();
   static void UnLockInBuffer(u8 in_u8Data);

   // Настройка
   virtual bool SetLID(char* in_pszHWID, u8 in_u8LID);

   // Проверка PIN кода
   virtual s8 TestPIN(eIridiumOperation in_eType, u32 in_u32PIN, void* in_pData);
      
   // Установка/получение информации о найденом устройстве
   virtual bool GetSearchInfo(iridium_search_info_t& out_rInfo);

   // Получение подробной информации об устройстве
   virtual bool GetDeviceInfo(iridium_device_info_t& out_rInfo);

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
   void Setup();
   
   // Основной цикл
   void Loop();

private:
   void WorkInputs();
   void FirmwareWork();

   // Обработка внешнего CAN порта
   void ReadFromExtCan();
   void WriteToExtCan();

   // Данные входящего и исходящего буфера
   u8 m_aInBuffer[IRIDIUM_BUS_IN_BUFFER_SIZE];
   u8 m_aOutBuffer[IRIDIUM_BUS_OUT_BUFFER_SIZE];
};

extern "C" void iRidiumDevice_Init(void);
extern "C" void iRidiumDevice_Setup(void);
extern "C" void iRidiumDevice_Loop(void);

#endif   // _C_DEVICE_H_INCLUDED_
