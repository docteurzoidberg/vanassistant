#pragma once

#include <cstdint>

#include <DrzGraphics.h>

using namespace drz;

const uint8_t Solid_Mono4pt7bBitmaps[] = {
  0x00, 0xF8, 0xB6, 0x80, 0xFF, 0x5B, 0x26, 0xD0, 0xE2, 0x4A, 0xB0, 0xEA,
  0xFE, 0xE0, 0x6A, 0xA4, 0xA5, 0x60, 0xDF, 0x00, 0x5D, 0x00, 0xC0, 0xE0,
  0x80, 0x29, 0x29, 0x00, 0xFF, 0xDE, 0xC9, 0x24, 0xF1, 0x24, 0xE0, 0x71,
  0x21, 0x70, 0x99, 0x71, 0x10, 0xEE, 0x11, 0x70, 0xEE, 0x99, 0xF0, 0x72,
  0x24, 0x40, 0xF9, 0x69, 0xF0, 0xF9, 0x71, 0x70, 0x90, 0x98, 0x0A, 0x44,
  0x40, 0x43, 0x80, 0x11, 0x15, 0x00, 0xF1, 0x20, 0x00, 0xFF, 0xFC, 0x75,
  0x97, 0x90, 0xF9, 0xE9, 0xF0, 0xE8, 0x88, 0xE0, 0xF9, 0x99, 0xF0, 0xE8,
  0xE8, 0xE0, 0xE8, 0xE8, 0x80, 0xE8, 0xB9, 0xF0, 0x99, 0xF9, 0x90, 0x49,
  0x24, 0x71, 0x11, 0x70, 0xAC, 0x8C, 0xA0, 0x88, 0x88, 0xE0, 0x9F, 0xF9,
  0x90, 0x9D, 0xDB, 0x90, 0xF9, 0x99, 0xF0, 0xF9, 0xE8, 0x80, 0xF6, 0xDA,
  0x90, 0xF9, 0xEA, 0x90, 0xE8, 0x61, 0x70, 0x49, 0x24, 0x99, 0x99, 0xF0,
  0x19, 0x66, 0x20, 0x99, 0xB5, 0x50, 0x17, 0x2D, 0x40, 0xB5, 0x24, 0x72,
  0x44, 0xE0, 0xF2, 0x49, 0x38, 0x12, 0x24, 0x40, 0xE4, 0x92, 0x78, 0x18,
  0x80, 0xE0, 0x11, 0x10, 0x77, 0x9F, 0x8F, 0x99, 0xF0, 0xE8, 0x8E, 0x1F,
  0x99, 0xF0, 0xFF, 0x8E, 0xDE, 0x48, 0xF9, 0x99, 0x77, 0x8F, 0x99, 0x90,
  0x41, 0x24, 0x80, 0x10, 0x71, 0x11, 0x17, 0x89, 0xAC, 0xA0, 0x49, 0x24,
  0xFD, 0x6B, 0x50, 0xF9, 0x99, 0xF9, 0x9F, 0xF9, 0x99, 0xE8, 0xF9, 0x99,
  0x71, 0xF6, 0x40, 0xE6, 0x17, 0x9A, 0x4C, 0x99, 0x9F, 0x09, 0x62, 0xAD,
  0x6B, 0xF0, 0xA9, 0x50, 0x99, 0x99, 0x77, 0x72, 0x4E, 0x34, 0x4C, 0x44,
  0x30, 0xFE, 0xC2, 0x23, 0x22, 0xC0, 0x8C };

const fontglyph Solid_Mono4pt7bGlyphs[] = {
  {     0,   1,   1,   5,    0,    0 },   // 0x20 ' '
  {     1,   1,   5,   5,    2,   -4 },   // 0x21 '!'
  {     2,   3,   3,   5,    1,   -4 },   // 0x22 '"'
  {     4,   2,   4,   4,    1,   -3 },   // 0x23 '#'
  {     5,   3,   7,   3,    0,   -5 },   // 0x24 '$'
  {     8,   4,   5,   5,    1,   -4 },   // 0x25 '%'
  {    11,   3,   5,   3,    0,   -4 },   // 0x26 '&'
  {    13,   1,   3,   5,    2,   -4 },   // 0x27 '''
  {    14,   2,   7,   5,    1,   -5 },   // 0x28 '('
  {    16,   2,   7,   5,    2,   -5 },   // 0x29 ')'
  {    18,   3,   3,   5,    1,   -3 },   // 0x2A '*'
  {    20,   3,   3,   5,    1,   -3 },   // 0x2B '+'
  {    22,   1,   2,   5,    2,    0 },   // 0x2C ','
  {    23,   3,   1,   5,    1,   -2 },   // 0x2D '-'
  {    24,   1,   1,   5,    2,    0 },   // 0x2E '.'
  {    25,   3,   7,   5,    1,   -5 },   // 0x2F '/'
  {    28,   3,   5,   5,    0,   -4 },   // 0x30 '0'
  {    30,   3,   5,   5,    1,   -4 },   // 0x31 '1'
  {    32,   4,   5,   5,    0,   -4 },   // 0x32 '2'
  {    35,   4,   5,   5,    0,   -4 },   // 0x33 '3'
  {    38,   4,   5,   5,    0,   -4 },   // 0x34 '4'
  {    41,   4,   5,   5,    0,   -4 },   // 0x35 '5'
  {    44,   4,   5,   5,    0,   -4 },   // 0x36 '6'
  {    47,   4,   5,   5,    0,   -4 },   // 0x37 '7'
  {    50,   4,   5,   5,    0,   -4 },   // 0x38 '8'
  {    53,   4,   5,   5,    0,   -4 },   // 0x39 '9'
  {    56,   1,   4,   5,    2,   -3 },   // 0x3A ':'
  {    57,   1,   5,   5,    2,   -3 },   // 0x3B ';'
  {    58,   3,   6,   5,    1,   -5 },   // 0x3C '<'
  {    61,   3,   3,   5,    1,   -3 },   // 0x3D '='
  {    63,   3,   6,   5,    1,   -5 },   // 0x3E '>'
  {    66,   4,   5,   4,    0,   -4 },   // 0x3F '?'
  {    69,   3,   5,   3,    0,   -4 },   // 0x40 '@'
  {    71,   4,   6,   5,    0,   -4 },   // 0x41 'A'
  {    74,   4,   5,   4,    0,   -4 },   // 0x42 'B'
  {    77,   4,   5,   4,    0,   -4 },   // 0x43 'C'
  {    80,   4,   5,   4,    0,   -4 },   // 0x44 'D'
  {    83,   4,   5,   4,    0,   -4 },   // 0x45 'E'
  {    86,   4,   5,   4,    0,   -4 },   // 0x46 'F'
  {    89,   4,   5,   4,    0,   -4 },   // 0x47 'G'
  {    92,   4,   5,   4,    0,   -4 },   // 0x48 'H'
  {    95,   3,   5,   5,    1,   -4 },   // 0x49 'I'
  {    97,   4,   5,   4,    0,   -4 },   // 0x4A 'J'
  {   100,   4,   5,   4,    0,   -4 },   // 0x4B 'K'
  {   103,   4,   5,   4,    0,   -4 },   // 0x4C 'L'
  {   106,   4,   5,   4,    0,   -4 },   // 0x4D 'M'
  {   109,   4,   5,   4,    0,   -4 },   // 0x4E 'N'
  {   112,   4,   5,   4,    0,   -4 },   // 0x4F 'O'
  {   115,   4,   5,   4,    0,   -4 },   // 0x50 'P'
  {   118,   3,   7,   3,    0,   -4 },   // 0x51 'Q'
  {   121,   4,   5,   4,    0,   -4 },   // 0x52 'R'
  {   124,   4,   5,   4,    0,   -4 },   // 0x53 'S'
  {   127,   3,   5,   5,    1,   -4 },   // 0x54 'T'
  {   129,   4,   5,   4,    0,   -4 },   // 0x55 'U'
  {   132,   4,   5,   5,    0,   -4 },   // 0x56 'V'
  {   135,   4,   5,   5,    0,   -4 },   // 0x57 'W'
  {   138,   3,   6,   5,    1,   -5 },   // 0x58 'X'
  {   141,   3,   5,   3,    0,   -4 },   // 0x59 'Y'
  {   143,   4,   5,   4,    0,   -4 },   // 0x5A 'Z'
  {   146,   3,   7,   5,    1,   -5 },   // 0x5B '['
  {   149,   3,   7,   5,    1,   -5 },   // 0x5C '\'
  {   152,   3,   7,   5,    1,   -5 },   // 0x5D ']'
  {   155,   3,   3,   5,    1,   -5 },   // 0x5E '^'
  {   157,   3,   1,   5,    1,    0 },   // 0x5F '_'
  {   158,   3,   4,   5,    1,   -5 },   // 0x60 '`'
  {   160,   4,   4,   4,    0,   -3 },   // 0x61 'a'
  {   162,   4,   5,   4,    0,   -4 },   // 0x62 'b'
  {   165,   4,   4,   4,    0,   -3 },   // 0x63 'c'
  {   167,   4,   5,   4,    0,   -4 },   // 0x64 'd'
  {   170,   4,   4,   4,    0,   -3 },   // 0x65 'e'
  {   172,   3,   5,   5,    1,   -4 },   // 0x66 'f'
  {   174,   4,   6,   4,    0,   -3 },   // 0x67 'g'
  {   177,   4,   5,   4,    0,   -4 },   // 0x68 'h'
  {   180,   3,   6,   5,    1,   -5 },   // 0x69 'i'
  {   183,   4,   8,   4,    0,   -5 },   // 0x6A 'j'
  {   187,   4,   5,   4,    0,   -4 },   // 0x6B 'k'
  {   190,   3,   5,   5,    1,   -4 },   // 0x6C 'l'
  {   192,   5,   4,   5,    0,   -3 },   // 0x6D 'm'
  {   195,   4,   4,   4,    0,   -3 },   // 0x6E 'n'
  {   197,   4,   4,   4,    0,   -3 },   // 0x6F 'o'
  {   199,   4,   6,   4,    0,   -3 },   // 0x70 'p'
  {   202,   4,   6,   4,    0,   -3 },   // 0x71 'q'
  {   205,   3,   4,   4,    1,   -3 },   // 0x72 'r'
  {   207,   4,   4,   4,    0,   -3 },   // 0x73 's'
  {   209,   3,   5,   5,    1,   -4 },   // 0x74 't'
  {   211,   4,   4,   4,    0,   -3 },   // 0x75 'u'
  {   213,   4,   4,   5,    0,   -3 },   // 0x76 'v'
  {   215,   5,   4,   5,    0,   -3 },   // 0x77 'w'
  {   218,   3,   4,   5,    1,   -3 },   // 0x78 'x'
  {   220,   4,   6,   4,    0,   -3 },   // 0x79 'y'
  {   223,   4,   4,   5,    1,   -3 },   // 0x7A 'z'
  {   225,   4,   7,   4,    0,   -5 },   // 0x7B '{'
  {   229,   1,   7,   5,    2,   -5 },   // 0x7C '|'
  {   230,   4,   7,   5,    0,   -5 },   // 0x7D '}'
  {   234,   3,   2,   5,    1,   -3 } }; // 0x7E '~'

const font Solid_Mono4pt7b = {
  (uint8_t  *)Solid_Mono4pt7bBitmaps,
  (fontglyph *)Solid_Mono4pt7bGlyphs,
  0x20, 0x7E, 11 };

// Approx. 907 bytes
