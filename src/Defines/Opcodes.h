#pragma once

#include <stdint.h>

constexpr uint8_t MOV_IM				= 0x00;
constexpr uint8_t MOV_R					= 0x01;
constexpr uint8_t MOV_A					= 0x02;
constexpr uint8_t MOV_ADDR_IM			= 0x03;
constexpr uint8_t MOV_ADDR_R			= 0x04;
constexpr uint8_t MOV_ADDR_A			= 0x05;

constexpr uint8_t ADD_R					= 0x06;
constexpr uint8_t ADD_I					= 0x07;
constexpr uint8_t ADD_RI				= 0x08;
constexpr uint8_t ADD_IR				= 0x09;

constexpr uint8_t SUB_R					= 0x0A;
constexpr uint8_t SUB_I					= 0x0B;
constexpr uint8_t SUB_RI				= 0x0C;
constexpr uint8_t SUB_IR				= 0x0D;

constexpr uint8_t ADC_R					= 0x0E;
constexpr uint8_t ADC_I					= 0x0F;
constexpr uint8_t ADC_RI				= 0x10;
constexpr uint8_t ADC_IR				= 0x11;

constexpr uint8_t SBC_R					= 0x12;
constexpr uint8_t SBC_I					= 0x13;
constexpr uint8_t SBC_RI				= 0x14;
constexpr uint8_t SBC_IR				= 0x15;

constexpr uint8_t MUL_R					= 0x16;
constexpr uint8_t MUL_I					= 0x17;
constexpr uint8_t MUL_RI				= 0x18;
constexpr uint8_t MUL_IR				= 0x19;

constexpr uint8_t DIV_R					= 0x1A;
constexpr uint8_t DIV_I					= 0x1B;
constexpr uint8_t DIV_RI				= 0x1C;
constexpr uint8_t DIV_IR				= 0x1D;

constexpr uint8_t AND_R					= 0x1E;
constexpr uint8_t AND_I					= 0x1F;
constexpr uint8_t AND_RI				= 0x20;
constexpr uint8_t AND_IR				= 0x21;

constexpr uint8_t OR_R					= 0x22;
constexpr uint8_t OR_I					= 0x23;
constexpr uint8_t OR_RI					= 0x24;
constexpr uint8_t OR_IR					= 0x25;

constexpr uint8_t XOR_R					= 0x26;
constexpr uint8_t XOR_I					= 0x27;
constexpr uint8_t XOR_RI				= 0x28;
constexpr uint8_t XOR_IR				= 0x29;

constexpr uint8_t NOT_R					= 0x2A;

constexpr uint8_t SHL_R					= 0x2B;
constexpr uint8_t SHL_I					= 0x2C;
constexpr uint8_t SHL_RI				= 0x2D;
constexpr uint8_t SHL_IR				= 0x2E;

constexpr uint8_t SHR_R					= 0x2F;
constexpr uint8_t SHR_I					= 0x30;
constexpr uint8_t SHR_RI				= 0x31;
constexpr uint8_t SHR_IR				= 0x32;

constexpr uint8_t CMP_R					= 0x33;
constexpr uint8_t CMP_RI				= 0x34;

constexpr uint8_t JMP					= 0x35;
constexpr uint8_t JNZ					= 0x36;
constexpr uint8_t JZ					= 0x37;
constexpr uint8_t JNS					= 0x38;
constexpr uint8_t JS					= 0x39;
constexpr uint8_t JNC					= 0x3A;
constexpr uint8_t JC					= 0x3B;

// Stack
constexpr uint8_t PUSH_IM_W				= 0x3C;
constexpr uint8_t PUSH_IM_B				= 0x3D;
constexpr uint8_t PUSH_R				= 0x3E;

constexpr uint8_t POP_R					= 0x3F;
constexpr uint8_t POP_A					= 0x40;

constexpr uint8_t HLT					= 0xFF;