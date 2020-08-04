#ifndef _MEMORY_MAP_H_INCLUDED_
#define _MEMORY_MAP_H_INCLUDED_

// Карта расположения данных для stm32f103c8t6
// [  0x8000000-0x8005FFF  Bootloader  загрузчик                  ]
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

#endif   // _MEMORY_MAP_H_INCLUDED_
