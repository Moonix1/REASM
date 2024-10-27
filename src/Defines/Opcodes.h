#pragma once

#include <stdint.h>

typedef uint8_t Byte;

constexpr Byte MOV_IM				= 0x00;
constexpr Byte MOV_R				= 0x01;
constexpr Byte MOV_A				= 0x02;
constexpr Byte MOV_ADDR_IM			= 0x03;
constexpr Byte MOV_ADDR_R			= 0x04;
constexpr Byte MOV_ADDR_A			= 0x05;

constexpr Byte ADD_R				= 0x06;
constexpr Byte ADD_I				= 0x07;
constexpr Byte ADD_RI				= 0x08;
constexpr Byte ADD_IR				= 0x09;

constexpr Byte SUB_R				= 0x0A;
constexpr Byte SUB_I				= 0x0B;
constexpr Byte SUB_RI				= 0x0C;
constexpr Byte SUB_IR				= 0x0D;

constexpr Byte ADC_R				= 0x0E;
constexpr Byte ADC_I				= 0x0F;
constexpr Byte ADC_RI				= 0x10;
constexpr Byte ADC_IR				= 0x11;

constexpr Byte SBC_R				= 0x12;
constexpr Byte SBC_I				= 0x13;
constexpr Byte SBC_RI				= 0x14;
constexpr Byte SBC_IR				= 0x15;

constexpr Byte MUL_R				= 0x16;
constexpr Byte MUL_I				= 0x17;
constexpr Byte MUL_RI				= 0x18;
constexpr Byte MUL_IR				= 0x19;

constexpr Byte DIV_R				= 0x1A;
constexpr Byte DIV_I				= 0x1B;
constexpr Byte DIV_RI				= 0x1C;
constexpr Byte DIV_IR				= 0x1D;

constexpr Byte DEC_R				= 0x1E;
constexpr Byte INC_R				= 0x1F;

constexpr Byte AND_R				= 0x20;
constexpr Byte AND_I				= 0x21;
constexpr Byte AND_RI				= 0x22;
constexpr Byte AND_IR				= 0x23;

constexpr Byte OR_R					= 0x24;
constexpr Byte OR_I					= 0x25;
constexpr Byte OR_RI				= 0x26;
constexpr Byte OR_IR				= 0x27;

constexpr Byte XOR_R				= 0x28;
constexpr Byte XOR_I				= 0x29;
constexpr Byte XOR_RI				= 0x2A;
constexpr Byte XOR_IR				= 0x2B;

constexpr Byte NOT_R				= 0x2C;

constexpr Byte SHL_R				= 0x2D;
constexpr Byte SHL_I				= 0x2E;
constexpr Byte SHL_RI				= 0x2F;
constexpr Byte SHL_IR				= 0x30;

constexpr Byte SHR_R				= 0x31;
constexpr Byte SHR_I				= 0x32;
constexpr Byte SHR_RI				= 0x33;
constexpr Byte SHR_IR				= 0x34;

constexpr Byte CMP_R				= 0x35;
constexpr Byte CMP_RI				= 0x36;

constexpr Byte IGT_R				= 0x37;
constexpr Byte IGT_RI				= 0x38;

constexpr Byte ILT_R				= 0x39;
constexpr Byte ILT_RI				= 0x3A;

constexpr Byte IGE_R				= 0x3B;
constexpr Byte IGE_RI				= 0x3C;

constexpr Byte ILE_R				= 0x3D;
constexpr Byte ILE_RI				= 0x3E;

constexpr Byte JMP					= 0x3F;
constexpr Byte JNZ					= 0x40;
constexpr Byte JZ					= 0x41;
constexpr Byte JNS					= 0x42;
constexpr Byte JS					= 0x43;
constexpr Byte JNC					= 0x44;
constexpr Byte JC					= 0x45;

// Stack
constexpr Byte PUSH_IM_W			= 0x46;
constexpr Byte PUSH_IM_B			= 0x47;
constexpr Byte PUSH_R				= 0x48;

constexpr Byte POP_R				= 0x49;
constexpr Byte POP_A				= 0x4A;

constexpr Byte HLT					= 0xFF;