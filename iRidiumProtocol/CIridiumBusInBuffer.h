/*******************************************************************************
 * Copyright (c) 2013-2019 iRidi Ltd. www.iridi.com
 *
 * Все права зарегистрированы. Эта программа и сопровождающие материалы доступны
 * на условиях Eclipse Public License v2.0 и Eclipse Distribution License v1.0,
 * которая сопровождает это распространение. 
 *
 * Текст Eclipse Public License доступен по ссылке
 *    http://www.eclipse.org/legal/epl-v20.html
 * Текст Eclipse Distribution License доступн по ссылке
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Участники:
 *    Марат Гилязетдинов, Сергей Королёв  - первая версия
 *******************************************************************************/
#ifndef _C_IRIDIUM_BUS_IN_BUFFER_H_INCLUDED_
#define _C_IRIDIUM_BUS_IN_BUFFER_H_INCLUDED_

// Включения
#include "CIridiumInBuffer.h"

typedef u8 (*lock_buffer_t)();
typedef void (*unlock_buffer_t)(u8);

//////////////////////////////////////////////////////////////////////////
// class CIridiumBusInBuffer
//////////////////////////////////////////////////////////////////////////
class CIridiumBusInBuffer : public CIridiumInBuffer
{
public:
   // Конструктор/деструктор
   CIridiumBusInBuffer();
   virtual ~CIridiumBusInBuffer();

   // Перегруженные методы
   virtual void SetBuffer(const void* in_pBuffer, size_t in_stSize);

   // Открытие/закрытие пакета
   virtual s8 OpenPacket();
   virtual void ClosePacket();

   // Фильтрация входящего потока
   bool FilterNoise();
   bool FilterNoiseAndForeignPacket(iridium_address_t in_Address);

   // Установка блокирование/разблокирование входящего буфера (данные методы предназначены для микроконтроллеров
   // если микроконтроллер получает данные из шины по прерываниям)
   void SetLockUnlock(lock_buffer_t in_pLock, unlock_buffer_t in_pUnLock);

   // Методы децентрализованной части протокола
   static bool FindBUSPacket(u8* in_pBuffer, size_t in_stSize, iridium_packet_header_t& out_rInPH, iridium_packet_t& out_rPacket);
private:
   size_t            m_stCount;                       // Количество найденных пакетов
   size_t            m_stFiltered;                    // Смещение на текущую позицию обработки входящего буфера
   lock_buffer_t     m_pLock;                         // Указатель на метод блокирования входящего буфера
   unlock_buffer_t   m_pUnLock;                       // Указатель на метод разблокирования входящего буфера
};
#endif   // _C_IRIDIUM_BUS_IN_BUFFER_H_INCLUDED_
