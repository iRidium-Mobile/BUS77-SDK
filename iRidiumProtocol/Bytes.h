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
#ifndef _BYTES_H_INCLUDED_
#define _BYTES_H_INCLUDED_

#include "IridiumTypes.h"

// Запись значений в буфер
u8* WriteU8(u8* in_pBuffer, u8 in_u8Value);        // Запись беззнакового 8 битного значения
u8* WriteS8(u8* in_pBuffer, s8 in_s8Value);        // Запись знакового 8 битного значения
u8* WriteU16LE(u8* in_pBuffer, u16 in_u16Value);   // Запись беззнакового 16 битного значения, в LE последовательности байт
u8* WriteU16BE(u8* in_pBuffer, u16 in_u16Value);   // Запись беззнакового 16 битного значения, в BE последовательности байт
u8* WriteS16LE(u8* in_pBuffer, s16 in_s16Value);   // Запись знакового 16 битного значения, в LE последовательности байт
u8* WriteS16BE(u8* in_pBuffer, s16 in_s16Value);   // Запись знакового 16 битного значения, в BE последовательности байт
u8* WriteU32LE(u8* in_pBuffer, u32 in_u32Value);   // Запись беззнакового 32 битного значения, в LE последовательности байт
u8* WriteU32BE(u8* in_pBuffer, u32 in_u32Value);   // Запись беззнакового 32 битного значения, в BE последовательности байт
u8* WriteS32LE(u8* in_pBuffer, s32 in_s32Value);   // Запись знакового 32 битного значения, в LE последовательности байт
u8* WriteS32BE(u8* in_pBuffer, s32 in_s32Value);   // Запись знакового 32 битного значения, в BE последовательности байт
u8* WriteF32LE(u8* in_pBuffer, f32 in_f32Value);   // Запись 32 битного значения с плавающей запятой, в LE последовательности байт
u8* WriteF32BE(u8* in_pBuffer, f32 in_f32Value);   // Запись 32 битного значения с плавающей запятой, в BE последовательности байт
u8* WriteU64LE(u8* in_pBuffer, u64 in_u64Value);   // Запись беззнакового 64 битного значения, в LE последовательности байт
u8* WriteU64BE(u8* in_pBuffer, u64 in_u64Value);   // Запись беззнакового 64 битного значения, в BE последовательности байт
u8* WriteS64LE(u8* in_pBuffer, s64 in_s64Value);   // Запись знакового 64 битного значения, в LE последовательности байт
u8* WriteS64BE(u8* in_pBuffer, s64 in_s64Value);   // Запись знакового 64 битного значения, в BE последовательности байт
u8* WriteF64LE(u8* in_pBuffer, f64 in_f64Value);   // Запись 64 битного значения с плавающей запятой, в LE последовательности байт
u8* WriteF64BE(u8* in_pBuffer, f64 in_f64Value);   // Запись 64 битного значения с плавающей запятой, в BE последовательности байт
u8* WriteSTBE(u8* in_pBuffer, size_t in_stValue);  // Запись значения в зависимости от битности системы, в BE последовательности байт
u8* WriteSTLE(u8* in_pBuffer, size_t in_stValue);  // Запись значения в зависимости от битности системы, в LE последовательности байт

// Чтение значений из буфера
u8* ReadU8(u8* in_pBuffer, u8& out_rValue);        // Чтение беззнакового 8 битного значения
u8* ReadS8(u8* in_pBuffer, s8& out_rValue);        // Чтение знакового 8 битного значения
u8* ReadU16LE(u8* in_pBuffer, u16& out_rValue);    // Чтение беззнакового 16 битного значения, в LE последовательности байт
u8* ReadU16BE(u8* in_pBuffer, u16& out_rValue);    // Чтение беззнакового 16 битного значения, в BE последовательности байт
u8* ReadS16LE(u8* in_pBuffer, s16& out_rValue);    // Чтение знакового 16 битного значения, в LE последовательности байт
u8* ReadS16BE(u8* in_pBuffer, s16& out_rValue);    // Чтение знакового 16 битного значения, в BE последовательности байт
u8* ReadU32LE(u8* in_pBuffer, u32& out_rValue);    // Чтение беззнакового 32 битного значения, в LE последовательности байт
u8* ReadU32BE(u8* in_pBuffer, u32& out_rValue);    // Чтение беззнакового 32 битного значения, в BE последовательности байт
u8* ReadS32LE(u8* in_pBuffer, s32& out_rValue);    // Чтение знакового 32 битного значения, в LE последовательности байт
u8* ReadS32BE(u8* in_pBuffer, s32& out_rValue);    // Чтение знакового 32 битного значения, в BE последовательности байт
u8* ReadF32LE(u8* in_pBuffer, f32& out_rValue);    // Чтение 32 битного значения с плавающей запятой, в LE последовательности байт
u8* ReadF32BE(u8* in_pBuffer, f32& out_rValue);    // Чтение 32 битного значения с плавающей запятой, в BE последовательности байт
u8* ReadU64LE(u8* in_pBuffer, u64& out_rValue);    // Чтение беззнакового 64 битного значения, в LE последовательности байт
u8* ReadU64BE(u8* in_pBuffer, u64& out_rValue);    // Чтение беззнакового 64 битного значения, в BE последовательности байт
u8* ReadS64LE(u8* in_pBuffer, s64& out_rValue);    // Чтение знакового 64 битного значения, в LE последовательности байт
u8* ReadS64BE(u8* in_pBuffer, s64& out_rValue);    // Чтение знакового 64 битного значения, в BE последовательности байт
u8* ReadF64LE(u8* in_pBuffer, f64& out_rValue);    // Чтение 64 битного значения с плавающей запятой, в LE последовательности байт
u8* ReadF64BE(u8* in_pBuffer, f64& out_rValue);    // Чтение 64 битного значения с плавающей запятой, в BE последовательности байт
u8* ReadSTBE(u8* in_pBuffer, size_t& out_rValue);  // Чтение значения в зависимости от битности системы, в BE последовательности байт
u8* ReadSTLE(u8* in_pBuffer, size_t& out_rValue);  // Чтение значения в зависимости от битности системы, в LE последовательности байт

//bool BytesUnitTest();

#endif // _BYTE_SWAP_H_INCLUDED_
