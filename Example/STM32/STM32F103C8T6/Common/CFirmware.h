#ifndef _C_FIRMWARE_H_INCLUDE_
#define _C_FIRMWARE_H_INCLUDE_

#include "iridium.h"

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
   void Open(u8* in_pBuffer, size_t in_stSize);
   void Close();
   // Проверка открытия прошивки
   bool IsOpen()
      { return (NULL != m_pBuffer); }

   // Пропуск данных
   void Skip(size_t in_stSize);

   // Получение указателя на данные
   u8* GetPtr()
      { return m_pBuffer; }
      
   // Размер данных
   size_t GetSize()
      { return m_pEnd - m_pBuffer; }

protected:
   iridium_address_t m_Address;                    // Адрес запросившего поток
   u8                m_u8StreamID;                 // Идентификатор потока
   u8                m_u8BlockID;                  // Идентификатор блока
   u32               m_u32Time;                    // Время отправки данных
   u8*               m_pBuffer;                    // Текущий указатель на данные 
   u8*               m_pEnd;                       // Указатель на конец буфера
};
#endif   // _C_FIRMWARE_H_INCLUDE_
