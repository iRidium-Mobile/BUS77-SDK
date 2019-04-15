#ifndef _C_CAN_PORT_H_INCLUDED_
#define _C_CAN_PORT_H_INCLUDED_

#include "IridiumTypes.h"

// 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
// [X][X][X][C][C][C][C][C][C][C][C][C][C][C][C][C][C][C][C][T][T][T][T][T][T][T][T][T][T][T][T][E]
// [X][X][X][C][C][C][C][C][C][C][C][C][C][C][C][C][C][C][C][T][T][T][B][A][A][A][A][A][A][A][A][E]
// E  - Признак замыкающего пакета
// B  - Признак широковещательного пакета
// T  - Идентиифкатор транзакции
// A  - Адрес устройства (для фильтрации CAN пакетов)
// С  - Идентификатор устройства отправителя
// X  - Зарезервировано

// Пример                            Маска    Сдвиг Описание
// 0000000000000000000000000000‭0001‬  0x0001   0     Конец
// ‭00000000000000000000000111111110‬  0x00FF   1     Адрес
// ‭00000000000000000000001000000000‬  0x0001   9     Широковещательность
// ‭00000000000000000001110000000000‬  0x0007   10    Идентификатор транзакции
// ‭00011111111111111110000000000000‬  0xFFFF   13    Идентификатор устройства

// ‭00000011111010000000011000000001‬
// 0000‭0011111010000000011000000001‬

// Маски для Ext ID
#define IRIDIUM_EXT_ID_END_MASK        0x0001      // Маска замыкающего пакета
#define IRIDIUM_EXT_ID_ADDRESS_MASK    0x00FF      // Маска адреса
#define IRIDIUM_EXT_ID_BROADCAST_MASK  0x0001      // Маска широковещательного пакета
#define IRIDIUM_EXT_ID_TID_MASK        0x0007      // Маска идентификатора транзакции
#define IRIDIUM_EXT_ID_CAN_ID_MASK     0xFFFF      // Маска идентиифкатора устройства

// Сдвиги для Ext ID
#define IRIDIUM_EXT_ID_ADDRESS_SHIFT   1           // Сдвиг адреса
#define IRIDIUM_EXT_ID_BROADCAST_SHIFT 9           // Сдвиг широковещательного пакета
#define IRIDIUM_EXT_ID_TID_SHIFT       10          // Сдвиг идентификатора транзакции
#define IRIDIUM_EXT_ID_CAN_ID_SHIFT    13          // Сдвиг идентиифкатора устройства

// Маска для сравнения Ext ID
#define IRIDIUM_EXT_ID_COMPARE_MASK    (IRIDIUM_EXT_ID_CAN_ID_MASK << IRIDIUM_EXT_ID_CAN_ID_SHIFT) |\
                                       (IRIDIUM_EXT_ID_TID_MASK << IRIDIUM_EXT_ID_TID_SHIFT) |\
                                       (IRIDIUM_EXT_ID_BROADCAST_MASK << IRIDIUM_EXT_ID_BROADCAST_SHIFT) |\
                                       (IRIDIUM_EXT_ID_ADDRESS_MASK << IRIDIUM_EXT_ID_ADDRESS_SHIFT)

// Структура фрейма
typedef struct can_frame_s
{
   u32   m_u32ExtID;                               // Идентификатор стандарта CAN-2.0B
   u8    m_u8Size;                                 // Размер данных
   u8    m_aData[8];                               // Полезная нагрузка CAN пакета
} can_frame_t;

// Структура данных буфера
typedef struct can_buffer_s
{
   size_t         m_stCount;                       // Количество фреймов
   size_t         m_stMax;                         // Максимальное количество фреймов
   can_frame_t*   m_pBuffer;                       // Указатель на буфер с фреймами

} can_buffer_t;

/**
   Класс для хранения и управления списком com портов
*/
class CCANPort
{

public:
   // Конструктор/деструктор
   CCANPort();
   virtual ~CCANPort();

   // Установка идентификатора CAN устройства
   void SetCANID(u16 in_u16CANID)
      { m_u16CANID = in_u16CANID; }
   // Установка идентификатора транзакции
   void SetTID(u8 in_u8TID)
      { m_u8TID = in_u8TID; }
   // Установка адреса
   void SetAddress(u8 in_u8Address)
      { m_u8Address = in_u8Address; }
   // Установка параметров входящего буфера
   bool SetInBuffer(void* in_pBuffer, size_t in_stSize);
   // Установка параметров исходящего буфера
   bool SetOutBuffer(void* in_pBuffer, size_t in_stSize);

   //////////////////////////////////////////////////////////////////////////
   // Получение данных из CAN шины
   //////////////////////////////////////////////////////////////////////////
   // Добавление фрейма в буфер
   bool AddFrame(can_frame_t* in_pFrame);
   // Получение данных
   bool GetPacket(void*& out_rBuffer, size_t& in_rSize);
      // Удаление пакета
   void DeletePacket();
   // Очистка буфера от обработанных фреймов
   void Flush();

   //////////////////////////////////////////////////////////////////////////
   // Отправка данных в CAN шину
   //////////////////////////////////////////////////////////////////////////
   // Разложение буфера на фреймы
   bool AddPacket(bool in_bBroadcast, u8 in_u8Address, void* in_pBuffer, size_t in_stSize);
   // Получение указателя на фрейм по индексу
   can_frame_t* GetFrame();
   // Удаление фрейма
   void DeleteFrame();

protected:
   // Очистка буфера
   void Clear(can_buffer_t& in_rBuffer);
   size_t Assembly(can_frame_t* in_pEnd, void* out_pBuffer, size_t in_stSize);
   // Получение идентификатора транзакции
   u8 GetTID();

   u8             m_u8Address;                     // Адрес порта
   u8             m_u8TID;                         // Идентификатор транзакции
   u16            m_u16CANID;                      // Идентификатор устройства в CAN шине
   can_buffer_t   m_InBuffer;                      // Данные входящего буфера
   can_buffer_t   m_OutBuffer;                     // Данные исходящего буфера
   u8             m_aPacketBuffer[8*33];           // Данные полученого пакета
   size_t         m_stPacketSize;                  // Размер полученого пакета
};
#endif   // _C_CAN_PORT_H_INCLUDED_

