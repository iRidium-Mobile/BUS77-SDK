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
#ifndef _C_IRIDIUM_CIPHER_H_INCLUDED_
#define _C_IRIDIUM_CIPHER_H_INCLUDED_

#include "IridiumTypes.h"
#include "Iridium.h"

// Идентификаторы шифрования
#define IRIDIUM_CRYPTION_NONE          0           // Шифрование отсутствует
#define IRIDIUM_CRYPTION_SIMPLE        1           // Шифрование по собственному способу, упрощенное
#define IRIDIUM_CRYPTION_GRASSHOPPER   2           // Шифрование с помощью блочного шифра "Кузнечик"
#define IRIDIUM_CRYPTION_AES256        3           // Шифрование с помощью блочного шифра AES 256

#define BLOCK_CIPHER_KEY_SIZE          32          // Размер ключа для блочного шифра в байтах (32 байта, 256 бит)
#define BLOCK_CIPHER_SIZE              16          // Размер блока для блочного шифра (16 байт, 128 бит)

// Класс без декодера
class CIridiumCipher
{
public:
   // Конструктор/деструктор
   CIridiumCipher()
      {}
   virtual ~CIridiumCipher()
      {}

   // Инициализация кодера и декодера
   virtual bool Init(const u8* in_pData)
      { return false; }

   // Получение типа
   virtual u8 GetType()
      { return IRIDIUM_CRYPTION_NONE; }

   // Получение размера блока
   virtual size_t GetBlockSize()
      { return 0; }

   // Включение/выключение использования вектора инициализации
#if defined(IRIDIUM_ENABLE_IV)
   virtual void EnableIV(bool in_bEnable)
      {}
#endif

   // Кодирование/декодирование буфера
   virtual bool Encode(u8* in_pBuffer, size_t in_stSize, size_t& out_rMaxSize)
      { return 0; }
   virtual bool Decode(u8* in_pBuffer, size_t in_stSize)
      { return 0; }
};
#endif   // _C_IRIDIUM_CIPHER_H_INCLUDED_
