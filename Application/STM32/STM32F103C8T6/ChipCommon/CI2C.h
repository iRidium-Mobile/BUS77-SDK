#ifndef _C_I2C_H_INCLUDED_
#define _C_I2C_H_INCLUDED_

#include "stm32f1xx_hal.h"
#include "IridiumTypes.h"

// Класс для работы с I2C
class CI2C
{
public:
   // Конструктор/деструктор
   CI2C();
   virtual ~CI2C();

   // Установка указателя на I2C интерфейс
   void SetInterface(I2C_HandleTypeDef* in_pInterface)
      { m_pInterface = in_pInterface; }
      
   // Установка адреса устройства
   void SetAddress(u8 in_u8Address)
      { m_u8Address = in_u8Address; }

   bool Write8(u16 in_u8Register, u8* in_pBuffer, u16 in_u16Size);
   bool Write16(u16 in_u8Register, u8* in_pBuffer, u16 in_u16Size);
      
   bool Read8(u16 in_u8Register, u8* out_pBuffer, u16 in_u16Size);
   bool Read16(u16 in_u8Register, u8* out_pBuffer, u16 in_u16Size);

protected:
   I2C_HandleTypeDef*   m_pInterface;              // Указатель на интерфейс I2C
   u8                   m_u8Address;               // Адрес устройства
};

#endif   // _C_I2C_H_INCLUDED_
