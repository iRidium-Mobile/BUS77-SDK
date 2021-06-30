#ifndef _C_FIRMWARE_H_INCLUDE_
#define _C_FIRMWARE_H_INCLUDE_

// Включения
#include "iridium.h"

// Определения
#define FIRMWARE_BLOCK_SIZE   128
#define FIRMWARE_WAIT_TIME    3000

class CFirmware
{
public:
   // Конструктор/деструктор
   CFirmware();
   virtual ~CFirmware();

   // Установка/получение адреса
   void SetAddress(iridium_address_t in_Address)
      { m_Address = in_Address; }
   iridium_address_t GetAddress()
      { return m_Address; }
      
   // Установка/получение идентификатора потока
   void SetStreamID(u8 in_u8StreamID)
      { m_u8StreamID = in_u8StreamID; }
   u8 GetStreamID()
      { return m_u8StreamID; }
      
   // Установка/получение идентификатора блока
   void SetBlockID(u8 in_u8BlockID)
      { m_u8BlockID = in_u8BlockID; }
   u8 GetBlockID()
      { return m_u8BlockID; }
      
   // Установка/получение времени
   void SetTime(u32 in_u32Time)
      { m_u32Time = in_u32Time; }
   u32 GetTime()
      { return m_u32Time; }

   // Открытие/закрытие прошивки
   void Open(size_t in_stStart, size_t in_stSize);
   void Close();
   // Проверка открытия прошивки
   bool IsOpen()
      { return (0 != m_stStart); }

   // Пропуск данных
   void Skip(size_t in_stSize);

   // Получение текущего указателя
   size_t GetPtr()
      { return m_stCur; }
      
   // Размер данных
   size_t GetSize()
      { return m_stEnd - m_stCur; }

protected:
   iridium_address_t m_Address;                    // Адрес устройства запросившего поток
   u8                m_u8StreamID;                 // Идентификатор потока
   u8                m_u8BlockID;                  // Идентификатор блока
   u32               m_u32Time;                    // Время отправки данных
   size_t            m_stStart;                    // Начальный адрес
   size_t            m_stCur;                      // Текущий адрес прошивки
   size_t            m_stEnd;                      // Адрес конца прошивки
};
#endif   // _C_FIRMWARE_H_INCLUDE_
