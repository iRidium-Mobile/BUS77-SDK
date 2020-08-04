#ifndef _EEPROM_H_INCLUDED_
#define _EEPROM_H_INCLUDED_

#include "IridiumTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

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
// R0
   // ---> version 1
   FWID_01_01 = 0,                           // 2 1W + 2 Dc
   FWID_01_02,                               // 4 Dc 
   FWID_01_03,                               // 2 Dc + 1 1W + 1 MotionDetector
   // ---> version 2
   FWID_01_09,                               // Dc + 1W + Sensor
   
// R5
   // ---> version 1
   FWID_02_01,                               // 4 Dc + 2 Relays
   FWID_02_02,                               // Leakage detecor: 1 blue Actuator + 4 Larnitech detectors
   FWID_02_03,                               // Thermostate for office iRidium
   FWID_02_04,                               // Leakage detecor: 1 unknow Actuator + 2 elemental detectors + 1 Dc
   FWID_02_05,                               // Leakage detecor: 1 blue Actuator + 2 elemental detectors
   FWID_02_06,                               // Leakage detecor: 1 blue Actuator + 4 Neptun detectors
   FWID_02_07,                               // Gate for WB Modbus
   FWID_02_08,                               // Leakage detecor: 1 blue Actuator + 2 H2O Kontakt isp 2 Normal_open + 1 Dc
   // ---> version 2
   FWID_02_09,                               // Dc + 1W + Sensor + 2 relays

// R10
   FWID_03_01,                               // Floor Heating

// WB-MR6C/B77
   FWID_04_01,                               // 7 Dc + 6 Relays

// WB-MRPS6/S/B77
   FWID_05_01,                               // 6 relays

// STM32 F103
   FWID_71_01,                               // Gate BUS77
   FWID_71_02,                               // Gate on base STM32F103C8T6 for WB-MSW v3

// STM32 F2
   FWID_72_01,                               // Gate BUS77 TCP/UDP

// STM32 F407
   FWID_73_01,                               // Gate BUS77 TCP/UDP

} eFirmwareID;

#define FIRMWARE_NAME                  "Firmware.bin"
#define RS485_NAME                     "rs485"
   
// Общая структура хранения данных в энергонезависимой памяти
typedef struct eeprom_common_s
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
} eeprom_common_t;

// Инициализация энергонезависимой памяти
void EEPROM_Init(size_t in_stStart, size_t in_stEnd);
   
// Буферизация EEPROM
void EEPROM_SetBuffer(u8* in_pBuffer, size_t in_stSize);
void EEPROM_NeedSaveBuffer(void);
bool EEPROM_ForceSaveBuffer(void);
void EEPROM_WorkBuffer(void);
   
#ifdef __cplusplus
}
#endif
#endif   // _EEPROM_H_INCLUDED_
