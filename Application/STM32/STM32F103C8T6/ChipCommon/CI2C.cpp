#include "CI2C.h"

/**
   Конструктор класса
   на выходе   :  *
*/
CI2C::CI2C()
{
   m_pInterface = NULL;
   m_u8Address = 0;
}

/**
   Деструктор класса
*/
CI2C::~CI2C()
{
}

/**
   Запись в регистр с 8 битной адресацией
   на выходе   :  in_u8Register  - 8 битный адрес регистра
                  in_pBuffer     - указатель на буфер с параметрами
                  in_u16Size     - размер данных в буфере с параметрами
   на выходе   :  успешность записи
*/
bool CI2C::Write8(u16 in_u8Register, u8* in_pBuffer, u16 in_u16Size)
{
   return (HAL_I2C_Mem_Write(m_pInterface, m_u8Address, in_u8Register, I2C_MEMADD_SIZE_8BIT, in_pBuffer, in_u16Size, 1000) == HAL_OK);
}

/**
   Запись в регистр с 16 битной адресацией
   на выходе   :  in_u16Register - 16 битный адрес регистра
                  in_pBuffer     - указатель на буфер с параметрами
                  in_u16Size     - размер данных в буфере с параметрами
   на выходе   :  успешность записи
*/
bool CI2C::Write16(u16 in_u16Register, u8* in_pBuffer, u16 in_u16Size)
{
   return (HAL_I2C_Mem_Write(m_pInterface, m_u8Address, in_u16Register, I2C_MEMADD_SIZE_16BIT, in_pBuffer, in_u16Size, 1000) == HAL_OK);
}

/**
   Чтение регистра с 8 битной адресацией
   на выходе   :  in_u8Register  - адрес регистра
                  out_pBuffer    - указатель на буфер с куда нужно поместить полученые параметры
                  in_u16Size     - размер буфере с параметрами
   на выходе   :  успешность записи
*/
bool CI2C::Read8(u16 in_u8Register, u8* out_pBuffer, u16 in_u16Size)
{
   return (HAL_I2C_Mem_Read(m_pInterface, m_u8Address, in_u8Register, I2C_MEMADD_SIZE_8BIT, out_pBuffer, in_u16Size, 1000) == HAL_OK);
}

/**
   Чтение регистра с 16 битной адресацией
   на выходе   :  in_u8Register  - адрес регистра
                  out_pBuffer    - указатель на буфер с куда нужно поместить полученые параметры
                  in_u16Size     - размер буфере с параметрами
   на выходе   :  успешность записи
*/
bool CI2C::Read16(u16 in_u16Register, u8* out_pBuffer, u16 in_u16Size)
{
   return (HAL_I2C_Mem_Read(m_pInterface, m_u8Address, in_u16Register, I2C_MEMADD_SIZE_16BIT, out_pBuffer, in_u16Size, 1000) == HAL_OK);
}
