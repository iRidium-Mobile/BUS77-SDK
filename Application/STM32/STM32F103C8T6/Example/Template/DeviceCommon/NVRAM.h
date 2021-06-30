#ifndef _NVRAM_H_INCLUDED_
#define _NVRAM_H_INCLUDED_

#include "IridiumTypes.h"

// Режим работы устройства
typedef enum bootloader_mode
{
   BOOTLOADER_MODE_RUN = 0,                        // Запуск прошивки
   BOOTLOADER_MODE_DOWNLOAD,                       // Загрузка прошивки
   BOOTLOADER_MODE_BOOT                            // Переход в режим прошивки
} eBootloaderMode;

// Идентификатор прошивки
typedef enum firmware_id
{
   EXAMPLE_ID  = 1,                                // Профиль 1
} eFirmwareID;

#define FIRMWARE_NAME                  "Firmware.bin"
   
// Общая структура хранения данных в энергонезависимой памяти
typedef struct nvram_common_s
{
   u32   m_u32EEPROMSize;                          // Размер энергонезависимой памяти (требуется для определения размера сохраняемой памяти)
   u8    m_u8Mode;                                 // Режим работы
   u32   m_u32FirmwareSize;                        // Размер прошивки
   u16   m_u16FirmwareCRC16;                       // Контрольная сумма прошивки
   u8    m_u8FirmwareID;                           // Идентификатор прошивки
   u16   m_u16FirmwareAddress;                     // Адрес устройства инициатора прошивки
   u16   m_u16FirmwareTID;                         // Идентификатр транзакции инициатора прошивки
   u8    m_u8LID;                                  // Локальный идентификатор устройства
   u8    m_aKey[32];                               // Ключ шифрования 256 бит
   u32   m_u32PIN;                                 // Персональное идентификационное число
   char  m_szDeviceName[64];                       // Имя устройства
} nvram_common_t;
   
#endif   // _NVRAM_H_INCLUDED_
