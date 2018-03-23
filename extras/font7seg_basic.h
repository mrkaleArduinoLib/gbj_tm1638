#ifndef GBJ_FONT7SEG_H
#define GBJ_FONT7SEG_H
#define GBJ_FONT7SEG_VERSION "GBJ_FONT7SEG_BASIC 1.0.0"

#if defined(__AVR__)
	#include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

const uint8_t gbjFont7segTable[] PROGMEM =
{
  // ASCII code, Font mask
  0x20, 0b00000000 // Space
, 0x27, 0b00100000 // '
, 0x28, 0b00111001 // (
, 0x29, 0b00001111 // )
, 0x2d, 0b01000000 // -
// Digits
, 0x30, 0b00111111 // 0
, 0x31, 0b00000110 // 1
, 0x32, 0b01011011 // 2
, 0x33, 0b01001111 // 3
, 0x34, 0b01100110 // 4
, 0x35, 0b01101101 // 5
, 0x36, 0b01111101 // 6
, 0x37, 0b00000111 // 7
, 0x38, 0b01111111 // 8
, 0x39, 0b01101111 // 9
// Uppercase letters
, 0x41, 0b01110111 // A
, 0x42, 0b01111100 // B = b
, 0x43, 0b00111001 // C
, 0x44, 0b01011110 // D = d
, 0x45, 0b01111001 // E
, 0x46, 0b01110001 // F
, 0x48, 0b01110110 // H
, 0x49, 0b00110000 // I
, 0x4a, 0b00001110 // J
, 0x4c, 0b00111000 // L
, 0x4f, 0b01010100 // N = n
, 0x4f, 0b01011100 // O = o
, 0x50, 0b01110011 // P
, 0x52, 0b01010000 // R = r
, 0x53, 0b01101101 // S = 5
, 0x54, 0b01111000 // T = t
, 0x55, 0b00111110 // U
//
, 0x5b, 0b00111001 // [
, 0x5d, 0b00001111 // ]
, 0x5f, 0b00001000 // _
// Lowercase letters
, 0x61, 0b01110111 // a = A
, 0x62, 0b01111100 // b
, 0x63, 0b01011000 // c
, 0x64, 0b01011110 // d
, 0x65, 0b01111001 // e = E
, 0x66, 0b01110001 // f = F
, 0x68, 0b01110100 // h
, 0x69, 0b00010000 // i
, 0x6a, 0b00001110 // j = J
, 0x6c, 0b00111000 // l = L
, 0x6e, 0b01010100 // n
, 0x6f, 0b01011100 // o
, 0x70, 0b01110011 // p = P
, 0x72, 0b01010000 // r
, 0x73, 0b01101101 // s = S
, 0x74, 0b01111000 // t
, 0x75, 0b00011100 // u
};

#endif
