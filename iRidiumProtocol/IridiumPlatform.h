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
#ifndef _IRIDIUM_PLATFORM_H_INCLUDED_
#define _IRIDIUM_PLATFORM_H_INCLUDED_

// Windows x86/x86-64
#if defined(_WIN32) || defined(WIN32) || defined(WIN64) || defined(_WIN64)
   #define IRIDIUM_WINDOWS_PLATFORM
   #ifndef WIN32_LEAN_AND_MEAN
      #define WIN32_LEAN_AND_MEAN
   #endif

   #include <windows.h>

   #if defined(WIN64) || defined(_WIN64)
      #define IRIDIUM_ARCH_64
   #else
      #define IRIDIUM_ARCH_32
   #endif
#endif

// Mac OSX & iOS
#if defined(__APPLE__)
   #include <string.h>
   #include <sys/types.h>

   #include "TargetConditionals.h"
   #if TARGET_OS_IOS
      #define IRIDIUM_IOS_PLATFORM
      //#warning IOS
   #elif TARGET_OS_TV
      #define IRIDIUM_IOS_PLATFORM
      //#warning TV
   #elif TARGET_OS_WATCH
      #define IRIDIUM_IOS_PLATFORM
      //#warning WATCH
   #else
      #define IRIDIUM_OSX_PLATFORM
      //#warning OSX
   #endif

   #if __LP64__
      #define IRIDIUM_ARCH_64
   #else
      #define IRIDIUM_ARCH_32
   #endif
#endif

// Android
#if defined(__ANDROID__)
   #include <string.h>
   #define IRIDIUM_ANDROID_PLATFORM
#endif   // defined(__ANDROID__)

// AVR или Arduino
#if defined(AVR)
   #include <string.h>
   #define IRIDIUM_AVR_PLATFORM
#endif   // defined(AVR)

#if defined(__GNUC__)
#endif

#if defined(ARM_MATH_CM0) || defined(ARM_MATH_CM3) || defined(ARM_MATH_CM4) || defined(__TARGET_CPU_CORTEX_M0) || defined(__TARGET_CPU_CORTEX_M3) || defined(__TARGET_CPU_CORTEX_M4) || defined(__TARGET_CPU_CORTEX_M4_FP)
   #include <string.h>
   #define IRIDIUM_CORTEX_M_PLATFORM
#endif   // defined(ARM_MATH_CM0) || defined(ARM_MATH_CM3) || defined(ARM_MATH_CM4) || defined(__TARGET_CPU_CORTEX_M0) || defined(__TARGET_CPU_CORTEX_M3) || defined(__TARGET_CPU_CORTEX_M4) || defined(__TARGET_CPU_CORTEX_M4_FP)

// Linux
#if !defined(IRIDIUM_ANDROID_PLATFORM) && !defined(IRIDIUM_WINDOWS_PLATFORM) && !defined(IRIDIUM_OSX_PLATFORM) && !defined(IRIDIUM_IOS_PLATFORM) && !defined(IRIDIUM_AVR_PLATFORM) && !defined(IRIDIUM_CORTEX_M_PLATFORM)
   #define IRIDIUM_LINUX_PLATFORM
   #include <string.h>
#endif   // define IRIDIUM_LINUX_PLATFORM

// Тип операционной системы
enum eOSType
{
   OST_NONE = 0,                                   // Без операционной системы
   OST_IOS,                                        // Apple iOS
   OST_WINDOWS,                                    // Windows x86/x86-64
   OST_WINDOWS_MOBILE,                             // Windows CE
   OST_MAC_OSX,                                    // Mac OS X
   OST_LINUX,                                      // Linux
   OST_ANDROID,                                    // Android
};

// Тип процессора
enum eProcessorType
{
   PT_UNKNOWN = 0,                                 // Неизвестная архитектура
   PT_X86,                                         // Intel x86
   PT_X86_64,                                      // Intel/AMD x86-64
   PT_ARM,                                         // ARM (Сюда же входят и микроконтроллеры на базе чипа Cortex-M0, Cortex-M3, Cortex-M4)
   PT_ARM_64,                                      // ARM 64
   PT_MIPS,                                        // Mips
   PT_AVR,                                         // AVR
};

// Класс устройства
enum eDeviceClassType
{
   DCT_UNKNOWN = 0,                                // Неизвестный тип устройства
   DCT_PHONE,                                      // Телефон
   DCT_TABLET,                                     // Планшет
   DCT_CONSOLE,                                    // Консоль (Apple TV)
   DCT_PC,                                         // Персональный компьютер
   DCT_PLC,                                        // Программируемый логический контроллер (сервер)
   DCT_MICROCONTROLLER,                            // Микроконтроллер
};

#if defined PLATFORM_x86
#define LINUX_TARGET_x86
#elif defined PLATFORM_x64
#define LINUX_TARGET_x64
#elif defined PLATFORM_RPI
#define LINUX_TARGET_RPI
#elif defined PLATFORM_GC
#define LINUX_TARGET_GC
#elif defined PLATFORM_LM || defined PLATFORM_LM2
#define LINUX_TARGET_LM
#elif defined PLATFORM_UMC
#define LINUX_TARGET_UMC
#elif defined PLATFORM_WB
#define LINUX_TARGET_WB
#elif defined PLATFORM_BS
#define LINUX_TARGET_BS
#elif defined PLATFORM_UC
#define LINUX_TARGET_UC
#elif defined PLATFORM_AT
#define LINUX_TARGET_AT
#elif defined PLATFORM_AA
#define LINUX_TARGET_AA
#else
#ifdef IRIDIUM_LINUX_PLATFORM
#warning "Linux defined but subplatform not"
#endif
#endif

#if defined LINUX_TARGET_RPI
#define IRIDIUM_OPENGL_ES
#endif

#endif   // _IRIDIUM_PLATFORM_H_INCLUDED_
