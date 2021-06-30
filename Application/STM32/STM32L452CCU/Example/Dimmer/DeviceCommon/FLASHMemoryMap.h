#ifndef _FLASH_MEMORY_MAP_H_INCLUDED_
#define _FLASH_MEMORY_MAP_H_INCLUDED_

// Границы загрузчика      - 18 кБ
#define BOOTLOADER_START   0x8000000
#define BOOTLOADER_END     0x8008FF0
#define BOOTLOADER_SIZE    ((BOOTLOADER_END - BOOTLOADER_START) + 1)

// Границы прошивки        - 491 кБ
#define FIRMWARE_START     0x8009000
#define FIRMWARE_END       0x803EFFF
#define FIRMWARE_SIZE      ((FIRMWARE_END - FIRMWARE_START) + 1)

// Границы энергонезависимой памяти - 4 кБ
#define EEPROM_START       0x803F000
#define EEPROM_END         0x803FFFF
#define EEPROM_SIZE        ((EEPROM_END - EEPROM_START) + 1)

#endif   // _FLASH_MEMORY_MAP_H_INCLUDED_
