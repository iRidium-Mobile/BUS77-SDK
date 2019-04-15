#ifndef _MEMORY_MAP_H_INCLUDED_
#define _MEMORY_MAP_H_INCLUDED_

// Карта расположения данных для stm32f103c8t6
// [  0x8000000-0x8005FFF  Bootloader  загрузщик                  ]
// [  0x8006000-0x800FBFF  Firmware    прошивка                   ]
// [  0x800FC00-0x800FFFF  EEPROM      энергонезависимая память   ]

// Границы загрузчика
#define BOOTLOADER_START   0x8000000
#define BOOTLOADER_END     0x8005FFF
#define BOOTLOADER_SIZE    ((BOOTLOADER_END - BOOTLOADER_START) + 1)

// Границы прошивки
#define FIRMWARE_START     0x8006000
#define FIRMWARE_END       0x800FBFF
#define FIRMWARE_SIZE      ((FIRMWARE_END - FIRMWARE_START) + 1)

// Границы энергонезависимой памяти
#define EEPROM_START       0x800FC00
#define EEPROM_END         0x800FFFF
#define EEPROM_SIZE        ((EEPROM_END - EEPROM_START) + 1)

///////////////////////////////////////////////////
// Параметры загрузчика
///////////////////////////////////////////////////
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
   FIRMWARE_ID_R0_4_BUTTON = 0,                       // Прошивка с 4 кнопками
   FIRMWARE_ID_R0_2_BUTTON_2_ONE_WIRE,                // Прошивка с 2 кнопками и 2 1-wire
   FIRMWARE_ID_R5_2_BUTTON_2_ONE_WIRE_2_RELAY,        // Прошивка с 2 кнопками, 2 1-wire и 2 реле
   FIRMWARE_ID_R5_2_BUTTON_2_ONE_WIRE_2_RELAY_SMART,  // Прошивка с 2 кнопками, 2 1-wire и 2 реле
   FIRMWARE_ID_R10_2_BUTTON_2_ONE_WIRE_1_RELAY_SMART, // Прошивка с 2 кнопками, 2 1-wire и 1 реле
   FIRMWARE_ID_WB_7_BUTTON_6_RELAY_SMART,             // Прошивка с 7 кнопками, 6 реле
   FIRMWARE_ID_WB_6_RELAY_SMART,                      // Прошивка с 6 реле
} eFirmwareID;

#define FIRMWARE_NAME                  "Firmware.bin"

#define MAX_DEVICE_NAME_SIZE           64             // Максимальная длинна имени устройства

// Общие данные
#define EEPROM_U8_MODE                 0                                            // Режим работы устройства
#define EEPROM_U32_FIRMWARE_SIZE       (EEPROM_U8_MODE + 1)                         // Размер прошивки
#define EEPROM_U16_FIRMWARE_CRC16      (EEPROM_U32_FIRMWARE_SIZE + 4)               // Контрольная сумма прошивки
#define EEPROM_U8_FIRMWARE_ID          (EEPROM_U16_FIRMWARE_CRC16 + 2)              // Идентификатор прошивки
#define EEPROM_U16_FIRMWARE_ADDRESS    (EEPROM_U8_FIRMWARE_ID + 1)                  // Адрес
#define EEPROM_U16_FIRMWARE_TID        (EEPROM_U16_FIRMWARE_ADDRESS + 2)            // Идентификатор транзакции
#define EEPROM_U8_LID                  (EEPROM_U16_FIRMWARE_TID + 2)                // Состояние сброса состояния
#define EEPROM_KEY                     (EEPROM_U8_LID + 1)                          // 256 битный ключ для блочного шифра
#define EEPROM_U32_PIN                 (EEPROM_KEY + BLOCK_CIPHER_KEY_SIZE)         // PIN код
#define EEPROM_DEVICE_NAME             (EEPROM_U32_PIN + 4)                         // Имя устройства
#define EEPROM_START_DEVICE_DATA       (EEPROM_DEVICE_NAME + MAX_DEVICE_NAME_SIZE)  // Начало данных устройства

// Максимальное количество байт
#define EEPROM_MAX                     768

#endif   // _MEMORY_MAP_H_INCLUDED_
