/* auto-generated on 2025-06-26 23:03:07 -0400. Do not edit! */
/* begin file src\simdutf.cpp */
#include "simdutf.h"

  // We include base64_tables once.
/* begin file src\tables\base64_tables.h */
#ifndef SIMDUTF_BASE64_TABLES_H
#define SIMDUTF_BASE64_TABLES_H
#include <array>
#include <cstdint>

namespace simdutf {
namespace {
namespace tables {
namespace base64 {
namespace base64_default {

const char e0[256] = {
    'A', 'A', 'A', 'A', 'B', 'B', 'B', 'B', 'C', 'C', 'C', 'C', 'D', 'D', 'D',
    'D', 'E', 'E', 'E', 'E', 'F', 'F', 'F', 'F', 'G', 'G', 'G', 'G', 'H', 'H',
    'H', 'H', 'I', 'I', 'I', 'I', 'J', 'J', 'J', 'J', 'K', 'K', 'K', 'K', 'L',
    'L', 'L', 'L', 'M', 'M', 'M', 'M', 'N', 'N', 'N', 'N', 'O', 'O', 'O', 'O',
    'P', 'P', 'P', 'P', 'Q', 'Q', 'Q', 'Q', 'R', 'R', 'R', 'R', 'S', 'S', 'S',
    'S', 'T', 'T', 'T', 'T', 'U', 'U', 'U', 'U', 'V', 'V', 'V', 'V', 'W', 'W',
    'W', 'W', 'X', 'X', 'X', 'X', 'Y', 'Y', 'Y', 'Y', 'Z', 'Z', 'Z', 'Z', 'a',
    'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'd', 'd', 'd', 'd',
    'e', 'e', 'e', 'e', 'f', 'f', 'f', 'f', 'g', 'g', 'g', 'g', 'h', 'h', 'h',
    'h', 'i', 'i', 'i', 'i', 'j', 'j', 'j', 'j', 'k', 'k', 'k', 'k', 'l', 'l',
    'l', 'l', 'm', 'm', 'm', 'm', 'n', 'n', 'n', 'n', 'o', 'o', 'o', 'o', 'p',
    'p', 'p', 'p', 'q', 'q', 'q', 'q', 'r', 'r', 'r', 'r', 's', 's', 's', 's',
    't', 't', 't', 't', 'u', 'u', 'u', 'u', 'v', 'v', 'v', 'v', 'w', 'w', 'w',
    'w', 'x', 'x', 'x', 'x', 'y', 'y', 'y', 'y', 'z', 'z', 'z', 'z', '0', '0',
    '0', '0', '1', '1', '1', '1', '2', '2', '2', '2', '3', '3', '3', '3', '4',
    '4', '4', '4', '5', '5', '5', '5', '6', '6', '6', '6', '7', '7', '7', '7',
    '8', '8', '8', '8', '9', '9', '9', '9', '+', '+', '+', '+', '/', '/', '/',
    '/'};

const char e1[256] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
    'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C',
    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+',
    '/'};

const char e2[256] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
    'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C',
    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+',
    '/'};

const uint32_t d0[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x000000f8, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x000000fc,
    0x000000d0, 0x000000d4, 0x000000d8, 0x000000dc, 0x000000e0, 0x000000e4,
    0x000000e8, 0x000000ec, 0x000000f0, 0x000000f4, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00000004, 0x00000008, 0x0000000c, 0x00000010, 0x00000014, 0x00000018,
    0x0000001c, 0x00000020, 0x00000024, 0x00000028, 0x0000002c, 0x00000030,
    0x00000034, 0x00000038, 0x0000003c, 0x00000040, 0x00000044, 0x00000048,
    0x0000004c, 0x00000050, 0x00000054, 0x00000058, 0x0000005c, 0x00000060,
    0x00000064, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x00000068, 0x0000006c, 0x00000070, 0x00000074, 0x00000078,
    0x0000007c, 0x00000080, 0x00000084, 0x00000088, 0x0000008c, 0x00000090,
    0x00000094, 0x00000098, 0x0000009c, 0x000000a0, 0x000000a4, 0x000000a8,
    0x000000ac, 0x000000b0, 0x000000b4, 0x000000b8, 0x000000bc, 0x000000c0,
    0x000000c4, 0x000000c8, 0x000000cc, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};

const uint32_t d1[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x0000e003, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x0000f003,
    0x00004003, 0x00005003, 0x00006003, 0x00007003, 0x00008003, 0x00009003,
    0x0000a003, 0x0000b003, 0x0000c003, 0x0000d003, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00001000, 0x00002000, 0x00003000, 0x00004000, 0x00005000, 0x00006000,
    0x00007000, 0x00008000, 0x00009000, 0x0000a000, 0x0000b000, 0x0000c000,
    0x0000d000, 0x0000e000, 0x0000f000, 0x00000001, 0x00001001, 0x00002001,
    0x00003001, 0x00004001, 0x00005001, 0x00006001, 0x00007001, 0x00008001,
    0x00009001, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x0000a001, 0x0000b001, 0x0000c001, 0x0000d001, 0x0000e001,
    0x0000f001, 0x00000002, 0x00001002, 0x00002002, 0x00003002, 0x00004002,
    0x00005002, 0x00006002, 0x00007002, 0x00008002, 0x00009002, 0x0000a002,
    0x0000b002, 0x0000c002, 0x0000d002, 0x0000e002, 0x0000f002, 0x00000003,
    0x00001003, 0x00002003, 0x00003003, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};

const uint32_t d2[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x00800f00, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00c00f00,
    0x00000d00, 0x00400d00, 0x00800d00, 0x00c00d00, 0x00000e00, 0x00400e00,
    0x00800e00, 0x00c00e00, 0x00000f00, 0x00400f00, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00400000, 0x00800000, 0x00c00000, 0x00000100, 0x00400100, 0x00800100,
    0x00c00100, 0x00000200, 0x00400200, 0x00800200, 0x00c00200, 0x00000300,
    0x00400300, 0x00800300, 0x00c00300, 0x00000400, 0x00400400, 0x00800400,
    0x00c00400, 0x00000500, 0x00400500, 0x00800500, 0x00c00500, 0x00000600,
    0x00400600, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x00800600, 0x00c00600, 0x00000700, 0x00400700, 0x00800700,
    0x00c00700, 0x00000800, 0x00400800, 0x00800800, 0x00c00800, 0x00000900,
    0x00400900, 0x00800900, 0x00c00900, 0x00000a00, 0x00400a00, 0x00800a00,
    0x00c00a00, 0x00000b00, 0x00400b00, 0x00800b00, 0x00c00b00, 0x00000c00,
    0x00400c00, 0x00800c00, 0x00c00c00, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};

const uint32_t d3[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x003e0000, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x003f0000,
    0x00340000, 0x00350000, 0x00360000, 0x00370000, 0x00380000, 0x00390000,
    0x003a0000, 0x003b0000, 0x003c0000, 0x003d0000, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00010000, 0x00020000, 0x00030000, 0x00040000, 0x00050000, 0x00060000,
    0x00070000, 0x00080000, 0x00090000, 0x000a0000, 0x000b0000, 0x000c0000,
    0x000d0000, 0x000e0000, 0x000f0000, 0x00100000, 0x00110000, 0x00120000,
    0x00130000, 0x00140000, 0x00150000, 0x00160000, 0x00170000, 0x00180000,
    0x00190000, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x001a0000, 0x001b0000, 0x001c0000, 0x001d0000, 0x001e0000,
    0x001f0000, 0x00200000, 0x00210000, 0x00220000, 0x00230000, 0x00240000,
    0x00250000, 0x00260000, 0x00270000, 0x00280000, 0x00290000, 0x002a0000,
    0x002b0000, 0x002c0000, 0x002d0000, 0x002e0000, 0x002f0000, 0x00300000,
    0x00310000, 0x00320000, 0x00330000, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
} // namespace base64_default

namespace base64_url {

const char e0[256] = {
    'A', 'A', 'A', 'A', 'B', 'B', 'B', 'B', 'C', 'C', 'C', 'C', 'D', 'D', 'D',
    'D', 'E', 'E', 'E', 'E', 'F', 'F', 'F', 'F', 'G', 'G', 'G', 'G', 'H', 'H',
    'H', 'H', 'I', 'I', 'I', 'I', 'J', 'J', 'J', 'J', 'K', 'K', 'K', 'K', 'L',
    'L', 'L', 'L', 'M', 'M', 'M', 'M', 'N', 'N', 'N', 'N', 'O', 'O', 'O', 'O',
    'P', 'P', 'P', 'P', 'Q', 'Q', 'Q', 'Q', 'R', 'R', 'R', 'R', 'S', 'S', 'S',
    'S', 'T', 'T', 'T', 'T', 'U', 'U', 'U', 'U', 'V', 'V', 'V', 'V', 'W', 'W',
    'W', 'W', 'X', 'X', 'X', 'X', 'Y', 'Y', 'Y', 'Y', 'Z', 'Z', 'Z', 'Z', 'a',
    'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'd', 'd', 'd', 'd',
    'e', 'e', 'e', 'e', 'f', 'f', 'f', 'f', 'g', 'g', 'g', 'g', 'h', 'h', 'h',
    'h', 'i', 'i', 'i', 'i', 'j', 'j', 'j', 'j', 'k', 'k', 'k', 'k', 'l', 'l',
    'l', 'l', 'm', 'm', 'm', 'm', 'n', 'n', 'n', 'n', 'o', 'o', 'o', 'o', 'p',
    'p', 'p', 'p', 'q', 'q', 'q', 'q', 'r', 'r', 'r', 'r', 's', 's', 's', 's',
    't', 't', 't', 't', 'u', 'u', 'u', 'u', 'v', 'v', 'v', 'v', 'w', 'w', 'w',
    'w', 'x', 'x', 'x', 'x', 'y', 'y', 'y', 'y', 'z', 'z', 'z', 'z', '0', '0',
    '0', '0', '1', '1', '1', '1', '2', '2', '2', '2', '3', '3', '3', '3', '4',
    '4', '4', '4', '5', '5', '5', '5', '6', '6', '6', '6', '7', '7', '7', '7',
    '8', '8', '8', '8', '9', '9', '9', '9', '-', '-', '-', '-', '_', '_', '_',
    '_'};

const char e1[256] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '-', '_', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '-', '_', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
    'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_', 'A', 'B', 'C',
    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-',
    '_'};

const char e2[256] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '-', '_', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '-', '_', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
    'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_', 'A', 'B', 'C',
    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-',
    '_'};

const uint32_t d0[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x000000f8, 0x01ffffff, 0x01ffffff,
    0x000000d0, 0x000000d4, 0x000000d8, 0x000000dc, 0x000000e0, 0x000000e4,
    0x000000e8, 0x000000ec, 0x000000f0, 0x000000f4, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00000004, 0x00000008, 0x0000000c, 0x00000010, 0x00000014, 0x00000018,
    0x0000001c, 0x00000020, 0x00000024, 0x00000028, 0x0000002c, 0x00000030,
    0x00000034, 0x00000038, 0x0000003c, 0x00000040, 0x00000044, 0x00000048,
    0x0000004c, 0x00000050, 0x00000054, 0x00000058, 0x0000005c, 0x00000060,
    0x00000064, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x000000fc,
    0x01ffffff, 0x00000068, 0x0000006c, 0x00000070, 0x00000074, 0x00000078,
    0x0000007c, 0x00000080, 0x00000084, 0x00000088, 0x0000008c, 0x00000090,
    0x00000094, 0x00000098, 0x0000009c, 0x000000a0, 0x000000a4, 0x000000a8,
    0x000000ac, 0x000000b0, 0x000000b4, 0x000000b8, 0x000000bc, 0x000000c0,
    0x000000c4, 0x000000c8, 0x000000cc, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
const uint32_t d1[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x0000e003, 0x01ffffff, 0x01ffffff,
    0x00004003, 0x00005003, 0x00006003, 0x00007003, 0x00008003, 0x00009003,
    0x0000a003, 0x0000b003, 0x0000c003, 0x0000d003, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00001000, 0x00002000, 0x00003000, 0x00004000, 0x00005000, 0x00006000,
    0x00007000, 0x00008000, 0x00009000, 0x0000a000, 0x0000b000, 0x0000c000,
    0x0000d000, 0x0000e000, 0x0000f000, 0x00000001, 0x00001001, 0x00002001,
    0x00003001, 0x00004001, 0x00005001, 0x00006001, 0x00007001, 0x00008001,
    0x00009001, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x0000f003,
    0x01ffffff, 0x0000a001, 0x0000b001, 0x0000c001, 0x0000d001, 0x0000e001,
    0x0000f001, 0x00000002, 0x00001002, 0x00002002, 0x00003002, 0x00004002,
    0x00005002, 0x00006002, 0x00007002, 0x00008002, 0x00009002, 0x0000a002,
    0x0000b002, 0x0000c002, 0x0000d002, 0x0000e002, 0x0000f002, 0x00000003,
    0x00001003, 0x00002003, 0x00003003, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
const uint32_t d2[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00800f00, 0x01ffffff, 0x01ffffff,
    0x00000d00, 0x00400d00, 0x00800d00, 0x00c00d00, 0x00000e00, 0x00400e00,
    0x00800e00, 0x00c00e00, 0x00000f00, 0x00400f00, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00400000, 0x00800000, 0x00c00000, 0x00000100, 0x00400100, 0x00800100,
    0x00c00100, 0x00000200, 0x00400200, 0x00800200, 0x00c00200, 0x00000300,
    0x00400300, 0x00800300, 0x00c00300, 0x00000400, 0x00400400, 0x00800400,
    0x00c00400, 0x00000500, 0x00400500, 0x00800500, 0x00c00500, 0x00000600,
    0x00400600, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00c00f00,
    0x01ffffff, 0x00800600, 0x00c00600, 0x00000700, 0x00400700, 0x00800700,
    0x00c00700, 0x00000800, 0x00400800, 0x00800800, 0x00c00800, 0x00000900,
    0x00400900, 0x00800900, 0x00c00900, 0x00000a00, 0x00400a00, 0x00800a00,
    0x00c00a00, 0x00000b00, 0x00400b00, 0x00800b00, 0x00c00b00, 0x00000c00,
    0x00400c00, 0x00800c00, 0x00c00c00, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
const uint32_t d3[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x003e0000, 0x01ffffff, 0x01ffffff,
    0x00340000, 0x00350000, 0x00360000, 0x00370000, 0x00380000, 0x00390000,
    0x003a0000, 0x003b0000, 0x003c0000, 0x003d0000, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00010000, 0x00020000, 0x00030000, 0x00040000, 0x00050000, 0x00060000,
    0x00070000, 0x00080000, 0x00090000, 0x000a0000, 0x000b0000, 0x000c0000,
    0x000d0000, 0x000e0000, 0x000f0000, 0x00100000, 0x00110000, 0x00120000,
    0x00130000, 0x00140000, 0x00150000, 0x00160000, 0x00170000, 0x00180000,
    0x00190000, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x003f0000,
    0x01ffffff, 0x001a0000, 0x001b0000, 0x001c0000, 0x001d0000, 0x001e0000,
    0x001f0000, 0x00200000, 0x00210000, 0x00220000, 0x00230000, 0x00240000,
    0x00250000, 0x00260000, 0x00270000, 0x00280000, 0x00290000, 0x002a0000,
    0x002b0000, 0x002c0000, 0x002d0000, 0x002e0000, 0x002f0000, 0x00300000,
    0x00310000, 0x00320000, 0x00330000, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
} // namespace base64_url

namespace base64_default_or_url {
const uint32_t d0[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x000000f8, 0x01ffffff, 0x000000f8, 0x01ffffff, 0x000000fc,
    0x000000d0, 0x000000d4, 0x000000d8, 0x000000dc, 0x000000e0, 0x000000e4,
    0x000000e8, 0x000000ec, 0x000000f0, 0x000000f4, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00000004, 0x00000008, 0x0000000c, 0x00000010, 0x00000014, 0x00000018,
    0x0000001c, 0x00000020, 0x00000024, 0x00000028, 0x0000002c, 0x00000030,
    0x00000034, 0x00000038, 0x0000003c, 0x00000040, 0x00000044, 0x00000048,
    0x0000004c, 0x00000050, 0x00000054, 0x00000058, 0x0000005c, 0x00000060,
    0x00000064, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x000000fc,
    0x01ffffff, 0x00000068, 0x0000006c, 0x00000070, 0x00000074, 0x00000078,
    0x0000007c, 0x00000080, 0x00000084, 0x00000088, 0x0000008c, 0x00000090,
    0x00000094, 0x00000098, 0x0000009c, 0x000000a0, 0x000000a4, 0x000000a8,
    0x000000ac, 0x000000b0, 0x000000b4, 0x000000b8, 0x000000bc, 0x000000c0,
    0x000000c4, 0x000000c8, 0x000000cc, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
const uint32_t d1[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x0000e003, 0x01ffffff, 0x0000e003, 0x01ffffff, 0x0000f003,
    0x00004003, 0x00005003, 0x00006003, 0x00007003, 0x00008003, 0x00009003,
    0x0000a003, 0x0000b003, 0x0000c003, 0x0000d003, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00001000, 0x00002000, 0x00003000, 0x00004000, 0x00005000, 0x00006000,
    0x00007000, 0x00008000, 0x00009000, 0x0000a000, 0x0000b000, 0x0000c000,
    0x0000d000, 0x0000e000, 0x0000f000, 0x00000001, 0x00001001, 0x00002001,
    0x00003001, 0x00004001, 0x00005001, 0x00006001, 0x00007001, 0x00008001,
    0x00009001, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x0000f003,
    0x01ffffff, 0x0000a001, 0x0000b001, 0x0000c001, 0x0000d001, 0x0000e001,
    0x0000f001, 0x00000002, 0x00001002, 0x00002002, 0x00003002, 0x00004002,
    0x00005002, 0x00006002, 0x00007002, 0x00008002, 0x00009002, 0x0000a002,
    0x0000b002, 0x0000c002, 0x0000d002, 0x0000e002, 0x0000f002, 0x00000003,
    0x00001003, 0x00002003, 0x00003003, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
const uint32_t d2[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x00800f00, 0x01ffffff, 0x00800f00, 0x01ffffff, 0x00c00f00,
    0x00000d00, 0x00400d00, 0x00800d00, 0x00c00d00, 0x00000e00, 0x00400e00,
    0x00800e00, 0x00c00e00, 0x00000f00, 0x00400f00, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00400000, 0x00800000, 0x00c00000, 0x00000100, 0x00400100, 0x00800100,
    0x00c00100, 0x00000200, 0x00400200, 0x00800200, 0x00c00200, 0x00000300,
    0x00400300, 0x00800300, 0x00c00300, 0x00000400, 0x00400400, 0x00800400,
    0x00c00400, 0x00000500, 0x00400500, 0x00800500, 0x00c00500, 0x00000600,
    0x00400600, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00c00f00,
    0x01ffffff, 0x00800600, 0x00c00600, 0x00000700, 0x00400700, 0x00800700,
    0x00c00700, 0x00000800, 0x00400800, 0x00800800, 0x00c00800, 0x00000900,
    0x00400900, 0x00800900, 0x00c00900, 0x00000a00, 0x00400a00, 0x00800a00,
    0x00c00a00, 0x00000b00, 0x00400b00, 0x00800b00, 0x00c00b00, 0x00000c00,
    0x00400c00, 0x00800c00, 0x00c00c00, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
const uint32_t d3[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x003e0000, 0x01ffffff, 0x003e0000, 0x01ffffff, 0x003f0000,
    0x00340000, 0x00350000, 0x00360000, 0x00370000, 0x00380000, 0x00390000,
    0x003a0000, 0x003b0000, 0x003c0000, 0x003d0000, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00010000, 0x00020000, 0x00030000, 0x00040000, 0x00050000, 0x00060000,
    0x00070000, 0x00080000, 0x00090000, 0x000a0000, 0x000b0000, 0x000c0000,
    0x000d0000, 0x000e0000, 0x000f0000, 0x00100000, 0x00110000, 0x00120000,
    0x00130000, 0x00140000, 0x00150000, 0x00160000, 0x00170000, 0x00180000,
    0x00190000, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x003f0000,
    0x01ffffff, 0x001a0000, 0x001b0000, 0x001c0000, 0x001d0000, 0x001e0000,
    0x001f0000, 0x00200000, 0x00210000, 0x00220000, 0x00230000, 0x00240000,
    0x00250000, 0x00260000, 0x00270000, 0x00280000, 0x00290000, 0x002a0000,
    0x002b0000, 0x002c0000, 0x002d0000, 0x002e0000, 0x002f0000, 0x00300000,
    0x00310000, 0x00320000, 0x00330000, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
} // namespace base64_default_or_url
const uint64_t thintable_epi8[256] = {
    0x0706050403020100, 0x0007060504030201, 0x0007060504030200,
    0x0000070605040302, 0x0007060504030100, 0x0000070605040301,
    0x0000070605040300, 0x0000000706050403, 0x0007060504020100,
    0x0000070605040201, 0x0000070605040200, 0x0000000706050402,
    0x0000070605040100, 0x0000000706050401, 0x0000000706050400,
    0x0000000007060504, 0x0007060503020100, 0x0000070605030201,
    0x0000070605030200, 0x0000000706050302, 0x0000070605030100,
    0x0000000706050301, 0x0000000706050300, 0x0000000007060503,
    0x0000070605020100, 0x0000000706050201, 0x0000000706050200,
    0x0000000007060502, 0x0000000706050100, 0x0000000007060501,
    0x0000000007060500, 0x0000000000070605, 0x0007060403020100,
    0x0000070604030201, 0x0000070604030200, 0x0000000706040302,
    0x0000070604030100, 0x0000000706040301, 0x0000000706040300,
    0x0000000007060403, 0x0000070604020100, 0x0000000706040201,
    0x0000000706040200, 0x0000000007060402, 0x0000000706040100,
    0x0000000007060401, 0x0000000007060400, 0x0000000000070604,
    0x0000070603020100, 0x0000000706030201, 0x0000000706030200,
    0x0000000007060302, 0x0000000706030100, 0x0000000007060301,
    0x0000000007060300, 0x0000000000070603, 0x0000000706020100,
    0x0000000007060201, 0x0000000007060200, 0x0000000000070602,
    0x0000000007060100, 0x0000000000070601, 0x0000000000070600,
    0x0000000000000706, 0x0007050403020100, 0x0000070504030201,
    0x0000070504030200, 0x0000000705040302, 0x0000070504030100,
    0x0000000705040301, 0x0000000705040300, 0x0000000007050403,
    0x0000070504020100, 0x0000000705040201, 0x0000000705040200,
    0x0000000007050402, 0x0000000705040100, 0x0000000007050401,
    0x0000000007050400, 0x0000000000070504, 0x0000070503020100,
    0x0000000705030201, 0x0000000705030200, 0x0000000007050302,
    0x0000000705030100, 0x0000000007050301, 0x0000000007050300,
    0x0000000000070503, 0x0000000705020100, 0x0000000007050201,
    0x0000000007050200, 0x0000000000070502, 0x0000000007050100,
    0x0000000000070501, 0x0000000000070500, 0x0000000000000705,
    0x0000070403020100, 0x0000000704030201, 0x0000000704030200,
    0x0000000007040302, 0x0000000704030100, 0x0000000007040301,
    0x0000000007040300, 0x0000000000070403, 0x0000000704020100,
    0x0000000007040201, 0x0000000007040200, 0x0000000000070402,
    0x0000000007040100, 0x0000000000070401, 0x0000000000070400,
    0x0000000000000704, 0x0000000703020100, 0x0000000007030201,
    0x0000000007030200, 0x0000000000070302, 0x0000000007030100,
    0x0000000000070301, 0x0000000000070300, 0x0000000000000703,
    0x0000000007020100, 0x0000000000070201, 0x0000000000070200,
    0x0000000000000702, 0x0000000000070100, 0x0000000000000701,
    0x0000000000000700, 0x0000000000000007, 0x0006050403020100,
    0x0000060504030201, 0x0000060504030200, 0x0000000605040302,
    0x0000060504030100, 0x0000000605040301, 0x0000000605040300,
    0x0000000006050403, 0x0000060504020100, 0x0000000605040201,
    0x0000000605040200, 0x0000000006050402, 0x0000000605040100,
    0x0000000006050401, 0x0000000006050400, 0x0000000000060504,
    0x0000060503020100, 0x0000000605030201, 0x0000000605030200,
    0x0000000006050302, 0x0000000605030100, 0x0000000006050301,
    0x0000000006050300, 0x0000000000060503, 0x0000000605020100,
    0x0000000006050201, 0x0000000006050200, 0x0000000000060502,
    0x0000000006050100, 0x0000000000060501, 0x0000000000060500,
    0x0000000000000605, 0x0000060403020100, 0x0000000604030201,
    0x0000000604030200, 0x0000000006040302, 0x0000000604030100,
    0x0000000006040301, 0x0000000006040300, 0x0000000000060403,
    0x0000000604020100, 0x0000000006040201, 0x0000000006040200,
    0x0000000000060402, 0x0000000006040100, 0x0000000000060401,
    0x0000000000060400, 0x0000000000000604, 0x0000000603020100,
    0x0000000006030201, 0x0000000006030200, 0x0000000000060302,
    0x0000000006030100, 0x0000000000060301, 0x0000000000060300,
    0x0000000000000603, 0x0000000006020100, 0x0000000000060201,
    0x0000000000060200, 0x0000000000000602, 0x0000000000060100,
    0x0000000000000601, 0x0000000000000600, 0x0000000000000006,
    0x0000050403020100, 0x0000000504030201, 0x0000000504030200,
    0x0000000005040302, 0x0000000504030100, 0x0000000005040301,
    0x0000000005040300, 0x0000000000050403, 0x0000000504020100,
    0x0000000005040201, 0x0000000005040200, 0x0000000000050402,
    0x0000000005040100, 0x0000000000050401, 0x0000000000050400,
    0x0000000000000504, 0x0000000503020100, 0x0000000005030201,
    0x0000000005030200, 0x0000000000050302, 0x0000000005030100,
    0x0000000000050301, 0x0000000000050300, 0x0000000000000503,
    0x0000000005020100, 0x0000000000050201, 0x0000000000050200,
    0x0000000000000502, 0x0000000000050100, 0x0000000000000501,
    0x0000000000000500, 0x0000000000000005, 0x0000000403020100,
    0x0000000004030201, 0x0000000004030200, 0x0000000000040302,
    0x0000000004030100, 0x0000000000040301, 0x0000000000040300,
    0x0000000000000403, 0x0000000004020100, 0x0000000000040201,
    0x0000000000040200, 0x0000000000000402, 0x0000000000040100,
    0x0000000000000401, 0x0000000000000400, 0x0000000000000004,
    0x0000000003020100, 0x0000000000030201, 0x0000000000030200,
    0x0000000000000302, 0x0000000000030100, 0x0000000000000301,
    0x0000000000000300, 0x0000000000000003, 0x0000000000020100,
    0x0000000000000201, 0x0000000000000200, 0x0000000000000002,
    0x0000000000000100, 0x0000000000000001, 0x0000000000000000,
    0x0000000000000000,
};

const uint8_t pshufb_combine_table[272] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x01, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x00, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

const unsigned char BitsSetTable256mul2[256] = {
    0,  2,  2,  4,  2,  4,  4,  6,  2,  4,  4,  6,  4,  6,  6,  8,  2,  4,  4,
    6,  4,  6,  6,  8,  4,  6,  6,  8,  6,  8,  8,  10, 2,  4,  4,  6,  4,  6,
    6,  8,  4,  6,  6,  8,  6,  8,  8,  10, 4,  6,  6,  8,  6,  8,  8,  10, 6,
    8,  8,  10, 8,  10, 10, 12, 2,  4,  4,  6,  4,  6,  6,  8,  4,  6,  6,  8,
    6,  8,  8,  10, 4,  6,  6,  8,  6,  8,  8,  10, 6,  8,  8,  10, 8,  10, 10,
    12, 4,  6,  6,  8,  6,  8,  8,  10, 6,  8,  8,  10, 8,  10, 10, 12, 6,  8,
    8,  10, 8,  10, 10, 12, 8,  10, 10, 12, 10, 12, 12, 14, 2,  4,  4,  6,  4,
    6,  6,  8,  4,  6,  6,  8,  6,  8,  8,  10, 4,  6,  6,  8,  6,  8,  8,  10,
    6,  8,  8,  10, 8,  10, 10, 12, 4,  6,  6,  8,  6,  8,  8,  10, 6,  8,  8,
    10, 8,  10, 10, 12, 6,  8,  8,  10, 8,  10, 10, 12, 8,  10, 10, 12, 10, 12,
    12, 14, 4,  6,  6,  8,  6,  8,  8,  10, 6,  8,  8,  10, 8,  10, 10, 12, 6,
    8,  8,  10, 8,  10, 10, 12, 8,  10, 10, 12, 10, 12, 12, 14, 6,  8,  8,  10,
    8,  10, 10, 12, 8,  10, 10, 12, 10, 12, 12, 14, 8,  10, 10, 12, 10, 12, 12,
    14, 10, 12, 12, 14, 12, 14, 14, 16};

constexpr uint8_t to_base64_value[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 64,  64,  255, 64,  64,  255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 64,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62,  255,
    255, 255, 63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255,
    255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  255, 255, 255, 255, 255, 255, 26,  27,  28,  29,  30,  31,  32,  33,
    34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255};

constexpr uint8_t to_base64_url_value[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 64,  64,  255, 64,  64,  255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 64,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    62,  255, 255, 52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255,
    255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  255, 255, 255, 255, 63,  255, 26,  27,  28,  29,  30,  31,  32,  33,
    34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255};

constexpr uint8_t to_base64_default_or_url_value[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 64,  64,  255, 64,  64,  255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 64,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62,  255,
    62,  255, 63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255,
    255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  255, 255, 255, 255, 63,  255, 26,  27,  28,  29,  30,  31,  32,  33,
    34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255};

static_assert(sizeof(to_base64_value) == 256,
              "to_base64_value must have 256 elements");
static_assert(sizeof(to_base64_url_value) == 256,
              "to_base64_url_value must have 256 elements");
static_assert(to_base64_value[uint8_t(' ')] == 64,
              "space must be == 64 in to_base64_value");
static_assert(to_base64_url_value[uint8_t(' ')] == 64,
              "space must be == 64 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('\t')] == 64,
              "tab must be == 64 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('\t')] == 64,
              "tab must be == 64 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('\r')] == 64,
              "cr must be == 64 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('\r')] == 64,
              "cr must be == 64 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('\n')] == 64,
              "lf must be == 64 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('\n')] == 64,
              "lf must be == 64 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('\f')] == 64,
              "ff must be == 64 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('\f')] == 64,
              "ff must be == 64 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('+')] == 62,
              "+ must be == 62 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('-')] == 62,
              "- must be == 62 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('/')] == 63,
              "/ must be == 62 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('_')] == 63,
              "_ must be == 62 in to_base64_url_value");
} // namespace base64
} // namespace tables
} // unnamed namespace
} // namespace simdutf

#endif // SIMDUTF_BASE64_TABLES_H
/* end file src\tables\base64_tables.h */

/* begin file src\encoding_types.cpp */
namespace simdutf {
bool match_system(endianness e) {
#if SIMDUTF_IS_BIG_ENDIAN
  return e == endianness::BIG;
#else
  return e == endianness::LITTLE;
#endif
}

std::string to_string(encoding_type bom) {
  switch (bom) {
  case UTF16_LE:
    return "UTF16 little-endian";
  case UTF16_BE:
    return "UTF16 big-endian";
  case UTF32_LE:
    return "UTF32 little-endian";
  case UTF32_BE:
    return "UTF32 big-endian";
  case UTF8:
    return "UTF8";
  case unspecified:
    return "unknown";
  default:
    return "error";
  }
}

namespace BOM {
// Note that BOM for UTF8 is discouraged.
encoding_type check_bom(const uint8_t *byte, size_t length) {
  if (length >= 2 && byte[0] == 0xff and byte[1] == 0xfe) {
    if (length >= 4 && byte[2] == 0x00 and byte[3] == 0x0) {
      return encoding_type::UTF32_LE;
    } else {
      return encoding_type::UTF16_LE;
    }
  } else if (length >= 2 && byte[0] == 0xfe and byte[1] == 0xff) {
    return encoding_type::UTF16_BE;
  } else if (length >= 4 && byte[0] == 0x00 and byte[1] == 0x00 and
             byte[2] == 0xfe and byte[3] == 0xff) {
    return encoding_type::UTF32_BE;
  } else if (length >= 4 && byte[0] == 0xef and byte[1] == 0xbb and
             byte[2] == 0xbf) {
    return encoding_type::UTF8;
  }
  return encoding_type::unspecified;
}

encoding_type check_bom(const char *byte, size_t length) {
  return check_bom(reinterpret_cast<const uint8_t *>(byte), length);
}

size_t bom_byte_size(encoding_type bom) {
  switch (bom) {
  case UTF16_LE:
    return 2;
  case UTF16_BE:
    return 2;
  case UTF32_LE:
    return 4;
  case UTF32_BE:
    return 4;
  case UTF8:
    return 3;
  case unspecified:
    return 0;
  default:
    return 0;
  }
}

} // namespace BOM
} // namespace simdutf
/* end file src\encoding_types.cpp */
/* begin file src\error.cpp */
namespace simdutf {
// deliberately empty
}
/* end file src\error.cpp */
// The large tables should be included once and they
// should not depend on a kernel.
/* begin file src\tables\utf8_to_utf16_tables.h */
#ifndef SIMDUTF_UTF8_TO_UTF16_TABLES_H
#define SIMDUTF_UTF8_TO_UTF16_TABLES_H
#include <cstdint>

namespace simdutf {
namespace {
namespace tables {
namespace utf8_to_utf16 {
/**
 * utf8bigindex uses about 8 kB
 * shufutf8 uses about 3344 B
 *
 * So we use a bit over 11 kB. It would be
 * easy to save about 4 kB by only
 * storing the index in utf8bigindex, and
 * deriving the consumed bytes otherwise.
 * However, this may come at a significant (10% to 20%)
 * performance penalty.
 */

const uint8_t shufutf8[209][16] = {
    {0, 255, 1, 255, 2, 255, 3, 255, 4, 255, 5, 255, 0, 0, 0, 0},
    {0, 255, 1, 255, 2, 255, 3, 255, 4, 255, 6, 5, 0, 0, 0, 0},
    {0, 255, 1, 255, 2, 255, 3, 255, 5, 4, 6, 255, 0, 0, 0, 0},
    {0, 255, 1, 255, 2, 255, 3, 255, 5, 4, 7, 6, 0, 0, 0, 0},
    {0, 255, 1, 255, 2, 255, 4, 3, 5, 255, 6, 255, 0, 0, 0, 0},
    {0, 255, 1, 255, 2, 255, 4, 3, 5, 255, 7, 6, 0, 0, 0, 0},
    {0, 255, 1, 255, 2, 255, 4, 3, 6, 5, 7, 255, 0, 0, 0, 0},
    {0, 255, 1, 255, 2, 255, 4, 3, 6, 5, 8, 7, 0, 0, 0, 0},
    {0, 255, 1, 255, 3, 2, 4, 255, 5, 255, 6, 255, 0, 0, 0, 0},
    {0, 255, 1, 255, 3, 2, 4, 255, 5, 255, 7, 6, 0, 0, 0, 0},
    {0, 255, 1, 255, 3, 2, 4, 255, 6, 5, 7, 255, 0, 0, 0, 0},
    {0, 255, 1, 255, 3, 2, 4, 255, 6, 5, 8, 7, 0, 0, 0, 0},
    {0, 255, 1, 255, 3, 2, 5, 4, 6, 255, 7, 255, 0, 0, 0, 0},
    {0, 255, 1, 255, 3, 2, 5, 4, 6, 255, 8, 7, 0, 0, 0, 0},
    {0, 255, 1, 255, 3, 2, 5, 4, 7, 6, 8, 255, 0, 0, 0, 0},
    {0, 255, 1, 255, 3, 2, 5, 4, 7, 6, 9, 8, 0, 0, 0, 0},
    {0, 255, 2, 1, 3, 255, 4, 255, 5, 255, 6, 255, 0, 0, 0, 0},
    {0, 255, 2, 1, 3, 255, 4, 255, 5, 255, 7, 6, 0, 0, 0, 0},
    {0, 255, 2, 1, 3, 255, 4, 255, 6, 5, 7, 255, 0, 0, 0, 0},
    {0, 255, 2, 1, 3, 255, 4, 255, 6, 5, 8, 7, 0, 0, 0, 0},
    {0, 255, 2, 1, 3, 255, 5, 4, 6, 255, 7, 255, 0, 0, 0, 0},
    {0, 255, 2, 1, 3, 255, 5, 4, 6, 255, 8, 7, 0, 0, 0, 0},
    {0, 255, 2, 1, 3, 255, 5, 4, 7, 6, 8, 255, 0, 0, 0, 0},
    {0, 255, 2, 1, 3, 255, 5, 4, 7, 6, 9, 8, 0, 0, 0, 0},
    {0, 255, 2, 1, 4, 3, 5, 255, 6, 255, 7, 255, 0, 0, 0, 0},
    {0, 255, 2, 1, 4, 3, 5, 255, 6, 255, 8, 7, 0, 0, 0, 0},
    {0, 255, 2, 1, 4, 3, 5, 255, 7, 6, 8, 255, 0, 0, 0, 0},
    {0, 255, 2, 1, 4, 3, 5, 255, 7, 6, 9, 8, 0, 0, 0, 0},
    {0, 255, 2, 1, 4, 3, 6, 5, 7, 255, 8, 255, 0, 0, 0, 0},
    {0, 255, 2, 1, 4, 3, 6, 5, 7, 255, 9, 8, 0, 0, 0, 0},
    {0, 255, 2, 1, 4, 3, 6, 5, 8, 7, 9, 255, 0, 0, 0, 0},
    {0, 255, 2, 1, 4, 3, 6, 5, 8, 7, 10, 9, 0, 0, 0, 0},
    {1, 0, 2, 255, 3, 255, 4, 255, 5, 255, 6, 255, 0, 0, 0, 0},
    {1, 0, 2, 255, 3, 255, 4, 255, 5, 255, 7, 6, 0, 0, 0, 0},
    {1, 0, 2, 255, 3, 255, 4, 255, 6, 5, 7, 255, 0, 0, 0, 0},
    {1, 0, 2, 255, 3, 255, 4, 255, 6, 5, 8, 7, 0, 0, 0, 0},
    {1, 0, 2, 255, 3, 255, 5, 4, 6, 255, 7, 255, 0, 0, 0, 0},
    {1, 0, 2, 255, 3, 255, 5, 4, 6, 255, 8, 7, 0, 0, 0, 0},
    {1, 0, 2, 255, 3, 255, 5, 4, 7, 6, 8, 255, 0, 0, 0, 0},
    {1, 0, 2, 255, 3, 255, 5, 4, 7, 6, 9, 8, 0, 0, 0, 0},
    {1, 0, 2, 255, 4, 3, 5, 255, 6, 255, 7, 255, 0, 0, 0, 0},
    {1, 0, 2, 255, 4, 3, 5, 255, 6, 255, 8, 7, 0, 0, 0, 0},
    {1, 0, 2, 255, 4, 3, 5, 255, 7, 6, 8, 255, 0, 0, 0, 0},
    {1, 0, 2, 255, 4, 3, 5, 255, 7, 6, 9, 8, 0, 0, 0, 0},
    {1, 0, 2, 255, 4, 3, 6, 5, 7, 255, 8, 255, 0, 0, 0, 0},
    {1, 0, 2, 255, 4, 3, 6, 5, 7, 255, 9, 8, 0, 0, 0, 0},
    {1, 0, 2, 255, 4, 3, 6, 5, 8, 7, 9, 255, 0, 0, 0, 0},
    {1, 0, 2, 255, 4, 3, 6, 5, 8, 7, 10, 9, 0, 0, 0, 0},
    {1, 0, 3, 2, 4, 255, 5, 255, 6, 255, 7, 255, 0, 0, 0, 0},
    {1, 0, 3, 2, 4, 255, 5, 255, 6, 255, 8, 7, 0, 0, 0, 0},
    {1, 0, 3, 2, 4, 255, 5, 255, 7, 6, 8, 255, 0, 0, 0, 0},
    {1, 0, 3, 2, 4, 255, 5, 255, 7, 6, 9, 8, 0, 0, 0, 0},
    {1, 0, 3, 2, 4, 255, 6, 5, 7, 255, 8, 255, 0, 0, 0, 0},
    {1, 0, 3, 2, 4, 255, 6, 5, 7, 255, 9, 8, 0, 0, 0, 0},
    {1, 0, 3, 2, 4, 255, 6, 5, 8, 7, 9, 255, 0, 0, 0, 0},
    {1, 0, 3, 2, 4, 255, 6, 5, 8, 7, 10, 9, 0, 0, 0, 0},
    {1, 0, 3, 2, 5, 4, 6, 255, 7, 255, 8, 255, 0, 0, 0, 0},
    {1, 0, 3, 2, 5, 4, 6, 255, 7, 255, 9, 8, 0, 0, 0, 0},
    {1, 0, 3, 2, 5, 4, 6, 255, 8, 7, 9, 255, 0, 0, 0, 0},
    {1, 0, 3, 2, 5, 4, 6, 255, 8, 7, 10, 9, 0, 0, 0, 0},
    {1, 0, 3, 2, 5, 4, 7, 6, 8, 255, 9, 255, 0, 0, 0, 0},
    {1, 0, 3, 2, 5, 4, 7, 6, 8, 255, 10, 9, 0, 0, 0, 0},
    {1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 10, 255, 0, 0, 0, 0},
    {1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 0, 0, 0, 0},
    {0, 255, 255, 255, 1, 255, 255, 255, 2, 255, 255, 255, 3, 255, 255, 255},
    {0, 255, 255, 255, 1, 255, 255, 255, 2, 255, 255, 255, 4, 3, 255, 255},
    {0, 255, 255, 255, 1, 255, 255, 255, 2, 255, 255, 255, 5, 4, 3, 255},
    {0, 255, 255, 255, 1, 255, 255, 255, 3, 2, 255, 255, 4, 255, 255, 255},
    {0, 255, 255, 255, 1, 255, 255, 255, 3, 2, 255, 255, 5, 4, 255, 255},
    {0, 255, 255, 255, 1, 255, 255, 255, 3, 2, 255, 255, 6, 5, 4, 255},
    {0, 255, 255, 255, 1, 255, 255, 255, 4, 3, 2, 255, 5, 255, 255, 255},
    {0, 255, 255, 255, 1, 255, 255, 255, 4, 3, 2, 255, 6, 5, 255, 255},
    {0, 255, 255, 255, 1, 255, 255, 255, 4, 3, 2, 255, 7, 6, 5, 255},
    {0, 255, 255, 255, 2, 1, 255, 255, 3, 255, 255, 255, 4, 255, 255, 255},
    {0, 255, 255, 255, 2, 1, 255, 255, 3, 255, 255, 255, 5, 4, 255, 255},
    {0, 255, 255, 255, 2, 1, 255, 255, 3, 255, 255, 255, 6, 5, 4, 255},
    {0, 255, 255, 255, 2, 1, 255, 255, 4, 3, 255, 255, 5, 255, 255, 255},
    {0, 255, 255, 255, 2, 1, 255, 255, 4, 3, 255, 255, 6, 5, 255, 255},
    {0, 255, 255, 255, 2, 1, 255, 255, 4, 3, 255, 255, 7, 6, 5, 255},
    {0, 255, 255, 255, 2, 1, 255, 255, 5, 4, 3, 255, 6, 255, 255, 255},
    {0, 255, 255, 255, 2, 1, 255, 255, 5, 4, 3, 255, 7, 6, 255, 255},
    {0, 255, 255, 255, 2, 1, 255, 255, 5, 4, 3, 255, 8, 7, 6, 255},
    {0, 255, 255, 255, 3, 2, 1, 255, 4, 255, 255, 255, 5, 255, 255, 255},
    {0, 255, 255, 255, 3, 2, 1, 255, 4, 255, 255, 255, 6, 5, 255, 255},
    {0, 255, 255, 255, 3, 2, 1, 255, 4, 255, 255, 255, 7, 6, 5, 255},
    {0, 255, 255, 255, 3, 2, 1, 255, 5, 4, 255, 255, 6, 255, 255, 255},
    {0, 255, 255, 255, 3, 2, 1, 255, 5, 4, 255, 255, 7, 6, 255, 255},
    {0, 255, 255, 255, 3, 2, 1, 255, 5, 4, 255, 255, 8, 7, 6, 255},
    {0, 255, 255, 255, 3, 2, 1, 255, 6, 5, 4, 255, 7, 255, 255, 255},
    {0, 255, 255, 255, 3, 2, 1, 255, 6, 5, 4, 255, 8, 7, 255, 255},
    {0, 255, 255, 255, 3, 2, 1, 255, 6, 5, 4, 255, 9, 8, 7, 255},
    {1, 0, 255, 255, 2, 255, 255, 255, 3, 255, 255, 255, 4, 255, 255, 255},
    {1, 0, 255, 255, 2, 255, 255, 255, 3, 255, 255, 255, 5, 4, 255, 255},
    {1, 0, 255, 255, 2, 255, 255, 255, 3, 255, 255, 255, 6, 5, 4, 255},
    {1, 0, 255, 255, 2, 255, 255, 255, 4, 3, 255, 255, 5, 255, 255, 255},
    {1, 0, 255, 255, 2, 255, 255, 255, 4, 3, 255, 255, 6, 5, 255, 255},
    {1, 0, 255, 255, 2, 255, 255, 255, 4, 3, 255, 255, 7, 6, 5, 255},
    {1, 0, 255, 255, 2, 255, 255, 255, 5, 4, 3, 255, 6, 255, 255, 255},
    {1, 0, 255, 255, 2, 255, 255, 255, 5, 4, 3, 255, 7, 6, 255, 255},
    {1, 0, 255, 255, 2, 255, 255, 255, 5, 4, 3, 255, 8, 7, 6, 255},
    {1, 0, 255, 255, 3, 2, 255, 255, 4, 255, 255, 255, 5, 255, 255, 255},
    {1, 0, 255, 255, 3, 2, 255, 255, 4, 255, 255, 255, 6, 5, 255, 255},
    {1, 0, 255, 255, 3, 2, 255, 255, 4, 255, 255, 255, 7, 6, 5, 255},
    {1, 0, 255, 255, 3, 2, 255, 255, 5, 4, 255, 255, 6, 255, 255, 255},
    {1, 0, 255, 255, 3, 2, 255, 255, 5, 4, 255, 255, 7, 6, 255, 255},
    {1, 0, 255, 255, 3, 2, 255, 255, 5, 4, 255, 255, 8, 7, 6, 255},
    {1, 0, 255, 255, 3, 2, 255, 255, 6, 5, 4, 255, 7, 255, 255, 255},
    {1, 0, 255, 255, 3, 2, 255, 255, 6, 5, 4, 255, 8, 7, 255, 255},
    {1, 0, 255, 255, 3, 2, 255, 255, 6, 5, 4, 255, 9, 8, 7, 255},
    {1, 0, 255, 255, 4, 3, 2, 255, 5, 255, 255, 255, 6, 255, 255, 255},
    {1, 0, 255, 255, 4, 3, 2, 255, 5, 255, 255, 255, 7, 6, 255, 255},
    {1, 0, 255, 255, 4, 3, 2, 255, 5, 255, 255, 255, 8, 7, 6, 255},
    {1, 0, 255, 255, 4, 3, 2, 255, 6, 5, 255, 255, 7, 255, 255, 255},
    {1, 0, 255, 255, 4, 3, 2, 255, 6, 5, 255, 255, 8, 7, 255, 255},
    {1, 0, 255, 255, 4, 3, 2, 255, 6, 5, 255, 255, 9, 8, 7, 255},
    {1, 0, 255, 255, 4, 3, 2, 255, 7, 6, 5, 255, 8, 255, 255, 255},
    {1, 0, 255, 255, 4, 3, 2, 255, 7, 6, 5, 255, 9, 8, 255, 255},
    {1, 0, 255, 255, 4, 3, 2, 255, 7, 6, 5, 255, 10, 9, 8, 255},
    {2, 1, 0, 255, 3, 255, 255, 255, 4, 255, 255, 255, 5, 255, 255, 255},
    {2, 1, 0, 255, 3, 255, 255, 255, 4, 255, 255, 255, 6, 5, 255, 255},
    {2, 1, 0, 255, 3, 255, 255, 255, 4, 255, 255, 255, 7, 6, 5, 255},
    {2, 1, 0, 255, 3, 255, 255, 255, 5, 4, 255, 255, 6, 255, 255, 255},
    {2, 1, 0, 255, 3, 255, 255, 255, 5, 4, 255, 255, 7, 6, 255, 255},
    {2, 1, 0, 255, 3, 255, 255, 255, 5, 4, 255, 255, 8, 7, 6, 255},
    {2, 1, 0, 255, 3, 255, 255, 255, 6, 5, 4, 255, 7, 255, 255, 255},
    {2, 1, 0, 255, 3, 255, 255, 255, 6, 5, 4, 255, 8, 7, 255, 255},
    {2, 1, 0, 255, 3, 255, 255, 255, 6, 5, 4, 255, 9, 8, 7, 255},
    {2, 1, 0, 255, 4, 3, 255, 255, 5, 255, 255, 255, 6, 255, 255, 255},
    {2, 1, 0, 255, 4, 3, 255, 255, 5, 255, 255, 255, 7, 6, 255, 255},
    {2, 1, 0, 255, 4, 3, 255, 255, 5, 255, 255, 255, 8, 7, 6, 255},
    {2, 1, 0, 255, 4, 3, 255, 255, 6, 5, 255, 255, 7, 255, 255, 255},
    {2, 1, 0, 255, 4, 3, 255, 255, 6, 5, 255, 255, 8, 7, 255, 255},
    {2, 1, 0, 255, 4, 3, 255, 255, 6, 5, 255, 255, 9, 8, 7, 255},
    {2, 1, 0, 255, 4, 3, 255, 255, 7, 6, 5, 255, 8, 255, 255, 255},
    {2, 1, 0, 255, 4, 3, 255, 255, 7, 6, 5, 255, 9, 8, 255, 255},
    {2, 1, 0, 255, 4, 3, 255, 255, 7, 6, 5, 255, 10, 9, 8, 255},
    {2, 1, 0, 255, 5, 4, 3, 255, 6, 255, 255, 255, 7, 255, 255, 255},
    {2, 1, 0, 255, 5, 4, 3, 255, 6, 255, 255, 255, 8, 7, 255, 255},
    {2, 1, 0, 255, 5, 4, 3, 255, 6, 255, 255, 255, 9, 8, 7, 255},
    {2, 1, 0, 255, 5, 4, 3, 255, 7, 6, 255, 255, 8, 255, 255, 255},
    {2, 1, 0, 255, 5, 4, 3, 255, 7, 6, 255, 255, 9, 8, 255, 255},
    {2, 1, 0, 255, 5, 4, 3, 255, 7, 6, 255, 255, 10, 9, 8, 255},
    {2, 1, 0, 255, 5, 4, 3, 255, 8, 7, 6, 255, 9, 255, 255, 255},
    {2, 1, 0, 255, 5, 4, 3, 255, 8, 7, 6, 255, 10, 9, 255, 255},
    {2, 1, 0, 255, 5, 4, 3, 255, 8, 7, 6, 255, 11, 10, 9, 255},
    {0, 255, 255, 255, 1, 255, 255, 255, 2, 255, 255, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 1, 255, 255, 255, 3, 2, 255, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 1, 255, 255, 255, 4, 3, 2, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 1, 255, 255, 255, 5, 4, 3, 2, 0, 0, 0, 0},
    {0, 255, 255, 255, 2, 1, 255, 255, 3, 255, 255, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 2, 1, 255, 255, 4, 3, 255, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 2, 1, 255, 255, 5, 4, 3, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 2, 1, 255, 255, 6, 5, 4, 3, 0, 0, 0, 0},
    {0, 255, 255, 255, 3, 2, 1, 255, 4, 255, 255, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 3, 2, 1, 255, 5, 4, 255, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 3, 2, 1, 255, 6, 5, 4, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 3, 2, 1, 255, 7, 6, 5, 4, 0, 0, 0, 0},
    {0, 255, 255, 255, 4, 3, 2, 1, 5, 255, 255, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 4, 3, 2, 1, 6, 5, 255, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 4, 3, 2, 1, 7, 6, 5, 255, 0, 0, 0, 0},
    {0, 255, 255, 255, 4, 3, 2, 1, 8, 7, 6, 5, 0, 0, 0, 0},
    {1, 0, 255, 255, 2, 255, 255, 255, 3, 255, 255, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 2, 255, 255, 255, 4, 3, 255, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 2, 255, 255, 255, 5, 4, 3, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 2, 255, 255, 255, 6, 5, 4, 3, 0, 0, 0, 0},
    {1, 0, 255, 255, 3, 2, 255, 255, 4, 255, 255, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 3, 2, 255, 255, 5, 4, 255, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 3, 2, 255, 255, 6, 5, 4, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 3, 2, 255, 255, 7, 6, 5, 4, 0, 0, 0, 0},
    {1, 0, 255, 255, 4, 3, 2, 255, 5, 255, 255, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 4, 3, 2, 255, 6, 5, 255, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 4, 3, 2, 255, 7, 6, 5, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 4, 3, 2, 255, 8, 7, 6, 5, 0, 0, 0, 0},
    {1, 0, 255, 255, 5, 4, 3, 2, 6, 255, 255, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 5, 4, 3, 2, 7, 6, 255, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 5, 4, 3, 2, 8, 7, 6, 255, 0, 0, 0, 0},
    {1, 0, 255, 255, 5, 4, 3, 2, 9, 8, 7, 6, 0, 0, 0, 0},
    {2, 1, 0, 255, 3, 255, 255, 255, 4, 255, 255, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 3, 255, 255, 255, 5, 4, 255, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 3, 255, 255, 255, 6, 5, 4, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 3, 255, 255, 255, 7, 6, 5, 4, 0, 0, 0, 0},
    {2, 1, 0, 255, 4, 3, 255, 255, 5, 255, 255, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 4, 3, 255, 255, 6, 5, 255, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 4, 3, 255, 255, 7, 6, 5, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 4, 3, 255, 255, 8, 7, 6, 5, 0, 0, 0, 0},
    {2, 1, 0, 255, 5, 4, 3, 255, 6, 255, 255, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 5, 4, 3, 255, 7, 6, 255, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 5, 4, 3, 255, 8, 7, 6, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 5, 4, 3, 255, 9, 8, 7, 6, 0, 0, 0, 0},
    {2, 1, 0, 255, 6, 5, 4, 3, 7, 255, 255, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 6, 5, 4, 3, 8, 7, 255, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 6, 5, 4, 3, 9, 8, 7, 255, 0, 0, 0, 0},
    {2, 1, 0, 255, 6, 5, 4, 3, 10, 9, 8, 7, 0, 0, 0, 0},
    {3, 2, 1, 0, 4, 255, 255, 255, 5, 255, 255, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 4, 255, 255, 255, 6, 5, 255, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 4, 255, 255, 255, 7, 6, 5, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 4, 255, 255, 255, 8, 7, 6, 5, 0, 0, 0, 0},
    {3, 2, 1, 0, 5, 4, 255, 255, 6, 255, 255, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 5, 4, 255, 255, 7, 6, 255, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 5, 4, 255, 255, 8, 7, 6, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 5, 4, 255, 255, 9, 8, 7, 6, 0, 0, 0, 0},
    {3, 2, 1, 0, 6, 5, 4, 255, 7, 255, 255, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 6, 5, 4, 255, 8, 7, 255, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 6, 5, 4, 255, 9, 8, 7, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 6, 5, 4, 255, 10, 9, 8, 7, 0, 0, 0, 0},
    {3, 2, 1, 0, 7, 6, 5, 4, 8, 255, 255, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 7, 6, 5, 4, 9, 8, 255, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 7, 6, 5, 4, 10, 9, 8, 255, 0, 0, 0, 0},
    {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 0, 0, 0, 0}};
/* number of two bytes : 64 */
/* number of two + three bytes : 145 */
/* number of two + three + four bytes : 209 */
const uint8_t utf8bigindex[4096][2] = {
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {145, 3},  {209, 12}, {209, 12}, {209, 12}, {146, 4},  {209, 12}, {149, 4},
    {161, 4},  {64, 4},   {209, 12}, {209, 12}, {209, 12}, {147, 5},  {209, 12},
    {150, 5},  {162, 5},  {65, 5},   {209, 12}, {153, 5},  {165, 5},  {67, 5},
    {177, 5},  {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12}, {209, 12},
    {148, 6},  {209, 12}, {151, 6},  {163, 6},  {66, 6},   {209, 12}, {154, 6},
    {166, 6},  {68, 6},   {178, 6},  {74, 6},   {92, 6},   {64, 4},   {209, 12},
    {157, 6},  {169, 6},  {70, 6},   {181, 6},  {76, 6},   {94, 6},   {65, 5},
    {193, 6},  {82, 6},   {100, 6},  {67, 5},   {118, 6},  {73, 5},   {91, 5},
    {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {152, 7},
    {164, 7},  {145, 3},  {209, 12}, {155, 7},  {167, 7},  {69, 7},   {179, 7},
    {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},  {170, 7},  {71, 7},
    {182, 7},  {77, 7},   {95, 7},   {65, 5},   {194, 7},  {83, 7},   {101, 7},
    {67, 5},   {119, 7},  {73, 5},   {91, 5},   {1, 7},    {209, 12}, {209, 12},
    {173, 7},  {148, 6},  {185, 7},  {79, 7},   {97, 7},   {66, 6},   {197, 7},
    {85, 7},   {103, 7},  {68, 6},   {121, 7},  {74, 6},   {92, 6},   {2, 7},
    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},  {76, 6},   {94, 6},
    {4, 7},    {193, 6},  {82, 6},   {100, 6},  {8, 7},    {118, 6},  {16, 7},
    {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {145, 3},  {209, 12}, {156, 8},  {168, 8},  {146, 4},
    {180, 8},  {149, 4},  {161, 4},  {64, 4},   {209, 12}, {159, 8},  {171, 8},
    {72, 8},   {183, 8},  {78, 8},   {96, 8},   {65, 5},   {195, 8},  {84, 8},
    {102, 8},  {67, 5},   {120, 8},  {73, 5},   {91, 5},   {64, 4},   {209, 12},
    {209, 12}, {174, 8},  {148, 6},  {186, 8},  {80, 8},   {98, 8},   {66, 6},
    {198, 8},  {86, 8},   {104, 8},  {68, 6},   {122, 8},  {74, 6},   {92, 6},
    {3, 8},    {209, 12}, {157, 6},  {110, 8},  {70, 6},   {128, 8},  {76, 6},
    {94, 6},   {5, 8},    {193, 6},  {82, 6},   {100, 6},  {9, 8},    {118, 6},
    {17, 8},   {33, 8},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {189, 8},  {152, 7},  {164, 7},  {145, 3},  {201, 8},  {88, 8},   {106, 8},
    {69, 7},   {124, 8},  {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},
    {112, 8},  {71, 7},   {130, 8},  {77, 7},   {95, 7},   {6, 8},    {194, 7},
    {83, 7},   {101, 7},  {10, 8},   {119, 7},  {18, 8},   {34, 8},   {1, 7},
    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {136, 8},  {79, 7},   {97, 7},
    {66, 6},   {197, 7},  {85, 7},   {103, 7},  {12, 8},   {121, 7},  {20, 8},
    {36, 8},   {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},
    {24, 8},   {40, 8},   {4, 7},    {193, 6},  {82, 6},   {48, 8},   {8, 7},
    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},  {209, 12}, {209, 12},
    {209, 12}, {146, 4},  {209, 12}, {149, 4},  {161, 4},  {64, 4},   {209, 12},
    {160, 9},  {172, 9},  {147, 5},  {184, 9},  {150, 5},  {162, 5},  {65, 5},
    {196, 9},  {153, 5},  {165, 5},  {67, 5},   {177, 5},  {73, 5},   {91, 5},
    {64, 4},   {209, 12}, {209, 12}, {175, 9},  {148, 6},  {187, 9},  {81, 9},
    {99, 9},   {66, 6},   {199, 9},  {87, 9},   {105, 9},  {68, 6},   {123, 9},
    {74, 6},   {92, 6},   {64, 4},   {209, 12}, {157, 6},  {111, 9},  {70, 6},
    {129, 9},  {76, 6},   {94, 6},   {65, 5},   {193, 6},  {82, 6},   {100, 6},
    {67, 5},   {118, 6},  {73, 5},   {91, 5},   {0, 6},    {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {190, 9},  {152, 7},  {164, 7},  {145, 3},  {202, 9},
    {89, 9},   {107, 9},  {69, 7},   {125, 9},  {75, 7},   {93, 7},   {64, 4},
    {209, 12}, {158, 7},  {113, 9},  {71, 7},   {131, 9},  {77, 7},   {95, 7},
    {7, 9},    {194, 7},  {83, 7},   {101, 7},  {11, 9},   {119, 7},  {19, 9},
    {35, 9},   {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {137, 9},
    {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},   {103, 7},  {13, 9},
    {121, 7},  {21, 9},   {37, 9},   {2, 7},    {209, 12}, {157, 6},  {109, 7},
    {70, 6},   {127, 7},  {25, 9},   {41, 9},   {4, 7},    {193, 6},  {82, 6},
    {49, 9},   {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},
    {205, 9},  {156, 8},  {168, 8},  {146, 4},  {180, 8},  {149, 4},  {161, 4},
    {64, 4},   {209, 12}, {159, 8},  {115, 9},  {72, 8},   {133, 9},  {78, 8},
    {96, 8},   {65, 5},   {195, 8},  {84, 8},   {102, 8},  {67, 5},   {120, 8},
    {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12}, {174, 8},  {148, 6},
    {139, 9},  {80, 8},   {98, 8},   {66, 6},   {198, 8},  {86, 8},   {104, 8},
    {14, 9},   {122, 8},  {22, 9},   {38, 9},   {3, 8},    {209, 12}, {157, 6},
    {110, 8},  {70, 6},   {128, 8},  {26, 9},   {42, 9},   {5, 8},    {193, 6},
    {82, 6},   {50, 9},   {9, 8},    {118, 6},  {17, 8},   {33, 8},   {0, 6},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {189, 8},  {152, 7},  {164, 7},
    {145, 3},  {201, 8},  {88, 8},   {106, 8},  {69, 7},   {124, 8},  {75, 7},
    {93, 7},   {64, 4},   {209, 12}, {158, 7},  {112, 8},  {71, 7},   {130, 8},
    {28, 9},   {44, 9},   {6, 8},    {194, 7},  {83, 7},   {52, 9},   {10, 8},
    {119, 7},  {18, 8},   {34, 8},   {1, 7},    {209, 12}, {209, 12}, {173, 7},
    {148, 6},  {136, 8},  {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},
    {56, 9},   {12, 8},   {121, 7},  {20, 8},   {36, 8},   {2, 7},    {209, 12},
    {157, 6},  {109, 7},  {70, 6},   {127, 7},  {24, 8},   {40, 8},   {4, 7},
    {193, 6},  {82, 6},   {48, 8},   {8, 7},    {118, 6},  {16, 7},   {32, 7},
    {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {145, 3},  {209, 12}, {209, 12}, {209, 12}, {146, 4},  {209, 12},
    {149, 4},  {161, 4},  {64, 4},   {209, 12}, {209, 12}, {209, 12}, {147, 5},
    {209, 12}, {150, 5},  {162, 5},  {65, 5},   {209, 12}, {153, 5},  {165, 5},
    {67, 5},   {177, 5},  {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12},
    {176, 10}, {148, 6},  {188, 10}, {151, 6},  {163, 6},  {66, 6},   {200, 10},
    {154, 6},  {166, 6},  {68, 6},   {178, 6},  {74, 6},   {92, 6},   {64, 4},
    {209, 12}, {157, 6},  {169, 6},  {70, 6},   {181, 6},  {76, 6},   {94, 6},
    {65, 5},   {193, 6},  {82, 6},   {100, 6},  {67, 5},   {118, 6},  {73, 5},
    {91, 5},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {191, 10},
    {152, 7},  {164, 7},  {145, 3},  {203, 10}, {90, 10},  {108, 10}, {69, 7},
    {126, 10}, {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},  {114, 10},
    {71, 7},   {132, 10}, {77, 7},   {95, 7},   {65, 5},   {194, 7},  {83, 7},
    {101, 7},  {67, 5},   {119, 7},  {73, 5},   {91, 5},   {1, 7},    {209, 12},
    {209, 12}, {173, 7},  {148, 6},  {138, 10}, {79, 7},   {97, 7},   {66, 6},
    {197, 7},  {85, 7},   {103, 7},  {68, 6},   {121, 7},  {74, 6},   {92, 6},
    {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},  {76, 6},
    {94, 6},   {4, 7},    {193, 6},  {82, 6},   {100, 6},  {8, 7},    {118, 6},
    {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {145, 3},  {206, 10}, {156, 8},  {168, 8},
    {146, 4},  {180, 8},  {149, 4},  {161, 4},  {64, 4},   {209, 12}, {159, 8},
    {116, 10}, {72, 8},   {134, 10}, {78, 8},   {96, 8},   {65, 5},   {195, 8},
    {84, 8},   {102, 8},  {67, 5},   {120, 8},  {73, 5},   {91, 5},   {64, 4},
    {209, 12}, {209, 12}, {174, 8},  {148, 6},  {140, 10}, {80, 8},   {98, 8},
    {66, 6},   {198, 8},  {86, 8},   {104, 8},  {15, 10},  {122, 8},  {23, 10},
    {39, 10},  {3, 8},    {209, 12}, {157, 6},  {110, 8},  {70, 6},   {128, 8},
    {27, 10},  {43, 10},  {5, 8},    {193, 6},  {82, 6},   {51, 10},  {9, 8},
    {118, 6},  {17, 8},   {33, 8},   {0, 6},    {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {189, 8},  {152, 7},  {164, 7},  {145, 3},  {201, 8},  {88, 8},
    {106, 8},  {69, 7},   {124, 8},  {75, 7},   {93, 7},   {64, 4},   {209, 12},
    {158, 7},  {112, 8},  {71, 7},   {130, 8},  {29, 10},  {45, 10},  {6, 8},
    {194, 7},  {83, 7},   {53, 10},  {10, 8},   {119, 7},  {18, 8},   {34, 8},
    {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {136, 8},  {79, 7},
    {97, 7},   {66, 6},   {197, 7},  {85, 7},   {57, 10},  {12, 8},   {121, 7},
    {20, 8},   {36, 8},   {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},
    {127, 7},  {24, 8},   {40, 8},   {4, 7},    {193, 6},  {82, 6},   {48, 8},
    {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},  {209, 12},
    {209, 12}, {209, 12}, {146, 4},  {209, 12}, {149, 4},  {161, 4},  {64, 4},
    {209, 12}, {160, 9},  {172, 9},  {147, 5},  {184, 9},  {150, 5},  {162, 5},
    {65, 5},   {196, 9},  {153, 5},  {165, 5},  {67, 5},   {177, 5},  {73, 5},
    {91, 5},   {64, 4},   {209, 12}, {209, 12}, {175, 9},  {148, 6},  {142, 10},
    {81, 9},   {99, 9},   {66, 6},   {199, 9},  {87, 9},   {105, 9},  {68, 6},
    {123, 9},  {74, 6},   {92, 6},   {64, 4},   {209, 12}, {157, 6},  {111, 9},
    {70, 6},   {129, 9},  {76, 6},   {94, 6},   {65, 5},   {193, 6},  {82, 6},
    {100, 6},  {67, 5},   {118, 6},  {73, 5},   {91, 5},   {0, 6},    {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {190, 9},  {152, 7},  {164, 7},  {145, 3},
    {202, 9},  {89, 9},   {107, 9},  {69, 7},   {125, 9},  {75, 7},   {93, 7},
    {64, 4},   {209, 12}, {158, 7},  {113, 9},  {71, 7},   {131, 9},  {30, 10},
    {46, 10},  {7, 9},    {194, 7},  {83, 7},   {54, 10},  {11, 9},   {119, 7},
    {19, 9},   {35, 9},   {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},
    {137, 9},  {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},   {58, 10},
    {13, 9},   {121, 7},  {21, 9},   {37, 9},   {2, 7},    {209, 12}, {157, 6},
    {109, 7},  {70, 6},   {127, 7},  {25, 9},   {41, 9},   {4, 7},    {193, 6},
    {82, 6},   {49, 9},   {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {145, 3},  {205, 9},  {156, 8},  {168, 8},  {146, 4},  {180, 8},  {149, 4},
    {161, 4},  {64, 4},   {209, 12}, {159, 8},  {115, 9},  {72, 8},   {133, 9},
    {78, 8},   {96, 8},   {65, 5},   {195, 8},  {84, 8},   {102, 8},  {67, 5},
    {120, 8},  {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12}, {174, 8},
    {148, 6},  {139, 9},  {80, 8},   {98, 8},   {66, 6},   {198, 8},  {86, 8},
    {60, 10},  {14, 9},   {122, 8},  {22, 9},   {38, 9},   {3, 8},    {209, 12},
    {157, 6},  {110, 8},  {70, 6},   {128, 8},  {26, 9},   {42, 9},   {5, 8},
    {193, 6},  {82, 6},   {50, 9},   {9, 8},    {118, 6},  {17, 8},   {33, 8},
    {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {189, 8},  {152, 7},
    {164, 7},  {145, 3},  {201, 8},  {88, 8},   {106, 8},  {69, 7},   {124, 8},
    {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},  {112, 8},  {71, 7},
    {130, 8},  {28, 9},   {44, 9},   {6, 8},    {194, 7},  {83, 7},   {52, 9},
    {10, 8},   {119, 7},  {18, 8},   {34, 8},   {1, 7},    {209, 12}, {209, 12},
    {173, 7},  {148, 6},  {136, 8},  {79, 7},   {97, 7},   {66, 6},   {197, 7},
    {85, 7},   {56, 9},   {12, 8},   {121, 7},  {20, 8},   {36, 8},   {2, 7},
    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},  {24, 8},   {40, 8},
    {4, 7},    {193, 6},  {82, 6},   {48, 8},   {8, 7},    {118, 6},  {16, 7},
    {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {145, 3},  {209, 12}, {209, 12}, {209, 12}, {146, 4},
    {209, 12}, {149, 4},  {161, 4},  {64, 4},   {209, 12}, {209, 12}, {209, 12},
    {147, 5},  {209, 12}, {150, 5},  {162, 5},  {65, 5},   {209, 12}, {153, 5},
    {165, 5},  {67, 5},   {177, 5},  {73, 5},   {91, 5},   {64, 4},   {209, 12},
    {209, 12}, {209, 12}, {148, 6},  {209, 12}, {151, 6},  {163, 6},  {66, 6},
    {209, 12}, {154, 6},  {166, 6},  {68, 6},   {178, 6},  {74, 6},   {92, 6},
    {64, 4},   {209, 12}, {157, 6},  {169, 6},  {70, 6},   {181, 6},  {76, 6},
    {94, 6},   {65, 5},   {193, 6},  {82, 6},   {100, 6},  {67, 5},   {118, 6},
    {73, 5},   {91, 5},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {192, 11}, {152, 7},  {164, 7},  {145, 3},  {204, 11}, {155, 7},  {167, 7},
    {69, 7},   {179, 7},  {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},
    {170, 7},  {71, 7},   {182, 7},  {77, 7},   {95, 7},   {65, 5},   {194, 7},
    {83, 7},   {101, 7},  {67, 5},   {119, 7},  {73, 5},   {91, 5},   {1, 7},
    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {185, 7},  {79, 7},   {97, 7},
    {66, 6},   {197, 7},  {85, 7},   {103, 7},  {68, 6},   {121, 7},  {74, 6},
    {92, 6},   {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},
    {76, 6},   {94, 6},   {4, 7},    {193, 6},  {82, 6},   {100, 6},  {8, 7},
    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},  {207, 11}, {156, 8},
    {168, 8},  {146, 4},  {180, 8},  {149, 4},  {161, 4},  {64, 4},   {209, 12},
    {159, 8},  {117, 11}, {72, 8},   {135, 11}, {78, 8},   {96, 8},   {65, 5},
    {195, 8},  {84, 8},   {102, 8},  {67, 5},   {120, 8},  {73, 5},   {91, 5},
    {64, 4},   {209, 12}, {209, 12}, {174, 8},  {148, 6},  {141, 11}, {80, 8},
    {98, 8},   {66, 6},   {198, 8},  {86, 8},   {104, 8},  {68, 6},   {122, 8},
    {74, 6},   {92, 6},   {3, 8},    {209, 12}, {157, 6},  {110, 8},  {70, 6},
    {128, 8},  {76, 6},   {94, 6},   {5, 8},    {193, 6},  {82, 6},   {100, 6},
    {9, 8},    {118, 6},  {17, 8},   {33, 8},   {0, 6},    {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {189, 8},  {152, 7},  {164, 7},  {145, 3},  {201, 8},
    {88, 8},   {106, 8},  {69, 7},   {124, 8},  {75, 7},   {93, 7},   {64, 4},
    {209, 12}, {158, 7},  {112, 8},  {71, 7},   {130, 8},  {77, 7},   {95, 7},
    {6, 8},    {194, 7},  {83, 7},   {101, 7},  {10, 8},   {119, 7},  {18, 8},
    {34, 8},   {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {136, 8},
    {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},   {103, 7},  {12, 8},
    {121, 7},  {20, 8},   {36, 8},   {2, 7},    {209, 12}, {157, 6},  {109, 7},
    {70, 6},   {127, 7},  {24, 8},   {40, 8},   {4, 7},    {193, 6},  {82, 6},
    {48, 8},   {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},
    {209, 12}, {209, 12}, {209, 12}, {146, 4},  {209, 12}, {149, 4},  {161, 4},
    {64, 4},   {209, 12}, {160, 9},  {172, 9},  {147, 5},  {184, 9},  {150, 5},
    {162, 5},  {65, 5},   {196, 9},  {153, 5},  {165, 5},  {67, 5},   {177, 5},
    {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12}, {175, 9},  {148, 6},
    {143, 11}, {81, 9},   {99, 9},   {66, 6},   {199, 9},  {87, 9},   {105, 9},
    {68, 6},   {123, 9},  {74, 6},   {92, 6},   {64, 4},   {209, 12}, {157, 6},
    {111, 9},  {70, 6},   {129, 9},  {76, 6},   {94, 6},   {65, 5},   {193, 6},
    {82, 6},   {100, 6},  {67, 5},   {118, 6},  {73, 5},   {91, 5},   {0, 6},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {190, 9},  {152, 7},  {164, 7},
    {145, 3},  {202, 9},  {89, 9},   {107, 9},  {69, 7},   {125, 9},  {75, 7},
    {93, 7},   {64, 4},   {209, 12}, {158, 7},  {113, 9},  {71, 7},   {131, 9},
    {31, 11},  {47, 11},  {7, 9},    {194, 7},  {83, 7},   {55, 11},  {11, 9},
    {119, 7},  {19, 9},   {35, 9},   {1, 7},    {209, 12}, {209, 12}, {173, 7},
    {148, 6},  {137, 9},  {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},
    {59, 11},  {13, 9},   {121, 7},  {21, 9},   {37, 9},   {2, 7},    {209, 12},
    {157, 6},  {109, 7},  {70, 6},   {127, 7},  {25, 9},   {41, 9},   {4, 7},
    {193, 6},  {82, 6},   {49, 9},   {8, 7},    {118, 6},  {16, 7},   {32, 7},
    {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {145, 3},  {205, 9},  {156, 8},  {168, 8},  {146, 4},  {180, 8},
    {149, 4},  {161, 4},  {64, 4},   {209, 12}, {159, 8},  {115, 9},  {72, 8},
    {133, 9},  {78, 8},   {96, 8},   {65, 5},   {195, 8},  {84, 8},   {102, 8},
    {67, 5},   {120, 8},  {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12},
    {174, 8},  {148, 6},  {139, 9},  {80, 8},   {98, 8},   {66, 6},   {198, 8},
    {86, 8},   {61, 11},  {14, 9},   {122, 8},  {22, 9},   {38, 9},   {3, 8},
    {209, 12}, {157, 6},  {110, 8},  {70, 6},   {128, 8},  {26, 9},   {42, 9},
    {5, 8},    {193, 6},  {82, 6},   {50, 9},   {9, 8},    {118, 6},  {17, 8},
    {33, 8},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {189, 8},
    {152, 7},  {164, 7},  {145, 3},  {201, 8},  {88, 8},   {106, 8},  {69, 7},
    {124, 8},  {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},  {112, 8},
    {71, 7},   {130, 8},  {28, 9},   {44, 9},   {6, 8},    {194, 7},  {83, 7},
    {52, 9},   {10, 8},   {119, 7},  {18, 8},   {34, 8},   {1, 7},    {209, 12},
    {209, 12}, {173, 7},  {148, 6},  {136, 8},  {79, 7},   {97, 7},   {66, 6},
    {197, 7},  {85, 7},   {56, 9},   {12, 8},   {121, 7},  {20, 8},   {36, 8},
    {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},  {24, 8},
    {40, 8},   {4, 7},    {193, 6},  {82, 6},   {48, 8},   {8, 7},    {118, 6},
    {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {145, 3},  {209, 12}, {209, 12}, {209, 12},
    {146, 4},  {209, 12}, {149, 4},  {161, 4},  {64, 4},   {209, 12}, {209, 12},
    {209, 12}, {147, 5},  {209, 12}, {150, 5},  {162, 5},  {65, 5},   {209, 12},
    {153, 5},  {165, 5},  {67, 5},   {177, 5},  {73, 5},   {91, 5},   {64, 4},
    {209, 12}, {209, 12}, {176, 10}, {148, 6},  {188, 10}, {151, 6},  {163, 6},
    {66, 6},   {200, 10}, {154, 6},  {166, 6},  {68, 6},   {178, 6},  {74, 6},
    {92, 6},   {64, 4},   {209, 12}, {157, 6},  {169, 6},  {70, 6},   {181, 6},
    {76, 6},   {94, 6},   {65, 5},   {193, 6},  {82, 6},   {100, 6},  {67, 5},
    {118, 6},  {73, 5},   {91, 5},   {0, 6},    {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {191, 10}, {152, 7},  {164, 7},  {145, 3},  {203, 10}, {90, 10},
    {108, 10}, {69, 7},   {126, 10}, {75, 7},   {93, 7},   {64, 4},   {209, 12},
    {158, 7},  {114, 10}, {71, 7},   {132, 10}, {77, 7},   {95, 7},   {65, 5},
    {194, 7},  {83, 7},   {101, 7},  {67, 5},   {119, 7},  {73, 5},   {91, 5},
    {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {138, 10}, {79, 7},
    {97, 7},   {66, 6},   {197, 7},  {85, 7},   {103, 7},  {68, 6},   {121, 7},
    {74, 6},   {92, 6},   {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},
    {127, 7},  {76, 6},   {94, 6},   {4, 7},    {193, 6},  {82, 6},   {100, 6},
    {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},  {206, 10},
    {156, 8},  {168, 8},  {146, 4},  {180, 8},  {149, 4},  {161, 4},  {64, 4},
    {209, 12}, {159, 8},  {116, 10}, {72, 8},   {134, 10}, {78, 8},   {96, 8},
    {65, 5},   {195, 8},  {84, 8},   {102, 8},  {67, 5},   {120, 8},  {73, 5},
    {91, 5},   {64, 4},   {209, 12}, {209, 12}, {174, 8},  {148, 6},  {140, 10},
    {80, 8},   {98, 8},   {66, 6},   {198, 8},  {86, 8},   {62, 11},  {15, 10},
    {122, 8},  {23, 10},  {39, 10},  {3, 8},    {209, 12}, {157, 6},  {110, 8},
    {70, 6},   {128, 8},  {27, 10},  {43, 10},  {5, 8},    {193, 6},  {82, 6},
    {51, 10},  {9, 8},    {118, 6},  {17, 8},   {33, 8},   {0, 6},    {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {189, 8},  {152, 7},  {164, 7},  {145, 3},
    {201, 8},  {88, 8},   {106, 8},  {69, 7},   {124, 8},  {75, 7},   {93, 7},
    {64, 4},   {209, 12}, {158, 7},  {112, 8},  {71, 7},   {130, 8},  {29, 10},
    {45, 10},  {6, 8},    {194, 7},  {83, 7},   {53, 10},  {10, 8},   {119, 7},
    {18, 8},   {34, 8},   {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},
    {136, 8},  {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},   {57, 10},
    {12, 8},   {121, 7},  {20, 8},   {36, 8},   {2, 7},    {209, 12}, {157, 6},
    {109, 7},  {70, 6},   {127, 7},  {24, 8},   {40, 8},   {4, 7},    {193, 6},
    {82, 6},   {48, 8},   {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {145, 3},  {209, 12}, {209, 12}, {209, 12}, {146, 4},  {209, 12}, {149, 4},
    {161, 4},  {64, 4},   {209, 12}, {160, 9},  {172, 9},  {147, 5},  {184, 9},
    {150, 5},  {162, 5},  {65, 5},   {196, 9},  {153, 5},  {165, 5},  {67, 5},
    {177, 5},  {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12}, {175, 9},
    {148, 6},  {142, 10}, {81, 9},   {99, 9},   {66, 6},   {199, 9},  {87, 9},
    {105, 9},  {68, 6},   {123, 9},  {74, 6},   {92, 6},   {64, 4},   {209, 12},
    {157, 6},  {111, 9},  {70, 6},   {129, 9},  {76, 6},   {94, 6},   {65, 5},
    {193, 6},  {82, 6},   {100, 6},  {67, 5},   {118, 6},  {73, 5},   {91, 5},
    {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {190, 9},  {152, 7},
    {164, 7},  {145, 3},  {202, 9},  {89, 9},   {107, 9},  {69, 7},   {125, 9},
    {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},  {113, 9},  {71, 7},
    {131, 9},  {30, 10},  {46, 10},  {7, 9},    {194, 7},  {83, 7},   {54, 10},
    {11, 9},   {119, 7},  {19, 9},   {35, 9},   {1, 7},    {209, 12}, {209, 12},
    {173, 7},  {148, 6},  {137, 9},  {79, 7},   {97, 7},   {66, 6},   {197, 7},
    {85, 7},   {58, 10},  {13, 9},   {121, 7},  {21, 9},   {37, 9},   {2, 7},
    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},  {25, 9},   {41, 9},
    {4, 7},    {193, 6},  {82, 6},   {49, 9},   {8, 7},    {118, 6},  {16, 7},
    {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {145, 3},  {205, 9},  {156, 8},  {168, 8},  {146, 4},
    {180, 8},  {149, 4},  {161, 4},  {64, 4},   {209, 12}, {159, 8},  {115, 9},
    {72, 8},   {133, 9},  {78, 8},   {96, 8},   {65, 5},   {195, 8},  {84, 8},
    {102, 8},  {67, 5},   {120, 8},  {73, 5},   {91, 5},   {64, 4},   {209, 12},
    {209, 12}, {174, 8},  {148, 6},  {139, 9},  {80, 8},   {98, 8},   {66, 6},
    {198, 8},  {86, 8},   {60, 10},  {14, 9},   {122, 8},  {22, 9},   {38, 9},
    {3, 8},    {209, 12}, {157, 6},  {110, 8},  {70, 6},   {128, 8},  {26, 9},
    {42, 9},   {5, 8},    {193, 6},  {82, 6},   {50, 9},   {9, 8},    {118, 6},
    {17, 8},   {33, 8},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {189, 8},  {152, 7},  {164, 7},  {145, 3},  {201, 8},  {88, 8},   {106, 8},
    {69, 7},   {124, 8},  {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},
    {112, 8},  {71, 7},   {130, 8},  {28, 9},   {44, 9},   {6, 8},    {194, 7},
    {83, 7},   {52, 9},   {10, 8},   {119, 7},  {18, 8},   {34, 8},   {1, 7},
    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {136, 8},  {79, 7},   {97, 7},
    {66, 6},   {197, 7},  {85, 7},   {56, 9},   {12, 8},   {121, 7},  {20, 8},
    {36, 8},   {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},
    {24, 8},   {40, 8},   {4, 7},    {193, 6},  {82, 6},   {48, 8},   {8, 7},
    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},  {209, 12}, {209, 12},
    {209, 12}, {146, 4},  {209, 12}, {149, 4},  {161, 4},  {64, 4},   {209, 12},
    {209, 12}, {209, 12}, {147, 5},  {209, 12}, {150, 5},  {162, 5},  {65, 5},
    {209, 12}, {153, 5},  {165, 5},  {67, 5},   {177, 5},  {73, 5},   {91, 5},
    {64, 4},   {209, 12}, {209, 12}, {209, 12}, {148, 6},  {209, 12}, {151, 6},
    {163, 6},  {66, 6},   {209, 12}, {154, 6},  {166, 6},  {68, 6},   {178, 6},
    {74, 6},   {92, 6},   {64, 4},   {209, 12}, {157, 6},  {169, 6},  {70, 6},
    {181, 6},  {76, 6},   {94, 6},   {65, 5},   {193, 6},  {82, 6},   {100, 6},
    {67, 5},   {118, 6},  {73, 5},   {91, 5},   {0, 6},    {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {152, 7},  {164, 7},  {145, 3},  {209, 12},
    {155, 7},  {167, 7},  {69, 7},   {179, 7},  {75, 7},   {93, 7},   {64, 4},
    {209, 12}, {158, 7},  {170, 7},  {71, 7},   {182, 7},  {77, 7},   {95, 7},
    {65, 5},   {194, 7},  {83, 7},   {101, 7},  {67, 5},   {119, 7},  {73, 5},
    {91, 5},   {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {185, 7},
    {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},   {103, 7},  {68, 6},
    {121, 7},  {74, 6},   {92, 6},   {2, 7},    {209, 12}, {157, 6},  {109, 7},
    {70, 6},   {127, 7},  {76, 6},   {94, 6},   {4, 7},    {193, 6},  {82, 6},
    {100, 6},  {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},
    {208, 12}, {156, 8},  {168, 8},  {146, 4},  {180, 8},  {149, 4},  {161, 4},
    {64, 4},   {209, 12}, {159, 8},  {171, 8},  {72, 8},   {183, 8},  {78, 8},
    {96, 8},   {65, 5},   {195, 8},  {84, 8},   {102, 8},  {67, 5},   {120, 8},
    {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12}, {174, 8},  {148, 6},
    {186, 8},  {80, 8},   {98, 8},   {66, 6},   {198, 8},  {86, 8},   {104, 8},
    {68, 6},   {122, 8},  {74, 6},   {92, 6},   {3, 8},    {209, 12}, {157, 6},
    {110, 8},  {70, 6},   {128, 8},  {76, 6},   {94, 6},   {5, 8},    {193, 6},
    {82, 6},   {100, 6},  {9, 8},    {118, 6},  {17, 8},   {33, 8},   {0, 6},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {189, 8},  {152, 7},  {164, 7},
    {145, 3},  {201, 8},  {88, 8},   {106, 8},  {69, 7},   {124, 8},  {75, 7},
    {93, 7},   {64, 4},   {209, 12}, {158, 7},  {112, 8},  {71, 7},   {130, 8},
    {77, 7},   {95, 7},   {6, 8},    {194, 7},  {83, 7},   {101, 7},  {10, 8},
    {119, 7},  {18, 8},   {34, 8},   {1, 7},    {209, 12}, {209, 12}, {173, 7},
    {148, 6},  {136, 8},  {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},
    {103, 7},  {12, 8},   {121, 7},  {20, 8},   {36, 8},   {2, 7},    {209, 12},
    {157, 6},  {109, 7},  {70, 6},   {127, 7},  {24, 8},   {40, 8},   {4, 7},
    {193, 6},  {82, 6},   {48, 8},   {8, 7},    {118, 6},  {16, 7},   {32, 7},
    {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {145, 3},  {209, 12}, {209, 12}, {209, 12}, {146, 4},  {209, 12},
    {149, 4},  {161, 4},  {64, 4},   {209, 12}, {160, 9},  {172, 9},  {147, 5},
    {184, 9},  {150, 5},  {162, 5},  {65, 5},   {196, 9},  {153, 5},  {165, 5},
    {67, 5},   {177, 5},  {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12},
    {175, 9},  {148, 6},  {144, 12}, {81, 9},   {99, 9},   {66, 6},   {199, 9},
    {87, 9},   {105, 9},  {68, 6},   {123, 9},  {74, 6},   {92, 6},   {64, 4},
    {209, 12}, {157, 6},  {111, 9},  {70, 6},   {129, 9},  {76, 6},   {94, 6},
    {65, 5},   {193, 6},  {82, 6},   {100, 6},  {67, 5},   {118, 6},  {73, 5},
    {91, 5},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {190, 9},
    {152, 7},  {164, 7},  {145, 3},  {202, 9},  {89, 9},   {107, 9},  {69, 7},
    {125, 9},  {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},  {113, 9},
    {71, 7},   {131, 9},  {77, 7},   {95, 7},   {7, 9},    {194, 7},  {83, 7},
    {101, 7},  {11, 9},   {119, 7},  {19, 9},   {35, 9},   {1, 7},    {209, 12},
    {209, 12}, {173, 7},  {148, 6},  {137, 9},  {79, 7},   {97, 7},   {66, 6},
    {197, 7},  {85, 7},   {103, 7},  {13, 9},   {121, 7},  {21, 9},   {37, 9},
    {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},  {25, 9},
    {41, 9},   {4, 7},    {193, 6},  {82, 6},   {49, 9},   {8, 7},    {118, 6},
    {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {145, 3},  {205, 9},  {156, 8},  {168, 8},
    {146, 4},  {180, 8},  {149, 4},  {161, 4},  {64, 4},   {209, 12}, {159, 8},
    {115, 9},  {72, 8},   {133, 9},  {78, 8},   {96, 8},   {65, 5},   {195, 8},
    {84, 8},   {102, 8},  {67, 5},   {120, 8},  {73, 5},   {91, 5},   {64, 4},
    {209, 12}, {209, 12}, {174, 8},  {148, 6},  {139, 9},  {80, 8},   {98, 8},
    {66, 6},   {198, 8},  {86, 8},   {104, 8},  {14, 9},   {122, 8},  {22, 9},
    {38, 9},   {3, 8},    {209, 12}, {157, 6},  {110, 8},  {70, 6},   {128, 8},
    {26, 9},   {42, 9},   {5, 8},    {193, 6},  {82, 6},   {50, 9},   {9, 8},
    {118, 6},  {17, 8},   {33, 8},   {0, 6},    {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {189, 8},  {152, 7},  {164, 7},  {145, 3},  {201, 8},  {88, 8},
    {106, 8},  {69, 7},   {124, 8},  {75, 7},   {93, 7},   {64, 4},   {209, 12},
    {158, 7},  {112, 8},  {71, 7},   {130, 8},  {28, 9},   {44, 9},   {6, 8},
    {194, 7},  {83, 7},   {52, 9},   {10, 8},   {119, 7},  {18, 8},   {34, 8},
    {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {136, 8},  {79, 7},
    {97, 7},   {66, 6},   {197, 7},  {85, 7},   {56, 9},   {12, 8},   {121, 7},
    {20, 8},   {36, 8},   {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},
    {127, 7},  {24, 8},   {40, 8},   {4, 7},    {193, 6},  {82, 6},   {48, 8},
    {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},  {209, 12},
    {209, 12}, {209, 12}, {146, 4},  {209, 12}, {149, 4},  {161, 4},  {64, 4},
    {209, 12}, {209, 12}, {209, 12}, {147, 5},  {209, 12}, {150, 5},  {162, 5},
    {65, 5},   {209, 12}, {153, 5},  {165, 5},  {67, 5},   {177, 5},  {73, 5},
    {91, 5},   {64, 4},   {209, 12}, {209, 12}, {176, 10}, {148, 6},  {188, 10},
    {151, 6},  {163, 6},  {66, 6},   {200, 10}, {154, 6},  {166, 6},  {68, 6},
    {178, 6},  {74, 6},   {92, 6},   {64, 4},   {209, 12}, {157, 6},  {169, 6},
    {70, 6},   {181, 6},  {76, 6},   {94, 6},   {65, 5},   {193, 6},  {82, 6},
    {100, 6},  {67, 5},   {118, 6},  {73, 5},   {91, 5},   {0, 6},    {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {191, 10}, {152, 7},  {164, 7},  {145, 3},
    {203, 10}, {90, 10},  {108, 10}, {69, 7},   {126, 10}, {75, 7},   {93, 7},
    {64, 4},   {209, 12}, {158, 7},  {114, 10}, {71, 7},   {132, 10}, {77, 7},
    {95, 7},   {65, 5},   {194, 7},  {83, 7},   {101, 7},  {67, 5},   {119, 7},
    {73, 5},   {91, 5},   {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},
    {138, 10}, {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},   {103, 7},
    {68, 6},   {121, 7},  {74, 6},   {92, 6},   {2, 7},    {209, 12}, {157, 6},
    {109, 7},  {70, 6},   {127, 7},  {76, 6},   {94, 6},   {4, 7},    {193, 6},
    {82, 6},   {100, 6},  {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {145, 3},  {206, 10}, {156, 8},  {168, 8},  {146, 4},  {180, 8},  {149, 4},
    {161, 4},  {64, 4},   {209, 12}, {159, 8},  {116, 10}, {72, 8},   {134, 10},
    {78, 8},   {96, 8},   {65, 5},   {195, 8},  {84, 8},   {102, 8},  {67, 5},
    {120, 8},  {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12}, {174, 8},
    {148, 6},  {140, 10}, {80, 8},   {98, 8},   {66, 6},   {198, 8},  {86, 8},
    {63, 12},  {15, 10},  {122, 8},  {23, 10},  {39, 10},  {3, 8},    {209, 12},
    {157, 6},  {110, 8},  {70, 6},   {128, 8},  {27, 10},  {43, 10},  {5, 8},
    {193, 6},  {82, 6},   {51, 10},  {9, 8},    {118, 6},  {17, 8},   {33, 8},
    {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {189, 8},  {152, 7},
    {164, 7},  {145, 3},  {201, 8},  {88, 8},   {106, 8},  {69, 7},   {124, 8},
    {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},  {112, 8},  {71, 7},
    {130, 8},  {29, 10},  {45, 10},  {6, 8},    {194, 7},  {83, 7},   {53, 10},
    {10, 8},   {119, 7},  {18, 8},   {34, 8},   {1, 7},    {209, 12}, {209, 12},
    {173, 7},  {148, 6},  {136, 8},  {79, 7},   {97, 7},   {66, 6},   {197, 7},
    {85, 7},   {57, 10},  {12, 8},   {121, 7},  {20, 8},   {36, 8},   {2, 7},
    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},  {24, 8},   {40, 8},
    {4, 7},    {193, 6},  {82, 6},   {48, 8},   {8, 7},    {118, 6},  {16, 7},
    {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {145, 3},  {209, 12}, {209, 12}, {209, 12}, {146, 4},
    {209, 12}, {149, 4},  {161, 4},  {64, 4},   {209, 12}, {160, 9},  {172, 9},
    {147, 5},  {184, 9},  {150, 5},  {162, 5},  {65, 5},   {196, 9},  {153, 5},
    {165, 5},  {67, 5},   {177, 5},  {73, 5},   {91, 5},   {64, 4},   {209, 12},
    {209, 12}, {175, 9},  {148, 6},  {142, 10}, {81, 9},   {99, 9},   {66, 6},
    {199, 9},  {87, 9},   {105, 9},  {68, 6},   {123, 9},  {74, 6},   {92, 6},
    {64, 4},   {209, 12}, {157, 6},  {111, 9},  {70, 6},   {129, 9},  {76, 6},
    {94, 6},   {65, 5},   {193, 6},  {82, 6},   {100, 6},  {67, 5},   {118, 6},
    {73, 5},   {91, 5},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {190, 9},  {152, 7},  {164, 7},  {145, 3},  {202, 9},  {89, 9},   {107, 9},
    {69, 7},   {125, 9},  {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},
    {113, 9},  {71, 7},   {131, 9},  {30, 10},  {46, 10},  {7, 9},    {194, 7},
    {83, 7},   {54, 10},  {11, 9},   {119, 7},  {19, 9},   {35, 9},   {1, 7},
    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {137, 9},  {79, 7},   {97, 7},
    {66, 6},   {197, 7},  {85, 7},   {58, 10},  {13, 9},   {121, 7},  {21, 9},
    {37, 9},   {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},
    {25, 9},   {41, 9},   {4, 7},    {193, 6},  {82, 6},   {49, 9},   {8, 7},
    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},  {205, 9},  {156, 8},
    {168, 8},  {146, 4},  {180, 8},  {149, 4},  {161, 4},  {64, 4},   {209, 12},
    {159, 8},  {115, 9},  {72, 8},   {133, 9},  {78, 8},   {96, 8},   {65, 5},
    {195, 8},  {84, 8},   {102, 8},  {67, 5},   {120, 8},  {73, 5},   {91, 5},
    {64, 4},   {209, 12}, {209, 12}, {174, 8},  {148, 6},  {139, 9},  {80, 8},
    {98, 8},   {66, 6},   {198, 8},  {86, 8},   {60, 10},  {14, 9},   {122, 8},
    {22, 9},   {38, 9},   {3, 8},    {209, 12}, {157, 6},  {110, 8},  {70, 6},
    {128, 8},  {26, 9},   {42, 9},   {5, 8},    {193, 6},  {82, 6},   {50, 9},
    {9, 8},    {118, 6},  {17, 8},   {33, 8},   {0, 6},    {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {189, 8},  {152, 7},  {164, 7},  {145, 3},  {201, 8},
    {88, 8},   {106, 8},  {69, 7},   {124, 8},  {75, 7},   {93, 7},   {64, 4},
    {209, 12}, {158, 7},  {112, 8},  {71, 7},   {130, 8},  {28, 9},   {44, 9},
    {6, 8},    {194, 7},  {83, 7},   {52, 9},   {10, 8},   {119, 7},  {18, 8},
    {34, 8},   {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {136, 8},
    {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},   {56, 9},   {12, 8},
    {121, 7},  {20, 8},   {36, 8},   {2, 7},    {209, 12}, {157, 6},  {109, 7},
    {70, 6},   {127, 7},  {24, 8},   {40, 8},   {4, 7},    {193, 6},  {82, 6},
    {48, 8},   {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},
    {209, 12}, {209, 12}, {209, 12}, {146, 4},  {209, 12}, {149, 4},  {161, 4},
    {64, 4},   {209, 12}, {209, 12}, {209, 12}, {147, 5},  {209, 12}, {150, 5},
    {162, 5},  {65, 5},   {209, 12}, {153, 5},  {165, 5},  {67, 5},   {177, 5},
    {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12}, {209, 12}, {148, 6},
    {209, 12}, {151, 6},  {163, 6},  {66, 6},   {209, 12}, {154, 6},  {166, 6},
    {68, 6},   {178, 6},  {74, 6},   {92, 6},   {64, 4},   {209, 12}, {157, 6},
    {169, 6},  {70, 6},   {181, 6},  {76, 6},   {94, 6},   {65, 5},   {193, 6},
    {82, 6},   {100, 6},  {67, 5},   {118, 6},  {73, 5},   {91, 5},   {0, 6},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {192, 11}, {152, 7},  {164, 7},
    {145, 3},  {204, 11}, {155, 7},  {167, 7},  {69, 7},   {179, 7},  {75, 7},
    {93, 7},   {64, 4},   {209, 12}, {158, 7},  {170, 7},  {71, 7},   {182, 7},
    {77, 7},   {95, 7},   {65, 5},   {194, 7},  {83, 7},   {101, 7},  {67, 5},
    {119, 7},  {73, 5},   {91, 5},   {1, 7},    {209, 12}, {209, 12}, {173, 7},
    {148, 6},  {185, 7},  {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},
    {103, 7},  {68, 6},   {121, 7},  {74, 6},   {92, 6},   {2, 7},    {209, 12},
    {157, 6},  {109, 7},  {70, 6},   {127, 7},  {76, 6},   {94, 6},   {4, 7},
    {193, 6},  {82, 6},   {100, 6},  {8, 7},    {118, 6},  {16, 7},   {32, 7},
    {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {145, 3},  {207, 11}, {156, 8},  {168, 8},  {146, 4},  {180, 8},
    {149, 4},  {161, 4},  {64, 4},   {209, 12}, {159, 8},  {117, 11}, {72, 8},
    {135, 11}, {78, 8},   {96, 8},   {65, 5},   {195, 8},  {84, 8},   {102, 8},
    {67, 5},   {120, 8},  {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12},
    {174, 8},  {148, 6},  {141, 11}, {80, 8},   {98, 8},   {66, 6},   {198, 8},
    {86, 8},   {104, 8},  {68, 6},   {122, 8},  {74, 6},   {92, 6},   {3, 8},
    {209, 12}, {157, 6},  {110, 8},  {70, 6},   {128, 8},  {76, 6},   {94, 6},
    {5, 8},    {193, 6},  {82, 6},   {100, 6},  {9, 8},    {118, 6},  {17, 8},
    {33, 8},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {189, 8},
    {152, 7},  {164, 7},  {145, 3},  {201, 8},  {88, 8},   {106, 8},  {69, 7},
    {124, 8},  {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},  {112, 8},
    {71, 7},   {130, 8},  {77, 7},   {95, 7},   {6, 8},    {194, 7},  {83, 7},
    {101, 7},  {10, 8},   {119, 7},  {18, 8},   {34, 8},   {1, 7},    {209, 12},
    {209, 12}, {173, 7},  {148, 6},  {136, 8},  {79, 7},   {97, 7},   {66, 6},
    {197, 7},  {85, 7},   {103, 7},  {12, 8},   {121, 7},  {20, 8},   {36, 8},
    {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},  {24, 8},
    {40, 8},   {4, 7},    {193, 6},  {82, 6},   {48, 8},   {8, 7},    {118, 6},
    {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {145, 3},  {209, 12}, {209, 12}, {209, 12},
    {146, 4},  {209, 12}, {149, 4},  {161, 4},  {64, 4},   {209, 12}, {160, 9},
    {172, 9},  {147, 5},  {184, 9},  {150, 5},  {162, 5},  {65, 5},   {196, 9},
    {153, 5},  {165, 5},  {67, 5},   {177, 5},  {73, 5},   {91, 5},   {64, 4},
    {209, 12}, {209, 12}, {175, 9},  {148, 6},  {143, 11}, {81, 9},   {99, 9},
    {66, 6},   {199, 9},  {87, 9},   {105, 9},  {68, 6},   {123, 9},  {74, 6},
    {92, 6},   {64, 4},   {209, 12}, {157, 6},  {111, 9},  {70, 6},   {129, 9},
    {76, 6},   {94, 6},   {65, 5},   {193, 6},  {82, 6},   {100, 6},  {67, 5},
    {118, 6},  {73, 5},   {91, 5},   {0, 6},    {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {190, 9},  {152, 7},  {164, 7},  {145, 3},  {202, 9},  {89, 9},
    {107, 9},  {69, 7},   {125, 9},  {75, 7},   {93, 7},   {64, 4},   {209, 12},
    {158, 7},  {113, 9},  {71, 7},   {131, 9},  {31, 11},  {47, 11},  {7, 9},
    {194, 7},  {83, 7},   {55, 11},  {11, 9},   {119, 7},  {19, 9},   {35, 9},
    {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {137, 9},  {79, 7},
    {97, 7},   {66, 6},   {197, 7},  {85, 7},   {59, 11},  {13, 9},   {121, 7},
    {21, 9},   {37, 9},   {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},
    {127, 7},  {25, 9},   {41, 9},   {4, 7},    {193, 6},  {82, 6},   {49, 9},
    {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},  {205, 9},
    {156, 8},  {168, 8},  {146, 4},  {180, 8},  {149, 4},  {161, 4},  {64, 4},
    {209, 12}, {159, 8},  {115, 9},  {72, 8},   {133, 9},  {78, 8},   {96, 8},
    {65, 5},   {195, 8},  {84, 8},   {102, 8},  {67, 5},   {120, 8},  {73, 5},
    {91, 5},   {64, 4},   {209, 12}, {209, 12}, {174, 8},  {148, 6},  {139, 9},
    {80, 8},   {98, 8},   {66, 6},   {198, 8},  {86, 8},   {61, 11},  {14, 9},
    {122, 8},  {22, 9},   {38, 9},   {3, 8},    {209, 12}, {157, 6},  {110, 8},
    {70, 6},   {128, 8},  {26, 9},   {42, 9},   {5, 8},    {193, 6},  {82, 6},
    {50, 9},   {9, 8},    {118, 6},  {17, 8},   {33, 8},   {0, 6},    {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {189, 8},  {152, 7},  {164, 7},  {145, 3},
    {201, 8},  {88, 8},   {106, 8},  {69, 7},   {124, 8},  {75, 7},   {93, 7},
    {64, 4},   {209, 12}, {158, 7},  {112, 8},  {71, 7},   {130, 8},  {28, 9},
    {44, 9},   {6, 8},    {194, 7},  {83, 7},   {52, 9},   {10, 8},   {119, 7},
    {18, 8},   {34, 8},   {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},
    {136, 8},  {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},   {56, 9},
    {12, 8},   {121, 7},  {20, 8},   {36, 8},   {2, 7},    {209, 12}, {157, 6},
    {109, 7},  {70, 6},   {127, 7},  {24, 8},   {40, 8},   {4, 7},    {193, 6},
    {82, 6},   {48, 8},   {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {145, 3},  {209, 12}, {209, 12}, {209, 12}, {146, 4},  {209, 12}, {149, 4},
    {161, 4},  {64, 4},   {209, 12}, {209, 12}, {209, 12}, {147, 5},  {209, 12},
    {150, 5},  {162, 5},  {65, 5},   {209, 12}, {153, 5},  {165, 5},  {67, 5},
    {177, 5},  {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12}, {176, 10},
    {148, 6},  {188, 10}, {151, 6},  {163, 6},  {66, 6},   {200, 10}, {154, 6},
    {166, 6},  {68, 6},   {178, 6},  {74, 6},   {92, 6},   {64, 4},   {209, 12},
    {157, 6},  {169, 6},  {70, 6},   {181, 6},  {76, 6},   {94, 6},   {65, 5},
    {193, 6},  {82, 6},   {100, 6},  {67, 5},   {118, 6},  {73, 5},   {91, 5},
    {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {191, 10}, {152, 7},
    {164, 7},  {145, 3},  {203, 10}, {90, 10},  {108, 10}, {69, 7},   {126, 10},
    {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},  {114, 10}, {71, 7},
    {132, 10}, {77, 7},   {95, 7},   {65, 5},   {194, 7},  {83, 7},   {101, 7},
    {67, 5},   {119, 7},  {73, 5},   {91, 5},   {1, 7},    {209, 12}, {209, 12},
    {173, 7},  {148, 6},  {138, 10}, {79, 7},   {97, 7},   {66, 6},   {197, 7},
    {85, 7},   {103, 7},  {68, 6},   {121, 7},  {74, 6},   {92, 6},   {2, 7},
    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},  {76, 6},   {94, 6},
    {4, 7},    {193, 6},  {82, 6},   {100, 6},  {8, 7},    {118, 6},  {16, 7},
    {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {145, 3},  {206, 10}, {156, 8},  {168, 8},  {146, 4},
    {180, 8},  {149, 4},  {161, 4},  {64, 4},   {209, 12}, {159, 8},  {116, 10},
    {72, 8},   {134, 10}, {78, 8},   {96, 8},   {65, 5},   {195, 8},  {84, 8},
    {102, 8},  {67, 5},   {120, 8},  {73, 5},   {91, 5},   {64, 4},   {209, 12},
    {209, 12}, {174, 8},  {148, 6},  {140, 10}, {80, 8},   {98, 8},   {66, 6},
    {198, 8},  {86, 8},   {62, 11},  {15, 10},  {122, 8},  {23, 10},  {39, 10},
    {3, 8},    {209, 12}, {157, 6},  {110, 8},  {70, 6},   {128, 8},  {27, 10},
    {43, 10},  {5, 8},    {193, 6},  {82, 6},   {51, 10},  {9, 8},    {118, 6},
    {17, 8},   {33, 8},   {0, 6},    {209, 12}, {209, 12}, {209, 12}, {209, 12},
    {189, 8},  {152, 7},  {164, 7},  {145, 3},  {201, 8},  {88, 8},   {106, 8},
    {69, 7},   {124, 8},  {75, 7},   {93, 7},   {64, 4},   {209, 12}, {158, 7},
    {112, 8},  {71, 7},   {130, 8},  {29, 10},  {45, 10},  {6, 8},    {194, 7},
    {83, 7},   {53, 10},  {10, 8},   {119, 7},  {18, 8},   {34, 8},   {1, 7},
    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {136, 8},  {79, 7},   {97, 7},
    {66, 6},   {197, 7},  {85, 7},   {57, 10},  {12, 8},   {121, 7},  {20, 8},
    {36, 8},   {2, 7},    {209, 12}, {157, 6},  {109, 7},  {70, 6},   {127, 7},
    {24, 8},   {40, 8},   {4, 7},    {193, 6},  {82, 6},   {48, 8},   {8, 7},
    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12}, {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},  {209, 12}, {209, 12},
    {209, 12}, {146, 4},  {209, 12}, {149, 4},  {161, 4},  {64, 4},   {209, 12},
    {160, 9},  {172, 9},  {147, 5},  {184, 9},  {150, 5},  {162, 5},  {65, 5},
    {196, 9},  {153, 5},  {165, 5},  {67, 5},   {177, 5},  {73, 5},   {91, 5},
    {64, 4},   {209, 12}, {209, 12}, {175, 9},  {148, 6},  {142, 10}, {81, 9},
    {99, 9},   {66, 6},   {199, 9},  {87, 9},   {105, 9},  {68, 6},   {123, 9},
    {74, 6},   {92, 6},   {64, 4},   {209, 12}, {157, 6},  {111, 9},  {70, 6},
    {129, 9},  {76, 6},   {94, 6},   {65, 5},   {193, 6},  {82, 6},   {100, 6},
    {67, 5},   {118, 6},  {73, 5},   {91, 5},   {0, 6},    {209, 12}, {209, 12},
    {209, 12}, {209, 12}, {190, 9},  {152, 7},  {164, 7},  {145, 3},  {202, 9},
    {89, 9},   {107, 9},  {69, 7},   {125, 9},  {75, 7},   {93, 7},   {64, 4},
    {209, 12}, {158, 7},  {113, 9},  {71, 7},   {131, 9},  {30, 10},  {46, 10},
    {7, 9},    {194, 7},  {83, 7},   {54, 10},  {11, 9},   {119, 7},  {19, 9},
    {35, 9},   {1, 7},    {209, 12}, {209, 12}, {173, 7},  {148, 6},  {137, 9},
    {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},   {58, 10},  {13, 9},
    {121, 7},  {21, 9},   {37, 9},   {2, 7},    {209, 12}, {157, 6},  {109, 7},
    {70, 6},   {127, 7},  {25, 9},   {41, 9},   {4, 7},    {193, 6},  {82, 6},
    {49, 9},   {8, 7},    {118, 6},  {16, 7},   {32, 7},   {0, 6},    {209, 12},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {209, 12}, {145, 3},
    {205, 9},  {156, 8},  {168, 8},  {146, 4},  {180, 8},  {149, 4},  {161, 4},
    {64, 4},   {209, 12}, {159, 8},  {115, 9},  {72, 8},   {133, 9},  {78, 8},
    {96, 8},   {65, 5},   {195, 8},  {84, 8},   {102, 8},  {67, 5},   {120, 8},
    {73, 5},   {91, 5},   {64, 4},   {209, 12}, {209, 12}, {174, 8},  {148, 6},
    {139, 9},  {80, 8},   {98, 8},   {66, 6},   {198, 8},  {86, 8},   {60, 10},
    {14, 9},   {122, 8},  {22, 9},   {38, 9},   {3, 8},    {209, 12}, {157, 6},
    {110, 8},  {70, 6},   {128, 8},  {26, 9},   {42, 9},   {5, 8},    {193, 6},
    {82, 6},   {50, 9},   {9, 8},    {118, 6},  {17, 8},   {33, 8},   {0, 6},
    {209, 12}, {209, 12}, {209, 12}, {209, 12}, {189, 8},  {152, 7},  {164, 7},
    {145, 3},  {201, 8},  {88, 8},   {106, 8},  {69, 7},   {124, 8},  {75, 7},
    {93, 7},   {64, 4},   {209, 12}, {158, 7},  {112, 8},  {71, 7},   {130, 8},
    {28, 9},   {44, 9},   {6, 8},    {194, 7},  {83, 7},   {52, 9},   {10, 8},
    {119, 7},  {18, 8},   {34, 8},   {1, 7},    {209, 12}, {209, 12}, {173, 7},
    {148, 6},  {136, 8},  {79, 7},   {97, 7},   {66, 6},   {197, 7},  {85, 7},
    {56, 9},   {12, 8},   {121, 7},  {20, 8},   {36, 8},   {2, 7},    {209, 12},
    {157, 6},  {109, 7},  {70, 6},   {127, 7},  {24, 8},   {40, 8},   {4, 7},
    {193, 6},  {82, 6},   {48, 8},   {8, 7},    {118, 6},  {16, 7},   {32, 7},
    {0, 6}};
} // namespace utf8_to_utf16
} // namespace tables
} // unnamed namespace
} // namespace simdutf

#endif // SIMDUTF_UTF8_TO_UTF16_TABLES_H
/* end file src\tables\utf8_to_utf16_tables.h */
/* begin file src\tables\utf16_to_utf8_tables.h */
// file generated by scripts/sse_convert_utf16_to_utf8.py
#ifndef SIMDUTF_UTF16_TO_UTF8_TABLES_H
#define SIMDUTF_UTF16_TO_UTF8_TABLES_H

namespace simdutf {
namespace {
namespace tables {
namespace utf16_to_utf8 {

// 1 byte for length, 16 bytes for mask
const uint8_t pack_1_2_utf8_bytes[256][17] = {
    {16, 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14},
    {15, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80},
    {15, 1, 0, 3, 2, 5, 4, 7, 6, 8, 11, 10, 13, 12, 15, 14, 0x80},
    {14, 0, 3, 2, 5, 4, 7, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {15, 1, 0, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80},
    {14, 0, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {14, 1, 0, 2, 5, 4, 7, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {13, 0, 2, 5, 4, 7, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {15, 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 10, 13, 12, 15, 14, 0x80},
    {14, 0, 3, 2, 5, 4, 7, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80},
    {14, 1, 0, 3, 2, 5, 4, 7, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80},
    {13, 0, 3, 2, 5, 4, 7, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {14, 1, 0, 2, 5, 4, 7, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80},
    {13, 0, 2, 5, 4, 7, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 5, 4, 7, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 5, 4, 7, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {15, 1, 0, 3, 2, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80},
    {14, 0, 3, 2, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {14, 1, 0, 3, 2, 4, 7, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {13, 0, 3, 2, 4, 7, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {14, 1, 0, 2, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {13, 0, 2, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 4, 7, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 4, 7, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {14, 1, 0, 3, 2, 4, 7, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80},
    {13, 0, 3, 2, 4, 7, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 4, 7, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 4, 7, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 4, 7, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 4, 7, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 4, 7, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 4, 7, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {15, 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 12, 15, 14, 0x80},
    {14, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80},
    {14, 1, 0, 3, 2, 5, 4, 7, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80},
    {13, 0, 3, 2, 5, 4, 7, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {14, 1, 0, 2, 5, 4, 7, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80},
    {13, 0, 2, 5, 4, 7, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 5, 4, 7, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 5, 4, 7, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {14, 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80},
    {13, 0, 3, 2, 5, 4, 7, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 5, 4, 7, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 5, 4, 7, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 5, 4, 7, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 5, 4, 7, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 5, 4, 7, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 5, 4, 7, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {14, 1, 0, 3, 2, 4, 7, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80},
    {13, 0, 3, 2, 4, 7, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 4, 7, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 4, 7, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 4, 7, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 4, 7, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 4, 7, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 4, 7, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 4, 7, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 4, 7, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 4, 7, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 4, 7, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 4, 7, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 4, 7, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 4, 7, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 4, 7, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {15, 1, 0, 3, 2, 5, 4, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80},
    {14, 0, 3, 2, 5, 4, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {14, 1, 0, 3, 2, 5, 4, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {13, 0, 3, 2, 5, 4, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {14, 1, 0, 2, 5, 4, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {13, 0, 2, 5, 4, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 5, 4, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 5, 4, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {14, 1, 0, 3, 2, 5, 4, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80},
    {13, 0, 3, 2, 5, 4, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 5, 4, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 5, 4, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 5, 4, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 5, 4, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 5, 4, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 5, 4, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {14, 1, 0, 3, 2, 4, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {13, 0, 3, 2, 4, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 4, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 4, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 4, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 4, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 4, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 4, 6, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 4, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 4, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 4, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 4, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 4, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 4, 6, 9, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 4, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 4, 6, 8, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {14, 1, 0, 3, 2, 5, 4, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80},
    {13, 0, 3, 2, 5, 4, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 5, 4, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 5, 4, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 5, 4, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 5, 4, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 5, 4, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 5, 4, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 5, 4, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 5, 4, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 5, 4, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 5, 4, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 5, 4, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 5, 4, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 5, 4, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 5, 4, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 4, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 4, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 4, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 4, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 4, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 4, 6, 9, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 4, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 4, 6, 8, 11, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 4, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 4, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 3, 2, 4, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 3, 2, 4, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 4, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 4, 6, 9, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 1, 0, 2, 4, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 0, 2, 4, 6, 8, 10, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {15, 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 14, 0x80},
    {14, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80},
    {14, 1, 0, 3, 2, 5, 4, 7, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80},
    {13, 0, 3, 2, 5, 4, 7, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {14, 1, 0, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80},
    {13, 0, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 5, 4, 7, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 5, 4, 7, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {14, 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80},
    {13, 0, 3, 2, 5, 4, 7, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 5, 4, 7, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 5, 4, 7, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 5, 4, 7, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 5, 4, 7, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 5, 4, 7, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 5, 4, 7, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {14, 1, 0, 3, 2, 4, 7, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80},
    {13, 0, 3, 2, 4, 7, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 4, 7, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 4, 7, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 4, 7, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 4, 7, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 4, 7, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 4, 7, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 4, 7, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 4, 7, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 4, 7, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 4, 7, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 4, 7, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 4, 7, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 4, 7, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 4, 7, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {14, 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80},
    {13, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 5, 4, 7, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 5, 4, 7, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 5, 4, 7, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 5, 4, 7, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 5, 4, 7, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 5, 4, 7, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 5, 4, 7, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 5, 4, 7, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 5, 4, 7, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 5, 4, 7, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 5, 4, 7, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 5, 4, 7, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 5, 4, 7, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 4, 7, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 4, 7, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 4, 7, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 4, 7, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 4, 7, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 4, 7, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 4, 7, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 4, 7, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 4, 7, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 4, 7, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 3, 2, 4, 7, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 3, 2, 4, 7, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 4, 7, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 4, 7, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 1, 0, 2, 4, 7, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 0, 2, 4, 7, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {14, 1, 0, 3, 2, 5, 4, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80},
    {13, 0, 3, 2, 5, 4, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 5, 4, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 5, 4, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 2, 5, 4, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 2, 5, 4, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 5, 4, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 5, 4, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 5, 4, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 5, 4, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 5, 4, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 5, 4, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 5, 4, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 5, 4, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 5, 4, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 5, 4, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {13, 1, 0, 3, 2, 4, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 4, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 4, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 4, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 4, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 4, 6, 9, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 4, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 4, 6, 8, 11, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 4, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 4, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 3, 2, 4, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 3, 2, 4, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 4, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 4, 6, 9, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 1, 0, 2, 4, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 0, 2, 4, 6, 8, 10, 13, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {13, 1, 0, 3, 2, 5, 4, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80},
    {12, 0, 3, 2, 5, 4, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 5, 4, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 5, 4, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 2, 5, 4, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 2, 5, 4, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 5, 4, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 5, 4, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 5, 4, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 5, 4, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 3, 2, 5, 4, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 3, 2, 5, 4, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 5, 4, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 5, 4, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 1, 0, 2, 5, 4, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 0, 2, 5, 4, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {12, 1, 0, 3, 2, 4, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 3, 2, 4, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 3, 2, 4, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 3, 2, 4, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 0, 2, 4, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 2, 4, 6, 9, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 1, 0, 2, 4, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 0, 2, 4, 6, 8, 11, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {11, 1, 0, 3, 2, 4, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 3, 2, 4, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 1, 0, 3, 2, 4, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 0, 3, 2, 4, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 1, 0, 2, 4, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 0, 2, 4, 6, 9, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 1, 0, 2, 4, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 0, 2, 4, 6, 8, 10, 12, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80}};

// 1 byte for length, 16 bytes for mask
const uint8_t pack_1_2_3_utf8_bytes[256][17] = {
    {12, 2, 3, 1, 6, 7, 5, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80},
    {9, 6, 7, 5, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {11, 3, 1, 6, 7, 5, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 0, 6, 7, 5, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 2, 3, 1, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 3, 1, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {11, 2, 3, 1, 7, 5, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 7, 5, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 3, 1, 7, 5, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 0, 7, 5, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 2, 3, 1, 4, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 4, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 3, 1, 4, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 0, 4, 10, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 2, 3, 1, 6, 7, 5, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 6, 7, 5, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 3, 1, 6, 7, 5, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 6, 7, 5, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 2, 3, 1, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 3, 1, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 2, 3, 1, 7, 5, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 7, 5, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 7, 5, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 7, 5, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 2, 3, 1, 4, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 4, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 3, 1, 4, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 0, 4, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {11, 2, 3, 1, 6, 7, 5, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 6, 7, 5, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 3, 1, 6, 7, 5, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 0, 6, 7, 5, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 2, 3, 1, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {10, 2, 3, 1, 7, 5, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 7, 5, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 3, 1, 7, 5, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 0, 7, 5, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 2, 3, 1, 4, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 4, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 3, 1, 4, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 4, 11, 9, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 2, 3, 1, 6, 7, 5, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 6, 7, 5, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 3, 1, 6, 7, 5, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 0, 6, 7, 5, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 2, 3, 1, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 3, 1, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 0, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 2, 3, 1, 7, 5, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 7, 5, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 3, 1, 7, 5, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 7, 5, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 2, 3, 1, 4, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 4, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 4, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 4, 8, 14, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 2, 3, 1, 6, 7, 5, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 6, 7, 5, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 3, 1, 6, 7, 5, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 6, 7, 5, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 2, 3, 1, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 3, 1, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 2, 3, 1, 7, 5, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 7, 5, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 7, 5, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 7, 5, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 2, 3, 1, 4, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 4, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 3, 1, 4, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 0, 4, 10, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 2, 3, 1, 6, 7, 5, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {3, 6, 7, 5, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 3, 1, 6, 7, 5, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 6, 7, 5, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 2, 3, 1, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {2, 3, 1, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {1, 0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {5, 2, 3, 1, 7, 5, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 7, 5, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 3, 1, 7, 5, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 0, 7, 5, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 2, 3, 1, 4, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {1, 4, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {3, 3, 1, 4, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {2, 0, 4, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 2, 3, 1, 6, 7, 5, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 6, 7, 5, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 6, 7, 5, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 6, 7, 5, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 2, 3, 1, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 3, 1, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 0, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {7, 2, 3, 1, 7, 5, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 7, 5, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 3, 1, 7, 5, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 0, 7, 5, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 2, 3, 1, 4, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {3, 4, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 3, 1, 4, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 4, 11, 9, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 2, 3, 1, 6, 7, 5, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 6, 7, 5, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 3, 1, 6, 7, 5, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 0, 6, 7, 5, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 2, 3, 1, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {1, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {3, 3, 1, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {2, 0, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 2, 3, 1, 7, 5, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {3, 7, 5, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 3, 1, 7, 5, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 7, 5, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 2, 3, 1, 4, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 4, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 3, 1, 4, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 0, 4, 8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {11, 2, 3, 1, 6, 7, 5, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 6, 7, 5, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 3, 1, 6, 7, 5, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 0, 6, 7, 5, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 2, 3, 1, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {10, 2, 3, 1, 7, 5, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 7, 5, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 3, 1, 7, 5, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 0, 7, 5, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 2, 3, 1, 4, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 4, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 3, 1, 4, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 4, 10, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 2, 3, 1, 6, 7, 5, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 6, 7, 5, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 6, 7, 5, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 6, 7, 5, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 2, 3, 1, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {4, 3, 1, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 0, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {7, 2, 3, 1, 7, 5, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 7, 5, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 3, 1, 7, 5, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 0, 7, 5, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 2, 3, 1, 4, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 4, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 3, 1, 4, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 4, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {10, 2, 3, 1, 6, 7, 5, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 6, 7, 5, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 3, 1, 6, 7, 5, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 0, 6, 7, 5, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 2, 3, 1, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 3, 1, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 0, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 2, 3, 1, 7, 5, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 7, 5, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 3, 1, 7, 5, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 7, 5, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 2, 3, 1, 4, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 4, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 4, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 4, 11, 9, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 2, 3, 1, 6, 7, 5, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 6, 7, 5, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 3, 1, 6, 7, 5, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 6, 7, 5, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 2, 3, 1, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 3, 1, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 2, 3, 1, 7, 5, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 7, 5, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 7, 5, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 7, 5, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 2, 3, 1, 4, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 4, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 3, 1, 4, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 0, 4, 8, 15, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {10, 2, 3, 1, 6, 7, 5, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 6, 7, 5, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 3, 1, 6, 7, 5, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 0, 6, 7, 5, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 2, 3, 1, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 3, 1, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 0, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 2, 3, 1, 7, 5, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 7, 5, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 3, 1, 7, 5, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 7, 5, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 2, 3, 1, 4, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 4, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 4, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 4, 10, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 2, 3, 1, 6, 7, 5, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 6, 7, 5, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 3, 1, 6, 7, 5, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 0, 6, 7, 5, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 2, 3, 1, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {1, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {3, 3, 1, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {2, 0, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 2, 3, 1, 7, 5, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {3, 7, 5, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 3, 1, 7, 5, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 7, 5, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 2, 3, 1, 4, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 4, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 3, 1, 4, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 0, 4, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {9, 2, 3, 1, 6, 7, 5, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 6, 7, 5, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 3, 1, 6, 7, 5, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 6, 7, 5, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 2, 3, 1, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 3, 1, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 2, 3, 1, 7, 5, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 7, 5, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 7, 5, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 7, 5, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 2, 3, 1, 4, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 4, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 3, 1, 4, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 0, 4, 11, 9, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 2, 3, 1, 6, 7, 5, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 6, 7, 5, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 3, 1, 6, 7, 5, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 6, 7, 5, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 2, 3, 1, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 3, 1, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 0, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {7, 2, 3, 1, 7, 5, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 7, 5, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 3, 1, 7, 5, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 0, 7, 5, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 2, 3, 1, 4, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {3, 4, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 3, 1, 4, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 4, 8, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80}};

} // namespace utf16_to_utf8
} // namespace tables
} // unnamed namespace
} // namespace simdutf

#endif // SIMDUTF_UTF16_TO_UTF8_TABLES_H
/* end file src\tables\utf16_to_utf8_tables.h */
/* begin file src\tables\utf32_to_utf16_tables.h */
// file generated by scripts/sse_convert_utf32_to_utf16.py
#ifndef SIMDUTF_UTF32_TO_UTF16_TABLES_H
#define SIMDUTF_UTF32_TO_UTF16_TABLES_H

namespace simdutf {
namespace {
namespace tables {
namespace utf32_to_utf16 {

const uint8_t pack_utf32_to_utf16le[16][16] = {
    {0, 1, 4, 5, 8, 9, 12, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0, 1, 2, 3, 4, 5, 8, 9, 12, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0, 1, 4, 5, 6, 7, 8, 9, 12, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 0x80, 0x80, 0x80, 0x80},
    {0, 1, 4, 5, 8, 9, 10, 11, 12, 13, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0, 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 0x80, 0x80, 0x80, 0x80},
    {0, 1, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0x80, 0x80, 0x80, 0x80},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0x80, 0x80},
    {0, 1, 4, 5, 8, 9, 12, 13, 14, 15, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0, 1, 2, 3, 4, 5, 8, 9, 12, 13, 14, 15, 0x80, 0x80, 0x80, 0x80},
    {0, 1, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15, 0x80, 0x80, 0x80, 0x80},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15, 0x80, 0x80},
    {0, 1, 4, 5, 8, 9, 10, 11, 12, 13, 14, 15, 0x80, 0x80, 0x80, 0x80},
    {0, 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 14, 15, 0x80, 0x80},
    {0, 1, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0x80, 0x80},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
};

const uint8_t pack_utf32_to_utf16be[16][16] = {
    {1, 0, 5, 4, 9, 8, 13, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {1, 0, 3, 2, 5, 4, 9, 8, 13, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {1, 0, 5, 4, 7, 6, 9, 8, 13, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 13, 12, 0x80, 0x80, 0x80, 0x80},
    {1, 0, 5, 4, 9, 8, 11, 10, 13, 12, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {1, 0, 3, 2, 5, 4, 9, 8, 11, 10, 13, 12, 0x80, 0x80, 0x80, 0x80},
    {1, 0, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 0x80, 0x80, 0x80, 0x80},
    {1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 0x80, 0x80},
    {1, 0, 5, 4, 9, 8, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {1, 0, 3, 2, 5, 4, 9, 8, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {1, 0, 5, 4, 7, 6, 9, 8, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 13, 12, 15, 14, 0x80, 0x80},
    {1, 0, 5, 4, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80, 0x80, 0x80},
    {1, 0, 3, 2, 5, 4, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {1, 0, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 0x80, 0x80},
    {1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14},
};

} // namespace utf32_to_utf16
} // namespace tables
} // unnamed namespace
} // namespace simdutf

#endif // SIMDUTF_UTF16_TO_UTF8_TABLES_H
/* end file src\tables\utf32_to_utf16_tables.h */
// End of tables.

// Implementations: they need to be setup before including
// scalar/* code, as the scalar code is sometimes enabled
// only for peculiar build targets.

// The best choice should always come first!
#ifndef SIMDUTF_REGULAR_VISUAL_STUDIO
SIMDUTF_DISABLE_UNUSED_WARNING
#endif
/* begin file src\simdutf\arm64.h */
#ifndef SIMDUTF_ARM64_H
#define SIMDUTF_ARM64_H

#ifdef SIMDUTF_FALLBACK_H
  #error "arm64.h must be included before fallback.h"
#endif


#ifndef SIMDUTF_IMPLEMENTATION_ARM64
  #define SIMDUTF_IMPLEMENTATION_ARM64 (SIMDUTF_IS_ARM64)
#endif
#if SIMDUTF_IMPLEMENTATION_ARM64 && SIMDUTF_IS_ARM64
  #define SIMDUTF_CAN_ALWAYS_RUN_ARM64 1
#else
  #define SIMDUTF_CAN_ALWAYS_RUN_ARM64 0
#endif


#if SIMDUTF_IMPLEMENTATION_ARM64

namespace simdutf {
/**
 * Implementation for NEON (ARMv8).
 */
namespace arm64 {} // namespace arm64
} // namespace simdutf

/* begin file src\simdutf\arm64\implementation.h */
#ifndef SIMDUTF_ARM64_IMPLEMENTATION_H
#define SIMDUTF_ARM64_IMPLEMENTATION_H


namespace simdutf {
namespace arm64 {

namespace {
using namespace simdutf;
}

class implementation final : public simdutf::implementation {
public:
  simdutf_really_inline implementation()
      : simdutf::implementation("arm64", "ARM NEON",
                                internal::instruction_set::NEON) {}

  simdutf_warn_unused result base64_to_binary(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused result
  base64_to_binary(const char16_t *input, size_t length, char *output,
                   base64_options options,
                   last_chunk_handling_options last_chunk_options =
                       last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char16_t *input, size_t length, char *output,
      base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  size_t binary_to_base64(const char *input, size_t length, char *output,
                          base64_options options) const noexcept;
  const char *find(const char *start, const char *end,
                   char character) const noexcept;
  const char16_t *find(const char16_t *start, const char16_t *end,
                       char16_t character) const noexcept;
};

} // namespace arm64
} // namespace simdutf

#endif // SIMDUTF_ARM64_IMPLEMENTATION_H
/* end file src\simdutf\arm64\implementation.h */

/* begin file src\simdutf\arm64\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "arm64"
// #define SIMDUTF_IMPLEMENTATION arm64
#define SIMDUTF_SIMD_HAS_BYTEMASK 1
/* end file src\simdutf\arm64\begin.h */

  // Declarations
/* begin file src\simdutf\arm64\intrinsics.h */
#ifndef SIMDUTF_ARM64_INTRINSICS_H
#define SIMDUTF_ARM64_INTRINSICS_H


// This should be the correct header whether
// you use visual studio or other compilers.
#include <arm_neon.h>

#endif //  SIMDUTF_ARM64_INTRINSICS_H
/* end file src\simdutf\arm64\intrinsics.h */
/* begin file src\simdutf\arm64\bitmanipulation.h */
#ifndef SIMDUTF_ARM64_BITMANIPULATION_H
#define SIMDUTF_ARM64_BITMANIPULATION_H

namespace simdutf {
namespace arm64 {
namespace {

/* result might be undefined when input_num is zero */
simdutf_really_inline int count_ones(uint64_t input_num) {
  return vaddv_u8(vcnt_u8(vcreate_u8(input_num)));
}

#if SIMDUTF_NEED_TRAILING_ZEROES
simdutf_really_inline int trailing_zeroes(uint64_t input_num) {
  #ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
  unsigned long ret;
  // Search the mask data from least significant bit (LSB)
  // to the most significant bit (MSB) for a set bit (1).
  _BitScanForward64(&ret, input_num);
  return (int)ret;
  #else  // SIMDUTF_REGULAR_VISUAL_STUDIO
  return __builtin_ctzll(input_num);
  #endif // SIMDUTF_REGULAR_VISUAL_STUDIO
}
#endif
template <typename T> T clear_least_significant_bit(T x) {
  return (x & (x - 1));
}

} // unnamed namespace
} // namespace arm64
} // namespace simdutf

#endif // SIMDUTF_ARM64_BITMANIPULATION_H
/* end file src\simdutf\arm64\bitmanipulation.h */
/* begin file src\simdutf\arm64\simd.h */
#ifndef SIMDUTF_ARM64_SIMD_H
#define SIMDUTF_ARM64_SIMD_H

#include <type_traits>

namespace simdutf {
namespace arm64 {
namespace {
namespace simd {

#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
namespace {
  // Start of private section with Visual Studio workaround

  #ifndef simdutf_make_uint8x16_t
    #define simdutf_make_uint8x16_t(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10,   \
                                    x11, x12, x13, x14, x15, x16)              \
      ([=]() {                                                                 \
        uint8_t array[16] = {x1, x2,  x3,  x4,  x5,  x6,  x7,  x8,             \
                             x9, x10, x11, x12, x13, x14, x15, x16};           \
        return vld1q_u8(array);                                                \
      }())
  #endif
  #ifndef simdutf_make_int8x16_t
    #define simdutf_make_int8x16_t(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10,    \
                                   x11, x12, x13, x14, x15, x16)               \
      ([=]() {                                                                 \
        int8_t array[16] = {x1, x2,  x3,  x4,  x5,  x6,  x7,  x8,              \
                            x9, x10, x11, x12, x13, x14, x15, x16};            \
        return vld1q_s8(array);                                                \
      }())
  #endif

  #ifndef simdutf_make_uint8x8_t
    #define simdutf_make_uint8x8_t(x1, x2, x3, x4, x5, x6, x7, x8)             \
      ([=]() {                                                                 \
        uint8_t array[8] = {x1, x2, x3, x4, x5, x6, x7, x8};                   \
        return vld1_u8(array);                                                 \
      }())
  #endif
  #ifndef simdutf_make_int8x8_t
    #define simdutf_make_int8x8_t(x1, x2, x3, x4, x5, x6, x7, x8)              \
      ([=]() {                                                                 \
        int8_t array[8] = {x1, x2, x3, x4, x5, x6, x7, x8};                    \
        return vld1_s8(array);                                                 \
      }())
  #endif
  #ifndef simdutf_make_uint16x8_t
    #define simdutf_make_uint16x8_t(x1, x2, x3, x4, x5, x6, x7, x8)            \
      ([=]() {                                                                 \
        uint16_t array[8] = {x1, x2, x3, x4, x5, x6, x7, x8};                  \
        return vld1q_u16(array);                                               \
      }())
  #endif
  #ifndef simdutf_make_int16x8_t
    #define simdutf_make_int16x8_t(x1, x2, x3, x4, x5, x6, x7, x8)             \
      ([=]() {                                                                 \
        int16_t array[8] = {x1, x2, x3, x4, x5, x6, x7, x8};                   \
        return vld1q_s16(array);                                               \
      }())
  #endif

// End of private section with Visual Studio workaround
} // namespace
#endif // SIMDUTF_REGULAR_VISUAL_STUDIO

template <typename T> struct simd8;

//
// Base class of simd8<uint8_t> and simd8<bool>, both of which use uint8x16_t
// internally.
//
template <typename T, typename Mask = simd8<bool>> struct base_u8 {
  uint8x16_t value;
  static const int SIZE = sizeof(value);
  void dump() const {
#ifdef SIMDUTF_LOGGING
    uint8_t temp[16];
    vst1q_u8(temp, *this);
    printf("[%04x, %04x, %04x, %04x, %04x, %04x, %04x, %04x,%04x, %04x, %04x, "
           "%04x, %04x, %04x, %04x, %04x]\n",
           temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6],
           temp[7], temp[8], temp[9], temp[10], temp[11], temp[12], temp[13],
           temp[14], temp[15]);
#endif // SIMDUTF_LOGGING
  }
  // Conversion from/to SIMD register
  simdutf_really_inline base_u8(const uint8x16_t _value) : value(_value) {}
  simdutf_really_inline operator const uint8x16_t &() const {
    return this->value;
  }

  // Bit operations
  simdutf_really_inline simd8<T> operator|(const simd8<T> other) const {
    return vorrq_u8(*this, other);
  }
  simdutf_really_inline simd8<T> operator&(const simd8<T> other) const {
    return vandq_u8(*this, other);
  }
  simdutf_really_inline simd8<T> operator^(const simd8<T> other) const {
    return veorq_u8(*this, other);
  }
  simdutf_really_inline simd8<T> &operator|=(const simd8<T> other) {
    auto this_cast = static_cast<simd8<T> *>(this);
    *this_cast = *this_cast | other;
    return *this_cast;
  }

  friend simdutf_really_inline Mask operator==(const simd8<T> lhs,
                                               const simd8<T> rhs) {
    return vceqq_u8(lhs, rhs);
  }

  template <int N = 1>
  simdutf_really_inline simd8<T> prev(const simd8<T> prev_chunk) const {
    return vextq_u8(prev_chunk, *this, 16 - N);
  }
};

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd8<bool> : base_u8<bool> {
  static simdutf_really_inline simd8<bool> splat(bool _value) {
    return vmovq_n_u8(uint8_t(-(!!_value)));
  }

  simdutf_really_inline simd8(const uint8x16_t _value)
      : base_u8<bool>(_value) {}
  // False constructor
  simdutf_really_inline simd8() : simd8(vdupq_n_u8(0)) {}
  // Splat constructor
  simdutf_really_inline simd8(bool _value) : simd8(splat(_value)) {}
  simdutf_really_inline void store(uint8_t dst[16]) const {
    return vst1q_u8(dst, *this);
  }

  // We return uint32_t instead of uint16_t because that seems to be more
  // efficient for most purposes (cutting it down to uint16_t costs performance
  // in some compilers).
  simdutf_really_inline uint32_t to_bitmask() const {
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
    const uint8x16_t bit_mask =
        simdutf_make_uint8x16_t(0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                                0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80);
#else
    const uint8x16_t bit_mask = {0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                                 0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
#endif
    auto minput = *this & bit_mask;
    uint8x16_t tmp = vpaddq_u8(minput, minput);
    tmp = vpaddq_u8(tmp, tmp);
    tmp = vpaddq_u8(tmp, tmp);
    return vgetq_lane_u16(vreinterpretq_u16_u8(tmp), 0);
  }

  // Returns 4-bit out of each byte, alternating between the high 4 bits and low
  // bits result it is 64 bit. This method is expected to be faster than none()
  // and is equivalent when the vector register is the result of a comparison,
  // with byte values 0xff and 0x00.
  simdutf_really_inline uint64_t to_bitmask64() const {
    return vget_lane_u64(
        vreinterpret_u64_u8(vshrn_n_u16(vreinterpretq_u16_u8(*this), 4)), 0);
  }
};

// Unsigned bytes
template <> struct simd8<uint8_t> : base_u8<uint8_t> {
  static simdutf_really_inline simd8<uint8_t> splat(uint8_t _value) {
    return vmovq_n_u8(_value);
  }
  static simdutf_really_inline simd8<uint8_t> zero() { return vdupq_n_u8(0); }
  static simdutf_really_inline simd8<uint8_t> load(const uint8_t *values) {
    return vld1q_u8(values);
  }
  simdutf_really_inline simd8(const uint8x16_t _value)
      : base_u8<uint8_t>(_value) {}
  // Zero constructor
  simdutf_really_inline simd8() : simd8(zero()) {}
  // Array constructor
  simdutf_really_inline simd8(const uint8_t values[16]) : simd8(load(values)) {}
  // Splat constructor
  simdutf_really_inline simd8(uint8_t _value) : simd8(splat(_value)) {}
  // Member-by-member initialization
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
  simdutf_really_inline
  simd8(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4, uint8_t v5,
        uint8_t v6, uint8_t v7, uint8_t v8, uint8_t v9, uint8_t v10,
        uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15)
      : simd8(simdutf_make_uint8x16_t(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9,
                                      v10, v11, v12, v13, v14, v15)) {}
#else
  simdutf_really_inline
  simd8(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4, uint8_t v5,
        uint8_t v6, uint8_t v7, uint8_t v8, uint8_t v9, uint8_t v10,
        uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15)
      : simd8(uint8x16_t{v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12,
                         v13, v14, v15}) {}
#endif

  // Repeat 16 values as many times as necessary (usually for lookup tables)
  simdutf_really_inline static simd8<uint8_t>
  repeat_16(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4,
            uint8_t v5, uint8_t v6, uint8_t v7, uint8_t v8, uint8_t v9,
            uint8_t v10, uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14,
            uint8_t v15) {
    return simd8<uint8_t>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12,
                          v13, v14, v15);
  }

  // Store to array
  simdutf_really_inline void store(uint8_t dst[16]) const {
    return vst1q_u8(dst, *this);
  }

  // Addition/subtraction are the same for signed and unsigned
  simdutf_really_inline simd8<uint8_t>
  operator-(const simd8<uint8_t> other) const {
    return vsubq_u8(*this, other);
  }
  simdutf_really_inline simd8<uint8_t> &operator-=(const simd8<uint8_t> other) {
    *this = *this - other;
    return *this;
  }

  // Order-specific operations
  simdutf_really_inline uint8_t max_val() const { return vmaxvq_u8(*this); }
  simdutf_really_inline simd8<bool>
  operator>=(const simd8<uint8_t> other) const {
    return vcgeq_u8(*this, other);
  }
  simdutf_really_inline simd8<bool>
  operator>(const simd8<uint8_t> other) const {
    return vcgtq_u8(*this, other);
  }
  // Same as >, but instead of guaranteeing all 1's == true, false = 0 and true
  // = nonzero. For ARM, returns all 1's.
  simdutf_really_inline simd8<uint8_t>
  gt_bits(const simd8<uint8_t> other) const {
    return simd8<uint8_t>(*this > other);
  }

  // Bit-specific operations
  simdutf_really_inline simd8<bool> any_bits_set(simd8<uint8_t> bits) const {
    return vtstq_u8(*this, bits);
  }

  simdutf_really_inline bool is_ascii() const {
    return this->max_val() < 0b10000000u;
  }

  simdutf_really_inline bool any_bits_set_anywhere() const {
    return this->max_val() != 0;
  }
  template <int N> simdutf_really_inline simd8<uint8_t> shr() const {
    return vshrq_n_u8(*this, N);
  }
  simdutf_really_inline uint16_t sum_bytes() const { return vaddvq_u8(*this); }

  // Perform a lookup assuming the value is between 0 and 16 (undefined behavior
  // for out of range values)
  template <typename L>
  simdutf_really_inline simd8<L> lookup_16(simd8<L> lookup_table) const {
    return lookup_table.apply_lookup_16_to(*this);
  }

  template <typename L>
  simdutf_really_inline simd8<L>
  lookup_16(L replace0, L replace1, L replace2, L replace3, L replace4,
            L replace5, L replace6, L replace7, L replace8, L replace9,
            L replace10, L replace11, L replace12, L replace13, L replace14,
            L replace15) const {
    return lookup_16(simd8<L>::repeat_16(
        replace0, replace1, replace2, replace3, replace4, replace5, replace6,
        replace7, replace8, replace9, replace10, replace11, replace12,
        replace13, replace14, replace15));
  }

  template <typename T>
  simdutf_really_inline simd8<uint8_t>
  apply_lookup_16_to(const simd8<T> original) const {
    return vqtbl1q_u8(*this, simd8<uint8_t>(original));
  }
};

// Signed bytes
template <> struct simd8<int8_t> {
  int8x16_t value;
  static const int SIZE = sizeof(value);

  static simdutf_really_inline simd8<int8_t> splat(int8_t _value) {
    return vmovq_n_s8(_value);
  }
  static simdutf_really_inline simd8<int8_t> zero() { return vdupq_n_s8(0); }
  static simdutf_really_inline simd8<int8_t> load(const int8_t values[16]) {
    return vld1q_s8(values);
  }

  // Use ST2 instead of UXTL+UXTL2 to interleave zeroes. UXTL is actually a
  // USHLL #0, and shifting in NEON is actually quite slow.
  //
  // While this needs the registers to be in a specific order, bigger cores can
  // interleave these with no overhead, and it still performs decently on little
  // cores.
  //    movi  v1.3d, #0
  //      mov   v0.16b, value[0]
  //    st2   {v0.16b, v1.16b}, [ptr], #32
  //      mov   v0.16b, value[1]
  //    st2   {v0.16b, v1.16b}, [ptr], #32
  //    ...
  template <endianness big_endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *p) const {
    int8x16x2_t pair = match_system(big_endian)
                           ? int8x16x2_t{{this->value, vmovq_n_s8(0)}}
                           : int8x16x2_t{{vmovq_n_s8(0), this->value}};
    vst2q_s8(reinterpret_cast<int8_t *>(p), pair);
  }

  // In places where the table can be reused, which is most uses in simdutf, it
  // is worth it to do 4 table lookups, as there is no direct zero extension
  // from u8 to u32.
  simdutf_really_inline void store_ascii_as_utf32_tbl(char32_t *p) const {
    const simd8<uint8_t> tb1{0, 255, 255, 255, 1, 255, 255, 255,
                             2, 255, 255, 255, 3, 255, 255, 255};
    const simd8<uint8_t> tb2{4, 255, 255, 255, 5, 255, 255, 255,
                             6, 255, 255, 255, 7, 255, 255, 255};
    const simd8<uint8_t> tb3{8,  255, 255, 255, 9,  255, 255, 255,
                             10, 255, 255, 255, 11, 255, 255, 255};
    const simd8<uint8_t> tb4{12, 255, 255, 255, 13, 255, 255, 255,
                             14, 255, 255, 255, 15, 255, 255, 255};

    // encourage store pairing and interleaving
    const auto shuf1 = this->apply_lookup_16_to(tb1);
    const auto shuf2 = this->apply_lookup_16_to(tb2);
    shuf1.store(reinterpret_cast<int8_t *>(p));
    shuf2.store(reinterpret_cast<int8_t *>(p + 4));

    const auto shuf3 = this->apply_lookup_16_to(tb3);
    const auto shuf4 = this->apply_lookup_16_to(tb4);
    shuf3.store(reinterpret_cast<int8_t *>(p + 8));
    shuf4.store(reinterpret_cast<int8_t *>(p + 12));
  }
  // Conversion from/to SIMD register
  simdutf_really_inline simd8(const int8x16_t _value) : value{_value} {}
  simdutf_really_inline operator const int8x16_t &() const {
    return this->value;
  }
#ifndef SIMDUTF_REGULAR_VISUAL_STUDIO
  simdutf_really_inline operator const uint8x16_t() const {
    return vreinterpretq_u8_s8(this->value);
  }
#endif
  simdutf_really_inline operator int8x16_t &() { return this->value; }

  // Zero constructor
  simdutf_really_inline simd8() : simd8(zero()) {}
  // Splat constructor
  simdutf_really_inline simd8(int8_t _value) : simd8(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd8(const int8_t *values) : simd8(load(values)) {}
  // Member-by-member initialization
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
  simdutf_really_inline simd8(int8_t v0, int8_t v1, int8_t v2, int8_t v3,
                              int8_t v4, int8_t v5, int8_t v6, int8_t v7,
                              int8_t v8, int8_t v9, int8_t v10, int8_t v11,
                              int8_t v12, int8_t v13, int8_t v14, int8_t v15)
      : simd8(simdutf_make_int8x16_t(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9,
                                     v10, v11, v12, v13, v14, v15)) {}
#else
  simdutf_really_inline simd8(int8_t v0, int8_t v1, int8_t v2, int8_t v3,
                              int8_t v4, int8_t v5, int8_t v6, int8_t v7,
                              int8_t v8, int8_t v9, int8_t v10, int8_t v11,
                              int8_t v12, int8_t v13, int8_t v14, int8_t v15)
      : simd8(int8x16_t{v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12,
                        v13, v14, v15}) {}
#endif

  // Store to array
  simdutf_really_inline void store(int8_t dst[16]) const {
    return vst1q_s8(dst, value);
  }
  // Explicit conversion to/from unsigned
  //
  // Under Visual Studio/ARM64 uint8x16_t and int8x16_t are apparently the same
  // type. In theory, we could check this occurrence with std::same_as and
  // std::enabled_if but it is C++14 and relatively ugly and hard to read.
#ifndef SIMDUTF_REGULAR_VISUAL_STUDIO
  simdutf_really_inline explicit simd8(const uint8x16_t other)
      : simd8(vreinterpretq_s8_u8(other)) {}
#endif
  simdutf_really_inline operator simd8<uint8_t>() const {
    return vreinterpretq_u8_s8(this->value);
  }

  simdutf_really_inline simd8<int8_t>
  operator|(const simd8<int8_t> other) const {
    return vorrq_s8(value, other.value);
  }

  simdutf_really_inline int8_t max_val() const { return vmaxvq_s8(value); }
  simdutf_really_inline int8_t min_val() const { return vminvq_s8(value); }
  simdutf_really_inline bool is_ascii() const { return this->min_val() >= 0; }

  // Order-sensitive comparisons
  simdutf_really_inline simd8<bool> operator>(const simd8<int8_t> other) const {
    return vcgtq_s8(value, other.value);
  }
  simdutf_really_inline simd8<bool> operator<(const simd8<int8_t> other) const {
    return vcltq_s8(value, other.value);
  }

  template <typename T>
  simdutf_really_inline simd8<int8_t>
  apply_lookup_16_to(const simd8<T> original) const {
    return vqtbl1q_s8(*this, simd8<uint8_t>(original));
  }
};

template <typename T> struct simd8x64 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd8<T>);
  static_assert(NUM_CHUNKS == 4,
                "ARM kernel should use four registers per 64-byte block.");
  simd8<T> chunks[NUM_CHUNKS];

  simd8x64(const simd8x64<T> &o) = delete; // no copy allowed
  simd8x64<T> &
  operator=(const simd8<T> other) = delete; // no assignment allowed
  simd8x64() = delete;                      // no default constructor allowed

  simdutf_really_inline simd8x64(const simd8<T> chunk0, const simd8<T> chunk1,
                                 const simd8<T> chunk2, const simd8<T> chunk3)
      : chunks{chunk0, chunk1, chunk2, chunk3} {}
  simdutf_really_inline simd8x64(const T *ptr)
      : chunks{simd8<T>::load(ptr),
               simd8<T>::load(ptr + sizeof(simd8<T>) / sizeof(T)),
               simd8<T>::load(ptr + 2 * sizeof(simd8<T>) / sizeof(T)),
               simd8<T>::load(ptr + 3 * sizeof(simd8<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + sizeof(simd8<T>) * 0 / sizeof(T));
    this->chunks[1].store(ptr + sizeof(simd8<T>) * 1 / sizeof(T));
    this->chunks[2].store(ptr + sizeof(simd8<T>) * 2 / sizeof(T));
    this->chunks[3].store(ptr + sizeof(simd8<T>) * 3 / sizeof(T));
  }

  simdutf_really_inline simd8x64<T> &operator|=(const simd8x64<T> &other) {
    this->chunks[0] |= other.chunks[0];
    this->chunks[1] |= other.chunks[1];
    this->chunks[2] |= other.chunks[2];
    this->chunks[3] |= other.chunks[3];
    return *this;
  }

  simdutf_really_inline simd8<T> reduce_or() const {
    return (this->chunks[0] | this->chunks[1]) |
           (this->chunks[2] | this->chunks[3]);
  }

  simdutf_really_inline bool is_ascii() const { return reduce_or().is_ascii(); }

  template <endianness endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    this->chunks[0].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 0);
    this->chunks[1].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 1);
    this->chunks[2].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 2);
    this->chunks[3].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 3);
  }

  simdutf_really_inline void store_ascii_as_utf32(char32_t *ptr) const {
    this->chunks[0].store_ascii_as_utf32_tbl(ptr + sizeof(simd8<T>) * 0);
    this->chunks[1].store_ascii_as_utf32_tbl(ptr + sizeof(simd8<T>) * 1);
    this->chunks[2].store_ascii_as_utf32_tbl(ptr + sizeof(simd8<T>) * 2);
    this->chunks[3].store_ascii_as_utf32_tbl(ptr + sizeof(simd8<T>) * 3);
  }

  simdutf_really_inline uint64_t to_bitmask() const {
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
    const uint8x16_t bit_mask =
        simdutf_make_uint8x16_t(0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                                0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80);
#else
    const uint8x16_t bit_mask = {0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                                 0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
#endif
    // Add each of the elements next to each other, successively, to stuff each
    // 8 byte mask into one.
    uint8x16_t sum0 =
        vpaddq_u8(vandq_u8(uint8x16_t(this->chunks[0]), bit_mask),
                  vandq_u8(uint8x16_t(this->chunks[1]), bit_mask));
    uint8x16_t sum1 =
        vpaddq_u8(vandq_u8(uint8x16_t(this->chunks[2]), bit_mask),
                  vandq_u8(uint8x16_t(this->chunks[3]), bit_mask));
    sum0 = vpaddq_u8(sum0, sum1);
    sum0 = vpaddq_u8(sum0, sum0);
    return vgetq_lane_u64(vreinterpretq_u64_u8(sum0), 0);
  }

  simdutf_really_inline uint64_t lt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] < mask, this->chunks[1] < mask,
                          this->chunks[2] < mask, this->chunks[3] < mask)
        .to_bitmask();
  }
  simdutf_really_inline uint64_t gt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] > mask, this->chunks[1] > mask,
                          this->chunks[2] > mask, this->chunks[3] > mask)
        .to_bitmask();
  }
  simdutf_really_inline uint64_t gteq_unsigned(const uint8_t m) const {
    const simd8<uint8_t> mask = simd8<uint8_t>::splat(m);
    return simd8x64<bool>(simd8<uint8_t>(uint8x16_t(this->chunks[0])) >= mask,
                          simd8<uint8_t>(uint8x16_t(this->chunks[1])) >= mask,
                          simd8<uint8_t>(uint8x16_t(this->chunks[2])) >= mask,
                          simd8<uint8_t>(uint8x16_t(this->chunks[3])) >= mask)
        .to_bitmask();
  }
}; // struct simd8x64<T>
/* begin file src\simdutf\arm64\simd16-inl.h */
template <typename T> struct simd16;

template <typename T, typename Mask = simd16<bool>> struct base_u16 {
  uint16x8_t value;
  /// the size of vector in bytes
  static const int SIZE = sizeof(value);
  /// the number of elements of type T a vector can hold
  static const int ELEMENTS = SIZE / sizeof(T);
  // Conversion from/to SIMD register
  simdutf_really_inline base_u16() = default;
  simdutf_really_inline base_u16(const uint16x8_t _value) : value(_value) {}
  simdutf_really_inline operator const uint16x8_t &() const {
    return this->value;
  }
  simdutf_really_inline operator uint16x8_t &() { return this->value; }
  // Bit operations
  simdutf_really_inline simd16<T> operator|(const simd16<T> other) const {
    return vorrq_u16(*this, other);
  }
  simdutf_really_inline simd16<T> operator&(const simd16<T> other) const {
    return vandq_u16(*this, other);
  }
  simdutf_really_inline simd16<T> operator^(const simd16<T> other) const {
    return veorq_u16(*this, other);
  }
  simdutf_really_inline simd16<T> bit_andnot(const simd16<T> other) const {
    return vbicq_u16(*this, other);
  }
  simdutf_really_inline simd16<T> operator~() const { return *this ^ 0xFFu; }
  simdutf_really_inline simd16<T> &operator|=(const simd16<T> other) {
    auto this_cast = static_cast<simd16<T> *>(this);
    *this_cast = *this_cast | other;
    return *this_cast;
  }
  simdutf_really_inline simd16<T> &operator&=(const simd16<T> other) {
    auto this_cast = static_cast<simd16<T> *>(this);
    *this_cast = *this_cast & other;
    return *this_cast;
  }
  simdutf_really_inline simd16<T> &operator^=(const simd16<T> other) {
    auto this_cast = static_cast<simd16<T> *>(this);
    *this_cast = *this_cast ^ other;
    return *this_cast;
  }

  friend simdutf_really_inline Mask operator==(const simd16<T> lhs,
                                               const simd16<T> rhs) {
    return vceqq_u16(lhs, rhs);
  }

  template <int N = 1>
  simdutf_really_inline simd16<T> prev(const simd16<T> prev_chunk) const {
    return vextq_u18(prev_chunk, *this, 8 - N);
  }
};

template <typename T, typename Mask = simd16<bool>>
struct base16 : base_u16<T> {
  typedef uint16_t bitmask_t;
  typedef uint32_t bitmask2_t;

  simdutf_really_inline base16() : base_u16<T>() {}
  simdutf_really_inline base16(const uint16x8_t _value) : base_u16<T>(_value) {}
  template <typename Pointer>
  simdutf_really_inline base16(const Pointer *ptr) : base16(vld1q_u16(ptr)) {}

  static const int SIZE = sizeof(base_u16<T>::value);
  void dump() const {
#ifdef SIMDUTF_LOGGING
    uint16_t temp[8];
    vst1q_u16(temp, *this);
    printf("[%04x, %04x, %04x, %04x, %04x, %04x, %04x, %04x]\n", temp[0],
           temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7]);
#endif // SIMDUTF_LOGGING
  }
  template <int N = 1>
  simdutf_really_inline simd16<T> prev(const simd16<T> prev_chunk) const {
    return vextq_u18(prev_chunk, *this, 8 - N);
  }
};

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd16<bool> : base16<bool> {
  static simdutf_really_inline simd16<bool> splat(bool _value) {
    return vmovq_n_u16(uint16_t(-(!!_value)));
  }

  simdutf_really_inline simd16() : base16() {}
  simdutf_really_inline simd16(const uint16x8_t _value)
      : base16<bool>(_value) {}
  // Splat constructor
  simdutf_really_inline simd16(bool _value) : base16<bool>(splat(_value)) {}
};

template <typename T> struct base16_numeric : base16<T> {
  static simdutf_really_inline simd16<T> splat(T _value) {
    return vmovq_n_u16(_value);
  }
  static simdutf_really_inline simd16<T> zero() { return vdupq_n_u16(0); }
  static simdutf_really_inline simd16<T> load(const T values[8]) {
    return vld1q_u16(reinterpret_cast<const uint16_t *>(values));
  }

  simdutf_really_inline base16_numeric() : base16<T>() {}
  simdutf_really_inline base16_numeric(const uint16x8_t _value)
      : base16<T>(_value) {}

  // Store to array
  simdutf_really_inline void store(T dst[8]) const {
    return vst1q_u16(dst, *this);
  }

  // Override to distinguish from bool version
  simdutf_really_inline simd16<T> operator~() const { return *this ^ 0xFFu; }

  // Addition/subtraction are the same for signed and unsigned
  simdutf_really_inline simd16<T> operator+(const simd16<T> other) const {
    return vaddq_u16(*this, other);
  }
  simdutf_really_inline simd16<T> operator-(const simd16<T> other) const {
    return vsubq_u16(*this, other);
  }
  simdutf_really_inline simd16<T> &operator+=(const simd16<T> other) {
    *this = *this + other;
    return *static_cast<simd16<T> *>(this);
  }
  simdutf_really_inline simd16<T> &operator-=(const simd16<T> other) {
    *this = *this - other;
    return *static_cast<simd16<T> *>(this);
  }
};

// Signed code units
template <> struct simd16<int16_t> : base16_numeric<int16_t> {
  simdutf_really_inline simd16() : base16_numeric<int16_t>() {}
#ifndef SIMDUTF_REGULAR_VISUAL_STUDIO
  simdutf_really_inline simd16(const uint16x8_t _value)
      : base16_numeric<int16_t>(_value) {}
#endif
  simdutf_really_inline simd16(const int16x8_t _value)
      : base16_numeric<int16_t>(vreinterpretq_u16_s16(_value)) {}

  // Splat constructor
  simdutf_really_inline simd16(int16_t _value) : simd16(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd16(const int16_t *values) : simd16(load(values)) {}
  simdutf_really_inline simd16(const char16_t *values)
      : simd16(load(reinterpret_cast<const int16_t *>(values))) {}
  simdutf_really_inline operator simd16<uint16_t>() const;
  simdutf_really_inline operator const uint16x8_t &() const {
    return this->value;
  }
  simdutf_really_inline operator const int16x8_t() const {
    return vreinterpretq_s16_u16(this->value);
  }

  simdutf_really_inline int16_t max_val() const {
    return vmaxvq_s16(vreinterpretq_s16_u16(this->value));
  }
  simdutf_really_inline int16_t min_val() const {
    return vminvq_s16(vreinterpretq_s16_u16(this->value));
  }
  // Order-sensitive comparisons
  simdutf_really_inline simd16<int16_t>
  max_val(const simd16<int16_t> other) const {
    return vmaxq_s16(vreinterpretq_s16_u16(this->value),
                     vreinterpretq_s16_u16(other.value));
  }
  simdutf_really_inline simd16<int16_t>
  min_val(const simd16<int16_t> other) const {
    return vmaxq_s16(vreinterpretq_s16_u16(this->value),
                     vreinterpretq_s16_u16(other.value));
  }
  simdutf_really_inline simd16<bool>
  operator>(const simd16<int16_t> other) const {
    return vcgtq_s16(vreinterpretq_s16_u16(this->value),
                     vreinterpretq_s16_u16(other.value));
  }
  simdutf_really_inline simd16<bool>
  operator<(const simd16<int16_t> other) const {
    return vcltq_s16(vreinterpretq_s16_u16(this->value),
                     vreinterpretq_s16_u16(other.value));
  }
};

// Unsigned code units
template <> struct simd16<uint16_t> : base16_numeric<uint16_t> {
  simdutf_really_inline simd16() : base16_numeric<uint16_t>() {}
  simdutf_really_inline simd16(const uint16x8_t _value)
      : base16_numeric<uint16_t>(_value) {}

  // Splat constructor
  simdutf_really_inline simd16(uint16_t _value) : simd16(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd16(const uint16_t *values) : simd16(load(values)) {}
  simdutf_really_inline simd16(const char16_t *values)
      : simd16(load(reinterpret_cast<const uint16_t *>(values))) {}

  simdutf_really_inline int16_t max_val() const { return vmaxvq_u16(*this); }
  simdutf_really_inline int16_t min_val() const { return vminvq_u16(*this); }
  // Saturated math
  simdutf_really_inline simd16<uint16_t>
  saturating_add(const simd16<uint16_t> other) const {
    return vqaddq_u16(*this, other);
  }
  simdutf_really_inline simd16<uint16_t>
  saturating_sub(const simd16<uint16_t> other) const {
    return vqsubq_u16(*this, other);
  }

  // Order-specific operations
  simdutf_really_inline simd16<uint16_t>
  max_val(const simd16<uint16_t> other) const {
    return vmaxq_u16(*this, other);
  }
  simdutf_really_inline simd16<uint16_t>
  min_val(const simd16<uint16_t> other) const {
    return vminq_u16(*this, other);
  }
  // Same as >, but only guarantees true is nonzero (< guarantees true = -1)
  simdutf_really_inline simd16<uint16_t>
  gt_bits(const simd16<uint16_t> other) const {
    return this->saturating_sub(other);
  }
  // Same as <, but only guarantees true is nonzero (< guarantees true = -1)
  simdutf_really_inline simd16<uint16_t>
  lt_bits(const simd16<uint16_t> other) const {
    return other.saturating_sub(*this);
  }
  simdutf_really_inline simd16<bool>
  operator<=(const simd16<uint16_t> other) const {
    return vcleq_u16(*this, other);
  }
  simdutf_really_inline simd16<bool>
  operator>=(const simd16<uint16_t> other) const {
    return vcgeq_u16(*this, other);
  }
  simdutf_really_inline simd16<bool>
  operator>(const simd16<uint16_t> other) const {
    return vcgtq_u16(*this, other);
  }
  simdutf_really_inline simd16<bool>
  operator<(const simd16<uint16_t> other) const {
    return vcltq_u16(*this, other);
  }

  // Bit-specific operations
  simdutf_really_inline simd16<bool> bits_not_set() const {
    return *this == uint16_t(0);
  }
  template <int N> simdutf_really_inline simd16<uint16_t> shr() const {
    return simd16<uint16_t>(vshrq_n_u16(*this, N));
  }
  template <int N> simdutf_really_inline simd16<uint16_t> shl() const {
    return simd16<uint16_t>(vshlq_n_u16(*this, N));
  }

  // Pack with the unsigned saturation of two uint16_t code units into single
  // uint8_t vector
  static simdutf_really_inline simd8<uint8_t> pack(const simd16<uint16_t> &v0,
                                                   const simd16<uint16_t> &v1) {
    return vqmovn_high_u16(vqmovn_u16(v0), v1);
  }

  // Change the endianness
  simdutf_really_inline simd16<uint16_t> swap_bytes() const {
    return vreinterpretq_u16_u8(vrev16q_u8(vreinterpretq_u8_u16(*this)));
  }

  void dump() const {
    uint16_t temp[8];
    vst1q_u16(temp, *this);
    printf("[%04x, %04x, %04x, %04x, %04x, %04x, %04x, %04x]\n", temp[0],
           temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7]);
  }

  simdutf_really_inline uint32_t sum() const { return vaddlvq_u16(value); }
};

simdutf_really_inline simd16<int16_t>::operator simd16<uint16_t>() const {
  return this->value;
}

template <typename T> struct simd16x32 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd16<T>);
  static_assert(NUM_CHUNKS == 4,
                "ARM kernel should use four registers per 64-byte block.");
  simd16<T> chunks[NUM_CHUNKS];

  simd16x32(const simd16x32<T> &o) = delete; // no copy allowed
  simd16x32<T> &
  operator=(const simd16<T> other) = delete; // no assignment allowed
  simd16x32() = delete;                      // no default constructor allowed

  simdutf_really_inline
  simd16x32(const simd16<T> chunk0, const simd16<T> chunk1,
            const simd16<T> chunk2, const simd16<T> chunk3)
      : chunks{chunk0, chunk1, chunk2, chunk3} {}
  simdutf_really_inline simd16x32(const T *ptr)
      : chunks{simd16<T>::load(ptr),
               simd16<T>::load(ptr + sizeof(simd16<T>) / sizeof(T)),
               simd16<T>::load(ptr + 2 * sizeof(simd16<T>) / sizeof(T)),
               simd16<T>::load(ptr + 3 * sizeof(simd16<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + sizeof(simd16<T>) * 0 / sizeof(T));
    this->chunks[1].store(ptr + sizeof(simd16<T>) * 1 / sizeof(T));
    this->chunks[2].store(ptr + sizeof(simd16<T>) * 2 / sizeof(T));
    this->chunks[3].store(ptr + sizeof(simd16<T>) * 3 / sizeof(T));
  }

  simdutf_really_inline simd16<T> reduce_or() const {
    return (this->chunks[0] | this->chunks[1]) |
           (this->chunks[2] | this->chunks[3]);
  }

  simdutf_really_inline bool is_ascii() const { return reduce_or().is_ascii(); }

  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    this->chunks[0].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 0);
    this->chunks[1].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 1);
    this->chunks[2].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 2);
    this->chunks[3].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 3);
  }

  simdutf_really_inline uint64_t to_bitmask() const {
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
    const uint8x16_t bit_mask =
        simdutf_make_uint8x16_t(0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                                0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80);
#else
    const uint8x16_t bit_mask = {0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                                 0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
#endif
    // Add each of the elements next to each other, successively, to stuff each
    // 8 byte mask into one.
    uint8x16_t sum0 = vpaddq_u8(
        vreinterpretq_u8_u16(this->chunks[0] & vreinterpretq_u16_u8(bit_mask)),
        vreinterpretq_u8_u16(this->chunks[1] & vreinterpretq_u16_u8(bit_mask)));
    uint8x16_t sum1 = vpaddq_u8(
        vreinterpretq_u8_u16(this->chunks[2] & vreinterpretq_u16_u8(bit_mask)),
        vreinterpretq_u8_u16(this->chunks[3] & vreinterpretq_u16_u8(bit_mask)));
    sum0 = vpaddq_u8(sum0, sum1);
    sum0 = vpaddq_u8(sum0, sum0);
    return vgetq_lane_u64(vreinterpretq_u64_u8(sum0), 0);
  }

  simdutf_really_inline void swap_bytes() {
    this->chunks[0] = this->chunks[0].swap_bytes();
    this->chunks[1] = this->chunks[1].swap_bytes();
    this->chunks[2] = this->chunks[2].swap_bytes();
    this->chunks[3] = this->chunks[3].swap_bytes();
  }

  simdutf_really_inline uint64_t lteq(const T m) const {
    const simd16<T> mask = simd16<T>::splat(m);
    return simd16x32<bool>(this->chunks[0] <= mask, this->chunks[1] <= mask,
                           this->chunks[2] <= mask, this->chunks[3] <= mask)
        .to_bitmask();
  }

  simdutf_really_inline uint64_t not_in_range(const T low, const T high) const {
    const simd16<T> mask_low = simd16<T>::splat(low);
    const simd16<T> mask_high = simd16<T>::splat(high);
    return simd16x32<bool>(
               (this->chunks[0] > mask_high) | (this->chunks[0] < mask_low),
               (this->chunks[1] > mask_high) | (this->chunks[1] < mask_low),
               (this->chunks[2] > mask_high) | (this->chunks[2] < mask_low),
               (this->chunks[3] > mask_high) | (this->chunks[3] < mask_low))
        .to_bitmask();
  }
}; // struct simd16x32<T>
template <>
simdutf_really_inline uint64_t simd16x32<uint16_t>::not_in_range(
    const uint16_t low, const uint16_t high) const {
  const simd16<uint16_t> mask_low = simd16<uint16_t>::splat(low);
  const simd16<uint16_t> mask_high = simd16<uint16_t>::splat(high);
  simd16x32<uint16_t> x(simd16<uint16_t>((this->chunks[0] > mask_high) |
                                         (this->chunks[0] < mask_low)),
                        simd16<uint16_t>((this->chunks[1] > mask_high) |
                                         (this->chunks[1] < mask_low)),
                        simd16<uint16_t>((this->chunks[2] > mask_high) |
                                         (this->chunks[2] < mask_low)),
                        simd16<uint16_t>((this->chunks[3] > mask_high) |
                                         (this->chunks[3] < mask_low)));
  return x.to_bitmask();
}

simdutf_really_inline simd16<uint16_t> min(const simd16<uint16_t> a,
                                           simd16<uint16_t> b) {
  return vminq_u16(a.value, b.value);
}
/* end file src\simdutf\arm64\simd16-inl.h */
/* begin file src\simdutf\arm64\simd32-inl.h */
template <typename T> struct simd32;

template <> struct simd32<uint32_t> {
  static const size_t SIZE = sizeof(uint32x4_t);
  static const size_t ELEMENTS = SIZE / sizeof(uint32_t);

  uint32x4_t value;

  simdutf_really_inline simd32(const uint32x4_t v) : value(v) {}

  template <typename Pointer>
  simdutf_really_inline simd32(const Pointer *ptr)
      : value(vld1q_u32(reinterpret_cast<const uint32_t *>(ptr))) {}

  simdutf_really_inline uint64_t sum() const { return vaddvq_u32(value); }

  simdutf_really_inline simd32<uint32_t> swap_bytes() const {
    return vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(value)));
  }

  template <int N> simdutf_really_inline simd32<uint32_t> shr() const {
    return vshrq_n_u32(value, N);
  }

  template <int N> simdutf_really_inline simd32<uint32_t> shl() const {
    return vshlq_n_u32(value, N);
  }

  void dump() const {
#ifdef SIMDUTF_LOGGING
    uint32_t temp[4];
    vst1q_u32(temp, value);
    printf("[%08x, %08x, %08x, %08x]\n", temp[0], temp[1], temp[2], temp[3]);
#endif // SIMDUTF_LOGGING
  }

  // operators
  simdutf_really_inline simd32 &operator+=(const simd32 other) {
    value = vaddq_u32(value, other.value);
    return *this;
  }

  // static members
  simdutf_really_inline static simd32<uint32_t> zero() {
    return vdupq_n_u32(0);
  }

  simdutf_really_inline static simd32<uint32_t> splat(uint32_t v) {
    return vdupq_n_u32(v);
  }
};

//----------------------------------------------------------------------

template <> struct simd32<bool> {
  uint32x4_t value;

  simdutf_really_inline simd32(const uint32x4_t v) : value(v) {}

  simdutf_really_inline bool any() const { return vmaxvq_u32(value) != 0; }
};

//----------------------------------------------------------------------

template <typename T>
simdutf_really_inline simd32<T> operator|(const simd32<T> a,
                                          const simd32<T> b) {
  return vorrq_u32(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> min(const simd32<uint32_t> a,
                                           const simd32<uint32_t> b) {
  return vminq_u32(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> max(const simd32<uint32_t> a,
                                           const simd32<uint32_t> b) {
  return vmaxq_u32(a.value, b.value);
}

simdutf_really_inline simd32<bool> operator==(const simd32<uint32_t> a,
                                              uint32_t b) {
  return vceqq_u32(a.value, vdupq_n_u32(b));
}

simdutf_really_inline simd32<uint32_t> operator&(const simd32<uint32_t> a,
                                                 const simd32<uint32_t> b) {
  return vandq_u32(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> operator&(const simd32<uint32_t> a,
                                                 uint32_t b) {
  return vandq_u32(a.value, vdupq_n_u32(b));
}

simdutf_really_inline simd32<uint32_t> operator|(const simd32<uint32_t> a,
                                                 uint32_t b) {
  return vorrq_u32(a.value, vdupq_n_u32(b));
}

simdutf_really_inline simd32<uint32_t> operator+(const simd32<uint32_t> a,
                                                 const simd32<uint32_t> b) {
  return vaddq_u32(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> operator-(const simd32<uint32_t> a,
                                                 uint32_t b) {
  return vsubq_u32(a.value, vdupq_n_u32(b));
}

simdutf_really_inline simd32<bool> operator>=(const simd32<uint32_t> a,
                                              const simd32<uint32_t> b) {
  return vcgeq_u32(a.value, b.value);
}

simdutf_really_inline simd32<bool> operator!(const simd32<bool> v) {
  return vmvnq_u32(v.value);
}

simdutf_really_inline simd32<bool> operator>(const simd32<uint32_t> a,
                                             const simd32<uint32_t> b) {
  return vcgtq_u32(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> select(const simd32<bool> cond,
                                              const simd32<uint32_t> v_true,
                                              const simd32<uint32_t> v_false) {
  return vbslq_u32(cond.value, v_true.value, v_false.value);
}
/* end file src\simdutf\arm64\simd32-inl.h */
/* begin file src\simdutf\arm64\simd64-inl.h */
template <typename T> struct simd64;

template <> struct simd64<uint64_t> {
  uint64x2_t value;

  simdutf_really_inline simd64(const uint64x2_t v) : value(v) {}

  template <typename Pointer>
  simdutf_really_inline simd64(const Pointer *ptr)
      : value(vld1q_u64(reinterpret_cast<const uint64_t *>(ptr))) {}

  simdutf_really_inline uint64_t sum() const { return vaddvq_u64(value); }

  // operators
  simdutf_really_inline simd64 &operator+=(const simd64 other) {
    value = vaddq_u64(value, other.value);
    return *this;
  }

  // static members
  simdutf_really_inline static simd64<uint64_t> zero() {
    return vdupq_n_u64(0);
  }

  simdutf_really_inline static simd64<uint64_t> splat(uint64_t v) {
    return vdupq_n_u64(v);
  }
};
/* end file src\simdutf\arm64\simd64-inl.h */

simdutf_really_inline simd64<uint64_t> sum_8bytes(const simd8<uint8_t> v) {
  // We do it as 3 instructions. There might be a faster way.
  // We hope that these 3 instructions are cheap.
  uint16x8_t first_sum = vpaddlq_u8(v);
  uint32x4_t second_sum = vpaddlq_u16(first_sum);
  return vpaddlq_u32(second_sum);
}

} // namespace simd
} // unnamed namespace
} // namespace arm64
} // namespace simdutf

#endif // SIMDUTF_ARM64_SIMD_H
/* end file src\simdutf\arm64\simd.h */

/* begin file src\simdutf\arm64\end.h */
#undef SIMDUTF_SIMD_HAS_BYTEMASK
/* end file src\simdutf\arm64\end.h */

#endif // SIMDUTF_IMPLEMENTATION_ARM64

#endif // SIMDUTF_ARM64_H
/* end file src\simdutf\arm64.h */
/* begin file src\simdutf\icelake.h */
#ifndef SIMDUTF_ICELAKE_H
#define SIMDUTF_ICELAKE_H


#ifdef __has_include
  // How do we detect that a compiler supports vbmi2?
  // For sure if the following header is found, we are ok?
  #if __has_include(<avx512vbmi2intrin.h>)
    #define SIMDUTF_COMPILER_SUPPORTS_VBMI2 1
  #endif
#endif

#ifdef _MSC_VER
  #if _MSC_VER >= 1930
    // Visual Studio 2022 and up support VBMI2 under x64 even if the header
    // avx512vbmi2intrin.h is not found.
    // Visual Studio 2019 technically supports VBMI2, but the implementation
    // might be unreliable. Search for visualstudio2019icelakeissue in our
    // tests.
    #define SIMDUTF_COMPILER_SUPPORTS_VBMI2 1
  #endif
#endif

// We allow icelake on x64 as long as the compiler is known to support VBMI2.
#ifndef SIMDUTF_IMPLEMENTATION_ICELAKE
  #define SIMDUTF_IMPLEMENTATION_ICELAKE                                       \
    ((SIMDUTF_IS_X86_64) && (SIMDUTF_COMPILER_SUPPORTS_VBMI2))
#endif

// To see why  (__BMI__) && (__LZCNT__) are not part of this next line, see
// https://github.com/simdutf/simdutf/issues/1247
#if ((SIMDUTF_IMPLEMENTATION_ICELAKE) && (SIMDUTF_IS_X86_64) && (__AVX2__) &&  \
     (SIMDUTF_HAS_AVX512F && SIMDUTF_HAS_AVX512DQ && SIMDUTF_HAS_AVX512VL &&   \
      SIMDUTF_HAS_AVX512VBMI2) &&                                              \
     (!SIMDUTF_IS_32BITS))
  #define SIMDUTF_CAN_ALWAYS_RUN_ICELAKE 1
#else
  #define SIMDUTF_CAN_ALWAYS_RUN_ICELAKE 0
#endif

#if SIMDUTF_IMPLEMENTATION_ICELAKE
  #if SIMDUTF_CAN_ALWAYS_RUN_ICELAKE
    #define SIMDUTF_TARGET_ICELAKE
  #else
    #define SIMDUTF_TARGET_ICELAKE                                             \
      SIMDUTF_TARGET_REGION(                                                   \
          "avx512f,avx512dq,avx512cd,avx512bw,avx512vbmi,avx512vbmi2,"         \
          "avx512vl,avx2,bmi,bmi2,pclmul,lzcnt,popcnt,avx512vpopcntdq")
  #endif

namespace simdutf {
namespace icelake {} // namespace icelake
} // namespace simdutf

  //
  // These two need to be included outside SIMDUTF_TARGET_REGION
  //
/* begin file src\simdutf\icelake\intrinsics.h */
#ifndef SIMDUTF_ICELAKE_INTRINSICS_H
#define SIMDUTF_ICELAKE_INTRINSICS_H


#ifdef SIMDUTF_VISUAL_STUDIO
  // under clang within visual studio, this will include <x86intrin.h>
  #include <intrin.h> // visual studio or clang
  #include <immintrin.h>
#else

  #if SIMDUTF_GCC11ORMORE
// We should not get warnings while including <x86intrin.h> yet we do
// under some versions of GCC.
// If the x86intrin.h header has uninitialized values that are problematic,
// it is a GCC issue, we want to ignore these warnings.
SIMDUTF_DISABLE_GCC_WARNING(-Wuninitialized)
  #endif

  #include <x86intrin.h> // elsewhere

  #if SIMDUTF_GCC11ORMORE
// cancels the suppression of the -Wuninitialized
SIMDUTF_POP_DISABLE_WARNINGS
  #endif

  #ifndef _tzcnt_u64
    #define _tzcnt_u64(x) __tzcnt_u64(x)
  #endif // _tzcnt_u64
#endif   // SIMDUTF_VISUAL_STUDIO

#ifdef SIMDUTF_CLANG_VISUAL_STUDIO
  /**
   * You are not supposed, normally, to include these
   * headers directly. Instead you should either include intrin.h
   * or x86intrin.h. However, when compiling with clang
   * under Windows (i.e., when _MSC_VER is set), these headers
   * only get included *if* the corresponding features are detected
   * from macros:
   * e.g., if __AVX2__ is set... in turn,  we normally set these
   * macros by compiling against the corresponding architecture
   * (e.g., arch:AVX2, -mavx2, etc.) which compiles the whole
   * software with these advanced instructions. In simdutf, we
   * want to compile the whole program for a generic target,
   * and only target our specific kernels. As a workaround,
   * we directly include the needed headers. These headers would
   * normally guard against such usage, but we carefully included
   * <x86intrin.h>  (or <intrin.h>) before, so the headers
   * are fooled.
   */
  #include <bmiintrin.h>   // for _blsr_u64
  #include <bmi2intrin.h>  // for _pext_u64, _pdep_u64
  #include <lzcntintrin.h> // for  __lzcnt64
  #include <immintrin.h>   // for most things (AVX2, AVX512, _popcnt64)
  #include <smmintrin.h>
  #include <tmmintrin.h>
  #include <avxintrin.h>
  #include <avx2intrin.h>
  // Important: we need the AVX-512 headers:
  #include <avx512fintrin.h>
  #include <avx512dqintrin.h>
  #include <avx512cdintrin.h>
  #include <avx512bwintrin.h>
  #include <avx512vlintrin.h>
  #include <avx512vlbwintrin.h>
  #include <avx512vbmiintrin.h>
  #include <avx512vbmi2intrin.h>
  #include <avx512vpopcntdqintrin.h>
  #include <avx512vpopcntdqvlintrin.h>
  // unfortunately, we may not get _blsr_u64, but, thankfully, clang
  // has it as a macro.
  #ifndef _blsr_u64
    // we roll our own
    #define _blsr_u64(n) ((n - 1) & n)
  #endif //  _blsr_u64
#endif   // SIMDUTF_CLANG_VISUAL_STUDIO

#if defined(__GNUC__) && !defined(__clang__)

  #if __GNUC__ == 8
    #define SIMDUTF_GCC8 1
  #elif __GNUC__ == 9
    #define SIMDUTF_GCC9 1
  #endif //  __GNUC__ == 8 || __GNUC__ == 9

#endif // defined(__GNUC__) && !defined(__clang__)

#if SIMDUTF_GCC8
  #pragma GCC push_options
  #pragma GCC target("avx512f")
/**
 * GCC 8 fails to provide _mm512_set_epi8. We roll our own.
 */
inline __m512i
_mm512_set_epi8(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4,
                uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8, uint8_t a9,
                uint8_t a10, uint8_t a11, uint8_t a12, uint8_t a13, uint8_t a14,
                uint8_t a15, uint8_t a16, uint8_t a17, uint8_t a18, uint8_t a19,
                uint8_t a20, uint8_t a21, uint8_t a22, uint8_t a23, uint8_t a24,
                uint8_t a25, uint8_t a26, uint8_t a27, uint8_t a28, uint8_t a29,
                uint8_t a30, uint8_t a31, uint8_t a32, uint8_t a33, uint8_t a34,
                uint8_t a35, uint8_t a36, uint8_t a37, uint8_t a38, uint8_t a39,
                uint8_t a40, uint8_t a41, uint8_t a42, uint8_t a43, uint8_t a44,
                uint8_t a45, uint8_t a46, uint8_t a47, uint8_t a48, uint8_t a49,
                uint8_t a50, uint8_t a51, uint8_t a52, uint8_t a53, uint8_t a54,
                uint8_t a55, uint8_t a56, uint8_t a57, uint8_t a58, uint8_t a59,
                uint8_t a60, uint8_t a61, uint8_t a62, uint8_t a63) {
  return _mm512_set_epi64(
      uint64_t(a7) + (uint64_t(a6) << 8) + (uint64_t(a5) << 16) +
          (uint64_t(a4) << 24) + (uint64_t(a3) << 32) + (uint64_t(a2) << 40) +
          (uint64_t(a1) << 48) + (uint64_t(a0) << 56),
      uint64_t(a15) + (uint64_t(a14) << 8) + (uint64_t(a13) << 16) +
          (uint64_t(a12) << 24) + (uint64_t(a11) << 32) +
          (uint64_t(a10) << 40) + (uint64_t(a9) << 48) + (uint64_t(a8) << 56),
      uint64_t(a23) + (uint64_t(a22) << 8) + (uint64_t(a21) << 16) +
          (uint64_t(a20) << 24) + (uint64_t(a19) << 32) +
          (uint64_t(a18) << 40) + (uint64_t(a17) << 48) + (uint64_t(a16) << 56),
      uint64_t(a31) + (uint64_t(a30) << 8) + (uint64_t(a29) << 16) +
          (uint64_t(a28) << 24) + (uint64_t(a27) << 32) +
          (uint64_t(a26) << 40) + (uint64_t(a25) << 48) + (uint64_t(a24) << 56),
      uint64_t(a39) + (uint64_t(a38) << 8) + (uint64_t(a37) << 16) +
          (uint64_t(a36) << 24) + (uint64_t(a35) << 32) +
          (uint64_t(a34) << 40) + (uint64_t(a33) << 48) + (uint64_t(a32) << 56),
      uint64_t(a47) + (uint64_t(a46) << 8) + (uint64_t(a45) << 16) +
          (uint64_t(a44) << 24) + (uint64_t(a43) << 32) +
          (uint64_t(a42) << 40) + (uint64_t(a41) << 48) + (uint64_t(a40) << 56),
      uint64_t(a55) + (uint64_t(a54) << 8) + (uint64_t(a53) << 16) +
          (uint64_t(a52) << 24) + (uint64_t(a51) << 32) +
          (uint64_t(a50) << 40) + (uint64_t(a49) << 48) + (uint64_t(a48) << 56),
      uint64_t(a63) + (uint64_t(a62) << 8) + (uint64_t(a61) << 16) +
          (uint64_t(a60) << 24) + (uint64_t(a59) << 32) +
          (uint64_t(a58) << 40) + (uint64_t(a57) << 48) +
          (uint64_t(a56) << 56));
}
  #pragma GCC pop_options
#endif // SIMDUTF_GCC8

#endif // SIMDUTF_HASWELL_INTRINSICS_H
/* end file src\simdutf\icelake\intrinsics.h */
/* begin file src\simdutf\icelake\implementation.h */
#ifndef SIMDUTF_ICELAKE_IMPLEMENTATION_H
#define SIMDUTF_ICELAKE_IMPLEMENTATION_H


namespace simdutf {
namespace icelake {

namespace {
using namespace simdutf;
}

class implementation final : public simdutf::implementation {
public:
  simdutf_really_inline implementation()
      : simdutf::implementation(
            "icelake",
            "Intel AVX512 (AVX-512BW, AVX-512CD, AVX-512VL, AVX-512VBMI2 "
            "extensions)",
            internal::instruction_set::AVX2 | internal::instruction_set::BMI1 |
                internal::instruction_set::BMI2 |
                internal::instruction_set::AVX512BW |
                internal::instruction_set::AVX512CD |
                internal::instruction_set::AVX512VL |
                internal::instruction_set::AVX512VBMI2 |
                internal::instruction_set::AVX512VPOPCNTDQ) {}

  simdutf_warn_unused result base64_to_binary(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused result
  base64_to_binary(const char16_t *input, size_t length, char *output,
                   base64_options options,
                   last_chunk_handling_options last_chunk_options =
                       last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char16_t *input, size_t length, char *output,
      base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  size_t binary_to_base64(const char *input, size_t length, char *output,
                          base64_options options) const noexcept;
  const char *find(const char *start, const char *end,
                   char character) const noexcept;
  const char16_t *find(const char16_t *start, const char16_t *end,
                       char16_t character) const noexcept;
};

} // namespace icelake
} // namespace simdutf

#endif // SIMDUTF_ICELAKE_IMPLEMENTATION_H
/* end file src\simdutf\icelake\implementation.h */

  //
  // The rest need to be inside the region
  //
/* begin file src\simdutf\icelake\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "icelake"
// #define SIMDUTF_IMPLEMENTATION icelake

#if SIMDUTF_CAN_ALWAYS_RUN_ICELAKE
// nothing needed.
#else
SIMDUTF_TARGET_ICELAKE
#endif

#if SIMDUTF_GCC11ORMORE // workaround for
                        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105593
// clang-format off
SIMDUTF_DISABLE_GCC_WARNING(-Wmaybe-uninitialized)
// clang-format on
#endif // end of workaround
/* end file src\simdutf\icelake\begin.h */
  // Declarations
/* begin file src\simdutf\icelake\bitmanipulation.h */
#ifndef SIMDUTF_ICELAKE_BITMANIPULATION_H
#define SIMDUTF_ICELAKE_BITMANIPULATION_H

namespace simdutf {
namespace icelake {
namespace {

#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
simdutf_really_inline unsigned __int64 count_ones(uint64_t input_num) {
  // note: we do not support legacy 32-bit Windows
  return __popcnt64(input_num); // Visual Studio wants two underscores
}
#else
simdutf_really_inline long long int count_ones(uint64_t input_num) {
  return _popcnt64(input_num);
}
#endif

#if SIMDUTF_NEED_TRAILING_ZEROES
// simdutf_really_inline int trailing_zeroes(uint64_t input_num) {
//   #if SIMDUTF_REGULAR_VISUAL_STUDIO
//   return (int)_tzcnt_u64(input_num);
//   #else  // SIMDUTF_REGULAR_VISUAL_STUDIO
//   return __builtin_ctzll(input_num);
//   #endif // SIMDUTF_REGULAR_VISUAL_STUDIO
// }
#endif

} // unnamed namespace
} // namespace icelake
} // namespace simdutf

#endif // SIMDUTF_ICELAKE_BITMANIPULATION_H
/* end file src\simdutf\icelake\bitmanipulation.h */
/* begin file src\simdutf\icelake\simd.h */
#ifndef SIMDUTF_ICELAKE_SIMD_H
#define SIMDUTF_ICELAKE_SIMD_H

namespace simdutf {
namespace icelake {
namespace {
namespace simd {

/* begin file src\simdutf\icelake\simd16-inl.h */
template <typename T> struct simd16;

template <> struct simd16<uint16_t> {
  static const size_t SIZE = sizeof(__m512i);
  static const size_t ELEMENTS = SIZE / sizeof(uint16_t);

  template <typename Pointer>
  static simdutf_really_inline simd16<uint16_t> load(const Pointer *ptr) {
    return simd16<uint16_t>(ptr);
  }

  __m512i value;

  simdutf_really_inline simd16(const __m512i v) : value(v) {}

  template <typename Pointer>
  simdutf_really_inline simd16(const Pointer *ptr)
      : value(_mm512_loadu_si512(reinterpret_cast<const __m512i *>(ptr))) {}

  // operators
  simdutf_really_inline simd16 &operator+=(const simd16 other) {
    value = _mm512_add_epi32(value, other.value);
    return *this;
  }

  simdutf_really_inline simd16 &operator-=(const simd16 other) {
    value = _mm512_sub_epi32(value, other.value);
    return *this;
  }

  // methods
  simdutf_really_inline simd16 swap_bytes() const {
    const __m512i byteflip = _mm512_setr_epi64(
        0x0607040502030001, 0x0e0f0c0d0a0b0809, 0x0607040502030001,
        0x0e0f0c0d0a0b0809, 0x0607040502030001, 0x0e0f0c0d0a0b0809,
        0x0607040502030001, 0x0e0f0c0d0a0b0809);

    return _mm512_shuffle_epi8(value, byteflip);
  }

  simdutf_really_inline uint64_t sum() const {
    const auto lo = _mm512_and_si512(value, _mm512_set1_epi32(0xffff));
    const auto hi = _mm512_srli_epi32(value, 16);
    const auto sum32 = _mm512_add_epi32(lo, hi);

    return _mm512_reduce_add_epi32(sum32);
  }

  // static members
  simdutf_really_inline static simd16<uint16_t> zero() {
    return _mm512_setzero_si512();
  }

  simdutf_really_inline static simd16<uint16_t> splat(uint16_t v) {
    return _mm512_set1_epi16(v);
  }
};

template <> struct simd16<bool> {
  __mmask32 value;

  simdutf_really_inline simd16(const __mmask32 v) : value(v) {}
};

// ------------------------------------------------------------

simdutf_really_inline simd16<uint16_t> min(const simd16<uint16_t> b,
                                           const simd16<uint16_t> a) {
  return _mm512_min_epu16(a.value, b.value);
}

simdutf_really_inline simd16<uint16_t> operator&(const simd16<uint16_t> a,
                                                 uint16_t b) {
  return _mm512_and_si512(a.value, _mm512_set1_epi16(b));
}

simdutf_really_inline simd16<uint16_t> operator^(const simd16<uint16_t> a,
                                                 uint16_t b) {
  return _mm512_xor_si512(a.value, _mm512_set1_epi16(b));
}

simdutf_really_inline simd16<uint16_t> operator^(const simd16<uint16_t> a,
                                                 const simd16<uint16_t> b) {
  return _mm512_xor_si512(a.value, b.value);
}

simdutf_really_inline simd16<bool> operator==(const simd16<uint16_t> a,
                                              uint16_t b) {
  return _mm512_cmpeq_epi16_mask(a.value, _mm512_set1_epi16(b));
}
/* end file src\simdutf\icelake\simd16-inl.h */
/* begin file src\simdutf\icelake\simd32-inl.h */
template <typename T> struct simd32;

template <> struct simd32<uint32_t> {
  static const size_t SIZE = sizeof(__m512i);
  static const size_t ELEMENTS = SIZE / sizeof(uint32_t);

  __m512i value;

  simdutf_really_inline simd32(const __m512i v) : value(v) {}

  template <typename Pointer>
  simdutf_really_inline simd32(const Pointer *ptr)
      : value(_mm512_loadu_si512(reinterpret_cast<const __m512i *>(ptr))) {}

  uint64_t sum() const {
    const __m512i mask = _mm512_set1_epi64(0xffffffff);
    const __m512i t0 = _mm512_and_si512(value, mask);
    const __m512i t1 = _mm512_srli_epi64(value, 32);
    const __m512i t2 = _mm512_add_epi64(t0, t1);
    return _mm512_reduce_add_epi64(t2);
  }

  // operators
  simdutf_really_inline simd32 &operator+=(const simd32 other) {
    value = _mm512_add_epi32(value, other.value);
    return *this;
  }

  // static members
  simdutf_really_inline static simd32<uint32_t> zero() {
    return _mm512_setzero_si512();
  }

  simdutf_really_inline static simd32<uint32_t> splat(uint32_t v) {
    return _mm512_set1_epi32(v);
  }
};

simdutf_really_inline simd32<uint32_t> min(const simd32<uint32_t> b,
                                           const simd32<uint32_t> a) {
  return _mm512_min_epu32(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> operator&(const simd32<uint32_t> b,
                                                 const simd32<uint32_t> a) {
  return _mm512_and_si512(a.value, b.value);
}
/* end file src\simdutf\icelake\simd32-inl.h */

} // namespace simd
} // unnamed namespace
} // namespace icelake
} // namespace simdutf

#endif // SIMDUTF_ICELAKE_SIMD_H
/* end file src\simdutf\icelake\simd.h */

/* begin file src\simdutf\icelake\end.h */
#if SIMDUTF_CAN_ALWAYS_RUN_ICELAKE
// nothing needed.
#else
SIMDUTF_UNTARGET_REGION
#endif


#if SIMDUTF_GCC11ORMORE // workaround for
                        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105593
SIMDUTF_POP_DISABLE_WARNINGS
#endif // end of workaround
/* end file src\simdutf\icelake\end.h */

#endif // SIMDUTF_IMPLEMENTATION_ICELAKE
#endif // SIMDUTF_ICELAKE_H
/* end file src\simdutf\icelake.h */
/* begin file src\simdutf\haswell.h */
#ifndef SIMDUTF_HASWELL_H
#define SIMDUTF_HASWELL_H

#ifdef SIMDUTF_WESTMERE_H
  #error "haswell.h must be included before westmere.h"
#endif
#ifdef SIMDUTF_FALLBACK_H
  #error "haswell.h must be included before fallback.h"
#endif


// Default Haswell to on if this is x86-64. Even if we are not compiled for it,
// it could be selected at runtime.
#ifndef SIMDUTF_IMPLEMENTATION_HASWELL
  //
  // You do not want to restrict it like so: SIMDUTF_IS_X86_64 && __AVX2__
  // because we want to rely on *runtime dispatch*.
  //
  #if SIMDUTF_CAN_ALWAYS_RUN_ICELAKE
    #define SIMDUTF_IMPLEMENTATION_HASWELL 0
  #else
    #define SIMDUTF_IMPLEMENTATION_HASWELL (SIMDUTF_IS_X86_64)
  #endif

#endif
// To see why  (__BMI__) && (__LZCNT__) are not part of this next line, see
// https://github.com/simdutf/simdutf/issues/1247
#if ((SIMDUTF_IMPLEMENTATION_HASWELL) && (SIMDUTF_IS_X86_64) && (__AVX2__))
  #define SIMDUTF_CAN_ALWAYS_RUN_HASWELL 1
#else
  #define SIMDUTF_CAN_ALWAYS_RUN_HASWELL 0
#endif

#if SIMDUTF_IMPLEMENTATION_HASWELL

  #define SIMDUTF_TARGET_HASWELL SIMDUTF_TARGET_REGION("avx2,bmi,lzcnt,popcnt")

namespace simdutf {
/**
 * Implementation for Haswell (Intel AVX2).
 */
namespace haswell {} // namespace haswell
} // namespace simdutf

  //
  // These two need to be included outside SIMDUTF_TARGET_REGION
  //
/* begin file src\simdutf\haswell\implementation.h */
#ifndef SIMDUTF_HASWELL_IMPLEMENTATION_H
#define SIMDUTF_HASWELL_IMPLEMENTATION_H


// The constructor may be executed on any host, so we take care not to use
// SIMDUTF_TARGET_REGION
namespace simdutf {
namespace haswell {

using namespace simdutf;

class implementation final : public simdutf::implementation {
public:
  simdutf_really_inline implementation()
      : simdutf::implementation("haswell", "Intel/AMD AVX2",
                                internal::instruction_set::AVX2 |
                                    internal::instruction_set::BMI1 |
                                    internal::instruction_set::BMI2) {}

  simdutf_warn_unused result base64_to_binary(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused result
  base64_to_binary(const char16_t *input, size_t length, char *output,
                   base64_options options,
                   last_chunk_handling_options last_chunk_options =
                       last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char16_t *input, size_t length, char *output,
      base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  size_t binary_to_base64(const char *input, size_t length, char *output,
                          base64_options options) const noexcept;
  const char *find(const char *start, const char *end,
                   char character) const noexcept;
  const char16_t *find(const char16_t *start, const char16_t *end,
                       char16_t character) const noexcept;
};

} // namespace haswell
} // namespace simdutf

#endif // SIMDUTF_HASWELL_IMPLEMENTATION_H
/* end file src\simdutf\haswell\implementation.h */
/* begin file src\simdutf\haswell\intrinsics.h */
#ifndef SIMDUTF_HASWELL_INTRINSICS_H
#define SIMDUTF_HASWELL_INTRINSICS_H


#ifdef SIMDUTF_VISUAL_STUDIO
  // under clang within visual studio, this will include <x86intrin.h>
  #include <intrin.h> // visual studio or clang
#else

  #if SIMDUTF_GCC11ORMORE
// We should not get warnings while including <x86intrin.h> yet we do
// under some versions of GCC.
// If the x86intrin.h header has uninitialized values that are problematic,
// it is a GCC issue, we want to ignore these warnings.
SIMDUTF_DISABLE_GCC_WARNING(-Wuninitialized)
  #endif

  #include <x86intrin.h> // elsewhere

  #if SIMDUTF_GCC11ORMORE
// cancels the suppression of the -Wuninitialized
SIMDUTF_POP_DISABLE_WARNINGS
  #endif

#endif // SIMDUTF_VISUAL_STUDIO

#ifdef SIMDUTF_CLANG_VISUAL_STUDIO
  /**
   * You are not supposed, normally, to include these
   * headers directly. Instead you should either include intrin.h
   * or x86intrin.h. However, when compiling with clang
   * under Windows (i.e., when _MSC_VER is set), these headers
   * only get included *if* the corresponding features are detected
   * from macros:
   * e.g., if __AVX2__ is set... in turn,  we normally set these
   * macros by compiling against the corresponding architecture
   * (e.g., arch:AVX2, -mavx2, etc.) which compiles the whole
   * software with these advanced instructions. In simdutf, we
   * want to compile the whole program for a generic target,
   * and only target our specific kernels. As a workaround,
   * we directly include the needed headers. These headers would
   * normally guard against such usage, but we carefully included
   * <x86intrin.h>  (or <intrin.h>) before, so the headers
   * are fooled.
   */
  #include <bmiintrin.h>   // for _blsr_u64
  #include <lzcntintrin.h> // for  __lzcnt64
  #include <immintrin.h>   // for most things (AVX2, AVX512, _popcnt64)
  #include <smmintrin.h>
  #include <tmmintrin.h>
  #include <avxintrin.h>
  #include <avx2intrin.h>
  // unfortunately, we may not get _blsr_u64, but, thankfully, clang
  // has it as a macro.
  #ifndef _blsr_u64
    // we roll our own
    #define _blsr_u64(n) (((n) - 1) & (n))
  #endif //  _blsr_u64
  // Same issue with _blsmsk_u32:
  #ifndef _blsmsk_u32
    // we roll our own
    #define _blsmsk_u32(n) (((n) - 1) ^ (n))
  #endif //  _blsmsk_u32
#endif   // SIMDUTF_CLANG_VISUAL_STUDIO

#endif // SIMDUTF_HASWELL_INTRINSICS_H
/* end file src\simdutf\haswell\intrinsics.h */

  //
  // The rest need to be inside the region
  //
/* begin file src\simdutf\haswell\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "haswell"
// #define SIMDUTF_IMPLEMENTATION haswell
#define SIMDUTF_SIMD_HAS_BYTEMASK 1

#if SIMDUTF_CAN_ALWAYS_RUN_HASWELL
// nothing needed.
#else
SIMDUTF_TARGET_HASWELL
#endif

#if SIMDUTF_GCC11ORMORE // workaround for
                        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105593
// clang-format off
SIMDUTF_DISABLE_GCC_WARNING(-Wmaybe-uninitialized)
// clang-format on
#endif // end of workaround
/* end file src\simdutf\haswell\begin.h */
  // Declarations
/* begin file src\simdutf\haswell\bitmanipulation.h */
#ifndef SIMDUTF_HASWELL_BITMANIPULATION_H
#define SIMDUTF_HASWELL_BITMANIPULATION_H

namespace simdutf {
namespace haswell {
namespace {

#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
simdutf_really_inline unsigned __int64 count_ones(uint64_t input_num) {
  // note: we do not support legacy 32-bit Windows
  return __popcnt64(input_num); // Visual Studio wants two underscores
}
#else
simdutf_really_inline long long int count_ones(uint64_t input_num) {
  return _popcnt64(input_num);
}
#endif

#if SIMDUTF_NEED_TRAILING_ZEROES
simdutf_really_inline int trailing_zeroes(uint64_t input_num) {
  #if SIMDUTF_REGULAR_VISUAL_STUDIO
  return (int)_tzcnt_u64(input_num);
  #else  // SIMDUTF_REGULAR_VISUAL_STUDIO
  return __builtin_ctzll(input_num);
  #endif // SIMDUTF_REGULAR_VISUAL_STUDIO
}
#endif

template <typename T> bool is_power_of_two(T x) { return (x & (x - 1)) == 0; }

} // unnamed namespace
} // namespace haswell
} // namespace simdutf

#endif // SIMDUTF_HASWELL_BITMANIPULATION_H
/* end file src\simdutf\haswell\bitmanipulation.h */
/* begin file src\simdutf\haswell\simd.h */
#ifndef SIMDUTF_HASWELL_SIMD_H
#define SIMDUTF_HASWELL_SIMD_H

namespace simdutf {
namespace haswell {
namespace {
namespace simd {

// Forward-declared so they can be used by splat and friends.
template <typename Child> struct base {
  __m256i value;

  // Zero constructor
  simdutf_really_inline base() : value{__m256i()} {}

  // Conversion from SIMD register
  simdutf_really_inline base(const __m256i _value) : value(_value) {}

  simdutf_really_inline operator const __m256i &() const { return this->value; }

  template <endianness big_endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    __m256i first = _mm256_cvtepu8_epi16(_mm256_castsi256_si128(*this));
    __m256i second = _mm256_cvtepu8_epi16(_mm256_extractf128_si256(*this, 1));
    if (big_endian) {
      const __m256i swap = _mm256_setr_epi8(
          1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 17, 16, 19, 18,
          21, 20, 23, 22, 25, 24, 27, 26, 29, 28, 31, 30);
      first = _mm256_shuffle_epi8(first, swap);
      second = _mm256_shuffle_epi8(second, swap);
    }
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(ptr), first);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(ptr + 16), second);
  }

  simdutf_really_inline void store_ascii_as_utf32(char32_t *ptr) const {
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(ptr),
                        _mm256_cvtepu8_epi32(_mm256_castsi256_si128(*this)));
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(ptr + 8),
                        _mm256_cvtepu8_epi32(_mm256_castsi256_si128(
                            _mm256_srli_si256(*this, 8))));
    _mm256_storeu_si256(
        reinterpret_cast<__m256i *>(ptr + 16),
        _mm256_cvtepu8_epi32(_mm256_extractf128_si256(*this, 1)));
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(ptr + 24),
                        _mm256_cvtepu8_epi32(_mm_srli_si128(
                            _mm256_extractf128_si256(*this, 1), 8)));
  }
  // Bit operations
  simdutf_really_inline Child operator|(const Child other) const {
    return _mm256_or_si256(*this, other);
  }
  simdutf_really_inline Child operator&(const Child other) const {
    return _mm256_and_si256(*this, other);
  }
  simdutf_really_inline Child operator^(const Child other) const {
    return _mm256_xor_si256(*this, other);
  }
  simdutf_really_inline Child &operator|=(const Child other) {
    auto this_cast = static_cast<Child *>(this);
    *this_cast = *this_cast | other;
    return *this_cast;
  }
};

// Forward-declared so they can be used by splat and friends.
template <typename T> struct simd8;

template <typename T, typename Mask = simd8<bool>>
struct base8 : base<simd8<T>> {
  simdutf_really_inline base8() : base<simd8<T>>() {}

  simdutf_really_inline base8(const __m256i _value) : base<simd8<T>>(_value) {}

  friend simdutf_always_inline Mask operator==(const simd8<T> lhs,
                                               const simd8<T> rhs) {
    return _mm256_cmpeq_epi8(lhs, rhs);
  }

  static const int SIZE = sizeof(base<T>::value);

  template <int N = 1>
  simdutf_really_inline simd8<T> prev(const simd8<T> prev_chunk) const {
    return _mm256_alignr_epi8(
        *this, _mm256_permute2x128_si256(prev_chunk, *this, 0x21), 16 - N);
  }
};

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd8<bool> : base8<bool> {
  static simdutf_really_inline simd8<bool> splat(bool _value) {
    return _mm256_set1_epi8(uint8_t(-(!!_value)));
  }

  simdutf_really_inline simd8(const __m256i _value) : base8<bool>(_value) {}

  simdutf_really_inline simd8(bool _value) : base8<bool>(splat(_value)) {}

  simdutf_really_inline uint32_t to_bitmask() const {
    return uint32_t(_mm256_movemask_epi8(value));
  }
};

template <typename T> struct base8_numeric : base8<T> {
  static simdutf_really_inline simd8<T> splat(T _value) {
    return _mm256_set1_epi8(_value);
  }
  static simdutf_really_inline simd8<T> zero() {
    return _mm256_setzero_si256();
  }
  static simdutf_really_inline simd8<T> load(const T values[32]) {
    return _mm256_loadu_si256(reinterpret_cast<const __m256i *>(values));
  }
  // Repeat 16 values as many times as necessary (usually for lookup tables)
  static simdutf_really_inline simd8<T> repeat_16(T v0, T v1, T v2, T v3, T v4,
                                                  T v5, T v6, T v7, T v8, T v9,
                                                  T v10, T v11, T v12, T v13,
                                                  T v14, T v15) {
    return simd8<T>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13,
                    v14, v15, v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11,
                    v12, v13, v14, v15);
  }

  simdutf_really_inline base8_numeric() : base8<T>() {}
  simdutf_really_inline base8_numeric(const __m256i _value)
      : base8<T>(_value) {}

  // Store to array
  simdutf_really_inline void store(T dst[32]) const {
    return _mm256_storeu_si256(reinterpret_cast<__m256i *>(dst), *this);
  }

  // Addition/subtraction are the same for signed and unsigned
  simdutf_really_inline simd8<T> operator-(const simd8<T> other) const {
    return _mm256_sub_epi8(*this, other);
  }
  simdutf_really_inline simd8<T> &operator-=(const simd8<T> other) {
    *this = *this - other;
    return *static_cast<simd8<T> *>(this);
  }

  // Override to distinguish from bool version
  simdutf_really_inline simd8<T> operator~() const { return *this ^ 0xFFu; }

  // Perform a lookup assuming the value is between 0 and 16 (undefined behavior
  // for out of range values)
  template <typename L>
  simdutf_really_inline simd8<L> lookup_16(simd8<L> lookup_table) const {
    return _mm256_shuffle_epi8(lookup_table, *this);
  }

  template <typename L>
  simdutf_really_inline simd8<L>
  lookup_16(L replace0, L replace1, L replace2, L replace3, L replace4,
            L replace5, L replace6, L replace7, L replace8, L replace9,
            L replace10, L replace11, L replace12, L replace13, L replace14,
            L replace15) const {
    return lookup_16(simd8<L>::repeat_16(
        replace0, replace1, replace2, replace3, replace4, replace5, replace6,
        replace7, replace8, replace9, replace10, replace11, replace12,
        replace13, replace14, replace15));
  }
};

// Signed bytes
template <> struct simd8<int8_t> : base8_numeric<int8_t> {
  simdutf_really_inline simd8() : base8_numeric<int8_t>() {}
  simdutf_really_inline simd8(const __m256i _value)
      : base8_numeric<int8_t>(_value) {}

  // Splat constructor
  simdutf_really_inline simd8(int8_t _value) : simd8(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd8(const int8_t values[32]) : simd8(load(values)) {}
  simdutf_really_inline operator simd8<uint8_t>() const;

  simdutf_really_inline bool is_ascii() const {
    return _mm256_movemask_epi8(*this) == 0;
  }
  // Order-sensitive comparisons
  simdutf_really_inline simd8<bool> operator>(const simd8<int8_t> other) const {
    return _mm256_cmpgt_epi8(*this, other);
  }
  simdutf_really_inline simd8<bool> operator<(const simd8<int8_t> other) const {
    return _mm256_cmpgt_epi8(other, *this);
  }
};

// Unsigned bytes
template <> struct simd8<uint8_t> : base8_numeric<uint8_t> {
  simdutf_really_inline simd8() : base8_numeric<uint8_t>() {}
  simdutf_really_inline simd8(const __m256i _value)
      : base8_numeric<uint8_t>(_value) {}
  // Splat constructor
  simdutf_really_inline simd8(uint8_t _value) : simd8(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd8(const uint8_t values[32]) : simd8(load(values)) {}
  // Member-by-member initialization
  simdutf_really_inline
  simd8(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4, uint8_t v5,
        uint8_t v6, uint8_t v7, uint8_t v8, uint8_t v9, uint8_t v10,
        uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15,
        uint8_t v16, uint8_t v17, uint8_t v18, uint8_t v19, uint8_t v20,
        uint8_t v21, uint8_t v22, uint8_t v23, uint8_t v24, uint8_t v25,
        uint8_t v26, uint8_t v27, uint8_t v28, uint8_t v29, uint8_t v30,
        uint8_t v31)
      : simd8(_mm256_setr_epi8(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11,
                               v12, v13, v14, v15, v16, v17, v18, v19, v20, v21,
                               v22, v23, v24, v25, v26, v27, v28, v29, v30,
                               v31)) {}

  // Saturated math
  simdutf_really_inline simd8<uint8_t>
  saturating_sub(const simd8<uint8_t> other) const {
    return _mm256_subs_epu8(*this, other);
  }

  // Order-specific operations
  simdutf_really_inline simd8<uint8_t>
  min_val(const simd8<uint8_t> other) const {
    return _mm256_min_epu8(other, *this);
  }
  // Same as >, but only guarantees true is nonzero (< guarantees true = -1)
  simdutf_really_inline simd8<uint8_t>
  gt_bits(const simd8<uint8_t> other) const {
    return this->saturating_sub(other);
  }
  simdutf_really_inline simd8<bool>
  operator>=(const simd8<uint8_t> other) const {
    return other.min_val(*this) == other;
  }

  // Bit-specific operations
  simdutf_really_inline bool is_ascii() const {
    return _mm256_movemask_epi8(*this) == 0;
  }
  simdutf_really_inline bool bits_not_set_anywhere() const {
    return _mm256_testz_si256(*this, *this);
  }

  simdutf_really_inline bool any_bits_set_anywhere() const {
    return !bits_not_set_anywhere();
  }

  template <int N> simdutf_really_inline simd8<uint8_t> shr() const {
    return simd8<uint8_t>(_mm256_srli_epi16(*this, N)) & uint8_t(0xFFu >> N);
  }

  simdutf_really_inline uint64_t sum_bytes() const {
    const auto tmp = _mm256_sad_epu8(value, _mm256_setzero_si256());

    return _mm256_extract_epi64(tmp, 0) + _mm256_extract_epi64(tmp, 1) +
           _mm256_extract_epi64(tmp, 2) + _mm256_extract_epi64(tmp, 3);
  }
};
simdutf_really_inline simd8<int8_t>::operator simd8<uint8_t>() const {
  return this->value;
}

template <typename T> struct simd8x64 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd8<T>);
  static_assert(NUM_CHUNKS == 2,
                "Haswell kernel should use two registers per 64-byte block.");
  simd8<T> chunks[NUM_CHUNKS];

  simd8x64(const simd8x64<T> &o) = delete; // no copy allowed
  simd8x64<T> &
  operator=(const simd8<T> other) = delete; // no assignment allowed
  simd8x64() = delete;                      // no default constructor allowed

  simdutf_really_inline simd8x64(const simd8<T> chunk0, const simd8<T> chunk1)
      : chunks{chunk0, chunk1} {}
  simdutf_really_inline simd8x64(const T *ptr)
      : chunks{simd8<T>::load(ptr),
               simd8<T>::load(ptr + sizeof(simd8<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + sizeof(simd8<T>) * 0 / sizeof(T));
    this->chunks[1].store(ptr + sizeof(simd8<T>) * 1 / sizeof(T));
  }

  simdutf_really_inline uint64_t to_bitmask() const {
    uint64_t r_lo = uint32_t(this->chunks[0].to_bitmask());
    uint64_t r_hi = this->chunks[1].to_bitmask();
    return r_lo | (r_hi << 32);
  }

  simdutf_really_inline simd8x64<T> &operator|=(const simd8x64<T> &other) {
    this->chunks[0] |= other.chunks[0];
    this->chunks[1] |= other.chunks[1];
    return *this;
  }

  simdutf_really_inline simd8<T> reduce_or() const {
    return this->chunks[0] | this->chunks[1];
  }

  simdutf_really_inline bool is_ascii() const {
    return this->reduce_or().is_ascii();
  }

  template <endianness endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    this->chunks[0].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 0);
    this->chunks[1].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 1);
  }

  simdutf_really_inline void store_ascii_as_utf32(char32_t *ptr) const {
    this->chunks[0].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 0);
    this->chunks[1].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 1);
  }

  simdutf_really_inline uint64_t in_range(const T low, const T high) const {
    const simd8<T> mask_low = simd8<T>::splat(low);
    const simd8<T> mask_high = simd8<T>::splat(high);

    return simd8x64<bool>(
               (this->chunks[0] <= mask_high) & (this->chunks[0] >= mask_low),
               (this->chunks[1] <= mask_high) & (this->chunks[1] >= mask_low))
        .to_bitmask();
  }

  simdutf_really_inline uint64_t lt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] < mask, this->chunks[1] < mask)
        .to_bitmask();
  }

  simdutf_really_inline uint64_t gt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] > mask, this->chunks[1] > mask)
        .to_bitmask();
  }
  simdutf_really_inline uint64_t eq(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] == mask, this->chunks[1] == mask)
        .to_bitmask();
  }
  simdutf_really_inline uint64_t gteq_unsigned(const uint8_t m) const {
    const simd8<uint8_t> mask = simd8<uint8_t>::splat(m);
    return simd8x64<bool>((simd8<uint8_t>(__m256i(this->chunks[0])) >= mask),
                          (simd8<uint8_t>(__m256i(this->chunks[1])) >= mask))
        .to_bitmask();
  }
}; // struct simd8x64<T>

/* begin file src\simdutf\haswell\simd16-inl.h */
#ifdef __GNUC__
  #if __GNUC__ < 8
    #define _mm256_set_m128i(xmm1, xmm2)                                       \
      _mm256_permute2f128_si256(_mm256_castsi128_si256(xmm1),                  \
                                _mm256_castsi128_si256(xmm2), 2)
    #define _mm256_setr_m128i(xmm2, xmm1)                                      \
      _mm256_permute2f128_si256(_mm256_castsi128_si256(xmm1),                  \
                                _mm256_castsi128_si256(xmm2), 2)
  #endif
#endif

template <typename T> struct simd16;

template <typename T, typename Mask = simd16<bool>>
struct base16 : base<simd16<T>> {
  using bitmask_type = uint32_t;

  simdutf_really_inline base16() : base<simd16<T>>() {}
  simdutf_really_inline base16(const __m256i _value)
      : base<simd16<T>>(_value) {}
  template <typename Pointer>
  simdutf_really_inline base16(const Pointer *ptr)
      : base16(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(ptr))) {}

  friend simdutf_always_inline Mask operator==(const simd16<T> lhs,
                                               const simd16<T> rhs) {
    return _mm256_cmpeq_epi16(lhs, rhs);
  }

  /// the size of vector in bytes
  static const int SIZE = sizeof(base<simd16<T>>::value);

  /// the number of elements of type T a vector can hold
  static const int ELEMENTS = SIZE / sizeof(T);
};

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd16<bool> : base16<bool> {
  static simdutf_really_inline simd16<bool> splat(bool _value) {
    return _mm256_set1_epi16(uint16_t(-(!!_value)));
  }

  simdutf_really_inline simd16() : base16() {}

  simdutf_really_inline simd16(const __m256i _value) : base16<bool>(_value) {}

  // Splat constructor
  simdutf_really_inline simd16(bool _value) : base16<bool>(splat(_value)) {}

  simdutf_really_inline bitmask_type to_bitmask() const {
    return _mm256_movemask_epi8(*this);
  }

  simdutf_really_inline simd16<bool> operator~() const { return *this ^ true; }
};

template <typename T> struct base16_numeric : base16<T> {
  static simdutf_really_inline simd16<T> splat(T _value) {
    return _mm256_set1_epi16(_value);
  }

  static simdutf_really_inline simd16<T> zero() {
    return _mm256_setzero_si256();
  }

  static simdutf_really_inline simd16<T> load(const T values[8]) {
    return _mm256_loadu_si256(reinterpret_cast<const __m256i *>(values));
  }

  simdutf_really_inline base16_numeric() : base16<T>() {}

  simdutf_really_inline base16_numeric(const __m256i _value)
      : base16<T>(_value) {}

  // Store to array
  simdutf_really_inline void store(T dst[8]) const {
    return _mm256_storeu_si256(reinterpret_cast<__m256i *>(dst), *this);
  }

  // Override to distinguish from bool version
  simdutf_really_inline simd16<T> operator~() const { return *this ^ 0xFFFFu; }

  // Addition/subtraction are the same for signed and unsigned
  simdutf_really_inline simd16<T> operator+(const simd16<T> other) const {
    return _mm256_add_epi16(*this, other);
  }
  simdutf_really_inline simd16<T> &operator+=(const simd16<T> other) {
    *this = *this + other;
    return *static_cast<simd16<T> *>(this);
  }
};

// Unsigned code units
template <> struct simd16<uint16_t> : base16_numeric<uint16_t> {
  simdutf_really_inline simd16() : base16_numeric<uint16_t>() {}
  simdutf_really_inline simd16(const __m256i _value)
      : base16_numeric<uint16_t>(_value) {}

  // Splat constructor
  simdutf_really_inline simd16(uint16_t _value) : simd16(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd16(const uint16_t *values) : simd16(load(values)) {}
  simdutf_really_inline simd16(const char16_t *values)
      : simd16(load(reinterpret_cast<const uint16_t *>(values))) {}

  // Order-specific operations
  simdutf_really_inline simd16<uint16_t>
  max_val(const simd16<uint16_t> other) const {
    return _mm256_max_epu16(*this, other);
  }
  simdutf_really_inline simd16<uint16_t>
  min_val(const simd16<uint16_t> other) const {
    return _mm256_min_epu16(*this, other);
  }
  // Same as <, but only guarantees true is nonzero (< guarantees true = -1)
  simdutf_really_inline simd16<bool>
  operator<=(const simd16<uint16_t> other) const {
    return other.max_val(*this) == other;
  }
  simdutf_really_inline simd16<bool>
  operator>=(const simd16<uint16_t> other) const {
    return other.min_val(*this) == other;
  }

  // Bit-specific operations
  simdutf_really_inline simd16<bool> bits_not_set() const {
    return *this == uint16_t(0);
  }

  simdutf_really_inline simd16<bool> any_bits_set() const {
    return ~this->bits_not_set();
  }

  template <int N> simdutf_really_inline simd16<uint16_t> shr() const {
    return simd16<uint16_t>(_mm256_srli_epi16(*this, N));
  }

  // Change the endianness
  simdutf_really_inline simd16<uint16_t> swap_bytes() const {
    const __m256i swap = _mm256_setr_epi8(
        1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 17, 16, 19, 18,
        21, 20, 23, 22, 25, 24, 27, 26, 29, 28, 31, 30);
    return _mm256_shuffle_epi8(*this, swap);
  }

  // Pack with the unsigned saturation of two uint16_t code units into single
  // uint8_t vector
  static simdutf_really_inline simd8<uint8_t> pack(const simd16<uint16_t> &v0,
                                                   const simd16<uint16_t> &v1) {
    // Note: the AVX2 variant of pack operates on 128-bit lanes, thus
    //       we have to shuffle lanes in order to produce bytes in the
    //       correct order.

    // get the 0th lanes
    const __m128i lo_0 = _mm256_extracti128_si256(v0, 0);
    const __m128i lo_1 = _mm256_extracti128_si256(v1, 0);

    // get the 1st lanes
    const __m128i hi_0 = _mm256_extracti128_si256(v0, 1);
    const __m128i hi_1 = _mm256_extracti128_si256(v1, 1);

    // build new vectors (shuffle lanes)
    const __m256i t0 = _mm256_set_m128i(lo_1, lo_0);
    const __m256i t1 = _mm256_set_m128i(hi_1, hi_0);

    // pack code units in linear order from v0 and v1
    return _mm256_packus_epi16(t0, t1);
  }

  simdutf_really_inline uint64_t sum() const {
    const auto lo_u16 = _mm256_and_si256(value, _mm256_set1_epi32(0x0000ffff));
    const auto hi_u16 = _mm256_srli_epi32(value, 16);
    const auto sum_u32 = _mm256_add_epi32(lo_u16, hi_u16);

    const auto lo_u32 =
        _mm256_and_si256(sum_u32, _mm256_set1_epi64x(0xffffffff));
    const auto hi_u32 = _mm256_srli_epi64(sum_u32, 32);
    const auto sum_u64 = _mm256_add_epi64(lo_u32, hi_u32);

    return uint64_t(_mm256_extract_epi64(sum_u64, 0)) +
           uint64_t(_mm256_extract_epi64(sum_u64, 1)) +
           uint64_t(_mm256_extract_epi64(sum_u64, 2)) +
           uint64_t(_mm256_extract_epi64(sum_u64, 3));
  }
};

template <typename T> struct simd16x32 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd16<T>);
  static_assert(NUM_CHUNKS == 2,
                "Haswell kernel should use two registers per 64-byte block.");
  simd16<T> chunks[NUM_CHUNKS];

  simd16x32(const simd16x32<T> &o) = delete; // no copy allowed
  simd16x32<T> &
  operator=(const simd16<T> other) = delete; // no assignment allowed
  simd16x32() = delete;                      // no default constructor allowed

  simdutf_really_inline simd16x32(const simd16<T> chunk0,
                                  const simd16<T> chunk1)
      : chunks{chunk0, chunk1} {}
  simdutf_really_inline simd16x32(const T *ptr)
      : chunks{simd16<T>::load(ptr),
               simd16<T>::load(ptr + sizeof(simd16<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + sizeof(simd16<T>) * 0 / sizeof(T));
    this->chunks[1].store(ptr + sizeof(simd16<T>) * 1 / sizeof(T));
  }

  simdutf_really_inline uint64_t to_bitmask() const {
    uint64_t r_lo = uint32_t(this->chunks[0].to_bitmask());
    uint64_t r_hi = this->chunks[1].to_bitmask();
    return r_lo | (r_hi << 32);
  }

  simdutf_really_inline simd16<T> reduce_or() const {
    return this->chunks[0] | this->chunks[1];
  }

  simdutf_really_inline bool is_ascii() const {
    return this->reduce_or().is_ascii();
  }

  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    this->chunks[0].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 0);
    this->chunks[1].store_ascii_as_utf16(ptr + sizeof(simd16<T>));
  }

  simdutf_really_inline void swap_bytes() {
    this->chunks[0] = this->chunks[0].swap_bytes();
    this->chunks[1] = this->chunks[1].swap_bytes();
  }

  simdutf_really_inline uint64_t lteq(const T m) const {
    const simd16<T> mask = simd16<T>::splat(m);
    return simd16x32<bool>(this->chunks[0] <= mask, this->chunks[1] <= mask)
        .to_bitmask();
  }
  simdutf_really_inline uint64_t eq(const T m) const {
    const simd16<T> mask = simd16<T>::splat(m);
    return simd16x32<bool>(this->chunks[0] == mask, this->chunks[1] == mask)
        .to_bitmask();
  }
  simdutf_really_inline uint64_t not_in_range(const T low, const T high) const {
    const simd16<T> mask_low = simd16<T>::splat(static_cast<T>(low - 1));
    const simd16<T> mask_high = simd16<T>::splat(static_cast<T>(high + 1));
    return simd16x32<bool>(
               (this->chunks[0] >= mask_high) | (this->chunks[0] <= mask_low),
               (this->chunks[1] >= mask_high) | (this->chunks[1] <= mask_low))
        .to_bitmask();
  }
}; // struct simd16x32<T>

simd16<uint16_t> min(const simd16<uint16_t> a, simd16<uint16_t> b) {
  return _mm256_min_epu16(a.value, b.value);
}
/* end file src\simdutf\haswell\simd16-inl.h */
/* begin file src\simdutf\haswell\simd32-inl.h */
template <typename T> struct simd32;

template <> struct simd32<uint32_t> {
  static const size_t SIZE = sizeof(__m256i);
  static const size_t ELEMENTS = SIZE / sizeof(uint32_t);

  __m256i value;

  simdutf_really_inline simd32(const __m256i v) : value(v) {}

  template <typename Pointer>
  simdutf_really_inline simd32(const Pointer *ptr)
      : value(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(ptr))) {}

  simdutf_really_inline uint64_t sum() const {
    const __m256i mask = _mm256_set1_epi64x(0xffffffff);
    const __m256i t0 = _mm256_and_si256(value, mask);
    const __m256i t1 = _mm256_srli_epi64(value, 32);
    const __m256i t2 = _mm256_add_epi64(t0, t1);

    return uint64_t(_mm256_extract_epi64(t2, 0)) +
           uint64_t(_mm256_extract_epi64(t2, 1)) +
           uint64_t(_mm256_extract_epi64(t2, 2)) +
           uint64_t(_mm256_extract_epi64(t2, 3));
  }

  simdutf_really_inline simd32<uint32_t> swap_bytes() const {
    const __m256i shuffle =
        _mm256_setr_epi8(3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11, 15, 14, 13, 12,
                         3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11, 15, 14, 13, 12);

    return _mm256_shuffle_epi8(value, shuffle);
  }

  // operators
  simdutf_really_inline simd32 &operator+=(const simd32 other) {
    value = _mm256_add_epi32(value, other.value);
    return *this;
  }

  // static members
  simdutf_really_inline static simd32<uint32_t> zero() {
    return _mm256_setzero_si256();
  }

  simdutf_really_inline static simd32<uint32_t> splat(uint32_t v) {
    return _mm256_set1_epi32(v);
  }
};

//----------------------------------------------------------------------

template <> struct simd32<bool> {
  // static const size_t SIZE = sizeof(__m128i);
  // static const size_t ELEMENTS = SIZE / sizeof(uint32_t);

  __m256i value;

  simdutf_really_inline simd32(const __m256i v) : value(v) {}

  simdutf_really_inline bool any() const {
    return _mm256_movemask_epi8(value) != 0;
  }
};

//----------------------------------------------------------------------

template <typename T>
simdutf_really_inline simd32<T> operator|(const simd32<T> a,
                                          const simd32<T> b) {
  return _mm256_or_si256(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> min(const simd32<uint32_t> b,
                                           const simd32<uint32_t> a) {
  return _mm256_min_epu32(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> max(const simd32<uint32_t> a,
                                           const simd32<uint32_t> b) {
  return _mm256_max_epu32(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> operator&(const simd32<uint32_t> b,
                                                 const simd32<uint32_t> a) {
  return _mm256_and_si256(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> operator+(const simd32<uint32_t> a,
                                                 const simd32<uint32_t> b) {
  return _mm256_add_epi32(a.value, b.value);
}

simdutf_really_inline simd32<bool> operator==(const simd32<uint32_t> a,
                                              const simd32<uint32_t> b) {
  return _mm256_cmpeq_epi32(a.value, b.value);
}

simdutf_really_inline simd32<bool> operator>=(const simd32<uint32_t> a,
                                              const simd32<uint32_t> b) {
  return _mm256_cmpeq_epi32(_mm256_max_epu32(a.value, b.value), a.value);
}

simdutf_really_inline simd32<bool> operator!(const simd32<bool> v) {
  return _mm256_xor_si256(v.value, _mm256_set1_epi8(-1));
}

simdutf_really_inline simd32<bool> operator>(const simd32<uint32_t> a,
                                             const simd32<uint32_t> b) {
  return !(b >= a);
}
/* end file src\simdutf\haswell\simd32-inl.h */
/* begin file src\simdutf\haswell\simd64-inl.h */
template <typename T> struct simd64;

template <> struct simd64<uint64_t> {
  // static const size_t SIZE = sizeof(__m256i);
  // static const size_t ELEMENTS = SIZE / sizeof(uint64_t);

  __m256i value;

  simdutf_really_inline simd64(const __m256i v) : value(v) {}

  template <typename Pointer>
  simdutf_really_inline simd64(const Pointer *ptr)
      : value(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(ptr))) {}

  simdutf_really_inline uint64_t sum() const {
    return _mm256_extract_epi64(value, 0) + _mm256_extract_epi64(value, 1) +
           _mm256_extract_epi64(value, 2) + _mm256_extract_epi64(value, 3);
  }

  // operators
  simdutf_really_inline simd64 &operator+=(const simd64 other) {
    value = _mm256_add_epi64(value, other.value);
    return *this;
  }

  // static members
  simdutf_really_inline static simd64<uint64_t> zero() {
    return _mm256_setzero_si256();
  }

  simdutf_really_inline static simd64<uint64_t> splat(uint64_t v) {
    return _mm256_set1_epi64x(v);
  }
};
/* end file src\simdutf\haswell\simd64-inl.h */

simdutf_really_inline simd64<uint64_t> sum_8bytes(const simd8<uint8_t> v) {
  return _mm256_sad_epu8(v.value, simd8<uint8_t>::zero());
}

} // namespace simd

} // unnamed namespace
} // namespace haswell
} // namespace simdutf

#endif // SIMDUTF_HASWELL_SIMD_H
/* end file src\simdutf\haswell\simd.h */

/* begin file src\simdutf\haswell\end.h */
#if SIMDUTF_CAN_ALWAYS_RUN_HASWELL
// nothing needed.
#else
SIMDUTF_UNTARGET_REGION
#endif

#undef SIMDUTF_SIMD_HAS_BYTEMASK

#if SIMDUTF_GCC11ORMORE // workaround for
                        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105593
SIMDUTF_POP_DISABLE_WARNINGS
#endif // end of workaround
/* end file src\simdutf\haswell\end.h */

#endif // SIMDUTF_IMPLEMENTATION_HASWELL
#endif // SIMDUTF_HASWELL_COMMON_H
/* end file src\simdutf\haswell.h */
/* begin file src\simdutf\westmere.h */
#ifndef SIMDUTF_WESTMERE_H
#define SIMDUTF_WESTMERE_H

#ifdef SIMDUTF_FALLBACK_H
  #error "westmere.h must be included before fallback.h"
#endif


// Default Westmere to on if this is x86-64, unless we'll always select Haswell.
#ifndef SIMDUTF_IMPLEMENTATION_WESTMERE
  //
  // You do not want to set it to (SIMDUTF_IS_X86_64 &&
  // !SIMDUTF_REQUIRES_HASWELL) because you want to rely on runtime dispatch!
  //
  #if SIMDUTF_CAN_ALWAYS_RUN_ICELAKE || SIMDUTF_CAN_ALWAYS_RUN_HASWELL
    #define SIMDUTF_IMPLEMENTATION_WESTMERE 0
  #else
    #define SIMDUTF_IMPLEMENTATION_WESTMERE (SIMDUTF_IS_X86_64)
  #endif

#endif

#if (SIMDUTF_IMPLEMENTATION_WESTMERE && SIMDUTF_IS_X86_64 && __SSE4_2__)
  #define SIMDUTF_CAN_ALWAYS_RUN_WESTMERE 1
#else
  #define SIMDUTF_CAN_ALWAYS_RUN_WESTMERE 0
#endif

#if SIMDUTF_IMPLEMENTATION_WESTMERE

  #define SIMDUTF_TARGET_WESTMERE SIMDUTF_TARGET_REGION("sse4.2,popcnt")

namespace simdutf {
/**
 * Implementation for Westmere (Intel SSE4.2).
 */
namespace westmere {} // namespace westmere
} // namespace simdutf

  //
  // These two need to be included outside SIMDUTF_TARGET_REGION
  //
/* begin file src\simdutf\westmere\implementation.h */
#ifndef SIMDUTF_WESTMERE_IMPLEMENTATION_H
#define SIMDUTF_WESTMERE_IMPLEMENTATION_H


// The constructor may be executed on any host, so we take care not to use
// SIMDUTF_TARGET_REGION
namespace simdutf {
namespace westmere {

namespace {
using namespace simdutf;
}

class implementation final : public simdutf::implementation {
public:
  simdutf_really_inline implementation()
      : simdutf::implementation("westmere", "Intel/AMD SSE4.2",
                                internal::instruction_set::SSE42) {}

  simdutf_warn_unused result base64_to_binary(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused result
  base64_to_binary(const char16_t *input, size_t length, char *output,
                   base64_options options,
                   last_chunk_handling_options last_chunk_options =
                       last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char16_t *input, size_t length, char *output,
      base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  size_t binary_to_base64(const char *input, size_t length, char *output,
                          base64_options options) const noexcept;
  const char *find(const char *start, const char *end,
                   char character) const noexcept;
  const char16_t *find(const char16_t *start, const char16_t *end,
                       char16_t character) const noexcept;
};

} // namespace westmere
} // namespace simdutf

#endif // SIMDUTF_WESTMERE_IMPLEMENTATION_H
/* end file src\simdutf\westmere\implementation.h */
/* begin file src\simdutf\westmere\intrinsics.h */
#ifndef SIMDUTF_WESTMERE_INTRINSICS_H
#define SIMDUTF_WESTMERE_INTRINSICS_H

#ifdef SIMDUTF_VISUAL_STUDIO
  // under clang within visual studio, this will include <x86intrin.h>
  #include <intrin.h> // visual studio or clang
#else

  #if SIMDUTF_GCC11ORMORE
// We should not get warnings while including <x86intrin.h> yet we do
// under some versions of GCC.
// If the x86intrin.h header has uninitialized values that are problematic,
// it is a GCC issue, we want to ignore these warnings.
SIMDUTF_DISABLE_GCC_WARNING(-Wuninitialized)
  #endif

  #include <x86intrin.h> // elsewhere

  #if SIMDUTF_GCC11ORMORE
// cancels the suppression of the -Wuninitialized
SIMDUTF_POP_DISABLE_WARNINGS
  #endif

#endif // SIMDUTF_VISUAL_STUDIO

#ifdef SIMDUTF_CLANG_VISUAL_STUDIO
  /**
   * You are not supposed, normally, to include these
   * headers directly. Instead you should either include intrin.h
   * or x86intrin.h. However, when compiling with clang
   * under Windows (i.e., when _MSC_VER is set), these headers
   * only get included *if* the corresponding features are detected
   * from macros:
   */
  #include <smmintrin.h> // for _mm_alignr_epi8
#endif

#endif // SIMDUTF_WESTMERE_INTRINSICS_H
/* end file src\simdutf\westmere\intrinsics.h */

  //
  // The rest need to be inside the region
  //
/* begin file src\simdutf\westmere\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "westmere"
// #define SIMDUTF_IMPLEMENTATION westmere
#define SIMDUTF_SIMD_HAS_BYTEMASK 1

#if SIMDUTF_CAN_ALWAYS_RUN_WESTMERE
// nothing needed.
#else
SIMDUTF_TARGET_WESTMERE
#endif
/* end file src\simdutf\westmere\begin.h */

  // Declarations
/* begin file src\simdutf\westmere\bitmanipulation.h */
#ifndef SIMDUTF_WESTMERE_BITMANIPULATION_H
#define SIMDUTF_WESTMERE_BITMANIPULATION_H

namespace simdutf {
namespace westmere {
namespace {

#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
simdutf_really_inline unsigned __int64 count_ones(uint64_t input_num) {
  // note: we do not support legacy 32-bit Windows
  return __popcnt64(input_num); // Visual Studio wants two underscores
}
#else
simdutf_really_inline long long int count_ones(uint64_t input_num) {
  return _popcnt64(input_num);
}
#endif

#if SIMDUTF_NEED_TRAILING_ZEROES
simdutf_really_inline int trailing_zeroes(uint64_t input_num) {
  #if SIMDUTF_REGULAR_VISUAL_STUDIO
  unsigned long ret;
  _BitScanForward64(&ret, input_num);
  return (int)ret;
  #else  // SIMDUTF_REGULAR_VISUAL_STUDIO
  return __builtin_ctzll(input_num);
  #endif // SIMDUTF_REGULAR_VISUAL_STUDIO
}
#endif

template <typename T> bool is_power_of_two(T x) { return (x & (x - 1)) == 0; }

} // unnamed namespace
} // namespace westmere
} // namespace simdutf

#endif // SIMDUTF_WESTMERE_BITMANIPULATION_H
/* end file src\simdutf\westmere\bitmanipulation.h */
/* begin file src\simdutf\westmere\simd.h */
#ifndef SIMDUTF_WESTMERE_SIMD_H
#define SIMDUTF_WESTMERE_SIMD_H

namespace simdutf {
namespace westmere {
namespace {
namespace simd {

template <typename Child> struct base {
  __m128i value;

  // Zero constructor
  simdutf_really_inline base() : value{__m128i()} {}

  // Conversion from SIMD register
  simdutf_really_inline base(const __m128i _value) : value(_value) {}
  // Conversion to SIMD register
  simdutf_really_inline operator const __m128i &() const { return this->value; }
  template <endianness big_endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *p) const {
    __m128i first = _mm_cvtepu8_epi16(*this);
    __m128i second = _mm_cvtepu8_epi16(_mm_srli_si128(*this, 8));
    if (big_endian) {
      const __m128i swap =
          _mm_setr_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
      first = _mm_shuffle_epi8(first, swap);
      second = _mm_shuffle_epi8(second, swap);
    }
    _mm_storeu_si128(reinterpret_cast<__m128i *>(p), first);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(p + 8), second);
  }
  simdutf_really_inline void store_ascii_as_utf32(char32_t *p) const {
    _mm_storeu_si128(reinterpret_cast<__m128i *>(p), _mm_cvtepu8_epi32(*this));
    _mm_storeu_si128(reinterpret_cast<__m128i *>(p + 4),
                     _mm_cvtepu8_epi32(_mm_srli_si128(*this, 4)));
    _mm_storeu_si128(reinterpret_cast<__m128i *>(p + 8),
                     _mm_cvtepu8_epi32(_mm_srli_si128(*this, 8)));
    _mm_storeu_si128(reinterpret_cast<__m128i *>(p + 12),
                     _mm_cvtepu8_epi32(_mm_srli_si128(*this, 12)));
  }
  // Bit operations
  simdutf_really_inline Child operator|(const Child other) const {
    return _mm_or_si128(*this, other);
  }
  simdutf_really_inline Child operator&(const Child other) const {
    return _mm_and_si128(*this, other);
  }
  simdutf_really_inline Child operator^(const Child other) const {
    return _mm_xor_si128(*this, other);
  }
  simdutf_really_inline Child &operator|=(const Child other) {
    auto this_cast = static_cast<Child *>(this);
    *this_cast = *this_cast | other;
    return *this_cast;
  }
};

// Forward-declared so they can be used by splat and friends.
template <typename T> struct simd8;

template <typename T, typename Mask = simd8<bool>>
struct base8 : base<simd8<T>> {
  typedef uint16_t bitmask_t;
  typedef uint32_t bitmask2_t;

  simdutf_really_inline T first() const { return _mm_extract_epi8(*this, 0); }
  simdutf_really_inline T last() const { return _mm_extract_epi8(*this, 15); }
  simdutf_really_inline base8() : base<simd8<T>>() {}
  simdutf_really_inline base8(const __m128i _value) : base<simd8<T>>(_value) {}

  friend simdutf_really_inline Mask operator==(const simd8<T> lhs,
                                               const simd8<T> rhs) {
    return _mm_cmpeq_epi8(lhs, rhs);
  }

  static const int SIZE = sizeof(base<simd8<T>>::value);

  template <int N = 1>
  simdutf_really_inline simd8<T> prev(const simd8<T> prev_chunk) const {
    return _mm_alignr_epi8(*this, prev_chunk, 16 - N);
  }
};

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd8<bool> : base8<bool> {
  static simdutf_really_inline simd8<bool> splat(bool _value) {
    return _mm_set1_epi8(uint8_t(-(!!_value)));
  }

  simdutf_really_inline simd8() : base8() {}
  simdutf_really_inline simd8(const __m128i _value) : base8<bool>(_value) {}
  // Splat constructor
  simdutf_really_inline simd8(bool _value) : base8<bool>(splat(_value)) {}

  simdutf_really_inline int to_bitmask() const {
    return _mm_movemask_epi8(*this);
  }
  simdutf_really_inline simd8<bool> operator~() const { return *this ^ true; }
};

template <typename T> struct base8_numeric : base8<T> {
  static simdutf_really_inline simd8<T> splat(T _value) {
    return _mm_set1_epi8(_value);
  }
  static simdutf_really_inline simd8<T> zero() { return _mm_setzero_si128(); }
  static simdutf_really_inline simd8<T> load(const T values[16]) {
    return _mm_loadu_si128(reinterpret_cast<const __m128i *>(values));
  }
  // Repeat 16 values as many times as necessary (usually for lookup tables)
  static simdutf_really_inline simd8<T> repeat_16(T v0, T v1, T v2, T v3, T v4,
                                                  T v5, T v6, T v7, T v8, T v9,
                                                  T v10, T v11, T v12, T v13,
                                                  T v14, T v15) {
    return simd8<T>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13,
                    v14, v15);
  }

  simdutf_really_inline base8_numeric() : base8<T>() {}
  simdutf_really_inline base8_numeric(const __m128i _value)
      : base8<T>(_value) {}

  // Store to array
  simdutf_really_inline void store(T dst[16]) const {
    return _mm_storeu_si128(reinterpret_cast<__m128i *>(dst), *this);
  }

  // Override to distinguish from bool version
  simdutf_really_inline simd8<T> operator~() const { return *this ^ 0xFFu; }

  // Addition/subtraction are the same for signed and unsigned
  simdutf_really_inline simd8<T> operator-(const simd8<T> other) const {
    return _mm_sub_epi8(*this, other);
  }
  simdutf_really_inline simd8<T> &operator-=(const simd8<T> other) {
    *this = *this - other;
    return *static_cast<simd8<T> *>(this);
  }

  // Perform a lookup assuming the value is between 0 and 16 (undefined behavior
  // for out of range values)
  template <typename L>
  simdutf_really_inline simd8<L> lookup_16(simd8<L> lookup_table) const {
    return _mm_shuffle_epi8(lookup_table, *this);
  }

  template <typename L>
  simdutf_really_inline simd8<L>
  lookup_16(L replace0, L replace1, L replace2, L replace3, L replace4,
            L replace5, L replace6, L replace7, L replace8, L replace9,
            L replace10, L replace11, L replace12, L replace13, L replace14,
            L replace15) const {
    return lookup_16(simd8<L>::repeat_16(
        replace0, replace1, replace2, replace3, replace4, replace5, replace6,
        replace7, replace8, replace9, replace10, replace11, replace12,
        replace13, replace14, replace15));
  }
};

// Signed bytes
template <> struct simd8<int8_t> : base8_numeric<int8_t> {
  simdutf_really_inline simd8() : base8_numeric<int8_t>() {}
  simdutf_really_inline simd8(const __m128i _value)
      : base8_numeric<int8_t>(_value) {}
  // Splat constructor
  simdutf_really_inline simd8(int8_t _value) : simd8(splat(_value)) {}
  // Member-by-member initialization
  simdutf_really_inline operator simd8<uint8_t>() const;
  simdutf_really_inline bool is_ascii() const {
    return _mm_movemask_epi8(*this) == 0;
  }

  // Order-sensitive comparisons
  simdutf_really_inline simd8<bool> operator>(const simd8<int8_t> other) const {
    return _mm_cmpgt_epi8(*this, other);
  }
  simdutf_really_inline simd8<bool> operator<(const simd8<int8_t> other) const {
    return _mm_cmpgt_epi8(other, *this);
  }
};

// Unsigned bytes
template <> struct simd8<uint8_t> : base8_numeric<uint8_t> {
  simdutf_really_inline simd8() : base8_numeric<uint8_t>() {}
  simdutf_really_inline simd8(const __m128i _value)
      : base8_numeric<uint8_t>(_value) {}

  // Splat constructor
  simdutf_really_inline simd8(uint8_t _value) : simd8(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd8(const uint8_t *values) : simd8(load(values)) {}
  // Member-by-member initialization
  simdutf_really_inline
  simd8(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4, uint8_t v5,
        uint8_t v6, uint8_t v7, uint8_t v8, uint8_t v9, uint8_t v10,
        uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15)
      : simd8(_mm_setr_epi8(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11,
                            v12, v13, v14, v15)) {}

  // Saturated math
  simdutf_really_inline simd8<uint8_t>
  saturating_sub(const simd8<uint8_t> other) const {
    return _mm_subs_epu8(*this, other);
  }

  // Order-specific operations
  simdutf_really_inline simd8<uint8_t>
  min_val(const simd8<uint8_t> other) const {
    return _mm_min_epu8(*this, other);
  }
  // Same as >, but only guarantees true is nonzero (< guarantees true = -1)
  simdutf_really_inline simd8<uint8_t>
  gt_bits(const simd8<uint8_t> other) const {
    return this->saturating_sub(other);
  }
  // Same as <, but only guarantees true is nonzero (< guarantees true = -1)
  simdutf_really_inline simd8<bool>
  operator>=(const simd8<uint8_t> other) const {
    return other.min_val(*this) == other;
  }

  // Bit-specific operations
  simdutf_really_inline simd8<bool> bits_not_set() const {
    return *this == uint8_t(0);
  }
  simdutf_really_inline simd8<bool> any_bits_set() const {
    return ~this->bits_not_set();
  }
  simdutf_really_inline bool is_ascii() const {
    return _mm_movemask_epi8(*this) == 0;
  }

  simdutf_really_inline bool bits_not_set_anywhere() const {
    return _mm_testz_si128(*this, *this);
  }
  simdutf_really_inline bool any_bits_set_anywhere() const {
    return !bits_not_set_anywhere();
  }
  template <int N> simdutf_really_inline simd8<uint8_t> shr() const {
    return simd8<uint8_t>(_mm_srli_epi16(*this, N)) & uint8_t(0xFFu >> N);
  }
  template <int N> simdutf_really_inline simd8<uint8_t> shl() const {
    return simd8<uint8_t>(_mm_slli_epi16(*this, N)) & uint8_t(0xFFu << N);
  }

  simdutf_really_inline uint64_t sum_bytes() const {
    const auto tmp = _mm_sad_epu8(value, _mm_setzero_si128());
    return _mm_extract_epi64(tmp, 0) + _mm_extract_epi64(tmp, 1);
  }
};

simdutf_really_inline simd8<int8_t>::operator simd8<uint8_t>() const {
  return this->value;
}

template <typename T> struct simd8x64 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd8<T>);
  static_assert(NUM_CHUNKS == 4,
                "Westmere kernel should use four registers per 64-byte block.");
  simd8<T> chunks[NUM_CHUNKS];

  simd8x64(const simd8x64<T> &o) = delete; // no copy allowed
  simd8x64<T> &
  operator=(const simd8<T> other) = delete; // no assignment allowed
  simd8x64() = delete;                      // no default constructor allowed

  simdutf_really_inline simd8x64(const simd8<T> chunk0, const simd8<T> chunk1,
                                 const simd8<T> chunk2, const simd8<T> chunk3)
      : chunks{chunk0, chunk1, chunk2, chunk3} {}
  simdutf_really_inline simd8x64(const T *ptr)
      : chunks{simd8<T>::load(ptr),
               simd8<T>::load(ptr + sizeof(simd8<T>) / sizeof(T)),
               simd8<T>::load(ptr + 2 * sizeof(simd8<T>) / sizeof(T)),
               simd8<T>::load(ptr + 3 * sizeof(simd8<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + sizeof(simd8<T>) * 0 / sizeof(T));
    this->chunks[1].store(ptr + sizeof(simd8<T>) * 1 / sizeof(T));
    this->chunks[2].store(ptr + sizeof(simd8<T>) * 2 / sizeof(T));
    this->chunks[3].store(ptr + sizeof(simd8<T>) * 3 / sizeof(T));
  }

  simdutf_really_inline simd8x64<T> &operator|=(const simd8x64<T> &other) {
    this->chunks[0] |= other.chunks[0];
    this->chunks[1] |= other.chunks[1];
    this->chunks[2] |= other.chunks[2];
    this->chunks[3] |= other.chunks[3];
    return *this;
  }

  simdutf_really_inline simd8<T> reduce_or() const {
    return (this->chunks[0] | this->chunks[1]) |
           (this->chunks[2] | this->chunks[3]);
  }

  simdutf_really_inline bool is_ascii() const {
    return this->reduce_or().is_ascii();
  }

  template <endianness endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    this->chunks[0].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 0);
    this->chunks[1].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 1);
    this->chunks[2].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 2);
    this->chunks[3].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 3);
  }

  simdutf_really_inline void store_ascii_as_utf32(char32_t *ptr) const {
    this->chunks[0].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 0);
    this->chunks[1].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 1);
    this->chunks[2].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 2);
    this->chunks[3].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 3);
  }

  simdutf_really_inline uint64_t to_bitmask() const {
    uint64_t r0 = uint32_t(this->chunks[0].to_bitmask());
    uint64_t r1 = this->chunks[1].to_bitmask();
    uint64_t r2 = this->chunks[2].to_bitmask();
    uint64_t r3 = this->chunks[3].to_bitmask();
    return r0 | (r1 << 16) | (r2 << 32) | (r3 << 48);
  }

  simdutf_really_inline uint64_t lt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] < mask, this->chunks[1] < mask,
                          this->chunks[2] < mask, this->chunks[3] < mask)
        .to_bitmask();
  }

  simdutf_really_inline uint64_t gt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] > mask, this->chunks[1] > mask,
                          this->chunks[2] > mask, this->chunks[3] > mask)
        .to_bitmask();
  }

  simdutf_really_inline uint64_t eq(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] == mask, this->chunks[1] == mask,
                          this->chunks[2] == mask, this->chunks[3] == mask)
        .to_bitmask();
  }

  simdutf_really_inline uint64_t gteq_unsigned(const uint8_t m) const {
    const simd8<uint8_t> mask = simd8<uint8_t>::splat(m);
    return simd8x64<bool>(simd8<uint8_t>(__m128i(this->chunks[0])) >= mask,
                          simd8<uint8_t>(__m128i(this->chunks[1])) >= mask,
                          simd8<uint8_t>(__m128i(this->chunks[2])) >= mask,
                          simd8<uint8_t>(__m128i(this->chunks[3])) >= mask)
        .to_bitmask();
  }
}; // struct simd8x64<T>

/* begin file src\simdutf\westmere\simd16-inl.h */
template <typename T> struct simd16;

template <typename T, typename Mask = simd16<bool>>
struct base16 : base<simd16<T>> {
  simdutf_really_inline base16() : base<simd16<T>>() {}

  simdutf_really_inline base16(const __m128i _value)
      : base<simd16<T>>(_value) {}

  friend simdutf_really_inline Mask operator==(const simd16<T> lhs,
                                               const simd16<T> rhs) {
    return _mm_cmpeq_epi16(lhs, rhs);
  }

  /// the size of vector in bytes
  static const int SIZE = sizeof(base<simd16<T>>::value);

  /// the number of elements of type T a vector can hold
  static const int ELEMENTS = SIZE / sizeof(T);
};

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd16<bool> : base16<bool> {
  static simdutf_really_inline simd16<bool> splat(bool _value) {
    return _mm_set1_epi16(uint16_t(-(!!_value)));
  }

  simdutf_really_inline simd16(const __m128i _value) : base16<bool>(_value) {}

  // Splat constructor
  simdutf_really_inline simd16(bool _value) : base16<bool>(splat(_value)) {}

  simdutf_really_inline int to_bitmask() const {
    return _mm_movemask_epi8(*this);
  }

  simdutf_really_inline simd16<bool> operator~() const { return *this ^ true; }
};

template <typename T> struct base16_numeric : base16<T> {
  static simdutf_really_inline simd16<T> splat(T _value) {
    return _mm_set1_epi16(_value);
  }

  static simdutf_really_inline simd16<T> zero() { return _mm_setzero_si128(); }

  static simdutf_really_inline simd16<T> load(const T values[8]) {
    return _mm_loadu_si128(reinterpret_cast<const __m128i *>(values));
  }

  simdutf_really_inline base16_numeric() : base16<T>() {}

  simdutf_really_inline base16_numeric(const __m128i _value)
      : base16<T>(_value) {}

  // Store to array
  simdutf_really_inline void store(T dst[8]) const {
    return _mm_storeu_si128(reinterpret_cast<__m128i *>(dst), *this);
  }

  // Override to distinguish from bool version
  simdutf_really_inline simd16<T> operator~() const { return *this ^ 0xFFu; }

  // Addition/subtraction are the same for signed and unsigned
  simdutf_really_inline simd16<T> operator+(const simd16<T> other) const {
    return _mm_add_epi16(*this, other);
  }
  simdutf_really_inline simd16<T> &operator+=(const simd16<T> other) {
    *this = *this + other;
    return *static_cast<simd16<T> *>(this);
  }
};

// Unsigned code units
template <> struct simd16<uint16_t> : base16_numeric<uint16_t> {
  simdutf_really_inline simd16() : base16_numeric<uint16_t>() {}

  simdutf_really_inline simd16(const __m128i _value)
      : base16_numeric<uint16_t>(_value) {}

  // Splat constructor
  simdutf_really_inline simd16(uint16_t _value) : simd16(splat(_value)) {}

  // Array constructor
  simdutf_really_inline simd16(const char16_t *values)
      : simd16(load(reinterpret_cast<const uint16_t *>(values))) {}

  // Order-specific operations
  simdutf_really_inline simd16<uint16_t>
  max_val(const simd16<uint16_t> other) const {
    return _mm_max_epu16(*this, other);
  }

  simdutf_really_inline simd16<uint16_t>
  min_val(const simd16<uint16_t> other) const {
    return _mm_min_epu16(*this, other);
  }

  simdutf_really_inline simd16<bool>
  operator<=(const simd16<uint16_t> other) const {
    return other.max_val(*this) == other;
  }
  simdutf_really_inline simd16<bool>
  operator>=(const simd16<uint16_t> other) const {
    return other.min_val(*this) == other;
  }

  // Bit-specific operations
  simdutf_really_inline simd16<bool> bits_not_set() const {
    return *this == uint16_t(0);
  }

  simdutf_really_inline simd16<bool> any_bits_set() const {
    return ~this->bits_not_set();
  }

  template <int N> simdutf_really_inline simd16<uint16_t> shr() const {
    return simd16<uint16_t>(_mm_srli_epi16(*this, N));
  }

  // Change the endianness
  simdutf_really_inline simd16<uint16_t> swap_bytes() const {
    const __m128i swap =
        _mm_setr_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
    return _mm_shuffle_epi8(*this, swap);
  }

  // Pack with the unsigned saturation of two uint16_t code units into single
  // uint8_t vector
  static simdutf_really_inline simd8<uint8_t> pack(const simd16<uint16_t> &v0,
                                                   const simd16<uint16_t> &v1) {
    return _mm_packus_epi16(v0, v1);
  }

  simdutf_really_inline uint64_t sum() const {
    const auto lo_u16 = _mm_and_si128(value, _mm_set1_epi32(0x0000ffff));
    const auto hi_u16 = _mm_srli_epi32(value, 16);
    const auto sum_u32 = _mm_add_epi32(lo_u16, hi_u16);

    const auto lo_u32 = _mm_and_si128(sum_u32, _mm_set1_epi64x(0xffffffff));
    const auto hi_u32 = _mm_srli_epi64(sum_u32, 32);
    const auto sum_u64 = _mm_add_epi64(lo_u32, hi_u32);

    return uint64_t(_mm_extract_epi64(sum_u64, 0)) +
           uint64_t(_mm_extract_epi64(sum_u64, 1));
  }
};

template <typename T> struct simd16x32 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd16<T>);
  static_assert(NUM_CHUNKS == 4,
                "Westmere kernel should use four registers per 64-byte block.");
  simd16<T> chunks[NUM_CHUNKS];

  simd16x32(const simd16x32<T> &o) = delete; // no copy allowed
  simd16x32<T> &
  operator=(const simd16<T> other) = delete; // no assignment allowed
  simd16x32() = delete;                      // no default constructor allowed

  simdutf_really_inline
  simd16x32(const simd16<T> chunk0, const simd16<T> chunk1,
            const simd16<T> chunk2, const simd16<T> chunk3)
      : chunks{chunk0, chunk1, chunk2, chunk3} {}
  simdutf_really_inline simd16x32(const T *ptr)
      : chunks{simd16<T>::load(ptr),
               simd16<T>::load(ptr + sizeof(simd16<T>) / sizeof(T)),
               simd16<T>::load(ptr + 2 * sizeof(simd16<T>) / sizeof(T)),
               simd16<T>::load(ptr + 3 * sizeof(simd16<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + sizeof(simd16<T>) * 0 / sizeof(T));
    this->chunks[1].store(ptr + sizeof(simd16<T>) * 1 / sizeof(T));
    this->chunks[2].store(ptr + sizeof(simd16<T>) * 2 / sizeof(T));
    this->chunks[3].store(ptr + sizeof(simd16<T>) * 3 / sizeof(T));
  }

  simdutf_really_inline simd16<T> reduce_or() const {
    return (this->chunks[0] | this->chunks[1]) |
           (this->chunks[2] | this->chunks[3]);
  }

  simdutf_really_inline bool is_ascii() const {
    return this->reduce_or().is_ascii();
  }

  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    this->chunks[0].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 0);
    this->chunks[1].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 1);
    this->chunks[2].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 2);
    this->chunks[3].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 3);
  }

  simdutf_really_inline uint64_t to_bitmask() const {
    uint64_t r0 = uint32_t(this->chunks[0].to_bitmask());
    uint64_t r1 = this->chunks[1].to_bitmask();
    uint64_t r2 = this->chunks[2].to_bitmask();
    uint64_t r3 = this->chunks[3].to_bitmask();
    return r0 | (r1 << 16) | (r2 << 32) | (r3 << 48);
  }

  simdutf_really_inline void swap_bytes() {
    this->chunks[0] = this->chunks[0].swap_bytes();
    this->chunks[1] = this->chunks[1].swap_bytes();
    this->chunks[2] = this->chunks[2].swap_bytes();
    this->chunks[3] = this->chunks[3].swap_bytes();
  }

  simdutf_really_inline uint64_t lteq(const T m) const {
    const simd16<T> mask = simd16<T>::splat(m);
    return simd16x32<bool>(this->chunks[0] <= mask, this->chunks[1] <= mask,
                           this->chunks[2] <= mask, this->chunks[3] <= mask)
        .to_bitmask();
  }

  simdutf_really_inline uint64_t eq(const T m) const {
    const simd16<T> mask = simd16<T>::splat(m);
    return simd16x32<bool>(this->chunks[0] == mask, this->chunks[1] == mask,
                           this->chunks[2] == mask, this->chunks[3] == mask)
        .to_bitmask();
  }

  simdutf_really_inline uint64_t not_in_range(const T low, const T high) const {
    const simd16<T> mask_low = simd16<T>::splat(static_cast<T>(low - 1));
    const simd16<T> mask_high = simd16<T>::splat(static_cast<T>(high + 1));
    return simd16x32<bool>(
               (this->chunks[0] >= mask_high) | (this->chunks[0] <= mask_low),
               (this->chunks[1] >= mask_high) | (this->chunks[1] <= mask_low),
               (this->chunks[2] >= mask_high) | (this->chunks[2] <= mask_low),
               (this->chunks[3] >= mask_high) | (this->chunks[3] <= mask_low))
        .to_bitmask();
  }
}; // struct simd16x32<T>

simd16<uint16_t> min(const simd16<uint16_t> a, simd16<uint16_t> b) {
  return _mm_min_epu16(a.value, b.value);
}
/* end file src\simdutf\westmere\simd16-inl.h */
/* begin file src\simdutf\westmere\simd32-inl.h */
template <typename T> struct simd32;

template <> struct simd32<uint32_t> {
  static const size_t SIZE = sizeof(__m128i);
  static const size_t ELEMENTS = SIZE / sizeof(uint32_t);

  __m128i value;

  simdutf_really_inline simd32(const __m128i v) : value(v) {}

  template <typename Pointer>
  simdutf_really_inline simd32(const Pointer *ptr)
      : value(_mm_loadu_si128(reinterpret_cast<const __m128i *>(ptr))) {}

  simdutf_really_inline uint64_t sum() const {
    return uint64_t(_mm_extract_epi32(value, 0)) +
           uint64_t(_mm_extract_epi32(value, 1)) +
           uint64_t(_mm_extract_epi32(value, 2)) +
           uint64_t(_mm_extract_epi32(value, 3));
  }

  simdutf_really_inline simd32<uint32_t> swap_bytes() const {
    const __m128i shuffle =
        _mm_setr_epi8(3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11, 15, 14, 13, 12);

    return _mm_shuffle_epi8(value, shuffle);
  }

  template <int N> simdutf_really_inline simd32<uint32_t> shr() const {
    return _mm_srli_epi32(value, N);
  }

  template <int N> simdutf_really_inline simd32<uint32_t> shl() const {
    return _mm_slli_epi32(value, N);
  }
  void dump() const {
#ifdef SIMDUTF_LOGGING
    printf("[%08x, %08x, %08x, %08x]\n", uint32_t(_mm_extract_epi32(value, 0)),
           uint32_t(_mm_extract_epi32(value, 1)),
           uint32_t(_mm_extract_epi32(value, 2)),
           uint32_t(_mm_extract_epi32(value, 3)));
#endif // SIMDUTF_LOGGING
  }

  // operators
  simdutf_really_inline simd32 &operator+=(const simd32 other) {
    value = _mm_add_epi32(value, other.value);
    return *this;
  }

  // static members
  simdutf_really_inline static simd32<uint32_t> zero() {
    return _mm_setzero_si128();
  }

  simdutf_really_inline static simd32<uint32_t> splat(uint32_t v) {
    return _mm_set1_epi32(v);
  }
};

//----------------------------------------------------------------------

template <> struct simd32<bool> {
  // static const size_t SIZE = sizeof(__m128i);
  // static const size_t ELEMENTS = SIZE / sizeof(uint32_t);

  __m128i value;

  simdutf_really_inline simd32(const __m128i v) : value(v) {}

  simdutf_really_inline bool any() const {
    return _mm_movemask_epi8(value) != 0;
  }

  simdutf_really_inline uint8_t to_4bit_bitmask() const {
    return uint8_t(_mm_movemask_ps(_mm_castsi128_ps(value)));
  }
};

//----------------------------------------------------------------------

template <typename T>
simdutf_really_inline simd32<T> operator|(const simd32<T> a,
                                          const simd32<T> b) {
  return _mm_or_si128(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> min(const simd32<uint32_t> a,
                                           const simd32<uint32_t> b) {
  return _mm_min_epu32(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> max(const simd32<uint32_t> a,
                                           const simd32<uint32_t> b) {
  return _mm_max_epu32(a.value, b.value);
}

simdutf_really_inline simd32<bool> operator==(const simd32<uint32_t> a,
                                              uint32_t b) {
  return _mm_cmpeq_epi32(a.value, _mm_set1_epi32(b));
}

simdutf_really_inline simd32<uint32_t> operator&(const simd32<uint32_t> a,
                                                 const simd32<uint32_t> b) {
  return _mm_and_si128(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> operator&(const simd32<uint32_t> a,
                                                 uint32_t b) {
  return _mm_and_si128(a.value, _mm_set1_epi32(b));
}

simdutf_really_inline simd32<uint32_t> operator|(const simd32<uint32_t> a,
                                                 uint32_t b) {
  return _mm_or_si128(a.value, _mm_set1_epi32(b));
}

simdutf_really_inline simd32<uint32_t> operator+(const simd32<uint32_t> a,
                                                 const simd32<uint32_t> b) {
  return _mm_add_epi32(a.value, b.value);
}

simdutf_really_inline simd32<uint32_t> operator-(const simd32<uint32_t> a,
                                                 uint32_t b) {
  return _mm_sub_epi32(a.value, _mm_set1_epi32(b));
}

simdutf_really_inline simd32<bool> operator==(const simd32<uint32_t> a,
                                              const simd32<uint32_t> b) {
  return _mm_cmpeq_epi32(a.value, b.value);
}

simdutf_really_inline simd32<bool> operator>=(const simd32<uint32_t> a,
                                              const simd32<uint32_t> b) {
  return _mm_cmpeq_epi32(_mm_max_epu32(a.value, b.value), a.value);
}

simdutf_really_inline simd32<bool> operator!(const simd32<bool> v) {
  return _mm_xor_si128(v.value, _mm_set1_epi8(-1));
}

simdutf_really_inline simd32<bool> operator>(const simd32<uint32_t> a,
                                             const simd32<uint32_t> b) {
  return !(b >= a);
}

simdutf_really_inline simd32<uint32_t> select(const simd32<bool> cond,
                                              const simd32<uint32_t> v_true,
                                              const simd32<uint32_t> v_false) {
  return _mm_blendv_epi8(v_false.value, v_true.value, cond.value);
}
/* end file src\simdutf\westmere\simd32-inl.h */
/* begin file src\simdutf\westmere\simd64-inl.h */
template <typename T> struct simd64;

template <> struct simd64<uint64_t> {
  // static const size_t SIZE = sizeof(__m128i);
  // static const size_t ELEMENTS = SIZE / sizeof(uint64_t);

  __m128i value;

  simdutf_really_inline simd64(const __m128i v) : value(v) {}

  template <typename Pointer>
  simdutf_really_inline simd64(const Pointer *ptr)
      : value(_mm_loadu_si128(reinterpret_cast<const __m128i *>(ptr))) {}

  simdutf_really_inline uint64_t sum() const {
    return _mm_extract_epi64(value, 0) + _mm_extract_epi64(value, 1);
  }

  // operators
  simdutf_really_inline simd64 &operator+=(const simd64 other) {
    value = _mm_add_epi64(value, other.value);
    return *this;
  }

  // static members
  simdutf_really_inline static simd64<uint64_t> zero() {
    return _mm_setzero_si128();
  }

  simdutf_really_inline static simd64<uint64_t> splat(uint64_t v) {
    return _mm_set1_epi64x(v);
  }
};
/* end file src\simdutf\westmere\simd64-inl.h */

simdutf_really_inline simd64<uint64_t> sum_8bytes(const simd8<uint8_t> v) {
  return _mm_sad_epu8(v.value, simd8<uint8_t>::zero());
}

simdutf_really_inline simd8<uint8_t> as_vector_u8(const simd32<uint32_t> v) {
  return simd8<uint8_t>(v.value);
}

} // namespace simd
} // unnamed namespace
} // namespace westmere
} // namespace simdutf

#endif // SIMDUTF_WESTMERE_SIMD_INPUT_H
/* end file src\simdutf\westmere\simd.h */

/* begin file src\simdutf\westmere\end.h */
#if SIMDUTF_CAN_ALWAYS_RUN_WESTMERE
// nothing needed.
#else
SIMDUTF_UNTARGET_REGION
#endif

#undef SIMDUTF_SIMD_HAS_BYTEMASK
/* end file src\simdutf\westmere\end.h */

#endif // SIMDUTF_IMPLEMENTATION_WESTMERE
#endif // SIMDUTF_WESTMERE_COMMON_H
/* end file src\simdutf\westmere.h */
/* begin file src\simdutf\ppc64.h */
#ifndef SIMDUTF_PPC64_H
#define SIMDUTF_PPC64_H

#ifdef SIMDUTF_FALLBACK_H
  #error "ppc64.h must be included before fallback.h"
#endif


#ifndef SIMDUTF_IMPLEMENTATION_PPC64
  #define SIMDUTF_IMPLEMENTATION_PPC64 (SIMDUTF_IS_PPC64)
#endif
#define SIMDUTF_CAN_ALWAYS_RUN_PPC64                                           \
  SIMDUTF_IMPLEMENTATION_PPC64 &&SIMDUTF_IS_PPC64


#if SIMDUTF_IMPLEMENTATION_PPC64

namespace simdutf {
/**
 * Implementation for ALTIVEC (PPC64).
 */
namespace ppc64 {} // namespace ppc64
} // namespace simdutf

/* begin file src\simdutf\ppc64\implementation.h */
#ifndef SIMDUTF_PPC64_IMPLEMENTATION_H
#define SIMDUTF_PPC64_IMPLEMENTATION_H


namespace simdutf {
namespace ppc64 {

namespace {
using namespace simdutf;

template <size_t N> simdutf_really_inline size_t align_down(size_t size) {
  return N * (size / N);
}
} // namespace

class implementation final : public simdutf::implementation {
public:
  simdutf_really_inline implementation()
      : simdutf::implementation("ppc64", "PPC64 ALTIVEC",
                                internal::instruction_set::ALTIVEC) {}

  simdutf_warn_unused size_t maximal_binary_length_from_base64(
      const char *input, size_t length) const noexcept;
  simdutf_warn_unused result base64_to_binary(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused result
  base64_to_binary(const char16_t *input, size_t length, char *output,
                   base64_options options,
                   last_chunk_handling_options last_chunk_options =
                       last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char16_t *input, size_t length, char *output,
      base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  size_t binary_to_base64(const char *input, size_t length, char *output,
                          base64_options options) const noexcept;

  const char *find(const char *start, const char *end,
                   char character) const noexcept;

  const char16_t *find(const char16_t *start, const char16_t *end,
                       char16_t character) const noexcept;

#ifdef SIMDUTF_INTERNAL_TESTS
  virtual std::vector<TestProcedure> internal_tests() const override;
#endif
};

} // namespace ppc64
} // namespace simdutf

#endif // SIMDUTF_PPC64_IMPLEMENTATION_H
/* end file src\simdutf\ppc64\implementation.h */

/* begin file src\simdutf\ppc64\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "ppc64"
// #define SIMDUTF_IMPLEMENTATION ppc64
/* end file src\simdutf\ppc64\begin.h */

  // Declarations
/* begin file src\simdutf\ppc64\intrinsics.h */
#ifndef SIMDUTF_PPC64_INTRINSICS_H
#define SIMDUTF_PPC64_INTRINSICS_H


// This should be the correct header whether
// you use visual studio or other compilers.
#include <altivec.h>

// These are defined by altivec.h in GCC toolchain, it is safe to undef them.
#ifdef bool
  #undef bool
#endif

#ifdef vector
  #undef vector
#endif

#endif //  SIMDUTF_PPC64_INTRINSICS_H
/* end file src\simdutf\ppc64\intrinsics.h */
/* begin file src\simdutf\ppc64\bitmanipulation.h */
#ifndef SIMDUTF_PPC64_BITMANIPULATION_H
#define SIMDUTF_PPC64_BITMANIPULATION_H

namespace simdutf {
namespace ppc64 {
namespace {

#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
simdutf_really_inline int count_ones(uint64_t input_num) {
  // note: we do not support legacy 32-bit Windows
  return __popcnt64(input_num); // Visual Studio wants two underscores
}
#else
simdutf_really_inline int count_ones(uint64_t input_num) {
  return __builtin_popcountll(input_num);
}
#endif

#if SIMDUTF_NEED_TRAILING_ZEROES
simdutf_really_inline int trailing_zeroes(uint64_t input_num) {
  return __builtin_ctzll(input_num);
}
#endif

} // unnamed namespace
} // namespace ppc64
} // namespace simdutf

#endif // SIMDUTF_PPC64_BITMANIPULATION_H
/* end file src\simdutf\ppc64\bitmanipulation.h */
/* begin file src\simdutf\ppc64\simd.h */
#ifndef SIMDUTF_PPC64_SIMD_H
#define SIMDUTF_PPC64_SIMD_H

#include <type_traits>

namespace simdutf {
namespace ppc64 {
namespace {
namespace simd {

using vec_bool_t = __vector __bool char;
using vec_bool16_t = __vector __bool short;
using vec_bool32_t = __vector __bool int;
using vec_u8_t = __vector unsigned char;
using vec_i8_t = __vector signed char;
using vec_u16_t = __vector unsigned short;
using vec_i16_t = __vector signed short;
using vec_u32_t = __vector unsigned int;
using vec_i32_t = __vector signed int;
using vec_u64_t = __vector unsigned long long;
using vec_i64_t = __vector signed long long;

// clang-format off
template <typename T> struct vector_u8_type_for_element_aux {
  using type = typename std::conditional<std::is_same<T, bool>::value,    vec_bool_t,
               typename std::conditional<std::is_same<T, uint8_t>::value, vec_u8_t,
               typename std::conditional<std::is_same<T, int8_t>::value,  vec_i8_t, void>::type>::type>::type;

  static_assert(not std::is_same<type, void>::value,
                "accepted element types are 8 bit integers or bool");
};

template <typename T> struct vector_u16_type_for_element_aux {
  using type = typename std::conditional<std::is_same<T, bool>::value,     vec_bool16_t,
               typename std::conditional<std::is_same<T, uint16_t>::value, vec_u16_t,
               typename std::conditional<std::is_same<T, int16_t>::value,  vec_i16_t, void>::type>::type>::type;

  static_assert(not std::is_same<type, void>::value,
                "accepted element types are 16 bit integers or bool");
};

template <typename T> struct vector_u32_type_for_element_aux {
  using type = typename std::conditional<std::is_same<T, bool>::value,     vec_bool32_t,
               typename std::conditional<std::is_same<T, uint32_t>::value, vec_u32_t,
               typename std::conditional<std::is_same<T, int32_t>::value,  vec_i32_t, void>::type>::type>::type;

  static_assert(not std::is_same<type, void>::value,
                "accepted element types are 32 bit integers or bool");
};
// clang-format on

template <typename T>
using vector_u8_type_for_element =
    typename vector_u8_type_for_element_aux<T>::type;

template <typename T>
using vector_u16_type_for_element =
    typename vector_u16_type_for_element_aux<T>::type;

template <typename T>
using vector_u32_type_for_element =
    typename vector_u32_type_for_element_aux<T>::type;

template <typename T> uint16_t move_mask_u8(T vec) {
  const vec_u8_t perm_mask = {15 * 8, 14 * 8, 13 * 8, 12 * 8, 11 * 8, 10 * 8,
                              9 * 8,  8 * 8,  7 * 8,  6 * 8,  5 * 8,  4 * 8,
                              3 * 8,  2 * 8,  1 * 8,  0 * 8};

  const auto result = (vec_u64_t)vec_vbpermq((vec_u8_t)vec, perm_mask);
#if SIMDUTF_IS_BIG_ENDIAN
  return static_cast<uint16_t>(result[0]);
#else
  return static_cast<uint16_t>(result[1]);
#endif
}

/* begin file src\simdutf\ppc64\simd8-inl.h */
// file included directly

template <typename T> struct base8 {
  using vector_type = vector_u8_type_for_element<T>;
  vector_type value;
  static const int SIZE = sizeof(vector_type);
  static const int ELEMENTS = sizeof(vector_type) / sizeof(T);

  // Zero constructor
  simdutf_really_inline base8() : value{vec_splats(T(0))} {}

  // Conversion from SIMD register
  simdutf_really_inline base8(const vector_type _value) : value{_value} {}

  // Splat scalar
  simdutf_really_inline base8(T v) : value{vec_splats(v)} {}

  // Conversion to SIMD register
  simdutf_really_inline operator const vector_type &() const {
    return this->value;
  }

  template <typename U> simdutf_really_inline void store(U *ptr) const {
    vec_xst(value, 0, reinterpret_cast<T *>(ptr));
  }

  template <typename SIMD8> void operator|=(const SIMD8 other) {
    this->value = vec_or(this->value, other.value);
  }

  template <int N = 1> vector_type prev_aux(vector_type prev_chunk) const {
    vector_type chunk = this->value;
#if !SIMDUTF_IS_BIG_ENDIAN
    chunk = (vector_type)vec_reve(this->value);
    prev_chunk = (vector_type)vec_reve((vector_type)prev_chunk);
#endif
    chunk = (vector_type)vec_sld((vector_type)prev_chunk, (vector_type)chunk,
                                 16 - N);
#if !SIMDUTF_IS_BIG_ENDIAN
    chunk = (vector_type)vec_reve((vector_type)chunk);
#endif
    return chunk;
  }

  simdutf_really_inline bool is_ascii() const {
    return move_mask_u8(this->value) == 0;
  }

  simdutf_really_inline uint16_t to_bitmask() const {
    return move_mask_u8(value);
  }

  template <endianness big_endian>
  simdutf_really_inline void store_bytes_as_utf16(char16_t *p) const {
    const vector_type zero = vec_splats(T(0));

    if (big_endian) {
      const vec_u8_t perm_lo = {16, 0, 16, 1, 16, 2, 16, 3,
                                16, 4, 16, 5, 16, 6, 16, 7};
      const vec_u8_t perm_hi = {16, 8,  16, 9,  16, 10, 16, 11,
                                16, 12, 16, 13, 16, 14, 16, 15};

      const vector_type v0 = vec_perm(value, zero, perm_lo);
      const vector_type v1 = vec_perm(value, zero, perm_hi);

#if defined(__clang__)
      vec_xst(v0, 0, reinterpret_cast<T *>(p));
      vec_xst(v1, 16, reinterpret_cast<T *>(p));
#else
      vec_xst(v0, 0, reinterpret_cast<vector_type *>(p));
      vec_xst(v1, 16, reinterpret_cast<vector_type *>(p));
#endif // defined(__clang__)
    } else {
      const vec_u8_t perm_lo = {0, 16, 1, 16, 2, 16, 3, 16,
                                4, 16, 5, 16, 6, 16, 7, 16};
      const vec_u8_t perm_hi = {8,  16, 9,  16, 10, 16, 11, 16,
                                12, 16, 13, 16, 14, 16, 15, 16};

      const vector_type v0 = vec_perm(value, zero, perm_lo);
      const vector_type v1 = vec_perm(value, zero, perm_hi);

#if defined(__clang__)
      vec_xst(v0, 0, reinterpret_cast<T *>(p));
      vec_xst(v1, 16, reinterpret_cast<T *>(p));
#else
      vec_xst(v0, 0, reinterpret_cast<vector_type *>(p));
      vec_xst(v1, 16, reinterpret_cast<vector_type *>(p));
#endif // defined(__clang__)
    }
  }

  template <endianness big_endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *p) const {
    store_bytes_as_utf16<big_endian>(p);
  }

  simdutf_really_inline void store_bytes_as_utf32(char32_t *p) const {
    const vector_type zero = vec_splats(T(0));

#if SIMDUTF_IS_BIG_ENDIAN
    const vec_u8_t perm0 = {16, 16, 16, 0, 16, 16, 16, 1,
                            16, 16, 16, 2, 16, 16, 16, 3};

    const vec_u8_t perm1 = {16, 16, 16, 4, 16, 16, 16, 5,
                            16, 16, 16, 6, 16, 16, 16, 7};

    const vec_u8_t perm2 = {16, 16, 16, 8,  16, 16, 16, 9,
                            16, 16, 16, 10, 16, 16, 16, 11};

    const vec_u8_t perm3 = {16, 16, 16, 12, 16, 16, 16, 13,
                            16, 16, 16, 14, 16, 16, 16, 15};
#else
    const vec_u8_t perm0 = {0, 16, 16, 16, 1, 16, 16, 16,
                            2, 16, 16, 16, 3, 16, 16, 16};

    const vec_u8_t perm1 = {4, 16, 16, 16, 5, 16, 16, 16,
                            6, 16, 16, 16, 7, 16, 16, 16};

    const vec_u8_t perm2 = {8,  16, 16, 16, 9,  16, 16, 16,
                            10, 16, 16, 16, 11, 16, 16, 16};

    const vec_u8_t perm3 = {12, 16, 16, 16, 13, 16, 16, 16,
                            14, 16, 16, 16, 15, 16, 16, 16};
#endif // SIMDUTF_IS_BIG_ENDIAN

    const vector_type v0 = vec_perm(value, zero, perm0);
    const vector_type v1 = vec_perm(value, zero, perm1);
    const vector_type v2 = vec_perm(value, zero, perm2);
    const vector_type v3 = vec_perm(value, zero, perm3);

    constexpr size_t n = base8<T>::SIZE;

#if defined(__clang__)
    vec_xst(v0, 0 * n, reinterpret_cast<T *>(p));
    vec_xst(v1, 1 * n, reinterpret_cast<T *>(p));
    vec_xst(v2, 2 * n, reinterpret_cast<T *>(p));
    vec_xst(v3, 3 * n, reinterpret_cast<T *>(p));
#else
    vec_xst(v0, 0 * n, reinterpret_cast<vector_type *>(p));
    vec_xst(v1, 1 * n, reinterpret_cast<vector_type *>(p));
    vec_xst(v2, 2 * n, reinterpret_cast<vector_type *>(p));
    vec_xst(v3, 3 * n, reinterpret_cast<vector_type *>(p));
#endif // defined(__clang__)
  }

  simdutf_really_inline void store_words_as_utf32(char32_t *p) const {
    const vector_type zero = vec_splats(T(0));

#if SIMDUTF_IS_BIG_ENDIAN
    const vec_u8_t perm0 = {16, 16, 0, 1, 16, 16, 2, 3,
                            16, 16, 4, 5, 16, 16, 6, 7};
    const vec_u8_t perm1 = {16, 16, 8,  9,  16, 16, 10, 11,
                            16, 16, 12, 13, 16, 16, 14, 15};
#else
    const vec_u8_t perm0 = {0, 1, 16, 16, 2, 3, 16, 16,
                            4, 5, 16, 16, 6, 7, 16, 16};
    const vec_u8_t perm1 = {8,  9,  16, 16, 10, 11, 16, 16,
                            12, 13, 16, 16, 14, 15, 16, 16};
#endif // SIMDUTF_IS_BIG_ENDIAN

    const vector_type v0 = vec_perm(value, zero, perm0);
    const vector_type v1 = vec_perm(value, zero, perm1);

    constexpr size_t n = base8<T>::SIZE;

#if defined(__clang__)
    vec_xst(v0, 0 * n, reinterpret_cast<T *>(p));
    vec_xst(v1, 1 * n, reinterpret_cast<T *>(p));
#else
    vec_xst(v0, 0 * n, reinterpret_cast<vector_type *>(p));
    vec_xst(v1, 1 * n, reinterpret_cast<vector_type *>(p));
#endif // defined(__clang__)
  }

  simdutf_really_inline void store_ascii_as_utf32(char32_t *p) const {
    store_bytes_as_utf32(p);
  }
};

// Forward declaration
template <typename T> struct simd8;

template <typename T>
simd8<bool> operator==(const simd8<T> a, const simd8<T> b);

template <typename T>
simd8<bool> operator!=(const simd8<T> a, const simd8<T> b);

template <typename T> simd8<T> operator&(const simd8<T> a, const simd8<T> b);

template <typename T> simd8<T> operator|(const simd8<T> a, const simd8<T> b);

template <typename T> simd8<T> operator^(const simd8<T> a, const simd8<T> b);

template <typename T> simd8<T> operator+(const simd8<T> a, const simd8<T> b);

template <typename T> simd8<bool> operator<(const simd8<T> a, const simd8<T> b);

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd8<bool> : base8<bool> {
  using super = base8<bool>;

  static simdutf_really_inline simd8<bool> splat(bool _value) {
    return (vector_type)vec_splats((unsigned char)(-(!!_value)));
  }

  simdutf_really_inline simd8() : super(vector_type()) {}
  simdutf_really_inline simd8(const vector_type _value) : super(_value) {}
  // Splat constructor
  simdutf_really_inline simd8(bool _value) : base8<bool>(splat(_value)) {}

  template <typename T>
  simdutf_really_inline simd8(simd8<T> other)
      : simd8(vector_type(other.value)) {}

  simdutf_really_inline uint16_t to_bitmask() const {
    return move_mask_u8(value);
  }

  simdutf_really_inline bool any() const {
    return !vec_all_eq(this->value, (vector_type)vec_splats(0));
  }

  simdutf_really_inline bool all() const { return to_bitmask() == 0xffff; }

  simdutf_really_inline simd8<bool> operator~() const {
    return this->value ^ (vector_type)splat(true);
  }
};

template <typename T> struct base8_numeric : base8<T> {
  using super = base8<T>;
  using vector_type = typename super::vector_type;

  static simdutf_really_inline simd8<T> splat(T value) {
    return (vector_type)vec_splats(value);
  }

  static simdutf_really_inline simd8<T> zero() { return splat(0); }

  template <typename U>
  static simdutf_really_inline simd8<T> load(const U *values) {
    return vec_xl(0, reinterpret_cast<const T *>(values));
  }

  // Repeat 16 values as many times as necessary (usually for lookup tables)
  static simdutf_really_inline simd8<T> repeat_16(T v0, T v1, T v2, T v3, T v4,
                                                  T v5, T v6, T v7, T v8, T v9,
                                                  T v10, T v11, T v12, T v13,
                                                  T v14, T v15) {
    return simd8<T>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13,
                    v14, v15);
  }

  simdutf_really_inline base8_numeric() : base8<T>() {}
  simdutf_really_inline base8_numeric(const vector_type _value)
      : base8<T>(_value) {}

  // Override to distinguish from bool version
  simdutf_really_inline simd8<T> operator~() const { return *this ^ 0xFFu; }

  simdutf_really_inline simd8<T> &operator-=(const simd8<T> other) {
    this->value = vec_sub(this->value, other.value);
    return *static_cast<simd8<T> *>(this);
  }

  // Perform a lookup assuming the value is between 0 and 16 (undefined behavior
  // for out of range values)
  template <typename L>
  simdutf_really_inline simd8<L> lookup_16(simd8<L> lookup_table) const {
    return (vector_type)vec_perm((vector_type)lookup_table,
                                 (vector_type)lookup_table, this->value);
  }

  template <typename L>
  simdutf_really_inline simd8<L>
  lookup_32(const simd8<L> lookup_table_lo,
            const simd8<L> lookup_table_hi) const {
    return (vector_type)vec_perm(lookup_table_lo.value, lookup_table_hi.value,
                                 this->value);
  }

  template <typename L>
  simdutf_really_inline simd8<L>
  lookup_16(L replace0, L replace1, L replace2, L replace3, L replace4,
            L replace5, L replace6, L replace7, L replace8, L replace9,
            L replace10, L replace11, L replace12, L replace13, L replace14,
            L replace15) const {
    return lookup_16(simd8<L>::repeat_16(
        replace0, replace1, replace2, replace3, replace4, replace5, replace6,
        replace7, replace8, replace9, replace10, replace11, replace12,
        replace13, replace14, replace15));
  }
};

// Unsigned bytes
template <> struct simd8<uint8_t> : base8_numeric<uint8_t> {
  using Self = simd8<uint8_t>;

  simdutf_really_inline simd8() : base8_numeric<uint8_t>() {}
  simdutf_really_inline simd8(const vector_type _value)
      : base8_numeric<uint8_t>(_value) {}
  // Splat constructor
  simdutf_really_inline simd8(uint8_t _value) : simd8(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd8(const uint8_t *values) : simd8(load(values)) {}
  // Member-by-member initialization
  simdutf_really_inline
  simd8(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4, uint8_t v5,
        uint8_t v6, uint8_t v7, uint8_t v8, uint8_t v9, uint8_t v10,
        uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15)
      : simd8((vector_type){v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11,
                            v12, v13, v14, v15}) {}
  // Repeat 16 values as many times as necessary (usually for lookup tables)
  simdutf_really_inline static simd8<uint8_t>
  repeat_16(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4,
            uint8_t v5, uint8_t v6, uint8_t v7, uint8_t v8, uint8_t v9,
            uint8_t v10, uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14,
            uint8_t v15) {
    return simd8<uint8_t>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12,
                          v13, v14, v15);
  }

  simdutf_really_inline bool is_ascii() const {
    return move_mask_u8(this->value) == 0;
  }

  template <typename T>
  simdutf_really_inline simd8(simd8<T> other)
      : simd8(vector_type(other.value)) {}

  template <int N>
  simdutf_really_inline Self prev(const Self prev_chunk) const {
    return prev_aux<N>(prev_chunk.value);
  }

  // Saturated math
  simdutf_really_inline simd8<uint8_t>
  saturating_sub(const simd8<uint8_t> other) const {
    return (vector_type)vec_subs(this->value, (vector_type)other);
  }

  // Same as >, but only guarantees true is nonzero (< guarantees true = -1)
  simdutf_really_inline simd8<uint8_t>
  gt_bits(const simd8<uint8_t> other) const {
    return this->saturating_sub(other);
  }

  // Same as <, but only guarantees true is nonzero (< guarantees true = -1)
  simdutf_really_inline simd8<uint8_t>
  lt_bits(const simd8<uint8_t> other) const {
    return other.saturating_sub(*this);
  }

  // Bit-specific operations
  simdutf_really_inline bool bits_not_set_anywhere() const {
    return vec_all_eq(this->value, (vector_type)vec_splats(0));
  }

  simdutf_really_inline bool any_bits_set_anywhere() const {
    return !bits_not_set_anywhere();
  }

  template <int N> simdutf_really_inline simd8<uint8_t> shr() const {
    return simd8<uint8_t>(
        (vector_type)vec_sr(this->value, (vector_type)vec_splat_u8(N)));
  }

  template <int N> simdutf_really_inline simd8<uint8_t> shl() const {
    return simd8<uint8_t>(
        (vector_type)vec_sl(this->value, (vector_type)vec_splat_u8(N)));
  }
  void dump() const {
#ifdef SIMDUTF_LOGGING
    uint8_t tmp[16];
    store(tmp);
    for (int i = 0; i < 16; i++) {
      if (i == 0) {
        printf("[%02x", tmp[i]);
      } else if (i == 15) {
        printf(" %02x]", tmp[i]);
      } else {
        printf(" %02x", tmp[i]);
      }
    }
    putchar('\n');
#endif // SIMDUTF_LOGGING
  }

  void dump_ascii() const {
#ifdef SIMDUTF_LOGGING
    uint8_t tmp[16];
    store(tmp);
    for (int i = 0; i < 16; i++) {
      if (i == 0) {
        printf("[%c", tmp[i]);
      } else if (i == 15) {
        printf("%c]", tmp[i]);
      } else {
        printf("%c", tmp[i]);
      }
    }
    putchar('\n');
#endif // SIMDUTF_LOGGING
  }
};

// Signed bytes
template <> struct simd8<int8_t> : base8_numeric<int8_t> {
  simdutf_really_inline simd8() : base8_numeric<int8_t>() {}
  simdutf_really_inline simd8(const vector_type _value)
      : base8_numeric<int8_t>(_value) {}

  template <typename T>
  simdutf_really_inline simd8(simd8<T> other)
      : simd8(vector_type(other.value)) {}

  // Splat constructor
  simdutf_really_inline simd8(int8_t _value) : simd8(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd8(const int8_t *values) : simd8(load(values)) {}

  simdutf_really_inline operator simd8<uint8_t>() const;

  // Saturated math
  simdutf_really_inline simd8<int8_t>
  saturating_add(const simd8<int8_t> other) const {
    return (vector_type)vec_adds(this->value, other.value);
  }

  void dump() const {
    int8_t tmp[16];
    store(tmp);
    for (int i = 0; i < 16; i++) {
      if (i == 0) {
        printf("[%02x", tmp[i]);
      } else if (i == 15) {
        printf("%02x]", tmp[i]);
      } else {
        printf("%02x", tmp[i]);
      }
    }
    putchar('\n');
  }
};

template <typename T>
simd8<bool> operator==(const simd8<T> a, const simd8<T> b) {
  return vec_cmpeq(a.value, b.value);
}

template <typename T>
simd8<bool> operator!=(const simd8<T> a, const simd8<T> b) {
  return vec_cmpne(a.value, b.value);
}

template <typename T> simd8<T> operator&(const simd8<T> a, const simd8<T> b) {
  return vec_and(a.value, b.value);
}

template <typename T, typename U> simd8<T> operator&(const simd8<T> a, U b) {
  return vec_and(a.value, vec_splats(T(b)));
}

template <typename T> simd8<T> operator|(const simd8<T> a, const simd8<T> b) {
  return vec_or(a.value, b.value);
}

template <typename T> simd8<T> operator^(const simd8<T> a, const simd8<T> b) {
  return vec_xor(a.value, b.value);
}

template <typename T, typename U> simd8<T> operator^(const simd8<T> a, U b) {
  return vec_xor(a.value, vec_splats(T(b)));
}

template <typename T> simd8<T> operator+(const simd8<T> a, const simd8<T> b) {
  return vec_add(a.value, b.value);
}

template <typename T, typename U> simd8<T> operator+(const simd8<T> a, U b) {
  return vec_add(a.value, vec_splats(T(b)));
}

simdutf_really_inline simd8<int8_t>::operator simd8<uint8_t>() const {
  return (simd8<uint8_t>::vector_type)value;
}

template <typename T>
simd8<bool> operator<(const simd8<T> a, const simd8<T> b) {
  return vec_cmplt(a.value, b.value);
}

template <typename T>
simd8<bool> operator>(const simd8<T> a, const simd8<T> b) {
  return vec_cmpgt(a.value, b.value);
}

template <typename T>
simd8<bool> operator>=(const simd8<T> a, const simd8<T> b) {
  return vec_cmpge(a.value, b.value);
}

template <typename T> struct simd8x64 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd8<T>);
  static constexpr size_t ELEMENTS = simd8<T>::ELEMENTS;

  static_assert(NUM_CHUNKS == 4,
                "PPC64 kernel should use four registers per 64-byte block.");
  simd8<T> chunks[NUM_CHUNKS];

  simd8x64(const simd8x64<T> &o) = delete; // no copy allowed
  simd8x64<T> &
  operator=(const simd8<T> other) = delete; // no assignment allowed
  simd8x64() = delete;                      // no default constructor allowed
  simd8x64(simd8x64<T> &&) = default;

  simdutf_really_inline simd8x64(const simd8<T> chunk0, const simd8<T> chunk1,
                                 const simd8<T> chunk2, const simd8<T> chunk3)
      : chunks{chunk0, chunk1, chunk2, chunk3} {}
  simdutf_really_inline simd8x64(const T *ptr)
      : chunks{simd8<T>::load(ptr),
               simd8<T>::load(ptr + sizeof(simd8<T>) / sizeof(T)),
               simd8<T>::load(ptr + 2 * sizeof(simd8<T>) / sizeof(T)),
               simd8<T>::load(ptr + 3 * sizeof(simd8<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + ELEMENTS * 0);
    this->chunks[1].store(ptr + ELEMENTS * 1);
    this->chunks[2].store(ptr + ELEMENTS * 2);
    this->chunks[3].store(ptr + ELEMENTS * 3);
  }

  simdutf_really_inline simd8x64<T> &operator|=(const simd8x64<T> &other) {
    this->chunks[0] |= other.chunks[0];
    this->chunks[1] |= other.chunks[1];
    this->chunks[2] |= other.chunks[2];
    this->chunks[3] |= other.chunks[3];
    return *this;
  }

  simdutf_really_inline simd8<T> reduce_or() const {
    return (this->chunks[0] | this->chunks[1]) |
           (this->chunks[2] | this->chunks[3]);
  }

  simdutf_really_inline bool is_ascii() const {
    return this->reduce_or().is_ascii();
  }

  template <endianness endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    this->chunks[0].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 0);
    this->chunks[1].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 1);
    this->chunks[2].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 2);
    this->chunks[3].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 3);
  }

  simdutf_really_inline void store_ascii_as_utf32(char32_t *ptr) const {
    this->chunks[0].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 0);
    this->chunks[1].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 1);
    this->chunks[2].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 2);
    this->chunks[3].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 3);
  }

  simdutf_really_inline uint64_t to_bitmask() const {
    uint64_t r0 = uint32_t(this->chunks[0].to_bitmask());
    uint64_t r1 = this->chunks[1].to_bitmask();
    uint64_t r2 = this->chunks[2].to_bitmask();
    uint64_t r3 = this->chunks[3].to_bitmask();
    return r0 | (r1 << 16) | (r2 << 32) | (r3 << 48);
  }

  simdutf_really_inline uint64_t lt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] < mask, this->chunks[1] < mask,
                          this->chunks[2] < mask, this->chunks[3] < mask)
        .to_bitmask();
  }

  simdutf_really_inline uint64_t gt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] > mask, this->chunks[1] > mask,
                          this->chunks[2] > mask, this->chunks[3] > mask)
        .to_bitmask();
  }
  simdutf_really_inline uint64_t eq(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] == mask, this->chunks[1] == mask,
                          this->chunks[2] == mask, this->chunks[3] == mask)
        .to_bitmask();
  }
  simdutf_really_inline uint64_t gteq_unsigned(const uint8_t m) const {
    const simd8<uint8_t> mask = simd8<uint8_t>::splat(m);
    return simd8x64<bool>(simd8<uint8_t>(this->chunks[0]) >= mask,
                          simd8<uint8_t>(this->chunks[1]) >= mask,
                          simd8<uint8_t>(this->chunks[2]) >= mask,
                          simd8<uint8_t>(this->chunks[3]) >= mask)
        .to_bitmask();
  }

  void dump() const {
    puts("");
    for (int i = 0; i < 4; i++) {
      printf("chunk[%d] = ", i);
      this->chunks[i].dump();
    }
  }
}; // struct simd8x64<T>

simdutf_really_inline simd8<uint8_t> avg(const simd8<uint8_t> a,
                                         const simd8<uint8_t> b) {
  return vec_avg(a.value, b.value);
}
/* end file src\simdutf\ppc64\simd8-inl.h */
/* begin file src\simdutf\ppc64\simd16-inl.h */
// file included directly

template <typename T> struct simd16;

template <typename T> struct base16 {
  using vector_type = vector_u16_type_for_element<T>;
  static const int SIZE = sizeof(vector_type);
  static const int ELEMENTS = sizeof(vector_type) / sizeof(T);

  vector_type value;

  // Zero constructor
  simdutf_really_inline base16() : value{vector_type()} {}

  // Conversion from SIMD register
  simdutf_really_inline base16(const vector_type _value) : value{_value} {}
  void dump() const {
#ifdef SIMDUTF_LOGGING
    uint16_t tmp[8];
    vec_xst(value, 0, reinterpret_cast<vector_type *>(tmp));
    for (int i = 0; i < 8; i++) {
      if (i == 0) {
        printf("[%04x", tmp[i]);
      } else if (i == 8 - 1) {
        printf(" %04x]", tmp[i]);
      } else {
        printf(" %04x", tmp[i]);
      }
    }
    putchar('\n');
#endif // SIMDUTF_LOGGING
  }
};

// Forward declaration
template <typename> struct simd16;

template <typename T>
simd16<bool> operator==(const simd16<T> a, const simd16<T> b);

template <typename T, typename U>
simd16<bool> operator==(const simd16<T> a, U b);

template <typename T> simd16<T> operator&(const simd16<T> a, const simd16<T> b);

template <typename T> simd16<T> operator|(const simd16<T> a, const simd16<T> b);

template <typename T, typename U> simd16<T> operator|(const simd16<T> a, U b);

template <typename T, typename U> simd16<T> operator^(const simd16<T> a, U b);

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd16<bool> : base16<bool> {
  static simdutf_really_inline simd16<bool> splat(bool _value) {
    return (vector_type)vec_splats(uint16_t(-(!!_value)));
  }

  simdutf_really_inline simd16() : base16() {}

  simdutf_really_inline simd16(const vector_type _value)
      : base16<bool>(_value) {}

  // Splat constructor
  simdutf_really_inline simd16(bool _value) : base16<bool>(splat(_value)) {}

  simdutf_really_inline uint16_t to_bitmask() const {
    return move_mask_u8(value);
  }

  simdutf_really_inline bool any() const {
    const auto tmp = vec_u64_t(value);

    return tmp[0] || tmp[1]; // Note: logical or, not binary one
  }

  simdutf_really_inline bool is_zero() const {
    const auto tmp = vec_u64_t(value);

    return (tmp[0] | tmp[1]) == 0;
  }

  simdutf_really_inline simd16<bool> &operator|=(const simd16<bool> rhs) {
    value = vec_or(this->value, rhs.value);
    return *this;
  }
};

template <typename T> struct base16_numeric : base16<T> {
  using vector_type = typename base16<T>::vector_type;

  static simdutf_really_inline simd16<T> splat(T _value) {
    return vec_splats(_value);
  }

  static simdutf_really_inline simd16<T> zero() { return splat(0); }

  template <typename U>
  static simdutf_really_inline simd16<T> load(const U *ptr) {
    return vec_xl(0, reinterpret_cast<const T *>(ptr));
  }

  simdutf_really_inline base16_numeric() : base16<T>() {}
  simdutf_really_inline base16_numeric(const vector_type _value)
      : base16<T>(_value) {}

  // Store to array
  template <typename U> simdutf_really_inline void store(U *dst) const {
#if defined(__clang__)
    return vec_xst(this->value, 0, reinterpret_cast<T *>(dst));
#else
    return vec_xst(this->value, 0, reinterpret_cast<vector_type *>(dst));
#endif // defined(__clang__)
  }

  // Override to distinguish from bool version
  simdutf_really_inline simd16<T> operator~() const {
    return vec_xor(this->value, vec_splats(T(0xffff)));
  }
};

// Signed code units
template <> struct simd16<int16_t> : base16_numeric<int16_t> {
  simdutf_really_inline simd16() : base16_numeric<int16_t>() {}
  simdutf_really_inline simd16(const vector_type _value)
      : base16_numeric<int16_t>(_value) {}
  // Splat constructor
  simdutf_really_inline simd16(int16_t _value) : simd16(splat(_value)) {}
  // Array constructor
  simdutf_really_inline operator simd16<uint16_t>() const;
};

// Unsigned code units
template <> struct simd16<uint16_t> : base16_numeric<uint16_t> {
  simdutf_really_inline simd16() : base16_numeric<uint16_t>() {}
  simdutf_really_inline simd16(const vector_type _value)
      : base16_numeric<uint16_t>(_value) {}

  // Splat constructor
  simdutf_really_inline simd16(uint16_t _value) : simd16(splat(_value)) {}

  // Array constructor
  simdutf_really_inline simd16(const char16_t *values)
      : simd16(load(reinterpret_cast<const uint16_t *>(values))) {}

  simdutf_really_inline bool is_ascii() const {
    return vec_all_lt(value, vec_splats(uint16_t(128)));
  }

  // Order-specific operations
  simdutf_really_inline simd16<uint16_t>
  max_val(const simd16<uint16_t> other) const {
    return vec_max(this->value, other.value);
  }
  simdutf_really_inline simd16<uint16_t>
  min_val(const simd16<uint16_t> other) const {
    return vec_min(this->value, other.value);
  }
  // Same as <, but only guarantees true is nonzero (< guarantees true = -1)
  simdutf_really_inline simd16<bool>
  operator<=(const simd16<uint16_t> other) const {
    return other.max_val(*this) == other;
  }

  simdutf_really_inline simd16<bool>
  operator>=(const simd16<uint16_t> other) const {
    return other.min_val(*this) == other;
  }

  simdutf_really_inline simd16<bool>
  operator<(const simd16<uint16_t> other) const {
    return vec_cmplt(value, other.value);
  }

  // Bit-specific operations
  template <int N> simdutf_really_inline simd16<uint16_t> shr() const {
    return vec_sr(value, vec_splats(uint16_t(N)));
  }

  template <int N> simdutf_really_inline simd16<uint16_t> shl() const {
    return vec_sl(value, vec_splats(uint16_t(N)));
  }

  // Change the endianness
  simdutf_really_inline simd16<uint16_t> swap_bytes() const {
    return vec_revb(value);
  }

  // Pack with the unsigned saturation of two uint16_t code units into single
  // uint8_t vector
  static simdutf_really_inline simd8<uint8_t> pack(const simd16<uint16_t> &v0,
                                                   const simd16<uint16_t> &v1) {
    return vec_packs(v0.value, v1.value);
  }
};

template <typename T>
simd16<bool> operator==(const simd16<T> a, const simd16<T> b) {
  return vec_cmpeq(a.value, b.value);
}

template <typename T, typename U>
simd16<bool> operator==(const simd16<T> a, U b) {
  return vec_cmpeq(a.value, vec_splats(T(b)));
}

template <typename T>
simd16<T> operator&(const simd16<T> a, const simd16<T> b) {
  return vec_and(a.value, b.value);
}

template <typename T, typename U> simd16<T> operator&(const simd16<T> a, U b) {
  return vec_and(a.value, vec_splats(T(b)));
}

template <typename T>
simd16<T> operator|(const simd16<T> a, const simd16<T> b) {
  return vec_or(a.value, b.value);
}

template <typename T, typename U> simd16<T> operator|(const simd16<T> a, U b) {
  return vec_or(a.value, vec_splats(T(b)));
}

template <typename T>
simd16<T> operator^(const simd16<T> a, const simd16<T> b) {
  return vec_xor(a.value, b.value);
}

template <typename T, typename U> simd16<T> operator^(const simd16<T> a, U b) {
  return vec_xor(a.value, vec_splats(T(b)));
}

simdutf_really_inline simd16<int16_t>::operator simd16<uint16_t>() const {
  return (vec_u16_t)(value);
}

template <typename T> struct simd16x32 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd16<T>);
  static_assert(NUM_CHUNKS == 4,
                "AltiVec kernel should use four registers per 64-byte block.");
  simd16<T> chunks[NUM_CHUNKS];

  simd16x32(const simd16x32<T> &o) = delete; // no copy allowed
  simd16x32<T> &
  operator=(const simd16<T> other) = delete; // no assignment allowed
  simd16x32() = delete;                      // no default constructor allowed

  simdutf_really_inline
  simd16x32(const simd16<T> chunk0, const simd16<T> chunk1,
            const simd16<T> chunk2, const simd16<T> chunk3)
      : chunks{chunk0, chunk1, chunk2, chunk3} {}
  simdutf_really_inline simd16x32(const T *ptr)
      : chunks{simd16<T>::load(ptr),
               simd16<T>::load(ptr + sizeof(simd16<T>) / sizeof(T)),
               simd16<T>::load(ptr + 2 * sizeof(simd16<T>) / sizeof(T)),
               simd16<T>::load(ptr + 3 * sizeof(simd16<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + sizeof(simd16<T>) * 0 / sizeof(T));
    this->chunks[1].store(ptr + sizeof(simd16<T>) * 1 / sizeof(T));
    this->chunks[2].store(ptr + sizeof(simd16<T>) * 2 / sizeof(T));
    this->chunks[3].store(ptr + sizeof(simd16<T>) * 3 / sizeof(T));
  }

  simdutf_really_inline simd16<T> reduce_or() const {
    return (this->chunks[0] | this->chunks[1]) |
           (this->chunks[2] | this->chunks[3]);
  }

  simdutf_really_inline bool is_ascii() const {
    return this->reduce_or().is_ascii();
  }

  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    this->chunks[0].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 0);
    this->chunks[1].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 1);
    this->chunks[2].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 2);
    this->chunks[3].store_ascii_as_utf16(ptr + sizeof(simd16<T>) * 3);
  }

  simdutf_really_inline uint64_t to_bitmask() const {
    uint64_t r0 = uint32_t(this->chunks[0].to_bitmask());
    uint64_t r1 = this->chunks[1].to_bitmask();
    uint64_t r2 = this->chunks[2].to_bitmask();
    uint64_t r3 = this->chunks[3].to_bitmask();
    return r0 | (r1 << 16) | (r2 << 32) | (r3 << 48);
  }

  simdutf_really_inline void swap_bytes() {
    this->chunks[0] = this->chunks[0].swap_bytes();
    this->chunks[1] = this->chunks[1].swap_bytes();
    this->chunks[2] = this->chunks[2].swap_bytes();
    this->chunks[3] = this->chunks[3].swap_bytes();
  }

  simdutf_really_inline uint64_t lteq(const T m) const {
    const simd16<T> mask = simd16<T>::splat(m);
    return simd16x32<bool>(this->chunks[0] <= mask, this->chunks[1] <= mask,
                           this->chunks[2] <= mask, this->chunks[3] <= mask)
        .to_bitmask();
  }

  simdutf_really_inline uint64_t eq(const T m) const {
    const simd16<T> mask = simd16<T>::splat(m);
    return simd16x32<bool>(this->chunks[0] == mask, this->chunks[1] == mask,
                           this->chunks[2] == mask, this->chunks[3] == mask)
        .to_bitmask();
  }

  simdutf_really_inline uint64_t not_in_range(const T low, const T high) const {
    const simd16<T> mask_low = simd16<T>::splat(static_cast<T>(low - 1));
    const simd16<T> mask_high = simd16<T>::splat(static_cast<T>(high + 1));
    return simd16x32<bool>(
               (this->chunks[0] >= mask_high) | (this->chunks[0] <= mask_low),
               (this->chunks[1] >= mask_high) | (this->chunks[1] <= mask_low),
               (this->chunks[2] >= mask_high) | (this->chunks[2] <= mask_low),
               (this->chunks[3] >= mask_high) | (this->chunks[3] <= mask_low))
        .to_bitmask();
  }
}; // struct simd16x32<T>
/* end file src\simdutf\ppc64\simd16-inl.h */
/* begin file src\simdutf\ppc64\simd32-inl.h */
// file included directly

template <typename T> struct simd32;

template <typename T> struct base32 {
  using vector_type = vector_u32_type_for_element<T>;
  static const int SIZE = sizeof(vector_type);
  static const int ELEMENTS = sizeof(vector_type) / sizeof(T);

  vector_type value;

  // Zero constructor
  simdutf_really_inline base32() : value{vector_type()} {}

  // Conversion from SIMD register
  simdutf_really_inline base32(const vector_type _value) : value{_value} {}

  // Splat for scalar
  simdutf_really_inline base32(T scalar) : value{vec_splats(scalar)} {}

  template <typename Pointer>
  simdutf_really_inline base32(const Pointer *ptr)
      : base32(vec_xl(0, reinterpret_cast<const T *>(ptr))) {}

  // Store to array
  template <typename U> simdutf_really_inline void store(U *dst) const {
#if defined(__clang__)
    return vec_xst(this->value, 0, reinterpret_cast<T *>(dst));
#else
    return vec_xst(this->value, 0, reinterpret_cast<vector_type *>(dst));
#endif // defined(__clang__)
  }
  void dump(const char *name = nullptr) const {
#ifdef SIMDUTF_LOGGING
    if (name != nullptr) {
      printf("%-10s = ", name);
    }

    uint32_t tmp[4];
    vec_xst(value, 0, reinterpret_cast<vector_type *>(tmp));
    for (int i = 0; i < 4; i++) {
      if (i == 0) {
        printf("[%08x", tmp[i]);
      } else if (i == 4 - 1) {
        printf(" %08x]", tmp[i]);
      } else {
        printf(" %08x", tmp[i]);
      }
    }
    putchar('\n');
#endif // SIMDUTF_LOGGING
  }
};

template <typename T> struct base32_numeric : base32<T> {
  using super = base32<T>;
  using vector_type = typename super::vector_type;

  static simdutf_really_inline simd32<T> splat(T _value) {
    return vec_splats(_value);
  }

  static simdutf_really_inline simd32<T> zero() { return splat(0); }

  template <typename U>
  static simdutf_really_inline simd32<T> load(const U *values) {
    return vec_xl(0, reinterpret_cast<const T *>(values));
  }

  simdutf_really_inline base32_numeric() : base32<T>() {}

  simdutf_really_inline base32_numeric(const vector_type _value)
      : base32<T>(_value) {}

  // Addition/subtraction are the same for signed and unsigned
  simdutf_really_inline simd32<T> operator+(const simd32<T> other) const {
    return vec_add(this->value, other.value);
  }

  simdutf_really_inline simd32<T> operator-(const simd32<T> other) const {
    return vec_sub(this->value, other.value);
  }

  simdutf_really_inline simd32<T> &operator+=(const simd32<T> other) {
    *this = *this + other;
    return *static_cast<simd32<T> *>(this);
  }

  simdutf_really_inline simd32<T> &operator-=(const simd32<T> other) {
    *this = *this - other;
    return *static_cast<simd32<T> *>(this);
  }
};

// Forward declaration
template <typename> struct simd32;

template <typename T>
simd32<bool> operator==(const simd32<T> a, const simd32<T> b);

template <typename T>
simd32<bool> operator!=(const simd32<T> a, const simd32<T> b);

template <typename T>
simd32<bool> operator>(const simd32<T> a, const simd32<T> b);

template <typename T> simd32<bool> operator==(const simd32<T> a, T b);

template <typename T> simd32<bool> operator!=(const simd32<T> a, T b);

template <typename T> simd32<T> operator&(const simd32<T> a, const simd32<T> b);

template <typename T> simd32<T> operator|(const simd32<T> a, const simd32<T> b);

template <typename T> simd32<T> operator^(const simd32<T> a, const simd32<T> b);

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd32<bool> : base32<bool> {
  static simdutf_really_inline simd32<bool> splat(bool _value) {
    return (vector_type)vec_splats(uint32_t(-(!!_value)));
  }

  simdutf_really_inline simd32(const vector_type _value)
      : base32<bool>(_value) {}

  // Splat constructor
  simdutf_really_inline simd32(bool _value) : base32<bool>(splat(_value)) {}

  simdutf_really_inline uint16_t to_bitmask() const {
    return move_mask_u8(value);
  }

  simdutf_really_inline bool any() const {
    const vec_u64_t tmp = (vec_u64_t)value;

    return tmp[0] || tmp[1]; // Note: logical or, not binary one
  }

  simdutf_really_inline bool is_zero() const {
    const vec_u64_t tmp = (vec_u64_t)value;

    return (tmp[0] | tmp[1]) == 0;
  }

  simdutf_really_inline simd32<bool> operator~() const {
    return (vec_bool32_t)vec_xor(this->value, vec_splats(uint32_t(0xffffffff)));
  }
};

// Unsigned code units
template <> struct simd32<uint32_t> : base32_numeric<uint32_t> {
  simdutf_really_inline simd32() : base32_numeric<uint32_t>() {}

  simdutf_really_inline simd32(const vector_type _value)
      : base32_numeric<uint32_t>(_value) {}

  // Splat constructor
  simdutf_really_inline simd32(uint32_t _value) : simd32(splat(_value)) {}

  // Array constructor
  simdutf_really_inline simd32(const char32_t *values)
      : simd32(load(reinterpret_cast<const uint32_t *>(values))) {}

  // Bit-specific operations
  template <int N> simdutf_really_inline simd32<uint32_t> shr() const {
    return vec_sr(value, vec_splats(uint32_t(N)));
  }

  template <int N> simdutf_really_inline simd32<uint32_t> shl() const {
    return vec_sl(value, vec_splats(uint32_t(N)));
  }

  // Change the endianness
  simdutf_really_inline simd32<uint32_t> swap_bytes() const {
    return vec_revb(value);
  }

  simdutf_really_inline uint64_t sum() const {
    return uint64_t(value[0]) + uint64_t(value[1]) + uint64_t(value[2]) +
           uint64_t(value[3]);
  }

  static simdutf_really_inline simd16<uint16_t>
  pack(const simd32<uint32_t> &v0, const simd32<uint32_t> &v1) {
    return vec_packs(v0.value, v1.value);
  }
};

template <typename T>
simd32<bool> operator==(const simd32<T> a, const simd32<T> b) {
  return vec_cmpeq(a.value, b.value);
}

template <typename T>
simd32<bool> operator!=(const simd32<T> a, const simd32<T> b) {
  return vec_cmpne(a.value, b.value);
}

template <typename T> simd32<bool> operator==(const simd32<T> a, T b) {
  return vec_cmpeq(a.value, vec_splats(b));
}

template <typename T> simd32<bool> operator!=(const simd32<T> a, T b) {
  return vec_cmpne(a.value, vec_splats(b));
}

template <typename T>
simd32<bool> operator>(const simd32<T> a, const simd32<T> b) {
  return vec_cmpgt(a.value, b.value);
}

template <typename T>
simd32<bool> operator>=(const simd32<T> a, const simd32<T> b) {
  return vec_cmpge(a.value, b.value);
}

template <typename T>
simd32<T> operator&(const simd32<T> a, const simd32<T> b) {
  return vec_and(a.value, b.value);
}

template <typename T, typename U> simd32<T> operator&(const simd32<T> a, U b) {
  return vec_and(a.value, vec_splats(T(b)));
}

template <typename T>
simd32<T> operator|(const simd32<T> a, const simd32<T> b) {
  return vec_or(a.value, b.value);
}

template <typename T>
simd32<T> operator^(const simd32<T> a, const simd32<T> b) {
  return vec_xor(a.value, b.value);
}

template <typename T, typename U> simd32<T> operator^(const simd32<T> a, U b) {
  return vec_xor(a.value, vec_splats(T(b)));
}

template <typename T> simd32<T> max_val(const simd32<T> a, const simd32<T> b) {
  return vec_max(a.value, b.value);
}

template <typename T>
simdutf_really_inline simd32<T> min(const simd32<T> b, const simd32<T> a) {
  return vec_min(a.value, b.value);
}
/* end file src\simdutf\ppc64\simd32-inl.h */

template <typename T>
simd8<T> select(const simd8<T> cond, const simd8<T> val_true,
                const simd8<T> val_false) {
  return vec_sel(val_false.value, val_true.value, cond.value);
}

template <typename T>
simd8<T> select(const T cond, const simd8<T> val_true,
                const simd8<T> val_false) {
  return vec_sel(val_false.value, val_true.value, vec_splats(cond));
}

template <typename T>
simd16<T> select(const simd16<T> cond, const simd16<T> val_true,
                 const simd16<T> val_false) {
  return vec_sel(val_false.value, val_true.value, cond.value);
}

template <typename T>
simd16<T> select(const T cond, const simd16<T> val_true,
                 const simd16<T> val_false) {
  return vec_sel(val_false.value, val_true.value, vec_splats(cond));
}

template <typename T>
simd32<T> select(const simd32<T> cond, const simd32<T> val_true,
                 const simd32<T> val_false) {
  return vec_sel(val_false.value, val_true.value, cond.value);
}

template <typename T>
simd32<T> select(const T cond, const simd32<T> val_true,
                 const simd32<T> val_false) {
  return vec_sel(val_false.value, val_true.value, vec_splats(cond));
}

using vector_u8 = simd8<uint8_t>;
using vector_u16 = simd16<uint16_t>;
using vector_u32 = simd32<uint32_t>;
using vector_i8 = simd8<int8_t>;

simdutf_really_inline vector_u8 as_vector_u8(const vector_u16 v) {
  return vector_u8::vector_type(v.value);
}

simdutf_really_inline vector_u8 as_vector_u8(const vector_u32 v) {
  return vector_u8::vector_type(v.value);
}

simdutf_really_inline vector_u8 as_vector_u8(const vector_i8 v) {
  return vector_u8::vector_type(v.value);
}

simdutf_really_inline vector_u8 as_vector_u8(const simd16<bool> v) {
  return vector_u8::vector_type(v.value);
}

simdutf_really_inline vector_i8 as_vector_i8(const vector_u8 v) {
  return vector_i8::vector_type(v.value);
}

simdutf_really_inline vector_u16 as_vector_u16(const vector_u8 v) {
  return vector_u16::vector_type(v.value);
}

simdutf_really_inline vector_u16 as_vector_u16(const simd16<bool> v) {
  return vector_u16::vector_type(v.value);
}

simdutf_really_inline vector_u32 as_vector_u32(const vector_u8 v) {
  return vector_u32::vector_type(v.value);
}

simdutf_really_inline vector_u32 as_vector_u32(const vector_u16 v) {
  return vector_u32::vector_type(v.value);
}

simdutf_really_inline vector_u32 max(vector_u32 a, vector_u32 b) {
  return vec_max(a.value, b.value);
}

simdutf_really_inline vector_u32 max(vector_u32 a, vector_u32 b, vector_u32 c) {
  return max(max(a, b), c);
}

simdutf_really_inline vector_u32 sum4bytes(vector_u8 bytes, vector_u32 acc) {
  return vec_sum4s(bytes.value, acc.value);
}

} // namespace simd
} // unnamed namespace
} // namespace ppc64
} // namespace simdutf

#endif // SIMDUTF_PPC64_SIMD_INPUT_H
/* end file src\simdutf\ppc64\simd.h */

/* begin file src\simdutf\ppc64\end.h */
/* end file src\simdutf\ppc64\end.h */

#endif // SIMDUTF_IMPLEMENTATION_PPC64

#endif // SIMDUTF_PPC64_H
/* end file src\simdutf\ppc64.h */
/* begin file src\simdutf\rvv.h */
#ifndef SIMDUTF_RVV_H
#define SIMDUTF_RVV_H

#ifdef SIMDUTF_FALLBACK_H
  #error "rvv.h must be included before fallback.h"
#endif


#define SIMDUTF_CAN_ALWAYS_RUN_RVV SIMDUTF_IS_RVV

#ifndef SIMDUTF_IMPLEMENTATION_RVV
  #define SIMDUTF_IMPLEMENTATION_RVV                                           \
    (SIMDUTF_CAN_ALWAYS_RUN_RVV ||                                             \
     (SIMDUTF_IS_RISCV64 && SIMDUTF_HAS_RVV_INTRINSICS &&                      \
      SIMDUTF_HAS_RVV_TARGET_REGION))
#endif

#if SIMDUTF_IMPLEMENTATION_RVV

  #if SIMDUTF_CAN_ALWAYS_RUN_RVV
    #define SIMDUTF_TARGET_RVV
  #else
    #define SIMDUTF_TARGET_RVV SIMDUTF_TARGET_REGION("arch=+v")
  #endif
  #if !SIMDUTF_IS_ZVBB && SIMDUTF_HAS_ZVBB_INTRINSICS
    #define SIMDUTF_TARGET_ZVBB SIMDUTF_TARGET_REGION("arch=+v,+zvbb")
  #endif

namespace simdutf {
namespace rvv {} // namespace rvv
} // namespace simdutf

/* begin file src\simdutf\rvv\implementation.h */
#ifndef SIMDUTF_RVV_IMPLEMENTATION_H
#define SIMDUTF_RVV_IMPLEMENTATION_H


namespace simdutf {
namespace rvv {

namespace {
using namespace simdutf;
} // namespace

class implementation final : public simdutf::implementation {
public:
  simdutf_really_inline implementation()
      : simdutf::implementation("rvv", "RISC-V Vector Extension",
                                internal::instruction_set::RVV),
        _supports_zvbb(internal::detect_supported_architectures() &
                       internal::instruction_set::ZVBB) {}

  simdutf_warn_unused result base64_to_binary(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused result
  base64_to_binary(const char16_t *input, size_t length, char *output,
                   base64_options options,
                   last_chunk_handling_options last_chunk_options =
                       last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char16_t *input, size_t length, char *output,
      base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  size_t binary_to_base64(const char *input, size_t length, char *output,
                          base64_options options) const noexcept;

  const char *find(const char *start, const char *end,
                   char character) const noexcept;
  const char16_t *find(const char16_t *start, const char16_t *end,
                       char16_t character) const noexcept;
private:
  const bool _supports_zvbb;

#if SIMDUTF_IS_ZVBB
  bool supports_zvbb() const { return true; }
#elif SIMDUTF_HAS_ZVBB_INTRINSICS
  bool supports_zvbb() const { return _supports_zvbb; }
#else
  bool supports_zvbb() const { return false; }
#endif
};

} // namespace rvv
} // namespace simdutf

#endif // SIMDUTF_RVV_IMPLEMENTATION_H
/* end file src\simdutf\rvv\implementation.h */
/* begin file src\simdutf\rvv\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "rvv"
// #define SIMDUTF_IMPLEMENTATION rvv

#if SIMDUTF_CAN_ALWAYS_RUN_RVV
// nothing needed.
#else
SIMDUTF_TARGET_RVV
#endif
/* end file src\simdutf\rvv\begin.h */
/* begin file src\simdutf\rvv\intrinsics.h */
#ifndef SIMDUTF_RVV_INTRINSICS_H
#define SIMDUTF_RVV_INTRINSICS_H


#include <riscv_vector.h>

#if __riscv_v_intrinsic >= 1000000 || __GCC__ >= 14
  #define simdutf_vrgather_u8m1x2(tbl, idx)                                    \
    __riscv_vcreate_v_u8m1_u8m2(                                               \
        __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m2_u8m1(idx, 0),        \
                                 __riscv_vsetvlmax_e8m1()),                    \
        __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m2_u8m1(idx, 1),        \
                                 __riscv_vsetvlmax_e8m1()));

  #define simdutf_vrgather_u8m1x4(tbl, idx)                                    \
    __riscv_vcreate_v_u8m1_u8m4(                                               \
        __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m4_u8m1(idx, 0),        \
                                 __riscv_vsetvlmax_e8m1()),                    \
        __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m4_u8m1(idx, 1),        \
                                 __riscv_vsetvlmax_e8m1()),                    \
        __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m4_u8m1(idx, 2),        \
                                 __riscv_vsetvlmax_e8m1()),                    \
        __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m4_u8m1(idx, 3),        \
                                 __riscv_vsetvlmax_e8m1()));
#else
  // This has worse codegen on gcc
  #define simdutf_vrgather_u8m1x2(tbl, idx)                                    \
    __riscv_vset_v_u8m1_u8m2(                                                  \
        __riscv_vlmul_ext_v_u8m1_u8m2(__riscv_vrgather_vv_u8m1(                \
            tbl, __riscv_vget_v_u8m2_u8m1(idx, 0), __riscv_vsetvlmax_e8m1())), \
        1,                                                                     \
        __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m2_u8m1(idx, 1),        \
                                 __riscv_vsetvlmax_e8m1()))

  #define simdutf_vrgather_u8m1x4(tbl, idx)                                    \
    __riscv_vset_v_u8m1_u8m4(                                                  \
        __riscv_vset_v_u8m1_u8m4(                                              \
            __riscv_vset_v_u8m1_u8m4(                                          \
                __riscv_vlmul_ext_v_u8m1_u8m4(__riscv_vrgather_vv_u8m1(        \
                    tbl, __riscv_vget_v_u8m4_u8m1(idx, 0),                     \
                    __riscv_vsetvlmax_e8m1())),                                \
                1,                                                             \
                __riscv_vrgather_vv_u8m1(tbl,                                  \
                                         __riscv_vget_v_u8m4_u8m1(idx, 1),     \
                                         __riscv_vsetvlmax_e8m1())),           \
            2,                                                                 \
            __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m4_u8m1(idx, 2),    \
                                     __riscv_vsetvlmax_e8m1())),               \
        3,                                                                     \
        __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m4_u8m1(idx, 3),        \
                                 __riscv_vsetvlmax_e8m1()))
#endif

/* Zvbb adds dedicated support for endianness swaps with vrev8, but if we can't
 * use that, we have to emulate it with the standard V extension.
 * Using LMUL=1 vrgathers could be faster than the srl+macc variant, but that
 * would increase register pressure, and vrgather implementations performance
 * varies a lot. */
enum class simdutf_ByteFlip { NONE, V, ZVBB };

template <simdutf_ByteFlip method>
simdutf_really_inline static uint16_t simdutf_byteflip(uint16_t v) {
  if (method != simdutf_ByteFlip::NONE)
    return (uint16_t)((v * 1u) << 8 | (v * 1u) >> 8);
  return v;
}

#ifdef SIMDUTF_TARGET_ZVBB
SIMDUTF_UNTARGET_REGION
SIMDUTF_TARGET_ZVBB
#endif

template <simdutf_ByteFlip method>
simdutf_really_inline static vuint16m1_t simdutf_byteflip(vuint16m1_t v,
                                                          size_t vl) {
#if SIMDUTF_HAS_ZVBB_INTRINSICS
  if (method == simdutf_ByteFlip::ZVBB)
    return __riscv_vrev8_v_u16m1(v, vl);
#endif
  if (method == simdutf_ByteFlip::V)
    return __riscv_vmacc_vx_u16m1(__riscv_vsrl_vx_u16m1(v, 8, vl), 0x100, v,
                                  vl);
  return v;
}

template <simdutf_ByteFlip method>
simdutf_really_inline static vuint16m2_t simdutf_byteflip(vuint16m2_t v,
                                                          size_t vl) {
#if SIMDUTF_HAS_ZVBB_INTRINSICS
  if (method == simdutf_ByteFlip::ZVBB)
    return __riscv_vrev8_v_u16m2(v, vl);
#endif
  if (method == simdutf_ByteFlip::V)
    return __riscv_vmacc_vx_u16m2(__riscv_vsrl_vx_u16m2(v, 8, vl), 0x100, v,
                                  vl);
  return v;
}

template <simdutf_ByteFlip method>
simdutf_really_inline static vuint16m4_t simdutf_byteflip(vuint16m4_t v,
                                                          size_t vl) {
#if SIMDUTF_HAS_ZVBB_INTRINSICS
  if (method == simdutf_ByteFlip::ZVBB)
    return __riscv_vrev8_v_u16m4(v, vl);
#endif
  if (method == simdutf_ByteFlip::V)
    return __riscv_vmacc_vx_u16m4(__riscv_vsrl_vx_u16m4(v, 8, vl), 0x100, v,
                                  vl);
  return v;
}

template <simdutf_ByteFlip method>
simdutf_really_inline static vuint16m8_t simdutf_byteflip(vuint16m8_t v,
                                                          size_t vl) {
#if SIMDUTF_HAS_ZVBB_INTRINSICS
  if (method == simdutf_ByteFlip::ZVBB)
    return __riscv_vrev8_v_u16m8(v, vl);
#endif
  if (method == simdutf_ByteFlip::V)
    return __riscv_vmacc_vx_u16m8(__riscv_vsrl_vx_u16m8(v, 8, vl), 0x100, v,
                                  vl);
  return v;
}

#ifdef SIMDUTF_TARGET_ZVBB
SIMDUTF_UNTARGET_REGION
SIMDUTF_TARGET_RVV
#endif

#endif //  SIMDUTF_RVV_INTRINSICS_H
/* end file src\simdutf\rvv\intrinsics.h */
/* begin file src\simdutf\rvv\end.h */
#if SIMDUTF_CAN_ALWAYS_RUN_RVV
// nothing needed.
#else
SIMDUTF_UNTARGET_REGION
#endif

/* end file src\simdutf\rvv\end.h */

#endif // SIMDUTF_IMPLEMENTATION_RVV

#endif // SIMDUTF_RVV_H
/* end file src\simdutf\rvv.h */
/* begin file src\simdutf\lsx.h */
#ifndef SIMDUTF_LSX_H
#define SIMDUTF_LSX_H

#ifdef SIMDUTF_FALLBACK_H
  #error "lsx.h must be included before fallback.h"
#endif


#ifndef SIMDUTF_IMPLEMENTATION_LSX
  #define SIMDUTF_IMPLEMENTATION_LSX (SIMDUTF_IS_LSX)
#endif
#if SIMDUTF_IMPLEMENTATION_LSX && SIMDUTF_IS_LSX
  #define SIMDUTF_CAN_ALWAYS_RUN_LSX 1
#else
  #define SIMDUTF_CAN_ALWAYS_RUN_LSX 0
#endif

#define SIMDUTF_CAN_ALWAYS_RUN_FALLBACK (SIMDUTF_IMPLEMENTATION_FALLBACK)

#if SIMDUTF_IMPLEMENTATION_LSX

namespace simdutf {
/**
 * Implementation for LoongArch SX.
 */
namespace lsx {} // namespace lsx
} // namespace simdutf

/* begin file src\simdutf\lsx\implementation.h */
#ifndef SIMDUTF_LSX_IMPLEMENTATION_H
#define SIMDUTF_LSX_IMPLEMENTATION_H


namespace simdutf {
namespace lsx {

namespace {
using namespace simdutf;
}

class implementation final : public simdutf::implementation {
public:
  simdutf_really_inline implementation()
      : simdutf::implementation("lsx", "LOONGARCH SX",
                                internal::instruction_set::LSX) {}

  simdutf_warn_unused result base64_to_binary(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused result
  base64_to_binary(const char16_t *input, size_t length, char *output,
                   base64_options options,
                   last_chunk_handling_options last_chunk_options =
                       last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char16_t *input, size_t length, char *output,
      base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  size_t binary_to_base64(const char *input, size_t length, char *output,
                          base64_options options) const noexcept;
  const char *find(const char *start, const char *end,
                   char character) const noexcept;
  const char16_t *find(const char16_t *start, const char16_t *end,
                       char16_t character) const noexcept;
};

} // namespace lsx
} // namespace simdutf

#endif // SIMDUTF_LSX_IMPLEMENTATION_H
/* end file src\simdutf\lsx\implementation.h */

/* begin file src\simdutf\lsx\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "lsx"
// #define SIMDUTF_IMPLEMENTATION lsx
#define SIMDUTF_SIMD_HAS_UNSIGNED_CMP 1
/* end file src\simdutf\lsx\begin.h */

  // Declarations
/* begin file src\simdutf\lsx\intrinsics.h */
#ifndef SIMDUTF_LSX_INTRINSICS_H
#define SIMDUTF_LSX_INTRINSICS_H


// This should be the correct header whether
// you use visual studio or other compilers.
#include <lsxintrin.h>

/*
Encoding of argument for LoongArch64 xvldi instruction.  See:
https://jia.je/unofficial-loongarch-intrinsics-guide/lasx/misc/#__m256i-__lasx_xvldi-imm_n1024_1023-imm

1: imm[12:8]=0b10000: broadcast imm[7:0] as 32-bit elements to all lanes

2: imm[12:8]=0b10001: broadcast imm[7:0] << 8 as 32-bit elements to all lanes

3: imm[12:8]=0b10010: broadcast imm[7:0] << 16 as 32-bit elements to all lanes

4: imm[12:8]=0b10011: broadcast imm[7:0] << 24 as 32-bit elements to all lanes

5: imm[12:8]=0b10100: broadcast imm[7:0] as 16-bit elements to all lanes

6: imm[12:8]=0b10101: broadcast imm[7:0] << 8 as 16-bit elements to all lanes

7: imm[12:8]=0b10110: broadcast (imm[7:0] << 8) | 0xFF as 32-bit elements to all
lanes

8: imm[12:8]=0b10111: broadcast (imm[7:0] << 16) | 0xFFFF as 32-bit elements to
all lanes

9: imm[12:8]=0b11000: broadcast imm[7:0] as 8-bit elements to all lanes

10: imm[12:8]=0b11001: repeat each bit of imm[7:0] eight times, and broadcast
the result as 64-bit elements to all lanes
*/

namespace vldi {

template <uint16_t v> class const_u16 {
  constexpr static const uint8_t b0 = ((v >> 0 * 8) & 0xff);
  constexpr static const uint8_t b1 = ((v >> 1 * 8) & 0xff);

  constexpr static bool is_case5 = uint16_t(b0) == v;
  constexpr static bool is_case6 = (uint16_t(b1) << 8) == v;
  constexpr static bool is_case9 = (b0 == b1);
  constexpr static bool is_case10 =
      ((b0 == 0xff) || (b0 == 0x00)) && ((b1 == 0xff) || (b1 == 0x00));

public:
  constexpr static uint16_t operation = is_case5    ? 0b10100
                                        : is_case6  ? 0b10101
                                        : is_case9  ? 0b11000
                                        : is_case10 ? 0x11001
                                                    : 0xffff;

  constexpr static uint16_t byte =
      is_case5    ? b0
      : is_case6  ? b1
      : is_case9  ? b0
      : is_case10 ? ((b0 ? 0x55 : 0x00) | (b1 ? 0xaa : 0x00))
                  : 0xffff;

  constexpr static int value = int((operation << 8) | byte) - 8192;
  constexpr static bool valid = operation != 0xffff;
};

template <uint32_t v> class const_u32 {
  constexpr static const uint8_t b0 = (v & 0xff);
  constexpr static const uint8_t b1 = ((v >> 8) & 0xff);
  constexpr static const uint8_t b2 = ((v >> 16) & 0xff);
  constexpr static const uint8_t b3 = ((v >> 24) & 0xff);

  constexpr static bool is_case1 = (uint32_t(b0) == v);
  constexpr static bool is_case2 = ((uint32_t(b1) << 8) == v);
  constexpr static bool is_case3 = ((uint32_t(b2) << 16) == v);
  constexpr static bool is_case4 = ((uint32_t(b3) << 24) == v);
  constexpr static bool is_case5 = (b0 == b2) && (b1 == 0) && (b3 == 0);
  constexpr static bool is_case6 = (b1 == b3) && (b0 == 0) && (b2 == 0);
  constexpr static bool is_case7 = (b3 == 0) && (b2 == 0) && (b0 == 0xff);
  constexpr static bool is_case8 = (b3 == 0) && (b1 == 0xff) && (b0 == 0xff);
  constexpr static bool is_case9 = (b0 == b1) && (b0 == b2) && (b0 == b3);
  constexpr static bool is_case10 =
      ((b0 == 0xff) || (b0 == 0x00)) && ((b1 == 0xff) || (b1 == 0x00)) &&
      ((b2 == 0xff) || (b2 == 0x00)) && ((b3 == 0xff) || (b3 == 0x00));

public:
  constexpr static uint16_t operation = is_case1    ? 0b10000
                                        : is_case2  ? 0b10001
                                        : is_case3  ? 0b10010
                                        : is_case4  ? 0b10011
                                        : is_case5  ? 0b10100
                                        : is_case6  ? 0b10101
                                        : is_case7  ? 0b10110
                                        : is_case8  ? 0b10111
                                        : is_case9  ? 0b11000
                                        : is_case10 ? 0b11001
                                                    : 0xffff;

  constexpr static uint16_t byte =
      is_case1    ? b0
      : is_case2  ? b1
      : is_case3  ? b2
      : is_case4  ? b3
      : is_case5  ? b0
      : is_case6  ? b1
      : is_case7  ? b1
      : is_case8  ? b2
      : is_case9  ? b0
      : is_case10 ? ((b0 ? 0x11 : 0x00) | (b1 ? 0x22 : 0x00) |
                     (b2 ? 0x44 : 0x00) | (b3 ? 0x88 : 0x00))
                  : 0xffff;

  constexpr static int value = int((operation << 8) | byte) - 8192;
  constexpr static bool valid = operation != 0xffff;
};

template <uint64_t v> class const_u64 {
  constexpr static const uint8_t b0 = ((v >> 0 * 8) & 0xff);
  constexpr static const uint8_t b1 = ((v >> 1 * 8) & 0xff);
  constexpr static const uint8_t b2 = ((v >> 2 * 8) & 0xff);
  constexpr static const uint8_t b3 = ((v >> 3 * 8) & 0xff);
  constexpr static const uint8_t b4 = ((v >> 4 * 8) & 0xff);
  constexpr static const uint8_t b5 = ((v >> 5 * 8) & 0xff);
  constexpr static const uint8_t b6 = ((v >> 6 * 8) & 0xff);
  constexpr static const uint8_t b7 = ((v >> 7 * 8) & 0xff);

  constexpr static bool is_case10 =
      ((b0 == 0xff) || (b0 == 0x00)) && ((b1 == 0xff) || (b1 == 0x00)) &&
      ((b2 == 0xff) || (b2 == 0x00)) && ((b3 == 0xff) || (b3 == 0x00)) &&
      ((b4 == 0xff) || (b4 == 0x00)) && ((b5 == 0xff) || (b5 == 0x00)) &&
      ((b6 == 0xff) || (b6 == 0x00)) && ((b7 == 0xff) || (b7 == 0x00));

public:
  constexpr static bool is_32bit =
      ((v & 0xffffffff) == (v >> 32)) && const_u32<(v >> 32)>::value;
  constexpr static uint8_t op_32bit = const_u32<(v >> 32)>::operation;
  constexpr static uint8_t byte_32bit = const_u32<(v >> 32)>::byte;

  constexpr static uint16_t operation = is_32bit    ? op_32bit
                                        : is_case10 ? 0x11001
                                                    : 0xffff;

  constexpr static uint16_t byte =
      is_32bit ? byte_32bit
      : is_case10
          ? ((b0 ? 0x01 : 0x00) | (b1 ? 0x02 : 0x00) | (b2 ? 0x04 : 0x00) |
             (b3 ? 0x08 : 0x00) | (b4 ? 0x10 : 0x00) | (b5 ? 0x20 : 0x00) |
             (b6 ? 0x40 : 0x00) | (b7 ? 0x80 : 0x00))
          : 0xffff;

  constexpr static int value = int((operation << 8) | byte) - 8192;
  constexpr static bool valid = operation != 0xffff;
};
} // namespace vldi

// Uncomment when running under QEMU affected
// by bug https://gitlab.com/qemu-project/qemu/-/issues/2865
// Versions <= 9.2.2 are affected, likely anything newer is correct.
#ifndef QEMU_VLDI_BUG
// #define QEMU_VLDI_BUG 1
#endif

#ifdef QEMU_VLDI_BUG
  #define lsx_splat_u16(v) __lsx_vreplgr2vr_h(v)
  #define lsx_splat_u32(v) __lsx_vreplgr2vr_w(v)
#else
template <uint16_t x> constexpr __m128i lsx_splat_u16_aux() {
  constexpr bool is_imm10 = (int16_t(x) < 512) && (int16_t(x) > -512);
  constexpr uint16_t imm10 = is_imm10 ? x : 0;
  constexpr bool is_vldi = vldi::const_u16<x>::valid;
  constexpr int vldi_imm = is_vldi ? vldi::const_u16<x>::value : 0;

  return is_imm10  ? __lsx_vrepli_h(int16_t(imm10))
         : is_vldi ? __lsx_vldi(vldi_imm)
                   : __lsx_vreplgr2vr_h(x);
}

template <uint32_t x> constexpr __m128i lsx_splat_u32_aux() {
  constexpr bool is_imm10 = (int32_t(x) < 512) && (int32_t(x) > -512);
  constexpr uint32_t imm10 = is_imm10 ? x : 0;
  constexpr bool is_vldi = vldi::const_u32<x>::valid;
  constexpr int vldi_imm = is_vldi ? vldi::const_u32<x>::value : 0;

  return is_imm10  ? __lsx_vrepli_w(int32_t(imm10))
         : is_vldi ? __lsx_vldi(vldi_imm)
                   : __lsx_vreplgr2vr_w(x);
}

  #define lsx_splat_u16(v) lsx_splat_u16_aux<(v)>()
  #define lsx_splat_u32(v) lsx_splat_u32_aux<(v)>()
#endif // QEMU_VLDI_BUG

#endif //  SIMDUTF_LSX_INTRINSICS_H
/* end file src\simdutf\lsx\intrinsics.h */
/* begin file src\simdutf\lsx\bitmanipulation.h */
#ifndef SIMDUTF_LSX_BITMANIPULATION_H
#define SIMDUTF_LSX_BITMANIPULATION_H

#include <limits>

namespace simdutf {
namespace lsx {
namespace {

simdutf_really_inline int count_ones(uint64_t input_num) {
  return __lsx_vpickve2gr_w(__lsx_vpcnt_d(__lsx_vreplgr2vr_d(input_num)), 0);
}

#if SIMDUTF_NEED_TRAILING_ZEROES
simdutf_really_inline int trailing_zeroes(uint64_t input_num) {
  return __builtin_ctzll(input_num);
}
#endif

} // unnamed namespace
} // namespace lsx
} // namespace simdutf

#endif // SIMDUTF_LSX_BITMANIPULATION_H
/* end file src\simdutf\lsx\bitmanipulation.h */
/* begin file src\simdutf\lsx\simd.h */
#ifndef SIMDUTF_LSX_SIMD_H
#define SIMDUTF_LSX_SIMD_H


namespace simdutf {
namespace lsx {
namespace {
namespace simd {

template <typename T> struct simd8;

//
// Base class of simd8<uint8_t> and simd8<bool>, both of which use __m128i
// internally.
//
template <typename T, typename Mask = simd8<bool>> struct base_u8 {
  __m128i value;
  static const int SIZE = sizeof(value);

  // Conversion from/to SIMD register
  simdutf_really_inline base_u8(const __m128i _value) : value(_value) {}
  simdutf_really_inline operator const __m128i &() const { return this->value; }
  simdutf_really_inline operator __m128i &() { return this->value; }

  // Bit operations
  simdutf_really_inline simd8<T> operator|(const simd8<T> other) const {
    return __lsx_vor_v(this->value, other);
  }
  simdutf_really_inline simd8<T> operator&(const simd8<T> other) const {
    return __lsx_vand_v(this->value, other);
  }
  simdutf_really_inline simd8<T> operator^(const simd8<T> other) const {
    return __lsx_vxor_v(this->value, other);
  }
  simdutf_really_inline simd8<T> operator~() const { return *this ^ 0xFFu; }
  simdutf_really_inline simd8<T> &operator|=(const simd8<T> other) {
    auto this_cast = static_cast<simd8<T> *>(this);
    *this_cast = *this_cast | other;
    return *this_cast;
  }

  friend simdutf_really_inline Mask operator==(const simd8<T> lhs,
                                               const simd8<T> rhs) {
    return __lsx_vseq_b(lhs, rhs);
  }

  template <int N = 1>
  simdutf_really_inline simd8<T> prev(const simd8<T> prev_chunk) const {
    return __lsx_vor_v(__lsx_vbsll_v(this->value, N),
                       __lsx_vbsrl_v(prev_chunk.value, 16 - N));
  }
};

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd8<bool> : base_u8<bool> {
  typedef uint16_t bitmask_t;
  typedef uint32_t bitmask2_t;

  static simdutf_really_inline simd8<bool> splat(bool _value) {
    return __lsx_vreplgr2vr_b(uint8_t(-(!!_value)));
  }

  simdutf_really_inline simd8(const __m128i _value) : base_u8<bool>(_value) {}
  // False constructor
  simdutf_really_inline simd8() : simd8(__lsx_vldi(0)) {}
  // Splat constructor
  simdutf_really_inline simd8(bool _value) : simd8(splat(_value)) {}
  simdutf_really_inline void store(uint8_t dst[16]) const {
    return __lsx_vst(this->value, dst, 0);
  }

  simdutf_really_inline uint32_t to_bitmask() const {
    return __lsx_vpickve2gr_wu(__lsx_vmsknz_b(*this), 0);
  }
};

// Unsigned bytes
template <> struct simd8<uint8_t> : base_u8<uint8_t> {
  static simdutf_really_inline simd8<uint8_t> splat(uint8_t _value) {
    return __lsx_vreplgr2vr_b(_value);
  }
  static simdutf_really_inline simd8<uint8_t> zero() { return __lsx_vldi(0); }
  static simdutf_really_inline simd8<uint8_t> load(const uint8_t *values) {
    return __lsx_vld(values, 0);
  }
  simdutf_really_inline simd8(const __m128i _value)
      : base_u8<uint8_t>(_value) {}
  // Zero constructor
  simdutf_really_inline simd8() : simd8(zero()) {}
  // Array constructor
  simdutf_really_inline simd8(const uint8_t values[16]) : simd8(load(values)) {}
  // Splat constructor
  simdutf_really_inline simd8(uint8_t _value) : simd8(splat(_value)) {}
  // Member-by-member initialization
  simdutf_really_inline
  simd8(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4, uint8_t v5,
        uint8_t v6, uint8_t v7, uint8_t v8, uint8_t v9, uint8_t v10,
        uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15)
      : simd8((__m128i)v16u8{v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11,
                             v12, v13, v14, v15}) {}

  // Repeat 16 values as many times as necessary (usually for lookup tables)
  simdutf_really_inline static simd8<uint8_t>
  repeat_16(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4,
            uint8_t v5, uint8_t v6, uint8_t v7, uint8_t v8, uint8_t v9,
            uint8_t v10, uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14,
            uint8_t v15) {
    return simd8<uint8_t>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12,
                          v13, v14, v15);
  }

  // Store to array
  simdutf_really_inline void store(uint8_t dst[16]) const {
    return __lsx_vst(this->value, dst, 0);
  }

  // Order-specific operations
  simdutf_really_inline simd8<bool>
  operator>=(const simd8<uint8_t> other) const {
    return __lsx_vsle_bu(other, *this);
  }
  simdutf_really_inline simd8<bool>
  operator>(const simd8<uint8_t> other) const {
    return __lsx_vslt_bu(other, *this);
  }
  simdutf_really_inline simd8 &operator-=(const simd8<uint8_t> other) {
    value = __lsx_vsub_b(value, other.value);
    return *this;
  }
  // Same as >, but instead of guaranteeing all 1's == true, false = 0 and true
  // = nonzero. For ARM, returns all 1's.
  simdutf_really_inline simd8<uint8_t>
  gt_bits(const simd8<uint8_t> other) const {
    return simd8<uint8_t>(*this > other);
  }

  // Bit-specific operations
  simdutf_really_inline simd8<bool> any_bits_set(simd8<uint8_t> bits) const {
    return __lsx_vslt_bu(__lsx_vldi(0), __lsx_vand_v(this->value, bits));
  }
  simdutf_really_inline bool is_ascii() const {
    return __lsx_vpickve2gr_hu(__lsx_vmskgez_b(this->value), 0) == 0xFFFF;
  }

  simdutf_really_inline bool any_bits_set_anywhere() const {
    return __lsx_vpickve2gr_hu(__lsx_vmsknz_b(this->value), 0) > 0;
  }
  template <int N> simdutf_really_inline simd8<uint8_t> shr() const {
    return __lsx_vsrli_b(this->value, N);
  }
  template <int N> simdutf_really_inline simd8<uint8_t> shl() const {
    return __lsx_vslli_b(this->value, N);
  }

  // Perform a lookup assuming the value is between 0 and 16 (undefined behavior
  // for out of range values)
  template <typename L>
  simdutf_really_inline simd8<L> lookup_16(simd8<L> lookup_table) const {
    return lookup_table.apply_lookup_16_to(*this);
  }

  template <typename L>
  simdutf_really_inline simd8<L>
  lookup_16(L replace0, L replace1, L replace2, L replace3, L replace4,
            L replace5, L replace6, L replace7, L replace8, L replace9,
            L replace10, L replace11, L replace12, L replace13, L replace14,
            L replace15) const {
    return lookup_16(simd8<L>::repeat_16(
        replace0, replace1, replace2, replace3, replace4, replace5, replace6,
        replace7, replace8, replace9, replace10, replace11, replace12,
        replace13, replace14, replace15));
  }

  template <typename T>
  simdutf_really_inline simd8<uint8_t>
  apply_lookup_16_to(const simd8<T> original) const {
    __m128i original_tmp = __lsx_vand_v(original, __lsx_vldi(0x1f));
    return __lsx_vshuf_b(__lsx_vldi(0), *this, simd8<uint8_t>(original_tmp));
  }

  simdutf_really_inline uint64_t sum_bytes() const {
    const auto sum_u16 = __lsx_vhaddw_hu_bu(value, value);
    const auto sum_u32 = __lsx_vhaddw_wu_hu(sum_u16, sum_u16);
    const auto sum_u64 = __lsx_vhaddw_du_wu(sum_u32, sum_u32);

    return uint64_t(__lsx_vpickve2gr_du(sum_u64, 0)) +
           uint64_t(__lsx_vpickve2gr_du(sum_u64, 1));
  }
};

// Signed bytes
template <> struct simd8<int8_t> {
  __m128i value;

  static const int SIZE = sizeof(value);

  static simdutf_really_inline simd8<int8_t> splat(int8_t _value) {
    return __lsx_vreplgr2vr_b(_value);
  }
  static simdutf_really_inline simd8<int8_t> zero() { return __lsx_vldi(0); }
  static simdutf_really_inline simd8<int8_t> load(const int8_t values[16]) {
    return __lsx_vld(values, 0);
  }

  template <endianness big_endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *p) const {
    __m128i zero = __lsx_vldi(0);
    if (match_system(big_endian)) {
      __lsx_vst(__lsx_vilvl_b(zero, (__m128i)this->value),
                reinterpret_cast<uint16_t *>(p), 0);
      __lsx_vst(__lsx_vilvh_b(zero, (__m128i)this->value),
                reinterpret_cast<uint16_t *>(p + 8), 0);
    } else {
      __lsx_vst(__lsx_vilvl_b((__m128i)this->value, zero),
                reinterpret_cast<uint16_t *>(p), 0);
      __lsx_vst(__lsx_vilvh_b((__m128i)this->value, zero),
                reinterpret_cast<uint16_t *>(p + 8), 0);
    }
  }

  simdutf_really_inline void store_ascii_as_utf32(char32_t *p) const {
    __m128i zero = __lsx_vldi(0);
    __m128i in16low = __lsx_vilvl_b(zero, (__m128i)this->value);
    __m128i in16high = __lsx_vilvh_b(zero, (__m128i)this->value);
    __m128i in32_0 = __lsx_vilvl_h(zero, in16low);
    __m128i in32_1 = __lsx_vilvh_h(zero, in16low);
    __m128i in32_2 = __lsx_vilvl_h(zero, in16high);
    __m128i in32_3 = __lsx_vilvh_h(zero, in16high);
    __lsx_vst(in32_0, reinterpret_cast<uint32_t *>(p), 0);
    __lsx_vst(in32_1, reinterpret_cast<uint32_t *>(p + 4), 0);
    __lsx_vst(in32_2, reinterpret_cast<uint32_t *>(p + 8), 0);
    __lsx_vst(in32_3, reinterpret_cast<uint32_t *>(p + 12), 0);
  }

  // In places where the table can be reused, which is most uses in simdutf, it
  // is worth it to do 4 table lookups, as there is no direct zero extension
  // from u8 to u32.
  simdutf_really_inline void store_ascii_as_utf32_tbl(char32_t *p) const {
    const simd8<uint8_t> tb1{0, 255, 255, 255, 1, 255, 255, 255,
                             2, 255, 255, 255, 3, 255, 255, 255};
    const simd8<uint8_t> tb2{4, 255, 255, 255, 5, 255, 255, 255,
                             6, 255, 255, 255, 7, 255, 255, 255};
    const simd8<uint8_t> tb3{8,  255, 255, 255, 9,  255, 255, 255,
                             10, 255, 255, 255, 11, 255, 255, 255};
    const simd8<uint8_t> tb4{12, 255, 255, 255, 13, 255, 255, 255,
                             14, 255, 255, 255, 15, 255, 255, 255};

    // encourage store pairing and interleaving
    const auto shuf1 = this->apply_lookup_16_to(tb1);
    const auto shuf2 = this->apply_lookup_16_to(tb2);
    shuf1.store(reinterpret_cast<int8_t *>(p));
    shuf2.store(reinterpret_cast<int8_t *>(p + 4));

    const auto shuf3 = this->apply_lookup_16_to(tb3);
    const auto shuf4 = this->apply_lookup_16_to(tb4);
    shuf3.store(reinterpret_cast<int8_t *>(p + 8));
    shuf4.store(reinterpret_cast<int8_t *>(p + 12));
  }
  // Conversion from/to SIMD register
  simdutf_really_inline simd8(const __m128i _value) : value(_value) {}

  // Zero constructor
  simdutf_really_inline simd8() : simd8(zero()) {}
  // Splat constructor
  simdutf_really_inline simd8(int8_t _value) : simd8(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd8(const int8_t *values) : simd8(load(values)) {}

  // Store to array
  simdutf_really_inline void store(int8_t dst[16]) const {
    return __lsx_vst(value, dst, 0);
  }

  simdutf_really_inline operator simd8<uint8_t>() const {
    return ((__m128i)this->value);
  }

  simdutf_really_inline simd8<int8_t>
  operator|(const simd8<int8_t> other) const {
    return __lsx_vor_v((__m128i)value, (__m128i)other.value);
  }

  simdutf_really_inline bool is_ascii() const {
    return (__lsx_vpickve2gr_hu(__lsx_vmskgez_b((__m128i)this->value), 0) ==
            0xffff);
  }

  // Order-sensitive comparisons
  simdutf_really_inline simd8<bool> operator>(const simd8<int8_t> other) const {
    return __lsx_vslt_b((__m128i)other.value, (__m128i)value);
  }
  simdutf_really_inline simd8<bool> operator<(const simd8<int8_t> other) const {
    return __lsx_vslt_b((__m128i)value, (__m128i)other.value);
  }

  template <int N = 1>
  simdutf_really_inline simd8<int8_t>
  prev(const simd8<int8_t> prev_chunk) const {
    return __lsx_vor_v(__lsx_vbsll_v(this->value, N),
                       __lsx_vbsrl_v(prev_chunk.value, 16 - N));
  }

  template <typename T>
  simdutf_really_inline simd8<int8_t>
  apply_lookup_16_to(const simd8<T> original) const {
    __m128i original_tmp = __lsx_vand_v(original, __lsx_vldi(0x1f));
    return __lsx_vshuf_b(__lsx_vldi(0), (__m128i)this->value,
                         simd8<uint8_t>(original_tmp));
  }
};

template <typename T> struct simd8x64 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd8<T>);
  static_assert(
      NUM_CHUNKS == 4,
      "LoongArch kernel should use four registers per 64-byte block.");
  simd8<T> chunks[NUM_CHUNKS];

  simd8x64(const simd8x64<T> &o) = delete; // no copy allowed
  simd8x64<T> &
  operator=(const simd8<T> other) = delete; // no assignment allowed
  simd8x64() = delete;                      // no default constructor allowed

  simdutf_really_inline simd8x64(const simd8<T> chunk0, const simd8<T> chunk1,
                                 const simd8<T> chunk2, const simd8<T> chunk3)
      : chunks{chunk0, chunk1, chunk2, chunk3} {}
  simdutf_really_inline simd8x64(const T *ptr)
      : chunks{simd8<T>::load(ptr),
               simd8<T>::load(ptr + sizeof(simd8<T>) / sizeof(T)),
               simd8<T>::load(ptr + 2 * sizeof(simd8<T>) / sizeof(T)),
               simd8<T>::load(ptr + 3 * sizeof(simd8<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + sizeof(simd8<T>) * 0 / sizeof(T));
    this->chunks[1].store(ptr + sizeof(simd8<T>) * 1 / sizeof(T));
    this->chunks[2].store(ptr + sizeof(simd8<T>) * 2 / sizeof(T));
    this->chunks[3].store(ptr + sizeof(simd8<T>) * 3 / sizeof(T));
  }

  simdutf_really_inline simd8x64<T> &operator|=(const simd8x64<T> &other) {
    this->chunks[0] |= other.chunks[0];
    this->chunks[1] |= other.chunks[1];
    this->chunks[2] |= other.chunks[2];
    this->chunks[3] |= other.chunks[3];
    return *this;
  }

  simdutf_really_inline simd8<T> reduce_or() const {
    return (this->chunks[0] | this->chunks[1]) |
           (this->chunks[2] | this->chunks[3]);
  }

  simdutf_really_inline bool is_ascii() const { return reduce_or().is_ascii(); }

  template <endianness endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    this->chunks[0].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 0);
    this->chunks[1].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 1);
    this->chunks[2].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 2);
    this->chunks[3].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 3);
  }

  simdutf_really_inline void store_ascii_as_utf32(char32_t *ptr) const {
    this->chunks[0].store_ascii_as_utf32_tbl(ptr + sizeof(simd8<T>) * 0);
    this->chunks[1].store_ascii_as_utf32_tbl(ptr + sizeof(simd8<T>) * 1);
    this->chunks[2].store_ascii_as_utf32_tbl(ptr + sizeof(simd8<T>) * 2);
    this->chunks[3].store_ascii_as_utf32_tbl(ptr + sizeof(simd8<T>) * 3);
  }

  simdutf_really_inline uint64_t to_bitmask() const {
    __m128i mask = __lsx_vbsll_v(__lsx_vmsknz_b(this->chunks[3]), 6);
    mask = __lsx_vor_v(mask, __lsx_vbsll_v(__lsx_vmsknz_b(this->chunks[2]), 4));
    mask = __lsx_vor_v(mask, __lsx_vbsll_v(__lsx_vmsknz_b(this->chunks[1]), 2));
    mask = __lsx_vor_v(mask, __lsx_vmsknz_b(this->chunks[0]));
    return __lsx_vpickve2gr_du(mask, 0);
  }

  simdutf_really_inline uint64_t lt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] < mask, this->chunks[1] < mask,
                          this->chunks[2] < mask, this->chunks[3] < mask)
        .to_bitmask();
  }
  simdutf_really_inline uint64_t gt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] > mask, this->chunks[1] > mask,
                          this->chunks[2] > mask, this->chunks[3] > mask)
        .to_bitmask();
  }
  simdutf_really_inline uint64_t gteq_unsigned(const uint8_t m) const {
    const simd8<uint8_t> mask = simd8<uint8_t>::splat(m);
    return simd8x64<bool>(simd8<uint8_t>(this->chunks[0].value) >= mask,
                          simd8<uint8_t>(this->chunks[1].value) >= mask,
                          simd8<uint8_t>(this->chunks[2].value) >= mask,
                          simd8<uint8_t>(this->chunks[3].value) >= mask)
        .to_bitmask();
  }
}; // struct simd8x64<T>

/* begin file src\simdutf\lsx\simd16-inl.h */
template <typename T> struct simd16;

template <typename T, typename Mask = simd16<bool>> struct base_u16 {
  __m128i value;
  static const size_t SIZE = sizeof(value);
  static const size_t ELEMENTS = sizeof(value) / sizeof(T);

  // Conversion from/to SIMD register
  simdutf_really_inline base_u16() = default;
  simdutf_really_inline base_u16(const __m128i _value) : value(_value) {}
  // Bit operations
  simdutf_really_inline simd16<T> operator|(const simd16<T> other) const {
    return __lsx_vor_v(this->value, other.value);
  }
  simdutf_really_inline simd16<T> operator&(const simd16<T> other) const {
    return __lsx_vand_v(this->value, other.value);
  }
  simdutf_really_inline simd16<T> operator~() const {
    return __lsx_vxori_b(this->value, 0xFF);
  }

  friend simdutf_really_inline Mask operator==(const simd16<T> lhs,
                                               const simd16<T> rhs) {
    return __lsx_vseq_h(lhs.value, rhs.value);
  }

  template <unsigned N>
  simdutf_really_inline simd16<T> byte_right_shift() const {
    return __lsx_vbsrl_v(this->value, N);
  }

  simdutf_really_inline uint16_t first() const {
    return uint16_t(__lsx_vpickve2gr_w(value, 0));
  }
};

template <typename T, typename Mask = simd16<bool>>
struct base16 : base_u16<T> {
  simdutf_really_inline base16() : base_u16<T>() {}
  simdutf_really_inline base16(const __m128i _value) : base_u16<T>(_value) {}
  template <typename Pointer>
  simdutf_really_inline base16(const Pointer *ptr)
      : base16(__lsx_vld(ptr, 0)) {}

  static const int SIZE = sizeof(base_u16<T>::value);

  template <unsigned N = 1>
  simdutf_really_inline simd16<T> prev(const simd16<T> prev_chunk) const {
    return __lsx_vor_v(__lsx_vbsll_v(*this, N * 2),
                       __lsx_vbsrl_v(prev_chunk, 16 - N * 2));
  }
};

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd16<bool> : base16<bool> {
  static simdutf_really_inline simd16<bool> splat(bool _value) {
    return __lsx_vreplgr2vr_h(uint16_t(-(!!_value)));
  }

  simdutf_really_inline simd16() : base16() {}
  simdutf_really_inline simd16(const __m128i _value) : base16<bool>(_value) {}

  simdutf_really_inline bool is_zero() const { return __lsx_bz_v(this->value); }
};

template <typename T> struct base16_numeric : base16<T> {
  static simdutf_really_inline simd16<T> splat(T _value) {
    return __lsx_vreplgr2vr_h(_value);
  }
  static simdutf_really_inline simd16<T> zero() { return __lsx_vldi(0); }

  template <typename Pointer>
  static simdutf_really_inline simd16<T> load(const Pointer values) {
    return __lsx_vld(values, 0);
  }

  simdutf_really_inline base16_numeric(const __m128i _value)
      : base16<T>(_value) {}

  // Store to array
  simdutf_really_inline void store(T dst[8]) const {
    return __lsx_vst(this->value, dst, 0);
  }

  // Override to distinguish from bool version
  simdutf_really_inline simd16<T> operator~() const {
    return __lsx_vxori_b(this->value, 0xFF);
  }
};

// Unsigned code unitstemplate<>
template <> struct simd16<uint16_t> : base16_numeric<uint16_t> {
  simdutf_really_inline simd16(const __m128i _value)
      : base16_numeric<uint16_t>((__m128i)_value) {}

  // Splat constructor
  simdutf_really_inline simd16(uint16_t _value) : simd16(splat(_value)) {}

  // Array constructor
  simdutf_really_inline simd16(const uint16_t *values) : simd16(load(values)) {}
  simdutf_really_inline simd16(const char16_t *values)
      : simd16(load(reinterpret_cast<const uint16_t *>(values))) {}

  // Copy constructor
  simdutf_really_inline simd16(const simd16<bool> mask) : simd16(mask.value) {}

  // Order-specific operations
  simdutf_really_inline simd16 &operator+=(const simd16 other) {
    value = __lsx_vadd_h(value, other.value);
    return *this;
  }

  template <unsigned N>
  static simdutf_really_inline simd8<uint8_t>
  pack_shifted_right(const simd16<uint16_t> &v0, const simd16<uint16_t> &v1) {
    return __lsx_vssrlni_bu_h(v1.value, v0.value, N);
  }

  // Pack with the unsigned saturation of two uint16_t code units into single
  // uint8_t vector
  static simdutf_really_inline simd8<uint8_t> pack(const simd16<uint16_t> &v0,
                                                   const simd16<uint16_t> &v1) {
    return pack_shifted_right<0>(v0, v1);
  }

  // Change the endianness
  simdutf_really_inline simd16<uint16_t> swap_bytes() const {
    return __lsx_vshuf4i_b(this->value, 0b10110001);
  }

  simdutf_really_inline uint64_t sum() const {
    const auto sum_u32 = __lsx_vhaddw_wu_hu(value, value);
    const auto sum_u64 = __lsx_vhaddw_du_wu(sum_u32, sum_u32);

    return uint64_t(__lsx_vpickve2gr_du(sum_u64, 0)) +
           uint64_t(__lsx_vpickve2gr_du(sum_u64, 1));
  }
};

template <typename T> struct simd16x32 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd16<T>);
  static_assert(
      NUM_CHUNKS == 4,
      "LOONGARCH kernel should use four registers per 64-byte block.");
  simd16<T> chunks[NUM_CHUNKS];

  simd16x32(const simd16x32<T> &o) = delete; // no copy allowed
  simd16x32<T> &
  operator=(const simd16<T> other) = delete; // no assignment allowed
  simd16x32() = delete;                      // no default constructor allowed

  simdutf_really_inline
  simd16x32(const simd16<T> chunk0, const simd16<T> chunk1,
            const simd16<T> chunk2, const simd16<T> chunk3)
      : chunks{chunk0, chunk1, chunk2, chunk3} {}
  simdutf_really_inline simd16x32(const T *ptr)
      : chunks{simd16<T>::load(ptr),
               simd16<T>::load(ptr + sizeof(simd16<T>) / sizeof(T)),
               simd16<T>::load(ptr + 2 * sizeof(simd16<T>) / sizeof(T)),
               simd16<T>::load(ptr + 3 * sizeof(simd16<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + sizeof(simd16<T>) * 0 / sizeof(T));
    this->chunks[1].store(ptr + sizeof(simd16<T>) * 1 / sizeof(T));
    this->chunks[2].store(ptr + sizeof(simd16<T>) * 2 / sizeof(T));
    this->chunks[3].store(ptr + sizeof(simd16<T>) * 3 / sizeof(T));
  }

  simdutf_really_inline void swap_bytes() {
    this->chunks[0] = this->chunks[0].swap_bytes();
    this->chunks[1] = this->chunks[1].swap_bytes();
    this->chunks[2] = this->chunks[2].swap_bytes();
    this->chunks[3] = this->chunks[3].swap_bytes();
  }
}; // struct simd16x32<T>

simdutf_really_inline simd16<uint16_t> operator^(const simd16<uint16_t> a,
                                                 uint16_t b) {
  const auto bv = __lsx_vreplgr2vr_h(b);
  return __lsx_vxor_v(a.value, bv);
}

simdutf_really_inline simd16<bool> operator^(const simd16<bool> a,
                                             const simd16<bool> b) {
  return __lsx_vxor_v(a.value, b.value);
}

simdutf_really_inline simd16<uint16_t> min(const simd16<uint16_t> a,
                                           const simd16<uint16_t> b) {
  return __lsx_vmin_hu(a.value, b.value);
}

simdutf_really_inline simd16<uint16_t> as_vector_u16(const simd16<bool> x) {
  return x.value;
}
/* end file src\simdutf\lsx\simd16-inl.h */
/* begin file src\simdutf\lsx\simd32-inl.h */
template <typename T> struct simd32;

template <> struct simd32<uint32_t> {
  __m128i value;
  static const int SIZE = sizeof(value);
  static const int ELEMENTS = SIZE / sizeof(uint32_t);

  // constructors
  simdutf_really_inline simd32(__m128i v) : value(v) {}

  template <typename Ptr>
  simdutf_really_inline simd32(Ptr *ptr) : value(__lsx_vld(ptr, 0)) {}

  // in-place operators
  simdutf_really_inline simd32 &operator-=(const simd32 other) {
    value = __lsx_vsub_w(value, other.value);
    return *this;
  }

  // members
  simdutf_really_inline uint64_t sum() const {
    return uint64_t(__lsx_vpickve2gr_wu(value, 0)) +
           uint64_t(__lsx_vpickve2gr_wu(value, 1)) +
           uint64_t(__lsx_vpickve2gr_wu(value, 2)) +
           uint64_t(__lsx_vpickve2gr_wu(value, 3));
  }

  // static members
  static simdutf_really_inline simd32<uint32_t> splat(uint32_t x) {
    return __lsx_vreplgr2vr_w(x);
  }

  static simdutf_really_inline simd32<uint32_t> zero() {
    return __lsx_vrepli_w(0);
  }
};

// ------------------------------------------------------------

template <> struct simd32<bool> {
  __m128i value;
  static const int SIZE = sizeof(value);

  // constructors
  simdutf_really_inline simd32(__m128i v) : value(v) {}
};

// ------------------------------------------------------------

simdutf_really_inline simd32<uint32_t> operator&(const simd32<uint32_t> a,
                                                 const simd32<uint32_t> b) {
  return __lsx_vor_v(a.value, b.value);
}

simdutf_really_inline simd32<bool> operator<(const simd32<uint32_t> a,
                                             const simd32<uint32_t> b) {
  return __lsx_vslt_wu(a.value, b.value);
}

simdutf_really_inline simd32<bool> operator>(const simd32<uint32_t> a,
                                             const simd32<uint32_t> b) {
  return __lsx_vslt_wu(b.value, a.value);
}

// ------------------------------------------------------------

simdutf_really_inline simd32<uint32_t> as_vector_u32(const simd32<bool> v) {
  return v.value;
}
/* end file src\simdutf\lsx\simd32-inl.h */
/* begin file src\simdutf\lsx\simd64-inl.h */
template <typename T> struct simd64;

template <> struct simd64<uint64_t> {
  __m128i value;
  static const int SIZE = sizeof(value);
  static const int ELEMENTS = SIZE / sizeof(uint64_t);

  // constructors
  simdutf_really_inline simd64(__m128i v) : value(v) {}

  template <typename Ptr>
  simdutf_really_inline simd64(Ptr *ptr) : value(__lsx_vld(ptr, 0)) {}

  // in-place operators
  simdutf_really_inline simd64 &operator+=(const simd64 other) {
    value = __lsx_vadd_d(value, other.value);
    return *this;
  }

  // members
  simdutf_really_inline uint64_t sum() const {
    return uint64_t(__lsx_vpickve2gr_du(value, 0)) +
           uint64_t(__lsx_vpickve2gr_du(value, 1));
  }

  // static members
  static simdutf_really_inline simd64<uint64_t> zero() {
    return __lsx_vrepli_d(0);
  }
};

// ------------------------------------------------------------

template <> struct simd64<bool> {
  __m128i value;
  static const int SIZE = sizeof(value);

  // constructors
  simdutf_really_inline simd64(__m128i v) : value(v) {}
};

// ------------------------------------------------------------

simd64<uint64_t> sum_8bytes(const simd8<uint8_t> v) {
  const auto sum_u16 = __lsx_vhaddw_hu_bu(v, v);
  const auto sum_u32 = __lsx_vhaddw_wu_hu(sum_u16, sum_u16);
  const auto sum_u64 = __lsx_vhaddw_du_wu(sum_u32, sum_u32);

  return simd64<uint64_t>(sum_u64);
}
/* end file src\simdutf\lsx\simd64-inl.h */

} // namespace simd
} // unnamed namespace
} // namespace lsx
} // namespace simdutf

#endif // SIMDUTF_LSX_SIMD_H
/* end file src\simdutf\lsx\simd.h */

/* begin file src\simdutf\lsx\end.h */
#undef SIMDUTF_SIMD_HAS_UNSIGNED_CMP
/* end file src\simdutf\lsx\end.h */

#endif // SIMDUTF_IMPLEMENTATION_LSX

#endif // SIMDUTF_LSX_H
/* end file src\simdutf\lsx.h */
/* begin file src\simdutf\lasx.h */
#ifndef SIMDUTF_LASX_H
#define SIMDUTF_LASX_H

#ifdef SIMDUTF_FALLBACK_H
  #error "lasx.h must be included before fallback.h"
#endif


#ifndef SIMDUTF_IMPLEMENTATION_LASX
  #define SIMDUTF_IMPLEMENTATION_LASX (SIMDUTF_IS_LASX)
#endif
#if SIMDUTF_IMPLEMENTATION_LASX && SIMDUTF_IS_LASX
  #define SIMDUTF_CAN_ALWAYS_RUN_LASX 1
#else
  #define SIMDUTF_CAN_ALWAYS_RUN_LASX 0
#endif

#define SIMDUTF_CAN_ALWAYS_RUN_FALLBACK (SIMDUTF_IMPLEMENTATION_FALLBACK)

#if SIMDUTF_IMPLEMENTATION_LASX

namespace simdutf {
/**
 * Implementation for LoongArch ASX.
 */
namespace lasx {} // namespace lasx
} // namespace simdutf

/* begin file src\simdutf\lasx\implementation.h */
#ifndef SIMDUTF_LASX_IMPLEMENTATION_H
#define SIMDUTF_LASX_IMPLEMENTATION_H


namespace simdutf {
namespace lasx {

namespace {
using namespace simdutf;
}

class implementation final : public simdutf::implementation {
public:
  simdutf_really_inline implementation()
      : simdutf::implementation("lasx", "LOONGARCH ASX",
                                internal::instruction_set::LSX |
                                    internal::instruction_set::LASX) {}

  simdutf_warn_unused result base64_to_binary(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused result
  base64_to_binary(const char16_t *input, size_t length, char *output,
                   base64_options options,
                   last_chunk_handling_options last_chunk_options =
                       last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char16_t *input, size_t length, char *output,
      base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  size_t binary_to_base64(const char *input, size_t length, char *output,
                          base64_options options) const noexcept;
  const char *find(const char *start, const char *end,
                   char character) const noexcept;
  const char16_t *find(const char16_t *start, const char16_t *end,
                       char16_t character) const noexcept;
};

} // namespace lasx
} // namespace simdutf

#endif // SIMDUTF_LASX_IMPLEMENTATION_H
/* end file src\simdutf\lasx\implementation.h */

/* begin file src\simdutf\lasx\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "lasx"
// #define SIMDUTF_IMPLEMENTATION lasx
#define SIMDUTF_SIMD_HAS_UNSIGNED_CMP 1
/* end file src\simdutf\lasx\begin.h */

  // Declarations
/* begin file src\simdutf\lasx\intrinsics.h */
#ifndef SIMDUTF_LASX_INTRINSICS_H
#define SIMDUTF_LASX_INTRINSICS_H


// This should be the correct header whether
// you use visual studio or other compilers.
#include <lsxintrin.h>
#include <lasxintrin.h>

#if defined(__loongarch_asx)
  #ifdef __clang__
    #define VREGS_PREFIX "$vr"
    #define XREGS_PREFIX "$xr"
  #else // GCC
    #define VREGS_PREFIX "$f"
    #define XREGS_PREFIX "$f"
  #endif
  #define __ALL_REGS                                                           \
    "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,"  \
    "27,28,29,30,31"
// Convert __m128i to __m256i
static inline __m256i ____m256i(__m128i in) {
  __m256i out = __lasx_xvldi(0);
  __asm__ volatile(".irp i," __ALL_REGS "\n\t"
                   " .ifc %[out], " XREGS_PREFIX "\\i    \n\t"
                   "  .irp j," __ALL_REGS "\n\t"
                   "   .ifc %[in], " VREGS_PREFIX "\\j  \n\t"
                   "    xvpermi.q $xr\\i, $xr\\j, 0x0  \n\t"
                   "   .endif                           \n\t"
                   "  .endr                             \n\t"
                   " .endif                             \n\t"
                   ".endr                               \n\t"
                   : [out] "+f"(out)
                   : [in] "f"(in));
  return out;
}
// Convert two __m128i to __m256i
static inline __m256i lasx_set_q(__m128i inhi, __m128i inlo) {
  __m256i out;
  __asm__ volatile(".irp i," __ALL_REGS "\n\t"
                   " .ifc %[hi], " VREGS_PREFIX "\\i    \n\t"
                   "  .irp j," __ALL_REGS "\n\t"
                   "   .ifc %[lo], " VREGS_PREFIX "\\j  \n\t"
                   "    xvpermi.q $xr\\i, $xr\\j, 0x20  \n\t"
                   "   .endif                           \n\t"
                   "  .endr                             \n\t"
                   " .endif                             \n\t"
                   ".endr                               \n\t"
                   ".ifnc %[out], %[hi]                 \n\t"
                   ".irp i," __ALL_REGS "\n\t"
                   " .ifc %[out], " XREGS_PREFIX "\\i   \n\t"
                   "  .irp j," __ALL_REGS "\n\t"
                   "   .ifc %[hi], " VREGS_PREFIX "\\j  \n\t"
                   "    xvori.b $xr\\i, $xr\\j, 0       \n\t"
                   "   .endif                           \n\t"
                   "  .endr                             \n\t"
                   " .endif                             \n\t"
                   ".endr                               \n\t"
                   ".endif                              \n\t"
                   : [out] "=f"(out), [hi] "+f"(inhi)
                   : [lo] "f"(inlo));
  return out;
}
// Convert __m256i low part to __m128i
static inline __m128i lasx_extracti128_lo(__m256i in) {
  __m128i out;
  __asm__ volatile(".ifnc %[out], %[in]                 \n\t"
                   ".irp i," __ALL_REGS "\n\t"
                   " .ifc %[out], " VREGS_PREFIX "\\i   \n\t"
                   "  .irp j," __ALL_REGS "\n\t"
                   "   .ifc %[in], " XREGS_PREFIX "\\j  \n\t"
                   "    vori.b $vr\\i, $vr\\j, 0        \n\t"
                   "   .endif                           \n\t"
                   "  .endr                             \n\t"
                   " .endif                             \n\t"
                   ".endr                               \n\t"
                   ".endif                              \n\t"
                   : [out] "=f"(out)
                   : [in] "f"(in));
  return out;
}
// Convert __m256i high part to __m128i
static inline __m128i lasx_extracti128_hi(__m256i in) {
  __m128i out;
  __asm__ volatile(".irp i," __ALL_REGS "\n\t"
                   " .ifc %[out], " VREGS_PREFIX "\\i   \n\t"
                   "  .irp j," __ALL_REGS "\n\t"
                   "   .ifc %[in], " XREGS_PREFIX "\\j  \n\t"
                   "    xvpermi.q $xr\\i, $xr\\j, 0x11  \n\t"
                   "   .endif                           \n\t"
                   "  .endr                             \n\t"
                   " .endif                             \n\t"
                   ".endr                               \n\t"
                   : [out] "=f"(out)
                   : [in] "f"(in));
  return out;
}
#endif

/*
Encoding of argument for LoongArch64 xvldi instruction.  See:
https://jia.je/unofficial-loongarch-intrinsics-guide/lasx/misc/#__m256i-__lasx_xvldi-imm_n1024_1023-imm

1: imm[12:8]=0b10000: broadcast imm[7:0] as 32-bit elements to all lanes

2: imm[12:8]=0b10001: broadcast imm[7:0] << 8 as 32-bit elements to all lanes

3: imm[12:8]=0b10010: broadcast imm[7:0] << 16 as 32-bit elements to all lanes

4: imm[12:8]=0b10011: broadcast imm[7:0] << 24 as 32-bit elements to all lanes

5: imm[12:8]=0b10100: broadcast imm[7:0] as 16-bit elements to all lanes

6: imm[12:8]=0b10101: broadcast imm[7:0] << 8 as 16-bit elements to all lanes

7: imm[12:8]=0b10110: broadcast (imm[7:0] << 8) | 0xFF as 32-bit elements to all
lanes

8: imm[12:8]=0b10111: broadcast (imm[7:0] << 16) | 0xFFFF as 32-bit elements to
all lanes

9: imm[12:8]=0b11000: broadcast imm[7:0] as 8-bit elements to all lanes

10: imm[12:8]=0b11001: repeat each bit of imm[7:0] eight times, and broadcast
the result as 64-bit elements to all lanes
*/

namespace lasx_vldi {

template <uint16_t v> class const_u16 {
  constexpr static const uint8_t b0 = ((v >> 0 * 8) & 0xff);
  constexpr static const uint8_t b1 = ((v >> 1 * 8) & 0xff);

  constexpr static bool is_case5 = uint16_t(b0) == v;
  constexpr static bool is_case6 = (uint16_t(b1) << 8) == v;
  constexpr static bool is_case9 = (b0 == b1);
  constexpr static bool is_case10 =
      ((b0 == 0xff) || (b0 == 0x00)) && ((b1 == 0xff) || (b1 == 0x00));

public:
  constexpr static uint16_t operation = is_case5    ? 0b10100
                                        : is_case6  ? 0b10101
                                        : is_case9  ? 0b11000
                                        : is_case10 ? 0x11001
                                                    : 0xffff;

  constexpr static uint16_t byte =
      is_case5    ? b0
      : is_case6  ? b1
      : is_case9  ? b0
      : is_case10 ? ((b0 ? 0x55 : 0x00) | (b1 ? 0xaa : 0x00))
                  : 0xffff;

  constexpr static int value = int((operation << 8) | byte) - 8192;
  constexpr static bool valid = operation != 0xffff;
};

template <uint32_t v> class const_u32 {
  constexpr static const uint8_t b0 = (v & 0xff);
  constexpr static const uint8_t b1 = ((v >> 8) & 0xff);
  constexpr static const uint8_t b2 = ((v >> 16) & 0xff);
  constexpr static const uint8_t b3 = ((v >> 24) & 0xff);

  constexpr static bool is_case1 = (uint32_t(b0) == v);
  constexpr static bool is_case2 = ((uint32_t(b1) << 8) == v);
  constexpr static bool is_case3 = ((uint32_t(b2) << 16) == v);
  constexpr static bool is_case4 = ((uint32_t(b3) << 24) == v);
  constexpr static bool is_case5 = (b0 == b2) && (b1 == 0) && (b3 == 0);
  constexpr static bool is_case6 = (b1 == b3) && (b0 == 0) && (b2 == 0);
  constexpr static bool is_case7 = (b3 == 0) && (b2 == 0) && (b0 == 0xff);
  constexpr static bool is_case8 = (b3 == 0) && (b1 == 0xff) && (b0 == 0xff);
  constexpr static bool is_case9 = (b0 == b1) && (b0 == b2) && (b0 == b3);
  constexpr static bool is_case10 =
      ((b0 == 0xff) || (b0 == 0x00)) && ((b1 == 0xff) || (b1 == 0x00)) &&
      ((b2 == 0xff) || (b2 == 0x00)) && ((b3 == 0xff) || (b3 == 0x00));

public:
  constexpr static uint16_t operation = is_case1    ? 0b10000
                                        : is_case2  ? 0b10001
                                        : is_case3  ? 0b10010
                                        : is_case4  ? 0b10011
                                        : is_case5  ? 0b10100
                                        : is_case6  ? 0b10101
                                        : is_case7  ? 0b10110
                                        : is_case8  ? 0b10111
                                        : is_case9  ? 0b11000
                                        : is_case10 ? 0b11001
                                                    : 0xffff;

  constexpr static uint16_t byte =
      is_case1    ? b0
      : is_case2  ? b1
      : is_case3  ? b2
      : is_case4  ? b3
      : is_case5  ? b0
      : is_case6  ? b1
      : is_case7  ? b1
      : is_case8  ? b2
      : is_case9  ? b0
      : is_case10 ? ((b0 ? 0x11 : 0x00) | (b1 ? 0x22 : 0x00) |
                     (b2 ? 0x44 : 0x00) | (b3 ? 0x88 : 0x00))
                  : 0xffff;

  constexpr static int value = int((operation << 8) | byte) - 8192;
  constexpr static bool valid = operation != 0xffff;
};

template <uint64_t v> class const_u64 {
  constexpr static const uint8_t b0 = ((v >> 0 * 8) & 0xff);
  constexpr static const uint8_t b1 = ((v >> 1 * 8) & 0xff);
  constexpr static const uint8_t b2 = ((v >> 2 * 8) & 0xff);
  constexpr static const uint8_t b3 = ((v >> 3 * 8) & 0xff);
  constexpr static const uint8_t b4 = ((v >> 4 * 8) & 0xff);
  constexpr static const uint8_t b5 = ((v >> 5 * 8) & 0xff);
  constexpr static const uint8_t b6 = ((v >> 6 * 8) & 0xff);
  constexpr static const uint8_t b7 = ((v >> 7 * 8) & 0xff);

  constexpr static bool is_case10 =
      ((b0 == 0xff) || (b0 == 0x00)) && ((b1 == 0xff) || (b1 == 0x00)) &&
      ((b2 == 0xff) || (b2 == 0x00)) && ((b3 == 0xff) || (b3 == 0x00)) &&
      ((b4 == 0xff) || (b4 == 0x00)) && ((b5 == 0xff) || (b5 == 0x00)) &&
      ((b6 == 0xff) || (b6 == 0x00)) && ((b7 == 0xff) || (b7 == 0x00));

public:
  constexpr static bool is_32bit =
      ((v & 0xffffffff) == (v >> 32)) && const_u32<(v >> 32)>::value;
  constexpr static uint8_t op_32bit = const_u32<(v >> 32)>::operation;
  constexpr static uint8_t byte_32bit = const_u32<(v >> 32)>::byte;

  constexpr static uint16_t operation = is_32bit    ? op_32bit
                                        : is_case10 ? 0x11001
                                                    : 0xffff;

  constexpr static uint16_t byte =
      is_32bit ? byte_32bit
      : is_case10
          ? ((b0 ? 0x01 : 0x00) | (b1 ? 0x02 : 0x00) | (b2 ? 0x04 : 0x00) |
             (b3 ? 0x08 : 0x00) | (b4 ? 0x10 : 0x00) | (b5 ? 0x20 : 0x00) |
             (b6 ? 0x40 : 0x00) | (b7 ? 0x80 : 0x00))
          : 0xffff;

  constexpr static int value = int((operation << 8) | byte) - 8192;
  constexpr static bool valid = operation != 0xffff;
};

} // namespace lasx_vldi

// Uncomment when running under QEMU affected
// by bug https://gitlab.com/qemu-project/qemu/-/issues/2865
// Versions <= 9.2.2 are affected, likely anything newer is correct.
#ifndef QEMU_VLDI_BUG
// #define QEMU_VLDI_BUG 1
#endif

#ifdef QEMU_VLDI_BUG
  #define lasx_splat_u16(v) __lasx_xvreplgr2vr_h(v)
  #define lasx_splat_u32(v) __lasx_xvreplgr2vr_w(v)
#else
template <uint16_t x> constexpr __m256i lasx_splat_u16_aux() {
  constexpr bool is_imm10 = (int16_t(x) < 512) && (int16_t(x) > -512);
  constexpr uint16_t imm10 = is_imm10 ? x : 0;
  constexpr bool is_vldi = lasx_vldi::const_u16<x>::valid;
  constexpr int vldi_imm = is_vldi ? lasx_vldi::const_u16<x>::value : 0;

  return is_imm10  ? __lasx_xvrepli_h(int16_t(imm10))
         : is_vldi ? __lasx_xvldi(vldi_imm)
                   : __lasx_xvreplgr2vr_h(x);
}

template <uint32_t x> constexpr __m256i lasx_splat_u32_aux() {
  constexpr bool is_imm10 = (int32_t(x) < 512) && (int32_t(x) > -512);
  constexpr uint32_t imm10 = is_imm10 ? x : 0;
  constexpr bool is_vldi = lasx_vldi::const_u32<x>::valid;
  constexpr int vldi_imm = is_vldi ? lasx_vldi::const_u32<x>::value : 0;

  return is_imm10  ? __lasx_xvrepli_w(int32_t(imm10))
         : is_vldi ? __lasx_xvldi(vldi_imm)
                   : __lasx_xvreplgr2vr_w(x);
}

  #define lasx_splat_u16(v) lasx_splat_u16_aux<(v)>()
  #define lasx_splat_u32(v) lasx_splat_u32_aux<(v)>()
#endif // QEMU_VLDI_BUG

#endif //  SIMDUTF_LASX_INTRINSICS_H
/* end file src\simdutf\lasx\intrinsics.h */
/* begin file src\simdutf\lasx\bitmanipulation.h */
#ifndef SIMDUTF_LASX_BITMANIPULATION_H
#define SIMDUTF_LASX_BITMANIPULATION_H

#include <limits>

namespace simdutf {
namespace lasx {
namespace {

simdutf_really_inline int count_ones(uint64_t input_num) {
  return __lsx_vpickve2gr_w(__lsx_vpcnt_d(__lsx_vreplgr2vr_d(input_num)), 0);
}

#if SIMDUTF_NEED_TRAILING_ZEROES
simdutf_really_inline int trailing_zeroes(uint64_t input_num) {
  return __builtin_ctzll(input_num);
}
#endif

} // unnamed namespace
} // namespace lasx
} // namespace simdutf

#endif // SIMDUTF_LASX_BITMANIPULATION_H
/* end file src\simdutf\lasx\bitmanipulation.h */
/* begin file src\simdutf\lasx\simd.h */
#ifndef SIMDUTF_LASX_SIMD_H
#define SIMDUTF_LASX_SIMD_H


namespace simdutf {
namespace lasx {
namespace {
namespace simd {

__attribute__((aligned(32))) static const uint8_t prev_shuf_table[32][32] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
     0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {0,  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
     31, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
    {0,  0,  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
     30, 31, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
    {0,  0,  0,  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
     29, 30, 31, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
    {0,  0,  0,  0,  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
     28, 29, 30, 31, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
    {0,  0,  0,  0,  0,  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
     27, 28, 29, 30, 31, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
    {0,  0,  0,  0,  0,  0,  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
     26, 27, 28, 29, 30, 31, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {0,  0,  0,  0,  0,  0,  0,  0, 1, 2, 3, 4, 5, 6, 7, 8,
     25, 26, 27, 28, 29, 30, 31, 0, 1, 2, 3, 4, 5, 6, 7, 8},
    {0,  0,  0,  0,  0,  0,  0,  0,  0, 1, 2, 3, 4, 5, 6, 7,
     24, 25, 26, 27, 28, 29, 30, 31, 0, 1, 2, 3, 4, 5, 6, 7},
    {0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 1, 2, 3, 4, 5, 6,
     23, 24, 25, 26, 27, 28, 29, 30, 31, 0, 1, 2, 3, 4, 5, 6},
    {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 1, 2, 3, 4, 5,
     22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 0, 1, 2, 3, 4, 5},
    {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 1, 2, 3, 4,
     21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 0, 1, 2, 3, 4},
    {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 1, 2, 3,
     20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 0, 1, 2, 3},
    {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 1, 2,
     19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 0, 1, 2},
    {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 1,
     18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 0, 1},
    {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 0},
    {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
     15, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
    {14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
     14, 15, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
    {13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
     13, 14, 15, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
    {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
     12, 13, 14, 15, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
    {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
     11, 12, 13, 14, 15, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
    {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
     10, 11, 12, 13, 14, 15, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
    {9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
     9, 10, 11, 12, 13, 14, 15, 0,  0,  0,  0,  0,  0,  0,  0,  0},
    {8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
     8, 9, 10, 11, 12, 13, 14, 15, 0,  0,  0,  0,  0,  0,  0,  0},
    {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
     7, 8, 9, 10, 11, 12, 13, 14, 15, 0,  0,  0,  0,  0,  0,  0},
    {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
     6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,  0,  0,  0,  0,  0},
    {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
     5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,  0,  0,  0,  0},
    {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
     4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,  0,  0,  0},
    {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
     3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,  0,  0},
    {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
     2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,  0},
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
     1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
     0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
};

__attribute__((aligned(32))) static const uint8_t bitsel_mask_table[32][32] = {
    {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0,  0x0,  0x0,  0x0,  0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0,  0x0,  0x0,  0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0,  0x0,  0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0,  0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0,  0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0,  0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0,  0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0,  0x0},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0}};

// Forward-declared so they can be used by splat and friends.
template <typename Child> struct base {
  __m256i value;

  // Zero constructor
  simdutf_really_inline base() : value{__m256i()} {}

  // Conversion from SIMD register
  simdutf_really_inline base(const __m256i _value) : value(_value) {}
  // Conversion to SIMD register
  simdutf_really_inline operator const __m256i &() const { return this->value; }
  simdutf_really_inline operator __m256i &() { return this->value; }
  template <endianness big_endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    if (big_endian) {
      __m256i zero = __lasx_xvldi(0);
      __m256i in8 = __lasx_xvpermi_d(this->value, 0b11011000);
      __m256i inlow = __lasx_xvilvl_b(in8, zero);
      __m256i inhigh = __lasx_xvilvh_b(in8, zero);
      __lasx_xvst(inlow, reinterpret_cast<uint16_t *>(ptr), 0);
      __lasx_xvst(inhigh, reinterpret_cast<uint16_t *>(ptr), 32);
    } else {
      __m256i inlow = __lasx_vext2xv_hu_bu(this->value);
      __m256i inhigh = __lasx_vext2xv_hu_bu(
          __lasx_xvpermi_q(this->value, this->value, 0b00000001));
      __lasx_xvst(inlow, reinterpret_cast<__m256i *>(ptr), 0);
      __lasx_xvst(inhigh, reinterpret_cast<__m256i *>(ptr), 32);
    }
  }
  simdutf_really_inline void store_ascii_as_utf32(char32_t *ptr) const {
    __m256i in32_0 = __lasx_vext2xv_wu_bu(this->value);
    __lasx_xvst(in32_0, reinterpret_cast<uint32_t *>(ptr), 0);

    __m256i in8_1 = __lasx_xvpermi_d(this->value, 0b00000001);
    __m256i in32_1 = __lasx_vext2xv_wu_bu(in8_1);
    __lasx_xvst(in32_1, reinterpret_cast<uint32_t *>(ptr), 32);

    __m256i in8_2 = __lasx_xvpermi_d(this->value, 0b00000010);
    __m256i in32_2 = __lasx_vext2xv_wu_bu(in8_2);
    __lasx_xvst(in32_2, reinterpret_cast<uint32_t *>(ptr), 64);

    __m256i in8_3 = __lasx_xvpermi_d(this->value, 0b00000011);
    __m256i in32_3 = __lasx_vext2xv_wu_bu(in8_3);
    __lasx_xvst(in32_3, reinterpret_cast<uint32_t *>(ptr), 96);
  }
  // Bit operations
  simdutf_really_inline Child operator|(const Child other) const {
    return __lasx_xvor_v(this->value, other);
  }
  simdutf_really_inline Child operator&(const Child other) const {
    return __lasx_xvand_v(this->value, other);
  }
  simdutf_really_inline Child operator^(const Child other) const {
    return __lasx_xvxor_v(this->value, other);
  }
  simdutf_really_inline Child &operator|=(const Child other) {
    auto this_cast = static_cast<Child *>(this);
    *this_cast = *this_cast | other;
    return *this_cast;
  }
};

template <typename T> struct simd8;

template <typename T, typename Mask = simd8<bool>>
struct base8 : base<simd8<T>> {
  simdutf_really_inline base8() : base<simd8<T>>() {}
  simdutf_really_inline base8(const __m256i _value) : base<simd8<T>>(_value) {}
  friend simdutf_really_inline Mask operator==(const simd8<T> lhs,
                                               const simd8<T> rhs) {
    return __lasx_xvseq_b(lhs, rhs);
  }

  static const int SIZE = sizeof(base<T>::value);

  template <unsigned N = 1>
  simdutf_really_inline simd8<T> prev(const simd8<T> prev_chunk) const {
    static_assert(N <= 16, "unsupported shift value");

    if (!N)
      return this->value;

    __m256i zero = __lasx_xvldi(0);
    __m256i result, shuf;
    if (N < 16) {
      shuf = __lasx_xvld(prev_shuf_table[N], 0);

      result = __lasx_xvshuf_b(
          __lasx_xvpermi_q(this->value, this->value, 0b00000001), this->value,
          shuf);
      __m256i srl_prev = __lasx_xvbsrl_v(
          __lasx_xvpermi_q(zero, prev_chunk.value, 0b00110001), (16 - N));
      __m256i mask = __lasx_xvld(bitsel_mask_table[N], 0);
      result = __lasx_xvbitsel_v(result, srl_prev, mask);

      return result;
    } else if (N == 16) {
      return __lasx_xvpermi_q(this->value, prev_chunk.value, 0b00100001);
    }
  }
};

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd8<bool> : base8<bool> {
  static simdutf_really_inline simd8<bool> splat(bool _value) {
    return __lasx_xvreplgr2vr_b(uint8_t(-(!!_value)));
  }

  simdutf_really_inline simd8() : base8() {}
  simdutf_really_inline simd8(const __m256i _value) : base8<bool>(_value) {}
  // Splat constructor
  simdutf_really_inline simd8(bool _value) : base8<bool>(splat(_value)) {}

  simdutf_really_inline uint32_t to_bitmask() const {
    __m256i mask = __lasx_xvmsknz_b(this->value);
    uint32_t mask0 = __lasx_xvpickve2gr_wu(mask, 0);
    uint32_t mask1 = __lasx_xvpickve2gr_wu(mask, 4);
    return (mask0 | (mask1 << 16));
  }
  simdutf_really_inline bool any() const {
    if (__lasx_xbz_b(this->value))
      return false;
    return true;
  }
  simdutf_really_inline simd8<bool> operator~() const { return *this ^ true; }
};

template <typename T> struct base8_numeric : base8<T> {
  static simdutf_really_inline simd8<T> splat(T _value) {
    return __lasx_xvreplgr2vr_b(_value);
  }
  static simdutf_really_inline simd8<T> zero() { return __lasx_xvldi(0); }
  static simdutf_really_inline simd8<T> load(const T values[32]) {
    return __lasx_xvld(reinterpret_cast<const __m256i *>(values), 0);
  }
  // Repeat 16 values as many times as necessary (usually for lookup tables)
  static simdutf_really_inline simd8<T> repeat_16(T v0, T v1, T v2, T v3, T v4,
                                                  T v5, T v6, T v7, T v8, T v9,
                                                  T v10, T v11, T v12, T v13,
                                                  T v14, T v15) {
    return simd8<T>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13,
                    v14, v15, v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11,
                    v12, v13, v14, v15);
  }

  simdutf_really_inline base8_numeric() : base8<T>() {}
  simdutf_really_inline base8_numeric(const __m256i _value)
      : base8<T>(_value) {}

  // Store to array
  simdutf_really_inline void store(T dst[32]) const {
    return __lasx_xvst(this->value, reinterpret_cast<__m256i *>(dst), 0);
  }

  // Override to distinguish from bool version
  simdutf_really_inline simd8<T> operator~() const { return *this ^ 0xFFu; }

  // Perform a lookup assuming the value is between 0 and 16 (undefined behavior
  // for out of range values)
  template <typename L>
  simdutf_really_inline simd8<L> lookup_16(simd8<L> lookup_table) const {
    __m256i origin = __lasx_xvand_v(this->value, __lasx_xvldi(0x1f));
    return __lasx_xvshuf_b(__lasx_xvldi(0), lookup_table, origin);
  }

  template <typename L>
  simdutf_really_inline simd8<L>
  lookup_16(L replace0, L replace1, L replace2, L replace3, L replace4,
            L replace5, L replace6, L replace7, L replace8, L replace9,
            L replace10, L replace11, L replace12, L replace13, L replace14,
            L replace15) const {
    return lookup_16(simd8<L>::repeat_16(
        replace0, replace1, replace2, replace3, replace4, replace5, replace6,
        replace7, replace8, replace9, replace10, replace11, replace12,
        replace13, replace14, replace15));
  }
};

// Signed bytes
template <> struct simd8<int8_t> : base8_numeric<int8_t> {
  simdutf_really_inline simd8() : base8_numeric<int8_t>() {}
  simdutf_really_inline simd8(const __m256i _value)
      : base8_numeric<int8_t>(_value) {}

  // Splat constructor
  simdutf_really_inline simd8(int8_t _value) : simd8(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd8(const int8_t values[32]) : simd8(load(values)) {}
  simdutf_really_inline operator simd8<uint8_t>() const;
  simdutf_really_inline bool is_ascii() const {
    __m256i ascii_mask = __lasx_xvslti_b(this->value, 0);
    if (__lasx_xbnz_v(ascii_mask))
      return false;
    return true;
  }
  // Order-sensitive comparisons
  simdutf_really_inline simd8<bool> operator>(const simd8<int8_t> other) const {
    return __lasx_xvslt_b(other, this->value);
  }
  simdutf_really_inline simd8<bool> operator<(const simd8<int8_t> other) const {
    return __lasx_xvslt_b(this->value, other);
  }
};

// Unsigned bytes
template <> struct simd8<uint8_t> : base8_numeric<uint8_t> {
  simdutf_really_inline simd8() : base8_numeric<uint8_t>() {}
  simdutf_really_inline simd8(const __m256i _value)
      : base8_numeric<uint8_t>(_value) {}
  // Splat constructor
  simdutf_really_inline simd8(uint8_t _value) : simd8(splat(_value)) {}
  // Array constructor
  simdutf_really_inline simd8(const uint8_t values[32]) : simd8(load(values)) {}
  // Member-by-member initialization
  simdutf_really_inline
  simd8(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4, uint8_t v5,
        uint8_t v6, uint8_t v7, uint8_t v8, uint8_t v9, uint8_t v10,
        uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15,
        uint8_t v16, uint8_t v17, uint8_t v18, uint8_t v19, uint8_t v20,
        uint8_t v21, uint8_t v22, uint8_t v23, uint8_t v24, uint8_t v25,
        uint8_t v26, uint8_t v27, uint8_t v28, uint8_t v29, uint8_t v30,
        uint8_t v31)
      : simd8((__m256i)v32u8{v0,  v1,  v2,  v3,  v4,  v5,  v6,  v7,
                             v8,  v9,  v10, v11, v12, v13, v14, v15,
                             v16, v17, v18, v19, v20, v21, v22, v23,
                             v24, v25, v26, v27, v28, v29, v30, v31}) {}

  // Saturated math
  simdutf_really_inline simd8<uint8_t>
  saturating_sub(const simd8<uint8_t> other) const {
    return __lasx_xvssub_bu(this->value, other);
  }

  // Same as >, but only guarantees true is nonzero (< guarantees true = -1)
  simdutf_really_inline simd8<uint8_t>
  gt_bits(const simd8<uint8_t> other) const {
    return this->saturating_sub(other);
  }
  simdutf_really_inline simd8<bool>
  operator>=(const simd8<uint8_t> other) const {
    return __lasx_xvsle_bu(other, *this);
  }
  simdutf_really_inline simd8 &operator-=(const simd8<uint8_t> other) {
    value = __lasx_xvsub_b(value, other.value);
    return *this;
  }

  // Bit-specific operations
  simdutf_really_inline bool is_ascii() const {
    __m256i ascii_mask = __lasx_xvslti_b(this->value, 0);
    if (__lasx_xbnz_v(ascii_mask))
      return false;
    return true;
  }
  simdutf_really_inline bool any_bits_set_anywhere() const {
    if (__lasx_xbnz_v(this->value))
      return true;
    return false;
  }
  template <int N> simdutf_really_inline simd8<uint8_t> shr() const {
    return __lasx_xvsrli_b(this->value, N);
  }
  template <int N> simdutf_really_inline simd8<uint8_t> shl() const {
    return __lasx_xvslli_b(this->value, N);
  }

  simdutf_really_inline uint64_t sum_bytes() const {
    const auto sum_u16 = __lasx_xvhaddw_hu_bu(value, value);
    const auto sum_u32 = __lasx_xvhaddw_wu_hu(sum_u16, sum_u16);
    const auto sum_u64 = __lasx_xvhaddw_du_wu(sum_u32, sum_u32);

    return uint64_t(__lasx_xvpickve2gr_du(sum_u64, 0)) +
           uint64_t(__lasx_xvpickve2gr_du(sum_u64, 1)) +
           uint64_t(__lasx_xvpickve2gr_du(sum_u64, 2)) +
           uint64_t(__lasx_xvpickve2gr_du(sum_u64, 3));
  }
};
simdutf_really_inline simd8<int8_t>::operator simd8<uint8_t>() const {
  return this->value;
}

template <typename T> struct simd8x64 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd8<T>);
  static_assert(NUM_CHUNKS == 2,
                "LASX kernel should use two registers per 64-byte block.");
  simd8<T> chunks[NUM_CHUNKS];

  simd8x64(const simd8x64<T> &o) = delete; // no copy allowed
  simd8x64<T> &
  operator=(const simd8<T> other) = delete; // no assignment allowed
  simd8x64() = delete;                      // no default constructor allowed

  simdutf_really_inline simd8x64(const simd8<T> chunk0, const simd8<T> chunk1)
      : chunks{chunk0, chunk1} {}
  simdutf_really_inline simd8x64(const T *ptr)
      : chunks{simd8<T>::load(ptr),
               simd8<T>::load(ptr + sizeof(simd8<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + sizeof(simd8<T>) * 0 / sizeof(T));
    this->chunks[1].store(ptr + sizeof(simd8<T>) * 1 / sizeof(T));
  }

  simdutf_really_inline uint64_t to_bitmask() const {
    uint64_t r_lo = uint32_t(this->chunks[0].to_bitmask());
    uint64_t r_hi = this->chunks[1].to_bitmask();
    return r_lo | (r_hi << 32);
  }

  simdutf_really_inline simd8x64<T> &operator|=(const simd8x64<T> &other) {
    this->chunks[0] |= other.chunks[0];
    this->chunks[1] |= other.chunks[1];
    return *this;
  }

  simdutf_really_inline simd8<T> reduce_or() const {
    return this->chunks[0] | this->chunks[1];
  }

  simdutf_really_inline bool is_ascii() const {
    return this->reduce_or().is_ascii();
  }

  template <endianness endian>
  simdutf_really_inline void store_ascii_as_utf16(char16_t *ptr) const {
    this->chunks[0].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 0);
    this->chunks[1].template store_ascii_as_utf16<endian>(ptr +
                                                          sizeof(simd8<T>) * 1);
  }

  simdutf_really_inline void store_ascii_as_utf32(char32_t *ptr) const {
    this->chunks[0].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 0);
    this->chunks[1].store_ascii_as_utf32(ptr + sizeof(simd8<T>) * 1);
  }

  simdutf_really_inline uint64_t lt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] < mask, this->chunks[1] < mask)
        .to_bitmask();
  }

  simdutf_really_inline uint64_t gt(const T m) const {
    const simd8<T> mask = simd8<T>::splat(m);
    return simd8x64<bool>(this->chunks[0] > mask, this->chunks[1] > mask)
        .to_bitmask();
  }
  simdutf_really_inline uint64_t gteq_unsigned(const uint8_t m) const {
    const simd8<uint8_t> mask = simd8<uint8_t>::splat(m);
    return simd8x64<bool>((simd8<uint8_t>(__m256i(this->chunks[0])) >= mask),
                          (simd8<uint8_t>(__m256i(this->chunks[1])) >= mask))
        .to_bitmask();
  }
}; // struct simd8x64<T>

/* begin file src\simdutf\lasx\simd16-inl.h */
template <typename T> struct simd16;

template <typename T, typename Mask = simd16<bool>>
struct base16 : base<simd16<T>> {
  using bitmask_type = uint32_t;

  simdutf_really_inline base16() : base<simd16<T>>() {}
  simdutf_really_inline base16(const __m256i _value)
      : base<simd16<T>>(_value) {}
  template <typename Pointer>
  simdutf_really_inline base16(const Pointer *ptr)
      : base16(__lasx_xvld(reinterpret_cast<const __m256i *>(ptr), 0)) {}

  /// the size of vector in bytes
  static const int SIZE = sizeof(base<simd16<T>>::value);

  /// the number of elements of type T a vector can hold
  static const int ELEMENTS = SIZE / sizeof(T);
};

// SIMD byte mask type (returned by things like eq and gt)
template <> struct simd16<bool> : base16<bool> {
  static simdutf_really_inline simd16<bool> splat(bool _value) {
    return __lasx_xvreplgr2vr_h(uint16_t(-(!!_value)));
  }

  simdutf_really_inline simd16() : base16() {}
  simdutf_really_inline simd16(const __m256i _value) : base16<bool>(_value) {}
  // Splat constructor
  simdutf_really_inline simd16(bool _value) : base16<bool>(splat(_value)) {}

  simdutf_really_inline bitmask_type to_bitmask() const {
    __m256i mask = __lasx_xvmsknz_b(this->value);
    bitmask_type mask0 = __lasx_xvpickve2gr_wu(mask, 0);
    bitmask_type mask1 = __lasx_xvpickve2gr_wu(mask, 4);
    return (mask0 | (mask1 << 16));
  }
  simdutf_really_inline simd16<bool> operator~() const { return *this ^ true; }

  simdutf_really_inline bool is_zero() const {
    return __lasx_xbz_v(this->value);
  }

  template <unsigned N> simdutf_really_inline simd16 byte_right_shift() const {
    const auto t0 = __lasx_xvbsrl_v(this->value, N);
    const auto t1 = __lasx_xvpermi_q(this->value, __lasx_xvldi(0), 0b00000011);
    const auto t2 = __lasx_xvbsll_v(t1, 16 - N);
    const auto t3 = __lasx_xvor_v(t0, t2);
    return t3;
  }

  simdutf_really_inline uint16_t first() const {
    return uint16_t(__lasx_xvpickve2gr_w(value, 0));
  }
};

template <typename T> struct base16_numeric : base16<T> {
  static simdutf_really_inline simd16<T> splat(T _value) {
    return __lasx_xvreplgr2vr_h((uint16_t)_value);
  }
  static simdutf_really_inline simd16<T> zero() { return __lasx_xvldi(0); }
  template <typename Pointer>
  static simdutf_really_inline simd16<T> load(const Pointer values) {
    return __lasx_xvld(values, 0);
  }

  simdutf_really_inline base16_numeric() : base16<T>() {}
  simdutf_really_inline base16_numeric(const __m256i _value)
      : base16<T>(_value) {}

  // Store to array
  simdutf_really_inline void store(T dst[8]) const {
    return __lasx_xvst(this->value, reinterpret_cast<__m256i *>(dst), 0);
  }

  // Override to distinguish from bool version
  simdutf_really_inline simd16<T> operator~() const { return *this ^ 0xFFFFu; }
};

// Unsigned code units
template <> struct simd16<uint16_t> : base16_numeric<uint16_t> {
  simdutf_really_inline simd16() : base16_numeric<uint16_t>() {}
  simdutf_really_inline simd16(const __m256i _value)
      : base16_numeric<uint16_t>(_value) {}

  // Splat constructor
  simdutf_really_inline simd16(uint16_t _value) : simd16(splat(_value)) {}

  // Array constructor
  simdutf_really_inline simd16(const uint16_t *values) : simd16(load(values)) {}
  simdutf_really_inline simd16(const char16_t *values)
      : simd16(load(reinterpret_cast<const uint16_t *>(values))) {}

  // Order-specific operations
  simdutf_really_inline simd16 &operator+=(const simd16 other) {
    value = __lasx_xvadd_h(value, other.value);
    return *this;
  }

  // Change the endianness
  simdutf_really_inline simd16<uint16_t> swap_bytes() const {
    return __lasx_xvshuf4i_b(this->value, 0b10110001);
  }

  template <unsigned N>
  static simdutf_really_inline simd8<uint8_t>
  pack_shifted_right(const simd16<uint16_t> &v0, const simd16<uint16_t> &v1) {
    return __lasx_xvpermi_d(__lasx_xvssrlni_bu_h(v1.value, v0.value, N),
                            0b11011000);
  }

  // Pack with the unsigned saturation of two uint16_t code units into single
  // uint8_t vector
  static simdutf_really_inline simd8<uint8_t> pack(const simd16<uint16_t> &v0,
                                                   const simd16<uint16_t> &v1) {

    return pack_shifted_right<0>(v0, v1);
  }

  simdutf_really_inline uint64_t sum() const {
    const auto sum_u32 = __lasx_xvhaddw_wu_hu(value, value);
    const auto sum_u64 = __lasx_xvhaddw_du_wu(sum_u32, sum_u32);

    return uint64_t(__lasx_xvpickve2gr_du(sum_u64, 0)) +
           uint64_t(__lasx_xvpickve2gr_du(sum_u64, 1)) +
           uint64_t(__lasx_xvpickve2gr_du(sum_u64, 2)) +
           uint64_t(__lasx_xvpickve2gr_du(sum_u64, 3));
  }

  template <unsigned N> simdutf_really_inline simd16 byte_right_shift() const {
    return __lasx_xvbsrl_v(this->value, N);
  }
};

template <typename T> struct simd16x32 {
  static constexpr int NUM_CHUNKS = 64 / sizeof(simd16<T>);
  static_assert(NUM_CHUNKS == 2,
                "LASX kernel should use two registers per 64-byte block.");
  simd16<T> chunks[NUM_CHUNKS];

  simd16x32(const simd16x32<T> &o) = delete; // no copy allowed
  simd16x32<T> &
  operator=(const simd16<T> other) = delete; // no assignment allowed
  simd16x32() = delete;                      // no default constructor allowed

  simdutf_really_inline simd16x32(const simd16<T> chunk0,
                                  const simd16<T> chunk1)
      : chunks{chunk0, chunk1} {}
  simdutf_really_inline simd16x32(const T *ptr)
      : chunks{simd16<T>::load(ptr),
               simd16<T>::load(ptr + sizeof(simd16<T>) / sizeof(T))} {}

  simdutf_really_inline void store(T *ptr) const {
    this->chunks[0].store(ptr + sizeof(simd16<T>) * 0 / sizeof(T));
    this->chunks[1].store(ptr + sizeof(simd16<T>) * 1 / sizeof(T));
  }

  simdutf_really_inline void swap_bytes() {
    this->chunks[0] = this->chunks[0].swap_bytes();
    this->chunks[1] = this->chunks[1].swap_bytes();
  }
}; // struct simd16x32<T>

simdutf_really_inline simd16<uint16_t> min(const simd16<uint16_t> a,
                                           const simd16<uint16_t> b) {
  return __lasx_xvmin_hu(a.value, b.value);
}

simdutf_really_inline simd16<bool> operator==(const simd16<uint16_t> a,
                                              uint16_t b) {
  const auto bv = __lasx_xvreplgr2vr_h(b);
  return __lasx_xvseq_h(a.value, bv);
}

simdutf_really_inline simd16<uint16_t> as_vector_u16(const simd16<bool> x) {
  return x.value;
}

simdutf_really_inline simd16<uint16_t> operator&(const simd16<uint16_t> a,
                                                 uint16_t b) {
  const auto bv = __lasx_xvreplgr2vr_h(b);
  return __lasx_xvand_v(a.value, bv);
}

simdutf_really_inline simd16<uint16_t> operator&(const simd16<uint16_t> a,
                                                 const simd16<uint16_t> b) {
  return __lasx_xvand_v(a.value, b.value);
}

simdutf_really_inline simd16<uint16_t> operator^(const simd16<uint16_t> a,
                                                 uint16_t b) {
  const auto bv = __lasx_xvreplgr2vr_h(b);
  return __lasx_xvxor_v(a.value, bv);
}

simdutf_really_inline simd16<bool> operator^(const simd16<bool> a,
                                             const simd16<bool> b) {
  return __lasx_xvxor_v(a.value, b.value);
}
/* end file src\simdutf\lasx\simd16-inl.h */
/* begin file src\simdutf\lasx\simd32-inl.h */
template <typename T> struct simd32;

template <> struct simd32<uint32_t> {
  __m256i value;
  static const int SIZE = sizeof(value);
  static const int ELEMENTS = SIZE / sizeof(uint32_t);

  // constructors
  simdutf_really_inline simd32(__m256i v) : value(v) {}

  template <typename Ptr>
  simdutf_really_inline simd32(Ptr *ptr) : value(__lasx_xvld(ptr, 0)) {}

  // in-place operators
  simdutf_really_inline simd32 &operator-=(const simd32 other) {
    value = __lasx_xvsub_w(value, other.value);
    return *this;
  }

  // members
  simdutf_really_inline uint64_t sum() const {
    const auto odd = __lasx_xvsrli_d(value, 32);
    const auto even = __lasx_xvand_v(value, __lasx_xvreplgr2vr_d(0xffffffff));

    const auto sum64 = __lasx_xvadd_d(odd, even);

    return uint64_t(__lasx_xvpickve2gr_du(sum64, 0)) +
           uint64_t(__lasx_xvpickve2gr_du(sum64, 1)) +
           uint64_t(__lasx_xvpickve2gr_du(sum64, 2)) +
           uint64_t(__lasx_xvpickve2gr_du(sum64, 3));
  }

  // static members
  static simdutf_really_inline simd32<uint32_t> splat(uint32_t x) {
    return __lasx_xvreplgr2vr_w(x);
  }

  static simdutf_really_inline simd32<uint32_t> zero() {
    return __lasx_xvrepli_w(0);
  }
};

// ------------------------------------------------------------

template <> struct simd32<bool> {
  __m256i value;
  static const int SIZE = sizeof(value);

  // constructors
  simdutf_really_inline simd32(__m256i v) : value(v) {}
};

// ------------------------------------------------------------

simdutf_really_inline simd32<uint32_t> operator&(const simd32<uint32_t> a,
                                                 const simd32<uint32_t> b) {
  return __lasx_xvor_v(a.value, b.value);
}

simdutf_really_inline simd32<bool> operator<(const simd32<uint32_t> a,
                                             const simd32<uint32_t> b) {
  return __lasx_xvslt_wu(a.value, b.value);
}

simdutf_really_inline simd32<bool> operator>(const simd32<uint32_t> a,
                                             const simd32<uint32_t> b) {
  return __lasx_xvslt_wu(b.value, a.value);
}

// ------------------------------------------------------------

simdutf_really_inline simd32<uint32_t> as_vector_u32(const simd32<bool> v) {
  return v.value;
}
/* end file src\simdutf\lasx\simd32-inl.h */
/* begin file src\simdutf\lasx\simd64-inl.h */
template <typename T> struct simd64;

template <> struct simd64<uint64_t> {
  __m256i value;
  static const int SIZE = sizeof(value);
  static const int ELEMENTS = SIZE / sizeof(uint64_t);

  // constructors
  simdutf_really_inline simd64(__m256i v) : value(v) {}

  template <typename Ptr>
  simdutf_really_inline simd64(Ptr *ptr) : value(__lasx_xvld(ptr, 0)) {}

  // in-place operators
  simdutf_really_inline simd64 &operator+=(const simd64 other) {
    value = __lasx_xvadd_d(value, other.value);
    return *this;
  }

  // members
  simdutf_really_inline uint64_t sum() const {
    return uint64_t(__lasx_xvpickve2gr_du(value, 0)) +
           uint64_t(__lasx_xvpickve2gr_du(value, 1)) +
           uint64_t(__lasx_xvpickve2gr_du(value, 2)) +
           uint64_t(__lasx_xvpickve2gr_du(value, 3));
  }

  // static members
  static simdutf_really_inline simd64<uint64_t> zero() {
    return __lasx_xvrepli_d(0);
  }
};

// ------------------------------------------------------------

template <> struct simd64<bool> {
  __m256i value;
  static const int SIZE = sizeof(value);

  // constructors
  simdutf_really_inline simd64(__m256i v) : value(v) {}
};

// ------------------------------------------------------------

simd64<uint64_t> sum_8bytes(const simd8<uint8_t> v) {
  const auto sum_u16 = __lasx_xvhaddw_hu_bu(v, v);
  const auto sum_u32 = __lasx_xvhaddw_wu_hu(sum_u16, sum_u16);
  const auto sum_u64 = __lasx_xvhaddw_du_wu(sum_u32, sum_u32);

  return simd64<uint64_t>(sum_u64);
}
/* end file src\simdutf\lasx\simd64-inl.h */

} // namespace simd
} // unnamed namespace
} // namespace lasx
} // namespace simdutf

#endif // SIMDUTF_LASX_SIMD_H
/* end file src\simdutf\lasx\simd.h */

/* begin file src\simdutf\lasx\end.h */
#undef SIMDUTF_SIMD_HAS_UNSIGNED_CMP
/* end file src\simdutf\lasx\end.h */

#endif // SIMDUTF_IMPLEMENTATION_LASX

#endif // SIMDUTF_LASX_H
/* end file src\simdutf\lasx.h */
/* begin file src\simdutf\fallback.h */
#ifndef SIMDUTF_FALLBACK_H
#define SIMDUTF_FALLBACK_H


// Note that fallback.h is always imported last.

// Default Fallback to on unless a builtin implementation has already been
// selected.
#ifndef SIMDUTF_IMPLEMENTATION_FALLBACK
  #if SIMDUTF_CAN_ALWAYS_RUN_ARM64 || SIMDUTF_CAN_ALWAYS_RUN_ICELAKE ||        \
      SIMDUTF_CAN_ALWAYS_RUN_HASWELL || SIMDUTF_CAN_ALWAYS_RUN_WESTMERE ||     \
      SIMDUTF_CAN_ALWAYS_RUN_PPC64 || SIMDUTF_CAN_ALWAYS_RUN_RVV ||            \
      SIMDUTF_CAN_ALWAYS_RUN_LSX || SIMDUTF_CAN_ALWAYS_RUN_LASX
    #define SIMDUTF_IMPLEMENTATION_FALLBACK 0
  #else
    #define SIMDUTF_IMPLEMENTATION_FALLBACK 1
  #endif
#endif

#define SIMDUTF_CAN_ALWAYS_RUN_FALLBACK (SIMDUTF_IMPLEMENTATION_FALLBACK)

#if SIMDUTF_IMPLEMENTATION_FALLBACK

namespace simdutf {
/**
 * Fallback implementation (runs on any machine).
 */
namespace fallback {} // namespace fallback
} // namespace simdutf

/* begin file src\simdutf\fallback\implementation.h */
#ifndef SIMDUTF_FALLBACK_IMPLEMENTATION_H
#define SIMDUTF_FALLBACK_IMPLEMENTATION_H


namespace simdutf {
namespace fallback {

namespace {
using namespace simdutf;
}

class implementation final : public simdutf::implementation {
public:
  simdutf_really_inline implementation()
      : simdutf::implementation("fallback", "Generic fallback implementation",
                                0) {}

  simdutf_warn_unused result base64_to_binary(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused result
  base64_to_binary(const char16_t *input, size_t length, char *output,
                   base64_options options,
                   last_chunk_handling_options last_chunk_options =
                       last_chunk_handling_options::loose) const noexcept;
  simdutf_warn_unused full_result base64_to_binary_details(
      const char16_t *input, size_t length, char *output,
      base64_options options,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept;
  size_t binary_to_base64(const char *input, size_t length, char *output,
                          base64_options options) const noexcept;
  const char *find(const char *start, const char *end,
                   char character) const noexcept;
  const char16_t *find(const char16_t *start, const char16_t *end,
                       char16_t character) const noexcept;

};
} // namespace fallback
} // namespace simdutf

#endif // SIMDUTF_FALLBACK_IMPLEMENTATION_H
/* end file src\simdutf\fallback\implementation.h */

/* begin file src\simdutf\fallback\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "fallback"
// #define SIMDUTF_IMPLEMENTATION fallback
/* end file src\simdutf\fallback\begin.h */

  // Declarations
/* begin file src\simdutf\fallback\bitmanipulation.h */
#ifndef SIMDUTF_FALLBACK_BITMANIPULATION_H
#define SIMDUTF_FALLBACK_BITMANIPULATION_H

#include <limits>

namespace simdutf {
namespace fallback {
namespace {} // unnamed namespace
} // namespace fallback
} // namespace simdutf

#endif // SIMDUTF_FALLBACK_BITMANIPULATION_H
/* end file src\simdutf\fallback\bitmanipulation.h */

/* begin file src\simdutf\fallback\end.h */
/* end file src\simdutf\fallback\end.h */

#endif // SIMDUTF_IMPLEMENTATION_FALLBACK
#endif // SIMDUTF_FALLBACK_H
/* end file src\simdutf\fallback.h */
#ifndef SIMDUTF_REGULAR_VISUAL_STUDIO
SIMDUTF_POP_DISABLE_WARNINGS
#endif

// The scalar routines should be included once.
/* begin file src\scalar\swap_bytes.h */
#ifndef SIMDUTF_SWAP_BYTES_H
#define SIMDUTF_SWAP_BYTES_H

namespace simdutf {
namespace scalar {

inline simdutf_warn_unused uint16_t u16_swap_bytes(const uint16_t word) {
  return uint16_t((word >> 8) | (word << 8));
}

inline simdutf_warn_unused uint32_t u32_swap_bytes(const uint32_t word) {
  return ((word >> 24) & 0xff) |      // move byte 3 to byte 0
         ((word << 8) & 0xff0000) |   // move byte 1 to byte 2
         ((word >> 8) & 0xff00) |     // move byte 2 to byte 1
         ((word << 24) & 0xff000000); // byte 0 to byte 3
}

} // namespace scalar
} // namespace simdutf

#endif
/* end file src\scalar\swap_bytes.h */
/* begin file src\scalar\base64.h */
#ifndef SIMDUTF_BASE64_H
#define SIMDUTF_BASE64_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>

namespace simdutf {
namespace scalar {
namespace {
namespace base64 {

// This function is not expected to be fast. Do not use in long loops.
// In most instances you should be using is_ignorable.
template <class char_type> bool is_ascii_white_space(char_type c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f';
}

template <class char_type> bool is_eight_byte(char_type c) {
  if (sizeof(char_type) == 1) {
    return true;
  }
  return uint8_t(c) == c;
}

template <class char_type>
bool is_ignorable(char_type c, simdutf::base64_options options) {
  const uint8_t *to_base64 =
      (options & base64_default_or_url)
          ? tables::base64::to_base64_default_or_url_value
          : ((options & base64_url) ? tables::base64::to_base64_url_value
                                    : tables::base64::to_base64_value);
  const bool ignore_garbage =
      (options == base64_options::base64_url_accept_garbage) ||
      (options == base64_options::base64_default_accept_garbage) ||
      (options == base64_options::base64_default_or_url_accept_garbage);
  uint8_t code = to_base64[uint8_t(c)];
  if (is_eight_byte(c) && code <= 63) {
    return false;
  }
  if (is_eight_byte(c) && code == 64) {
    return true;
  }
  return ignore_garbage;
}
template <class char_type>
bool is_base64(char_type c, simdutf::base64_options options) {
  const uint8_t *to_base64 =
      (options & base64_default_or_url)
          ? tables::base64::to_base64_default_or_url_value
          : ((options & base64_url) ? tables::base64::to_base64_url_value
                                    : tables::base64::to_base64_value);
  uint8_t code = to_base64[uint8_t(c)];
  if (is_eight_byte(c) && code <= 63) {
    return true;
  }
  return false;
}

template <class char_type>
bool is_base64_or_padding(char_type c, simdutf::base64_options options) {
  const uint8_t *to_base64 =
      (options & base64_default_or_url)
          ? tables::base64::to_base64_default_or_url_value
          : ((options & base64_url) ? tables::base64::to_base64_url_value
                                    : tables::base64::to_base64_value);
  if (c == '=') {
    return true;
  }
  uint8_t code = to_base64[uint8_t(c)];
  if (is_eight_byte(c) && code <= 63) {
    return true;
  }
  return false;
}

template <class char_type>
bool is_ignorable_or_padding(char_type c, simdutf::base64_options options) {
  return is_ignorable(c, options) || c == '=';
}

struct reduced_input {
  size_t equalsigns;    // number of padding characters '=', typically 0, 1, 2.
  size_t equallocation; // location of the first padding character if any
  size_t srclen;        // length of the input buffer before padding
  size_t full_input_length; // length of the input buffer with padding but
                            // without ignorable characters
};

// find the end of the base64 input buffer
// It returns the number of padding characters, the location of the first
// padding character if any, the length of the input buffer before padding
// and the length of the input buffer with padding but without ignorable
// characters. The input buffer is not modified.
// The function assumes that there are at most two padding characters.
template <class char_type>
reduced_input find_end(const char_type *src, size_t srclen,
                       simdutf::base64_options options) {
  const uint8_t *to_base64 =
      (options & base64_default_or_url)
          ? tables::base64::to_base64_default_or_url_value
          : ((options & base64_url) ? tables::base64::to_base64_url_value
                                    : tables::base64::to_base64_value);
  const bool ignore_garbage =
      (options == base64_options::base64_url_accept_garbage) ||
      (options == base64_options::base64_default_accept_garbage) ||
      (options == base64_options::base64_default_or_url_accept_garbage);

  size_t equalsigns = 0;
  // skip trailing spaces
  while (!ignore_garbage && srclen > 0 &&
         scalar::base64::is_eight_byte(src[srclen - 1]) &&
         to_base64[uint8_t(src[srclen - 1])] == 64) {
    srclen--;
  }
  size_t full_input_length = srclen;
  size_t equallocation =
      srclen; // location of the first padding character if any
  if (ignore_garbage) {
    // Technically, we don't need to find the first padding character, we can
    // just change our algorithms, but it adds substantial complexity.
    auto it = simdutf::find(src, src + srclen, '=');
    if (it != src + srclen) {
      equallocation = it - src;
      equalsigns = 1;
      srclen = equallocation;
      full_input_length = equallocation + 1;
    }
    return {equalsigns, equallocation, srclen, full_input_length};
  }
  if (!ignore_garbage && srclen > 0 && src[srclen - 1] == '=') {
    // This is the last '=' sign.
    equallocation = srclen - 1;
    srclen--;
    equalsigns = 1;
    // skip trailing spaces
    while (srclen > 0 && scalar::base64::is_eight_byte(src[srclen - 1]) &&
           to_base64[uint8_t(src[srclen - 1])] == 64) {
      srclen--;
    }
    if (srclen > 0 && src[srclen - 1] == '=') {
      // This is the second '=' sign.
      equallocation = srclen - 1;
      srclen--;
      equalsigns = 2;
    }
  }
  return {equalsigns, equallocation, srclen, full_input_length};
}

// Returns true upon success. The destination buffer must be large enough.
// This functions assumes that the padding (=) has been removed.
// if check_capacity is true, it will check that the destination buffer is
// large enough. If it is not, it will return OUTPUT_BUFFER_TOO_SMALL.
template <bool check_capacity, class char_type>
full_result base64_tail_decode_impl(
    char *dst, size_t outlen, const char_type *src, size_t length,
    size_t padding_characters, // number of padding characters
                               // '=', typically 0, 1, 2.
    base64_options options, last_chunk_handling_options last_chunk_options) {
  char *dstend = dst + outlen;
  (void)dstend;
  // This looks like 10 branches, but we expect the compiler to resolve this to
  // two branches (easily predicted):
  const uint8_t *to_base64 =
      (options & base64_default_or_url)
          ? tables::base64::to_base64_default_or_url_value
          : ((options & base64_url) ? tables::base64::to_base64_url_value
                                    : tables::base64::to_base64_value);
  const uint32_t *d0 =
      (options & base64_default_or_url)
          ? tables::base64::base64_default_or_url::d0
          : ((options & base64_url) ? tables::base64::base64_url::d0
                                    : tables::base64::base64_default::d0);
  const uint32_t *d1 =
      (options & base64_default_or_url)
          ? tables::base64::base64_default_or_url::d1
          : ((options & base64_url) ? tables::base64::base64_url::d1
                                    : tables::base64::base64_default::d1);
  const uint32_t *d2 =
      (options & base64_default_or_url)
          ? tables::base64::base64_default_or_url::d2
          : ((options & base64_url) ? tables::base64::base64_url::d2
                                    : tables::base64::base64_default::d2);
  const uint32_t *d3 =
      (options & base64_default_or_url)
          ? tables::base64::base64_default_or_url::d3
          : ((options & base64_url) ? tables::base64::base64_url::d3
                                    : tables::base64::base64_default::d3);
  const bool ignore_garbage =
      (options == base64_options::base64_url_accept_garbage) ||
      (options == base64_options::base64_default_accept_garbage) ||
      (options == base64_options::base64_default_or_url_accept_garbage);

  const char_type *srcend = src + length;
  const char_type *srcinit = src;
  const char *dstinit = dst;

  uint32_t x;
  size_t idx;
  uint8_t buffer[4];
  while (true) {
    while (src + 4 <= srcend && is_eight_byte(src[0]) &&
           is_eight_byte(src[1]) && is_eight_byte(src[2]) &&
           is_eight_byte(src[3]) &&
           (x = d0[uint8_t(src[0])] | d1[uint8_t(src[1])] |
                d2[uint8_t(src[2])] | d3[uint8_t(src[3])]) < 0x01FFFFFF) {
      if (match_system(endianness::BIG)) {
        x = scalar::u32_swap_bytes(x);
      }
      if (check_capacity && dstend - dst < 3) {
        return {OUTPUT_BUFFER_TOO_SMALL, size_t(src - srcinit),
                size_t(dst - dstinit)};
      }
      std::memcpy(dst, &x, 3); // optimization opportunity: copy 4 bytes
      dst += 3;
      src += 4;
    }
    const char_type *srccur = src;
    idx = 0;
    // we need at least four characters.
#ifdef __clang__
    // If possible, we read four characters at a time. (It is an optimization.)
    if (ignore_garbage && src + 4 <= srcend) {
      char_type c0 = src[0];
      char_type c1 = src[1];
      char_type c2 = src[2];
      char_type c3 = src[3];

      uint8_t code0 = to_base64[uint8_t(c0)];
      uint8_t code1 = to_base64[uint8_t(c1)];
      uint8_t code2 = to_base64[uint8_t(c2)];
      uint8_t code3 = to_base64[uint8_t(c3)];

      buffer[idx] = code0;
      idx += (is_eight_byte(c0) && code0 <= 63);
      buffer[idx] = code1;
      idx += (is_eight_byte(c1) && code1 <= 63);
      buffer[idx] = code2;
      idx += (is_eight_byte(c2) && code2 <= 63);
      buffer[idx] = code3;
      idx += (is_eight_byte(c3) && code3 <= 63);
      src += 4;
    }
#endif
    while ((idx < 4) && (src < srcend)) {
      char_type c = *src;

      uint8_t code = to_base64[uint8_t(c)];
      buffer[idx] = uint8_t(code);
      if (is_eight_byte(c) && code <= 63) {
        idx++;
      } else if (!ignore_garbage &&
                 (code > 64 || !scalar::base64::is_eight_byte(c))) {
        return {INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit)};
      } else {
        // We have a space or a newline or garbage. We ignore it.
      }
      src++;
    }
    if (idx != 4) {
      simdutf_log_assert(idx < 4, "idx should be less than 4");
      // The idea here is that in loose mode,
      // if there is padding at all, it must be used
      // to form 4-wise chunk. However, in loose mode,
      // we do accept no padding at all.
      if (!ignore_garbage &&
          last_chunk_options == last_chunk_handling_options::loose &&
          (idx >= 2) && padding_characters > 0 &&
          ((idx + padding_characters) & 3) != 0) {
        return {INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit), true};
      } else

        // The idea here is that in strict mode, we do not want to accept
        // incomplete base64 chunks. So if the chunk was otherwise valid, we
        // return BASE64_INPUT_REMAINDER.
        if (!ignore_garbage &&
            last_chunk_options == last_chunk_handling_options::strict &&
            (idx >= 2) && ((idx + padding_characters) & 3) != 0) {
          // The partial chunk was at src - idx
          return {BASE64_INPUT_REMAINDER, size_t(src - srcinit),
                  size_t(dst - dstinit), true};
        } else
          // If there is a partial chunk with insufficient padding, with
          // stop_before_partial, we need to just ignore it. In "only full"
          // mode, skip the minute there are padding characters.
          if ((last_chunk_options ==
                   last_chunk_handling_options::stop_before_partial &&
               (padding_characters + idx < 4) &&
               (idx >= 2 || padding_characters == 0)) ||
              (last_chunk_options ==
                   last_chunk_handling_options::only_full_chunks &&
               (idx >= 2 || padding_characters == 0))) {
            // Rewind src to before partial chunk

            // partial means that we are *not* going to consume the read
            // characters. We need to rewind the src pointer.
            src = srccur;
            // adjust, skipping ignorable characters
            for (; src < srcend; src++) {
              char_type c = *src;
              uint8_t code = to_base64[uint8_t(c)];
              if (is_eight_byte(c) && code <= 63) {
                break;
              }
            }
            return {SUCCESS, size_t(src - srcinit), size_t(dst - dstinit)};
          } else {
            if (idx == 2) {
              uint32_t triple = (uint32_t(buffer[0]) << 3 * 6) +
                                (uint32_t(buffer[1]) << 2 * 6);
              if (!ignore_garbage &&
                  (last_chunk_options == last_chunk_handling_options::strict) &&
                  (triple & 0xffff)) {
                return {BASE64_EXTRA_BITS, size_t(src - srcinit),
                        size_t(dst - dstinit)};
              }
              if (check_capacity && dstend - dst < 1) {
                return {OUTPUT_BUFFER_TOO_SMALL, size_t(srccur - srcinit),
                        size_t(dst - dstinit)};
              }
              if (match_system(endianness::BIG)) {
                triple <<= 8;
                std::memcpy(dst, &triple, 1);
              } else {
                triple = scalar::u32_swap_bytes(triple);
                triple >>= 8;
                std::memcpy(dst, &triple, 1);
              }
              dst += 1;
            } else if (idx == 3) {
              uint32_t triple = (uint32_t(buffer[0]) << 3 * 6) +
                                (uint32_t(buffer[1]) << 2 * 6) +
                                (uint32_t(buffer[2]) << 1 * 6);
              if (!ignore_garbage &&
                  (last_chunk_options == last_chunk_handling_options::strict) &&
                  (triple & 0xff)) {
                return {BASE64_EXTRA_BITS, size_t(src - srcinit),
                        size_t(dst - dstinit)};
              }
              if (check_capacity && dstend - dst < 2) {
                return {OUTPUT_BUFFER_TOO_SMALL, size_t(srccur - srcinit),
                        size_t(dst - dstinit)};
              }
              if (match_system(endianness::BIG)) {
                triple <<= 8;
                std::memcpy(dst, &triple, 2);
              } else {
                triple = scalar::u32_swap_bytes(triple);
                triple >>= 8;
                std::memcpy(dst, &triple, 2);
              }
              dst += 2;
            } else if (!ignore_garbage && idx == 1 &&
                       (!is_partial(last_chunk_options) ||
                        (is_partial(last_chunk_options) &&
                         padding_characters > 0))) {
              return {BASE64_INPUT_REMAINDER, size_t(src - srcinit),
                      size_t(dst - dstinit)};
            } else if (!ignore_garbage && idx == 0 && padding_characters > 0) {
              return {INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                      size_t(dst - dstinit), true};
            }
            return {SUCCESS, size_t(src - srcinit), size_t(dst - dstinit)};
          }
    }
    if (check_capacity && dstend - dst < 3) {
      return {OUTPUT_BUFFER_TOO_SMALL, size_t(srccur - srcinit),
              size_t(dst - dstinit)};
    }
    uint32_t triple =
        (uint32_t(buffer[0]) << 3 * 6) + (uint32_t(buffer[1]) << 2 * 6) +
        (uint32_t(buffer[2]) << 1 * 6) + (uint32_t(buffer[3]) << 0 * 6);
    if (match_system(endianness::BIG)) {
      triple <<= 8;
      std::memcpy(dst, &triple, 3);
    } else {
      triple = scalar::u32_swap_bytes(triple);
      triple >>= 8;
      std::memcpy(dst, &triple, 3);
    }
    dst += 3;
  }
}

template <class char_type>
full_result
base64_tail_decode(char *dst, const char_type *src, size_t length,
                   size_t padding_characters, // number of padding characters
                                              // '=', typically 0, 1, 2.
                   base64_options options,
                   last_chunk_handling_options last_chunk_options) {
  return base64_tail_decode_impl<false>(dst, 0, src, length, padding_characters,
                                        options, last_chunk_options);
}

// like base64_tail_decode, but it will not write past the end of the output
// buffer. The outlen parameter is modified to reflect the number of bytes
// written. This functions assumes that the padding (=) has been removed.
//
template <class char_type>
full_result base64_tail_decode_safe(
    char *dst, size_t outlen, const char_type *src, size_t length,
    size_t padding_characters, // number of padding characters
                               // '=', typically 0, 1, 2.
    base64_options options, last_chunk_handling_options last_chunk_options) {
  return base64_tail_decode_impl<true>(dst, outlen, src, length,
                                       padding_characters, options,
                                       last_chunk_options);
}

inline full_result
patch_tail_result(full_result r, size_t previous_input, size_t previous_output,
                  size_t equallocation, size_t full_input_length,
                  last_chunk_handling_options last_chunk_options) {
  r.input_count += previous_input;
  r.output_count += previous_output;
  if (r.padding_error) {
    r.input_count = equallocation;
  }

  if (r.error == error_code::SUCCESS) {
    if (!is_partial(last_chunk_options)) {
      // A success when we are not in stop_before_partial mode.
      // means that we have consumed the whole input buffer.
      r.input_count = full_input_length;
    } else if (r.output_count % 3 != 0) {
      r.input_count = full_input_length;
    }
  }
  return r;
}

// Returns the number of bytes written. The destination buffer must be large
// enough. It will add padding (=) if needed.
size_t tail_encode_base64(char *dst, const char *src, size_t srclen,
                          base64_options options) {
  // By default, we use padding if we are not using the URL variant.
  // This is check with ((options & base64_url) == 0) which returns true if we
  // are not using the URL variant. However, we also allow 'inversion' of the
  // convention with the base64_reverse_padding option. If the
  // base64_reverse_padding option is set, we use padding if we are using the
  // URL variant, and we omit it if we are not using the URL variant. This is
  // checked with
  // ((options & base64_reverse_padding) == base64_reverse_padding).
  bool use_padding =
      ((options & base64_url) == 0) ^
      ((options & base64_reverse_padding) == base64_reverse_padding);
  // This looks like 3 branches, but we expect the compiler to resolve this to
  // a single branch:
  const char *e0 = (options & base64_url) ? tables::base64::base64_url::e0
                                          : tables::base64::base64_default::e0;
  const char *e1 = (options & base64_url) ? tables::base64::base64_url::e1
                                          : tables::base64::base64_default::e1;
  const char *e2 = (options & base64_url) ? tables::base64::base64_url::e2
                                          : tables::base64::base64_default::e2;
  char *out = dst;
  size_t i = 0;
  uint8_t t1, t2, t3;
  for (; i + 2 < srclen; i += 3) {
    t1 = uint8_t(src[i]);
    t2 = uint8_t(src[i + 1]);
    t3 = uint8_t(src[i + 2]);
    *out++ = e0[t1];
    *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
    *out++ = e1[((t2 & 0x0F) << 2) | ((t3 >> 6) & 0x03)];
    *out++ = e2[t3];
  }
  switch (srclen - i) {
  case 0:
    break;
  case 1:
    t1 = uint8_t(src[i]);
    *out++ = e0[t1];
    *out++ = e1[(t1 & 0x03) << 4];
    if (use_padding) {
      *out++ = '=';
      *out++ = '=';
    }
    break;
  default: /* case 2 */
    t1 = uint8_t(src[i]);
    t2 = uint8_t(src[i + 1]);
    *out++ = e0[t1];
    *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
    *out++ = e2[(t2 & 0x0F) << 2];
    if (use_padding) {
      *out++ = '=';
    }
  }
  return (size_t)(out - dst);
}

template <class char_type>
simdutf_warn_unused size_t maximal_binary_length_from_base64(
    const char_type *input, size_t length) noexcept {
  // We follow https://infra.spec.whatwg.org/#forgiving-base64-decode
  size_t padding = 0;
  if (length > 0) {
    if (input[length - 1] == '=') {
      padding++;
      if (length > 1 && input[length - 2] == '=') {
        padding++;
      }
    }
  }
  size_t actual_length = length - padding;
  if (actual_length % 4 <= 1) {
    return actual_length / 4 * 3;
  }
  // if we have a valid input, then the remainder must be 2 or 3 adding one or
  // two extra bytes.
  return actual_length / 4 * 3 + (actual_length % 4) - 1;
}

template <typename char_type>
simdutf_warn_unused full_result base64_to_binary_details_impl(
    const char_type *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) noexcept {
  const bool ignore_garbage =
      (options == base64_options::base64_url_accept_garbage) ||
      (options == base64_options::base64_default_accept_garbage) ||
      (options == base64_options::base64_default_or_url_accept_garbage);
  auto ri = simdutf::scalar::base64::find_end(input, length, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  length = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  (void)full_input_length;
  if (length == 0) {
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation, 0};
    }
    return {SUCCESS, 0, 0};
  }
  full_result r = scalar::base64::base64_tail_decode(
      output, input, length, equalsigns, options, last_chunk_options);
  r = scalar::base64::patch_tail_result(r, 0, 0, equallocation,
                                        full_input_length, last_chunk_options);
  if (!is_partial(last_chunk_options) && r.error == error_code::SUCCESS &&
      equalsigns > 0 && !ignore_garbage) {
    // additional checks
    if ((r.output_count % 3 == 0) ||
        ((r.output_count % 3) + 1 + equalsigns != 4)) {
      return {INVALID_BASE64_CHARACTER, equallocation, r.output_count};
    }
  }
  return r;
}

template <typename char_type>
simdutf_warn_unused full_result base64_to_binary_details_safe_impl(
    const char_type *input, size_t length, char *output, size_t outlen,
    base64_options options,
    last_chunk_handling_options last_chunk_options) noexcept {
  const bool ignore_garbage =
      (options == base64_options::base64_url_accept_garbage) ||
      (options == base64_options::base64_default_accept_garbage) ||
      (options == base64_options::base64_default_or_url_accept_garbage);
  auto ri = simdutf::scalar::base64::find_end(input, length, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  length = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  (void)full_input_length;
  if (length == 0) {
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation, 0};
    }
    return {SUCCESS, 0, 0};
  }
  full_result r = scalar::base64::base64_tail_decode_safe(
      output, outlen, input, length, equalsigns, options, last_chunk_options);
  r = scalar::base64::patch_tail_result(r, 0, 0, equallocation,
                                        full_input_length, last_chunk_options);
  if (!is_partial(last_chunk_options) && r.error == error_code::SUCCESS &&
      equalsigns > 0 && !ignore_garbage) {
    // additional checks
    if ((r.output_count % 3 == 0) ||
        ((r.output_count % 3) + 1 + equalsigns != 4)) {
      return {INVALID_BASE64_CHARACTER, equallocation, r.output_count};
    }
  }
  return r;
}

simdutf_warn_unused size_t
base64_length_from_binary(size_t length, base64_options options) noexcept {
  // By default, we use padding if we are not using the URL variant.
  // This is check with ((options & base64_url) == 0) which returns true if we
  // are not using the URL variant. However, we also allow 'inversion' of the
  // convention with the base64_reverse_padding option. If the
  // base64_reverse_padding option is set, we use padding if we are using the
  // URL variant, and we omit it if we are not using the URL variant. This is
  // checked with
  // ((options & base64_reverse_padding) == base64_reverse_padding).
  bool use_padding =
      ((options & base64_url) == 0) ^
      ((options & base64_reverse_padding) == base64_reverse_padding);
  if (!use_padding) {
    return length / 3 * 4 + ((length % 3) ? (length % 3) + 1 : 0);
  }
  return (length + 2) / 3 *
         4; // We use padding to make the length a multiple of 4.
}

// Return the length of the prefix that contains count base64 characters.
// Thus, if count is 3, the function returns the length of the prefix
// that contains 3 base64 characters.
// The function returns (size_t)-1 if there is not enough base64 characters in
// the input.
template <typename char_type>
simdutf_warn_unused size_t prefix_length(size_t count,
                                         simdutf::base64_options options,
                                         const char_type *input,
                                         size_t length) noexcept {
  size_t i = 0;
  while (i < length && is_ignorable(input[i], options)) {
    i++;
  }
  if (count == 0) {
    return i; // duh!
  }
  for (; i < length; i++) {
    if (is_ignorable(input[i], options)) {
      continue;
    }
    // We have a base64 character or a padding character.
    count--;
    if (count == 0) {
      return i + 1;
    }
  }
  simdutf_log_assert(false, "You never get here");

  return -1; // should never happen
}

} // namespace base64
} // unnamed namespace
} // namespace scalar
} // namespace simdutf

#endif
/* end file src\scalar\base64.h */

/* begin file src\implementation.cpp */
#include <initializer_list>
#include <climits>
#include <type_traits>
#if SIMDUTF_ATOMIC_REF
/* begin file src\scalar\atomic_util.h */
#ifndef SIMDUTF_ATOMIC_UTIL_H
#define SIMDUTF_ATOMIC_UTIL_H
#if SIMDUTF_ATOMIC_REF
  #include <atomic>
namespace simdutf {
namespace scalar {

// This function is a memcpy that uses atomic operations to read from the
// source.
inline void memcpy_atomic_read(char *dst, const char *src, size_t len) {
  static_assert(std::atomic_ref<char>::required_alignment == sizeof(char),
                "std::atomic_ref requires the same alignment as char_type");
  // We expect all 64-bit systems to be able to read 64-bit words from an
  // aligned memory region atomically. You might be able to do better on
  // specific systems, e.g., x64 systems can read 128-bit words atomically.
  constexpr size_t alignment = sizeof(uint64_t);

  // Lambda for atomic byte-by-byte copy
  auto bbb_memcpy_atomic_read = [](char *bytedst, const char *bytesrc,
                                   size_t bytelen) noexcept {
    char *mutable_src = const_cast<char *>(bytesrc);
    for (size_t j = 0; j < bytelen; ++j) {
      bytedst[j] =
          std::atomic_ref<char>(mutable_src[j]).load(std::memory_order_relaxed);
    }
  };

  // Handle unaligned start
  size_t offset = reinterpret_cast<std::uintptr_t>(src) % alignment;
  if (offset) {
    size_t to_align = std::min(len, alignment - offset);
    bbb_memcpy_atomic_read(dst, src, to_align);
    src += to_align;
    dst += to_align;
    len -= to_align;
  }

  // Process aligned 64-bit chunks
  while (len >= alignment) {
    auto *src_aligned = reinterpret_cast<uint64_t *>(const_cast<char *>(src));
    const auto dst_value =
        std::atomic_ref<uint64_t>(*src_aligned).load(std::memory_order_relaxed);
    std::memcpy(dst, &dst_value, sizeof(uint64_t));
    src += alignment;
    dst += alignment;
    len -= alignment;
  }

  // Handle remaining bytes
  if (len) {
    bbb_memcpy_atomic_read(dst, src, len);
  }
}

// This function is a memcpy that uses atomic operations to write to the
// destination.
inline void memcpy_atomic_write(char *dst, const char *src, size_t len) {
  static_assert(std::atomic_ref<char>::required_alignment == sizeof(char),
                "std::atomic_ref requires the same alignment as char");
  // We expect all 64-bit systems to be able to write 64-bit words to an aligned
  // memory region atomically.
  // You might be able to do better on specific systems, e.g., x64 systems can
  // write 128-bit words atomically.
  constexpr size_t alignment = sizeof(uint64_t);

  // Lambda for atomic byte-by-byte write
  auto bbb_memcpy_atomic_write = [](char *bytedst, const char *bytesrc,
                                    size_t bytelen) noexcept {
    for (size_t j = 0; j < bytelen; ++j) {
      std::atomic_ref<char>(bytedst[j])
          .store(bytesrc[j], std::memory_order_relaxed);
    }
  };

  // Handle unaligned start
  size_t offset = reinterpret_cast<std::uintptr_t>(dst) % alignment;
  if (offset) {
    size_t to_align = std::min(len, alignment - offset);
    bbb_memcpy_atomic_write(dst, src, to_align);
    dst += to_align;
    src += to_align;
    len -= to_align;
  }

  // Process aligned 64-bit chunks
  while (len >= alignment) {
    auto *dst_aligned = reinterpret_cast<uint64_t *>(dst);
    uint64_t src_val;
    std::memcpy(&src_val, src, sizeof(uint64_t)); // Non-atomic read from src
    std::atomic_ref<uint64_t>(*dst_aligned)
        .store(src_val, std::memory_order_relaxed);
    dst += alignment;
    src += alignment;
    len -= alignment;
  }

  // Handle remaining bytes
  if (len) {
    bbb_memcpy_atomic_write(dst, src, len);
  }
}
} // namespace scalar
} // namespace simdutf
#endif // SIMDUTF_ATOMIC_REF
#endif // SIMDUTF_ATOMIC_UTIL_H
/* end file src\scalar\atomic_util.h */
#endif

static_assert(sizeof(uint8_t) == sizeof(char),
              "simdutf requires that uint8_t be a char");
static_assert(sizeof(uint16_t) == sizeof(char16_t),
              "simdutf requires that char16_t be 16 bits");
static_assert(sizeof(uint32_t) == sizeof(char32_t),
              "simdutf requires that char32_t be 32 bits");
// next line is redundant, but it is kept to catch defective systems.
static_assert(CHAR_BIT == 8, "simdutf requires 8-bit bytes");

// Useful for debugging purposes
namespace simdutf {
namespace {

template <typename T> std::string toBinaryString(T b) {
  std::string binary = "";
  T mask = T(1) << (sizeof(T) * CHAR_BIT - 1);
  while (mask > 0) {
    binary += ((b & mask) == 0) ? '0' : '1';
    mask >>= 1;
  }
  return binary;
}
} // namespace
} // namespace simdutf

namespace simdutf {
bool implementation::supported_by_runtime_system() const {
  uint32_t required_instruction_sets = this->required_instruction_sets();
  uint32_t supported_instruction_sets =
      internal::detect_supported_architectures();
  return ((supported_instruction_sets & required_instruction_sets) ==
          required_instruction_sets);
}

simdutf_warn_unused size_t implementation::maximal_binary_length_from_base64(
    const char *input, size_t length) const noexcept {
  return scalar::base64::maximal_binary_length_from_base64(input, length);
}

simdutf_warn_unused size_t implementation::maximal_binary_length_from_base64(
    const char16_t *input, size_t length) const noexcept {
  return scalar::base64::maximal_binary_length_from_base64(input, length);
}

simdutf_warn_unused size_t implementation::base64_length_from_binary(
    size_t length, base64_options options) const noexcept {
  return scalar::base64::base64_length_from_binary(length, options);
}

namespace internal {
// When there is a single implementation, we should not pay a price
// for dispatching to the best implementation. We should just use the
// one we have. This is a compile-time check.
#define SIMDUTF_SINGLE_IMPLEMENTATION                                          \
  (SIMDUTF_IMPLEMENTATION_ICELAKE + SIMDUTF_IMPLEMENTATION_HASWELL +           \
       SIMDUTF_IMPLEMENTATION_WESTMERE + SIMDUTF_IMPLEMENTATION_ARM64 +        \
       SIMDUTF_IMPLEMENTATION_PPC64 + SIMDUTF_IMPLEMENTATION_LSX +             \
       SIMDUTF_IMPLEMENTATION_LASX + SIMDUTF_IMPLEMENTATION_FALLBACK ==        \
   1)

// Static array of known implementations. We are hoping these get baked into the
// executable without requiring a static initializer.

#if SIMDUTF_IMPLEMENTATION_ICELAKE
static const icelake::implementation *get_icelake_singleton() {
  static const icelake::implementation icelake_singleton{};
  return &icelake_singleton;
}
#endif
#if SIMDUTF_IMPLEMENTATION_HASWELL
static const haswell::implementation *get_haswell_singleton() {
  static const haswell::implementation haswell_singleton{};
  return &haswell_singleton;
}
#endif
#if SIMDUTF_IMPLEMENTATION_WESTMERE
static const westmere::implementation *get_westmere_singleton() {
  static const westmere::implementation westmere_singleton{};
  return &westmere_singleton;
}
#endif
#if SIMDUTF_IMPLEMENTATION_ARM64
static const arm64::implementation *get_arm64_singleton() {
  static const arm64::implementation arm64_singleton{};
  return &arm64_singleton;
}
#endif
#if SIMDUTF_IMPLEMENTATION_PPC64
static const ppc64::implementation *get_ppc64_singleton() {
  static const ppc64::implementation ppc64_singleton{};
  return &ppc64_singleton;
}
#endif
#if SIMDUTF_IMPLEMENTATION_RVV
static const rvv::implementation *get_rvv_singleton() {
  static const rvv::implementation rvv_singleton{};
  return &rvv_singleton;
}
#endif
#if SIMDUTF_IMPLEMENTATION_LSX
static const lsx::implementation *get_lsx_singleton() {
  static const lsx::implementation lsx_singleton{};
  return &lsx_singleton;
}
#endif
#if SIMDUTF_IMPLEMENTATION_LASX
static const lasx::implementation *get_lasx_singleton() {
  static const lasx::implementation lasx_singleton{};
  return &lasx_singleton;
}
#endif
#if SIMDUTF_IMPLEMENTATION_FALLBACK
static const fallback::implementation *get_fallback_singleton() {
  static const fallback::implementation fallback_singleton{};
  return &fallback_singleton;
}
#endif

#if SIMDUTF_SINGLE_IMPLEMENTATION
static const implementation *get_single_implementation() {
  return
  #if SIMDUTF_IMPLEMENTATION_ICELAKE
      get_icelake_singleton();
  #endif
  #if SIMDUTF_IMPLEMENTATION_HASWELL
  get_haswell_singleton();
  #endif
  #if SIMDUTF_IMPLEMENTATION_WESTMERE
  get_westmere_singleton();
  #endif
  #if SIMDUTF_IMPLEMENTATION_ARM64
  get_arm64_singleton();
  #endif
  #if SIMDUTF_IMPLEMENTATION_PPC64
  get_ppc64_singleton();
  #endif
  #if SIMDUTF_IMPLEMENTATION_LSX
  get_lsx_singleton();
  #endif
  #if SIMDUTF_IMPLEMENTATION_LASX
  get_lasx_singleton();
  #endif
  #if SIMDUTF_IMPLEMENTATION_FALLBACK
  get_fallback_singleton();
  #endif
}
#endif

/**
 * @private Detects best supported implementation on first use, and sets it
 */
class detect_best_supported_implementation_on_first_use final
    : public implementation {
public:
  std::string name() const noexcept final { return set_best()->name(); }
  std::string description() const noexcept final {
    return set_best()->description();
  }
  uint32_t required_instruction_sets() const noexcept final {
    return set_best()->required_instruction_sets();
  }

  simdutf_warn_unused result base64_to_binary(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_handling_options =
          last_chunk_handling_options::loose) const noexcept override {
    return set_best()->base64_to_binary(input, length, output, options,
                                        last_chunk_handling_options);
  }

  simdutf_warn_unused full_result base64_to_binary_details(
      const char *input, size_t length, char *output, base64_options options,
      last_chunk_handling_options last_chunk_handling_options =
          last_chunk_handling_options::loose) const noexcept override {
    return set_best()->base64_to_binary_details(input, length, output, options,
                                                last_chunk_handling_options);
  }

  simdutf_warn_unused result base64_to_binary(
      const char16_t *input, size_t length, char *output,
      base64_options options,
      last_chunk_handling_options last_chunk_handling_options =
          last_chunk_handling_options::loose) const noexcept override {
    return set_best()->base64_to_binary(input, length, output, options,
                                        last_chunk_handling_options);
  }

  simdutf_warn_unused full_result base64_to_binary_details(
      const char16_t *input, size_t length, char *output,
      base64_options options,
      last_chunk_handling_options last_chunk_handling_options =
          last_chunk_handling_options::loose) const noexcept override {
    return set_best()->base64_to_binary_details(input, length, output, options,
                                                last_chunk_handling_options);
  }

  size_t binary_to_base64(const char *input, size_t length, char *output,
                          base64_options options) const noexcept override {
    return set_best()->binary_to_base64(input, length, output, options);
  }

  const char *find(const char *start, const char *end,
                   char character) const noexcept override {
    return set_best()->find(start, end, character);
  }

  const char16_t *find(const char16_t *start, const char16_t *end,
                       char16_t character) const noexcept override {
    return set_best()->find(start, end, character);
  }

  simdutf_really_inline
  detect_best_supported_implementation_on_first_use() noexcept
      : implementation("best_supported_detector",
                       "Detects the best supported implementation and sets it",
                       0) {}

private:
  const implementation *set_best() const noexcept;
};

static_assert(std::is_trivially_destructible<
                  detect_best_supported_implementation_on_first_use>::value,
              "detect_best_supported_implementation_on_first_use should be "
              "trivially destructible");

static const std::initializer_list<const implementation *> &
get_available_implementation_pointers() {
  static const std::initializer_list<const implementation *>
      available_implementation_pointers{
#if SIMDUTF_IMPLEMENTATION_ICELAKE
          get_icelake_singleton(),
#endif
#if SIMDUTF_IMPLEMENTATION_HASWELL
          get_haswell_singleton(),
#endif
#if SIMDUTF_IMPLEMENTATION_WESTMERE
          get_westmere_singleton(),
#endif
#if SIMDUTF_IMPLEMENTATION_ARM64
          get_arm64_singleton(),
#endif
#if SIMDUTF_IMPLEMENTATION_PPC64
          get_ppc64_singleton(),
#endif
#if SIMDUTF_IMPLEMENTATION_RVV
          get_rvv_singleton(),
#endif
#if SIMDUTF_IMPLEMENTATION_LSX
          get_lsx_singleton(),
#endif
#if SIMDUTF_IMPLEMENTATION_LASX
          get_lasx_singleton(),
#endif
#if SIMDUTF_IMPLEMENTATION_FALLBACK
          get_fallback_singleton(),
#endif
      }; // available_implementation_pointers
  return available_implementation_pointers;
}

// So we can return UNSUPPORTED_ARCHITECTURE from the parser when there is no
// support
class unsupported_implementation final : public implementation {
public:

  simdutf_warn_unused result
  base64_to_binary(const char *, size_t, char *, base64_options,
                   last_chunk_handling_options) const noexcept override {
    return result(error_code::OTHER, 0);
  }

  simdutf_warn_unused full_result base64_to_binary_details(
      const char *, size_t, char *, base64_options,
      last_chunk_handling_options) const noexcept override {
    return full_result(error_code::OTHER, 0, 0);
  }

  simdutf_warn_unused result
  base64_to_binary(const char16_t *, size_t, char *, base64_options,
                   last_chunk_handling_options) const noexcept override {
    return result(error_code::OTHER, 0);
  }

  simdutf_warn_unused full_result base64_to_binary_details(
      const char16_t *, size_t, char *, base64_options,
      last_chunk_handling_options) const noexcept override {
    return full_result(error_code::OTHER, 0, 0);
  }

  size_t binary_to_base64(const char *, size_t, char *,
                          base64_options) const noexcept override {
    return 0;
  }
  const char *find(const char *, const char *, char) const noexcept override {
    return nullptr;
  }
  const char16_t *find(const char16_t *, const char16_t *,
                       char16_t) const noexcept override {
    return nullptr;
  }

  unsupported_implementation()
      : implementation("unsupported",
                       "Unsupported CPU (no detected SIMD instructions)", 0) {}
};

const unsupported_implementation *get_unsupported_singleton() {
  static const unsupported_implementation unsupported_singleton{};
  return &unsupported_singleton;
}
static_assert(std::is_trivially_destructible<unsupported_implementation>::value,
              "unsupported_singleton should be trivially destructible");

size_t available_implementation_list::size() const noexcept {
  return internal::get_available_implementation_pointers().size();
}
const implementation *const *
available_implementation_list::begin() const noexcept {
  return internal::get_available_implementation_pointers().begin();
}
const implementation *const *
available_implementation_list::end() const noexcept {
  return internal::get_available_implementation_pointers().end();
}
const implementation *
available_implementation_list::detect_best_supported() const noexcept {
  // They are prelisted in priority order, so we just go down the list
  uint32_t supported_instruction_sets =
      internal::detect_supported_architectures();
  for (const implementation *impl :
       internal::get_available_implementation_pointers()) {
    uint32_t required_instruction_sets = impl->required_instruction_sets();
    if ((supported_instruction_sets & required_instruction_sets) ==
        required_instruction_sets) {
      return impl;
    }
  }
  return get_unsupported_singleton(); // this should never happen?
}

const implementation *
detect_best_supported_implementation_on_first_use::set_best() const noexcept {
  SIMDUTF_PUSH_DISABLE_WARNINGS
  SIMDUTF_DISABLE_DEPRECATED_WARNING // Disable CRT_SECURE warning on MSVC:
                                     // manually verified this is safe
      char *force_implementation_name = getenv("SIMDUTF_FORCE_IMPLEMENTATION");
  SIMDUTF_POP_DISABLE_WARNINGS

  if (force_implementation_name) {
    auto force_implementation =
        get_available_implementations()[force_implementation_name];
    if (force_implementation) {
      return get_active_implementation() = force_implementation;
    } else {
      // Note: abort() and stderr usage within the library is forbidden.
      return get_active_implementation() = get_unsupported_singleton();
    }
  }
  return get_active_implementation() =
             get_available_implementations().detect_best_supported();
}

} // namespace internal

/**
 * The list of available implementations compiled into simdutf.
 */
SIMDUTF_DLLIMPORTEXPORT const internal::available_implementation_list &
get_available_implementations() {
  static const internal::available_implementation_list
      available_implementations{};
  return available_implementations;
}

/**
 * The active implementation.
 */
SIMDUTF_DLLIMPORTEXPORT internal::atomic_ptr<const implementation> &
get_active_implementation() {
#if SIMDUTF_SINGLE_IMPLEMENTATION
  // skip runtime detection
  static internal::atomic_ptr<const implementation> active_implementation{
      internal::get_single_implementation()};
  return active_implementation;
#else
  static const internal::detect_best_supported_implementation_on_first_use
      detect_best_supported_implementation_on_first_use_singleton;
  static internal::atomic_ptr<const implementation> active_implementation{
      &detect_best_supported_implementation_on_first_use_singleton};
  return active_implementation;
#endif
}

#if SIMDUTF_SINGLE_IMPLEMENTATION
const implementation *get_default_implementation() {
  return internal::get_single_implementation();
}
#else
internal::atomic_ptr<const implementation> &get_default_implementation() {
  return get_active_implementation();
}
#endif
#define SIMDUTF_GET_CURRENT_IMPLEMENTION

simdutf_warn_unused const char *find(const char *start, const char *end,
                                     char character) noexcept {
  return get_default_implementation()->find(start, end, character);
}
simdutf_warn_unused const char16_t *
find(const char16_t *start, const char16_t *end, char16_t character) noexcept {
  return get_default_implementation()->find(start, end, character);
}

simdutf_warn_unused size_t
maximal_binary_length_from_base64(const char *input, size_t length) noexcept {
  return get_default_implementation()->maximal_binary_length_from_base64(
      input, length);
}

simdutf_warn_unused result base64_to_binary(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_handling_options) noexcept {
  return get_default_implementation()->base64_to_binary(
      input, length, output, options, last_chunk_handling_options);
}

simdutf_warn_unused size_t maximal_binary_length_from_base64(
    const char16_t *input, size_t length) noexcept {
  return get_default_implementation()->maximal_binary_length_from_base64(
      input, length);
}

simdutf_warn_unused result base64_to_binary(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_handling_options) noexcept {
  return get_default_implementation()->base64_to_binary(
      input, length, output, options, last_chunk_handling_options);
}

template <typename chartype>
simdutf_warn_unused result slow_base64_to_binary_safe_impl(
    const chartype *input, size_t length, char *output, size_t &outlen,
    base64_options options,
    last_chunk_handling_options last_chunk_options) noexcept {
  const bool ignore_garbage = (options & base64_default_accept_garbage) != 0;
  auto ri = simdutf::scalar::base64::find_end(input, length, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  length = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  (void)full_input_length;
  if (length == 0) {
    outlen = 0;
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation};
    }
    return {SUCCESS, 0};
  }

  // The parameters of base64_tail_decode_safe are:
  // - dst: the output buffer
  // - outlen: the size of the output buffer
  // - srcr: the input buffer
  // - length: the size of the input buffer
  // - padded_characters: the number of padding characters
  // - options: the options for the base64 decoder
  // - last_chunk_options: the options for the last chunk
  // The function will return the number of bytes written to the output buffer
  // and the number of bytes read from the input buffer.
  // The function will also return an error code if the input buffer is not
  // valid base64.
  full_result r = scalar::base64::base64_tail_decode_safe(
      output, outlen, input, length, equalsigns, options, last_chunk_options);
  r = scalar::base64::patch_tail_result(r, 0, 0, equallocation,
                                        full_input_length, last_chunk_options);
  outlen = r.output_count;
  if (!is_partial(last_chunk_options) && r.error == error_code::SUCCESS &&
      equalsigns > 0) {
    // additional checks
    if ((outlen % 3 == 0) || ((outlen % 3) + 1 + equalsigns != 4)) {
      r.error = error_code::INVALID_BASE64_CHARACTER;
    }
  }
  return {r.error, r.input_count}; // we cannot return r itself because it gets
                                   // converted to error/output_count
}
simdutf_warn_unused bool base64_ignorable(char input,
                                          base64_options options) noexcept {
  return scalar::base64::is_ignorable(input, options);
}
simdutf_warn_unused bool base64_ignorable(char16_t input,
                                          base64_options options) noexcept {
  return scalar::base64::is_ignorable(input, options);
}
simdutf_warn_unused bool base64_valid(char input,
                                      base64_options options) noexcept {
  return scalar::base64::is_base64(input, options);
}
simdutf_warn_unused bool base64_valid(char16_t input,
                                      base64_options options) noexcept {
  return scalar::base64::is_base64(input, options);
}
simdutf_warn_unused bool
base64_valid_or_padding(char input, base64_options options) noexcept {
  return scalar::base64::is_base64_or_padding(input, options);
}
simdutf_warn_unused bool
base64_valid_or_padding(char16_t input, base64_options options) noexcept {
  return scalar::base64::is_base64_or_padding(input, options);
}

template <typename chartype>
simdutf_warn_unused result base64_to_binary_safe_impl(
    const chartype *input, size_t length, char *output, size_t &outlen,
    base64_options options,
    last_chunk_handling_options last_chunk_handling_options,
    bool decode_up_to_bad_char) noexcept {
  static_assert(std::is_same<chartype, char>::value ||
                    std::is_same<chartype, char16_t>::value,
                "Only char and char16_t are supported.");
  size_t remaining_input_length = length;
  size_t remaining_output_length = outlen;
  size_t input_position = 0;
  size_t output_position = 0;

  // We also do a first pass using the fast path to decode as much as possible
  size_t safe_input = (std::min)(
      remaining_input_length,
      base64_length_from_binary(remaining_output_length / 3 * 3, options));
  bool done_with_partial = (safe_input == remaining_input_length);
  simdutf::full_result r =
      get_default_implementation()->base64_to_binary_details(
          input + input_position, safe_input, output + output_position, options,
          done_with_partial
              ? last_chunk_handling_options
              : simdutf::last_chunk_handling_options::only_full_chunks);
  simdutf_log_assert(r.input_count <= safe_input,
                     "You should not read more than safe_input");
  simdutf_log_assert(r.output_count <= remaining_output_length,
                     "You should not write more than remaining_output_length");
  // Technically redundant, but we want to be explicit about it.
  input_position += r.input_count;
  output_position += r.output_count;
  remaining_input_length -= r.input_count;
  remaining_output_length -= r.output_count;
  if (r.error != simdutf::error_code::SUCCESS) {
    // There is an error. We return.
    if (decode_up_to_bad_char &&
        r.error == error_code::INVALID_BASE64_CHARACTER) {
      return slow_base64_to_binary_safe_impl(
          input, length, output, outlen, options, last_chunk_handling_options);
    }
    outlen = output_position;
    return {r.error, input_position};
  }
  if (done_with_partial) {
    // We are done. We have decoded everything.
    outlen = output_position;
    return {simdutf::error_code::SUCCESS, input_position};
  }

  // We have decoded some data, but we still have some data to decode.
  // We need to decode the rest of the input buffer.
  r = simdutf::scalar::base64::base64_to_binary_details_safe_impl(
      input + input_position, remaining_input_length, output + output_position,
      remaining_output_length, options, last_chunk_handling_options);

  input_position += r.input_count;
  output_position += r.output_count;
  remaining_input_length -= r.input_count;
  remaining_output_length -= r.output_count;

  if (r.error != simdutf::error_code::SUCCESS) {
    // There is an error. We return.
    if (decode_up_to_bad_char &&
        r.error == error_code::INVALID_BASE64_CHARACTER) {
      return slow_base64_to_binary_safe_impl(
          input, length, output, outlen, options, last_chunk_handling_options);
    }
    outlen = output_position;
    return {r.error, input_position};
  }
  outlen = output_position;
  return {simdutf::error_code::SUCCESS, input_position};
}

  #if SIMDUTF_ATOMIC_REF
size_t atomic_binary_to_base64(const char *input, size_t length, char *output,
                               base64_options options) noexcept {
  size_t retval = 0;
    #if defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION)
  // We use a smaller buffer during fuzzing to more easily detect bugs.
  constexpr size_t input_block_size = 128 * 3;
    #else
  // Arbitrary block sizes: 3KB for input which produces 4KB in output.
  constexpr size_t input_block_size = 1024 * 3;
    #endif
  std::array<char, input_block_size> inbuf;
  for (size_t i = 0; i < length; i += input_block_size) {
    const size_t current_block_size = std::min(input_block_size, length - i);
    simdutf::scalar::memcpy_atomic_read(inbuf.data(), input + i,
                                        current_block_size);
    const size_t written = binary_to_base64(inbuf.data(), current_block_size,
                                            output + retval, options);
    retval += written;
  }
  return retval;
}
  #endif // SIMDUTF_ATOMIC_REF

simdutf_warn_unused result
base64_to_binary_safe(const char *input, size_t length, char *output,
                      size_t &outlen, base64_options options,
                      last_chunk_handling_options last_chunk_handling_options,
                      bool decode_up_to_bad_char) noexcept {
  return base64_to_binary_safe_impl<char>(input, length, output, outlen,
                                          options, last_chunk_handling_options,
                                          decode_up_to_bad_char);
}
simdutf_warn_unused result
base64_to_binary_safe(const char16_t *input, size_t length, char *output,
                      size_t &outlen, base64_options options,
                      last_chunk_handling_options last_chunk_handling_options,
                      bool decode_up_to_bad_char) noexcept {
  return base64_to_binary_safe_impl<char16_t>(
      input, length, output, outlen, options, last_chunk_handling_options,
      decode_up_to_bad_char);
}

simdutf_warn_unused size_t
base64_length_from_binary(size_t length, base64_options options) noexcept {
  return get_default_implementation()->base64_length_from_binary(length,
                                                                 options);
}

size_t binary_to_base64(const char *input, size_t length, char *output,
                        base64_options options) noexcept {
  return get_default_implementation()->binary_to_base64(input, length, output,
                                                        options);
}

const implementation *builtin_implementation() {
  static const implementation *builtin_impl =
      get_available_implementations()[SIMDUTF_STRINGIFY(
          SIMDUTF_BUILTIN_IMPLEMENTATION)];
  return builtin_impl;
}

} // namespace simdutf
/* end file src\implementation.cpp */

SIMDUTF_PUSH_DISABLE_WARNINGS
SIMDUTF_DISABLE_UNDESIRED_WARNINGS

#if SIMDUTF_IMPLEMENTATION_ARM64
/* begin file src\arm64\implementation.cpp */
/* begin file src\simdutf\arm64\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "arm64"
// #define SIMDUTF_IMPLEMENTATION arm64
#define SIMDUTF_SIMD_HAS_BYTEMASK 1
/* end file src\simdutf\arm64\begin.h */
namespace simdutf {
namespace arm64 {
namespace {
#ifndef SIMDUTF_ARM64_H
  #error "arm64.h must be included"
#endif
using namespace simd;

/* begin file src\arm64\arm_base64.cpp */
/**
 * References and further reading:
 *
 * Wojciech Muła, Daniel Lemire, Base64 encoding and decoding at almost the
 * speed of a memory copy, Software: Practice and Experience 50 (2), 2020.
 * https://arxiv.org/abs/1910.05109
 *
 * Wojciech Muła, Daniel Lemire, Faster Base64 Encoding and Decoding using AVX2
 * Instructions, ACM Transactions on the Web 12 (3), 2018.
 * https://arxiv.org/abs/1704.00605
 *
 * Simon Josefsson. 2006. The Base16, Base32, and Base64 Data Encodings.
 * https://tools.ietf.org/html/rfc4648. (2006). Internet Engineering Task Force,
 * Request for Comments: 4648.
 *
 * Alfred Klomp. 2014a. Fast Base64 encoding/decoding with SSE vectorization.
 * http://www.alfredklomp.com/programming/sse-base64/. (2014).
 *
 * Alfred Klomp. 2014b. Fast Base64 stream encoder/decoder in C99, with SIMD
 * acceleration. https://github.com/aklomp/base64. (2014).
 *
 * Hanson Char. 2014. A Fast and Correct Base 64 Codec. (2014).
 * https://aws.amazon.com/blogs/developer/a-fast-and-correct-base-64-codec/
 *
 * Nick Kopp. 2013. Base64 Encoding on a GPU.
 * https://www.codeproject.com/Articles/276993/Base-Encoding-on-a-GPU. (2013).
 */

size_t encode_base64(char *dst, const char *src, size_t srclen,
                     base64_options options) {
  // credit: Wojciech Muła
  uint8_t *out = (uint8_t *)dst;
  constexpr static uint8_t source_table[64] = {
      'A', 'Q', 'g', 'w', 'B', 'R', 'h', 'x', 'C', 'S', 'i', 'y', 'D',
      'T', 'j', 'z', 'E', 'U', 'k', '0', 'F', 'V', 'l', '1', 'G', 'W',
      'm', '2', 'H', 'X', 'n', '3', 'I', 'Y', 'o', '4', 'J', 'Z', 'p',
      '5', 'K', 'a', 'q', '6', 'L', 'b', 'r', '7', 'M', 'c', 's', '8',
      'N', 'd', 't', '9', 'O', 'e', 'u', '+', 'P', 'f', 'v', '/',
  };
  constexpr static uint8_t source_table_url[64] = {
      'A', 'Q', 'g', 'w', 'B', 'R', 'h', 'x', 'C', 'S', 'i', 'y', 'D',
      'T', 'j', 'z', 'E', 'U', 'k', '0', 'F', 'V', 'l', '1', 'G', 'W',
      'm', '2', 'H', 'X', 'n', '3', 'I', 'Y', 'o', '4', 'J', 'Z', 'p',
      '5', 'K', 'a', 'q', '6', 'L', 'b', 'r', '7', 'M', 'c', 's', '8',
      'N', 'd', 't', '9', 'O', 'e', 'u', '-', 'P', 'f', 'v', '_',
  };
  const uint8x16_t v3f = vdupq_n_u8(0x3f);
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
  // When trying to load a uint8_t array, Visual Studio might
  // error with: error C2664: '__n128x4 neon_ld4m_q8(const char *)':
  // cannot convert argument 1 from 'const uint8_t [64]' to 'const char *
  const uint8x16x4_t table = vld4q_u8(
      (reinterpret_cast<const char *>(options & base64_url) ? source_table_url
                                                            : source_table));
#else
  const uint8x16x4_t table =
      vld4q_u8((options & base64_url) ? source_table_url : source_table);
#endif
  size_t i = 0;
  for (; i + 16 * 3 <= srclen; i += 16 * 3) {
    const uint8x16x3_t in = vld3q_u8((const uint8_t *)src + i);
    uint8x16x4_t result;
    result.val[0] = vshrq_n_u8(in.val[0], 2);
    result.val[1] =
        vandq_u8(vsliq_n_u8(vshrq_n_u8(in.val[1], 4), in.val[0], 4), v3f);
    result.val[2] =
        vandq_u8(vsliq_n_u8(vshrq_n_u8(in.val[2], 6), in.val[1], 2), v3f);
    result.val[3] = vandq_u8(in.val[2], v3f);
    result.val[0] = vqtbl4q_u8(table, result.val[0]);
    result.val[1] = vqtbl4q_u8(table, result.val[1]);
    result.val[2] = vqtbl4q_u8(table, result.val[2]);
    result.val[3] = vqtbl4q_u8(table, result.val[3]);
    vst4q_u8(out, result);
    out += 64;
  }

  if (i + 24 <= srclen) {
    const uint8x8_t v3f_d = vdup_n_u8(0x3f);
    const uint8x8x3_t in = vld3_u8((const uint8_t *)src + i);
    uint8x8x4_t result;
    result.val[0] = vshr_n_u8(in.val[0], 2);
    result.val[1] =
        vand_u8(vsli_n_u8(vshr_n_u8(in.val[1], 4), in.val[0], 4), v3f_d);
    result.val[2] =
        vand_u8(vsli_n_u8(vshr_n_u8(in.val[2], 6), in.val[1], 2), v3f_d);
    result.val[3] = vand_u8(in.val[2], v3f_d);
    result.val[0] = vqtbl4_u8(table, result.val[0]);
    result.val[1] = vqtbl4_u8(table, result.val[1]);
    result.val[2] = vqtbl4_u8(table, result.val[2]);
    result.val[3] = vqtbl4_u8(table, result.val[3]);
    vst4_u8(out, result);
    out += 32;
    i += 24;
  }

  out += scalar::base64::tail_encode_base64((char *)out, src + i, srclen - i,
                                            options);

  return size_t((char *)out - dst);
}

static inline void compress(uint8x16_t data, uint16_t mask, char *output) {
  if (mask == 0) {
    vst1q_u8((uint8_t *)output, data);
    return;
  }
  uint8_t mask1 = uint8_t(mask);      // least significant 8 bits
  uint8_t mask2 = uint8_t(mask >> 8); // most significant 8 bits
  uint64x2_t compactmasku64 = {tables::base64::thintable_epi8[mask1],
                               tables::base64::thintable_epi8[mask2]};
  uint8x16_t compactmask = vreinterpretq_u8_u64(compactmasku64);
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
  const uint8x16_t off =
      simdutf_make_uint8x16_t(0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8);
#else
  const uint8x16_t off = {0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8};
#endif

  compactmask = vaddq_u8(compactmask, off);
  uint8x16_t pruned = vqtbl1q_u8(data, compactmask);

  int pop1 = tables::base64::BitsSetTable256mul2[mask1];
  // then load the corresponding mask, what it does is to write
  // only the first pop1 bytes from the first 8 bytes, and then
  // it fills in with the bytes from the second 8 bytes + some filling
  // at the end.
  compactmask = vld1q_u8(tables::base64::pshufb_combine_table + pop1 * 8);
  uint8x16_t answer = vqtbl1q_u8(pruned, compactmask);
  vst1q_u8((uint8_t *)output, answer);
}

struct block64 {
  uint8x16_t chunks[4];
};

static_assert(sizeof(block64) == 64, "block64 is not 64 bytes");
template <bool base64_url, bool default_or_url>
uint64_t to_base64_mask(block64 *b, bool *error) {
  uint8x16_t v0f = vdupq_n_u8(0xf);
  uint8x16_t v01 = vdupq_n_u8(0x1);

  uint8x16_t lo_nibbles0 = vandq_u8(b->chunks[0], v0f);
  uint8x16_t lo_nibbles1 = vandq_u8(b->chunks[1], v0f);
  uint8x16_t lo_nibbles2 = vandq_u8(b->chunks[2], v0f);
  uint8x16_t lo_nibbles3 = vandq_u8(b->chunks[3], v0f);

  // Needed by the decoding step.
  uint8x16_t hi_bits0 = vshrq_n_u8(b->chunks[0], 3);
  uint8x16_t hi_bits1 = vshrq_n_u8(b->chunks[1], 3);
  uint8x16_t hi_bits2 = vshrq_n_u8(b->chunks[2], 3);
  uint8x16_t hi_bits3 = vshrq_n_u8(b->chunks[3], 3);
  uint8x16_t lut_lo;
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
  if (default_or_url) {
    lut_lo =
        simdutf_make_uint8x16_t(0xa9, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
                                0xf8, 0xf9, 0xf1, 0xa2, 0xa1, 0xa5, 0xa0, 0xa6);
  } else if (base64_url) {
    lut_lo =
        simdutf_make_uint8x16_t(0xa9, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
                                0xf8, 0xf9, 0xf1, 0xa0, 0xa1, 0xa5, 0xa0, 0xa2);
  } else {
    lut_lo =
        simdutf_make_uint8x16_t(0xa9, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
                                0xf8, 0xf9, 0xf1, 0xa2, 0xa1, 0xa1, 0xa0, 0xa4);
  }
#else
  if (default_or_url) {
    lut_lo = uint8x16_t{0xa9, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
                        0xf8, 0xf9, 0xf1, 0xa2, 0xa1, 0xa5, 0xa0, 0xa6};
  } else if (base64_url) {
    lut_lo = uint8x16_t{0xa9, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
                        0xf8, 0xf9, 0xf1, 0xa0, 0xa1, 0xa5, 0xa0, 0xa2};
  } else {
    lut_lo = uint8x16_t{0xa9, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
                        0xf8, 0xf9, 0xf1, 0xa2, 0xa1, 0xa1, 0xa0, 0xa4};
  }
#endif
  uint8x16_t lo0 = vqtbl1q_u8(lut_lo, lo_nibbles0);
  uint8x16_t lo1 = vqtbl1q_u8(lut_lo, lo_nibbles1);
  uint8x16_t lo2 = vqtbl1q_u8(lut_lo, lo_nibbles2);
  uint8x16_t lo3 = vqtbl1q_u8(lut_lo, lo_nibbles3);
  uint8x16_t lut_hi;
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
  if (default_or_url) {
    lut_hi =
        simdutf_make_uint8x16_t(0x0, 0x1, 0x0, 0x0, 0x1, 0x6, 0x8, 0x8, 0x10,
                                0x20, 0x20, 0x12, 0x40, 0x80, 0x80, 0x40);
  } else if (base64_url) {
    lut_hi =
        simdutf_make_uint8x16_t(0x0, 0x1, 0x0, 0x0, 0x1, 0x6, 0x8, 0x8, 0x10,
                                0x20, 0x20, 0x12, 0x40, 0x80, 0x80, 0x40);
  } else {
    lut_hi =
        simdutf_make_uint8x16_t(0x0, 0x1, 0x0, 0x0, 0x1, 0x6, 0x8, 0x8, 0x10,
                                0x20, 0x20, 0x10, 0x40, 0x80, 0x80, 0x40);
  }
#else
  if (default_or_url) {
    lut_hi = uint8x16_t{0x0,  0x1,  0x0,  0x0,  0x1,  0x6,  0x8,  0x8,
                        0x10, 0x20, 0x20, 0x12, 0x40, 0x80, 0x80, 0x40};
  } else if (base64_url) {
    lut_hi = uint8x16_t{0x0,  0x1,  0x0,  0x0,  0x1,  0x4,  0x8,  0x8,
                        0x10, 0x20, 0x20, 0x12, 0x40, 0x80, 0x80, 0x40};
  } else {
    lut_hi = uint8x16_t{0x0,  0x1,  0x0,  0x0,  0x1,  0x6,  0x8,  0x8,
                        0x10, 0x20, 0x20, 0x10, 0x40, 0x80, 0x80, 0x40};
  }
#endif
  uint8x16_t hi0 = vqtbl1q_u8(lut_hi, hi_bits0);
  uint8x16_t hi1 = vqtbl1q_u8(lut_hi, hi_bits1);
  uint8x16_t hi2 = vqtbl1q_u8(lut_hi, hi_bits2);
  uint8x16_t hi3 = vqtbl1q_u8(lut_hi, hi_bits3);

  // maps error byte to 0 and space byte to 1, valid bytes are >1
  uint8x16_t res0 = vandq_u8(lo0, hi0);
  uint8x16_t res1 = vandq_u8(lo1, hi1);
  uint8x16_t res2 = vandq_u8(lo2, hi2);
  uint8x16_t res3 = vandq_u8(lo3, hi3);

  uint8_t checks =
      vminvq_u8(vminq_u8(vminq_u8(res0, res1), vminq_u8(res2, res3)));
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
  const uint8x16_t bit_mask =
      simdutf_make_uint8x16_t(0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                              0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80);
#else
  const uint8x16_t bit_mask = {0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                               0x01, 0x02, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
#endif
  uint64_t badcharmask = 0;
  *error = checks == 0;
  if (checks <= 1) {
    // Add each of the elements next to each other, successively, to stuff each
    // 8 byte mask into one.
    uint8x16_t test0 = vcleq_u8(res0, v01);
    uint8x16_t test1 = vcleq_u8(res1, v01);
    uint8x16_t test2 = vcleq_u8(res2, v01);
    uint8x16_t test3 = vcleq_u8(res3, v01);
    uint8x16_t sum0 =
        vpaddq_u8(vandq_u8(test0, bit_mask), vandq_u8(test1, bit_mask));
    uint8x16_t sum1 =
        vpaddq_u8(vandq_u8(test2, bit_mask), vandq_u8(test3, bit_mask));
    sum0 = vpaddq_u8(sum0, sum1);
    sum0 = vpaddq_u8(sum0, sum0);
    badcharmask = vgetq_lane_u64(vreinterpretq_u64_u8(sum0), 0);
  }
  // This is the transformation step that can be done while we are waiting for
  // sum0
  uint8x16_t roll_lut;
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
  if (default_or_url) {
    roll_lut =
        simdutf_make_uint8x16_t(0xBF, 0xE0, 0xB9, 0x13, 0x04, 0xBF, 0xBF, 0xB9,
                                0xB9, 0x00, 0xFF, 0x11, 0xFF, 0xBF, 0x10, 0xB9);
  } else if (base64_url) {
    roll_lut =
        simdutf_make_uint8x16_t(0xB9, 0xB9, 0xBF, 0xBF, 0x04, 0x11, 0xE0, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  } else {
    roll_lut =
        simdutf_make_uint8x16_t(0xB9, 0xB9, 0xBF, 0xBF, 0x04, 0x10, 0x13, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  }
#else
  if (default_or_url) {
    roll_lut = uint8x16_t{0xBF, 0xE0, 0xB9, 0x13, 0x04, 0xBF, 0xBF, 0xB9,
                          0xB9, 0x00, 0xFF, 0x11, 0xFF, 0xBF, 0x10, 0xB9};
  } else if (base64_url) {
    roll_lut = uint8x16_t{0xB9, 0xB9, 0xBF, 0xBF, 0x04, 0x11, 0xE0, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  } else {
    roll_lut = uint8x16_t{0xB9, 0xB9, 0xBF, 0xBF, 0x04, 0x10, 0x13, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  }
#endif
  uint8x16_t roll0, roll1, roll2, roll3;
  if (default_or_url) {
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
    const uint8x16_t delta_asso =
        simdutf_make_uint8x16_t(0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x16);
#else
    const uint8x16_t delta_asso =
        uint8x16_t{0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x16};
#endif
    // the logic of translating is based on westmere
    uint8x16_t delta_hash0 =
        vrhaddq_u8(vqtbl1q_u8(delta_asso, lo_nibbles0), hi_bits0);
    uint8x16_t delta_hash1 =
        vrhaddq_u8(vqtbl1q_u8(delta_asso, lo_nibbles1), hi_bits1);
    uint8x16_t delta_hash2 =
        vrhaddq_u8(vqtbl1q_u8(delta_asso, lo_nibbles2), hi_bits2);
    uint8x16_t delta_hash3 =
        vrhaddq_u8(vqtbl1q_u8(delta_asso, lo_nibbles3), hi_bits3);
    const uint8x16x2_t roll_lut_2 = {roll_lut, roll_lut};
    roll0 = vqtbl2q_u8(roll_lut_2, delta_hash0);
    roll1 = vqtbl2q_u8(roll_lut_2, delta_hash1);
    roll2 = vqtbl2q_u8(roll_lut_2, delta_hash2);
    roll3 = vqtbl2q_u8(roll_lut_2, delta_hash3);
  } else {
    uint8x16_t delta_hash0 = vclzq_u8(res0);
    uint8x16_t delta_hash1 = vclzq_u8(res1);
    uint8x16_t delta_hash2 = vclzq_u8(res2);
    uint8x16_t delta_hash3 = vclzq_u8(res3);
    roll0 = vqtbl1q_u8(roll_lut, delta_hash0);
    roll1 = vqtbl1q_u8(roll_lut, delta_hash1);
    roll2 = vqtbl1q_u8(roll_lut, delta_hash2);
    roll3 = vqtbl1q_u8(roll_lut, delta_hash3);
  }

  b->chunks[0] = vaddq_u8(b->chunks[0], roll0);
  b->chunks[1] = vaddq_u8(b->chunks[1], roll1);
  b->chunks[2] = vaddq_u8(b->chunks[2], roll2);
  b->chunks[3] = vaddq_u8(b->chunks[3], roll3);
  return badcharmask;
}

void copy_block(block64 *b, char *output) {
  vst1q_u8((uint8_t *)output, b->chunks[0]);
  vst1q_u8((uint8_t *)output + 16, b->chunks[1]);
  vst1q_u8((uint8_t *)output + 32, b->chunks[2]);
  vst1q_u8((uint8_t *)output + 48, b->chunks[3]);
}

uint64_t compress_block(block64 *b, uint64_t mask, char *output) {
  uint64_t popcounts =
      vget_lane_u64(vreinterpret_u64_u8(vcnt_u8(vcreate_u8(~mask))), 0);
  uint64_t offsets = popcounts * 0x0101010101010101;
  compress(b->chunks[0], uint16_t(mask), output);
  compress(b->chunks[1], uint16_t(mask >> 16), &output[(offsets >> 8) & 0xFF]);
  compress(b->chunks[2], uint16_t(mask >> 32), &output[(offsets >> 24) & 0xFF]);
  compress(b->chunks[3], uint16_t(mask >> 48), &output[(offsets >> 40) & 0xFF]);
  return offsets >> 56;
}

// The caller of this function is responsible to ensure that there are 64 bytes
// available from reading at src. The data is read into a block64 structure.
void load_block(block64 *b, const char *src) {
  b->chunks[0] = vld1q_u8(reinterpret_cast<const uint8_t *>(src));
  b->chunks[1] = vld1q_u8(reinterpret_cast<const uint8_t *>(src) + 16);
  b->chunks[2] = vld1q_u8(reinterpret_cast<const uint8_t *>(src) + 32);
  b->chunks[3] = vld1q_u8(reinterpret_cast<const uint8_t *>(src) + 48);
}

// The caller of this function is responsible to ensure that there are 32 bytes
// available from reading at data. It returns a 16-byte value, narrowing with
// saturation the 16-bit words.
inline uint8x16_t load_satured(const uint16_t *data) {
  uint16x8_t in1 = vld1q_u16(data);
  uint16x8_t in2 = vld1q_u16(data + 8);
  return vqmovn_high_u16(vqmovn_u16(in1), in2);
}

// The caller of this function is responsible to ensure that there are 128 bytes
// available from reading at src. The data is read into a block64 structure.
void load_block(block64 *b, const char16_t *src) {
  b->chunks[0] = load_satured(reinterpret_cast<const uint16_t *>(src));
  b->chunks[1] = load_satured(reinterpret_cast<const uint16_t *>(src) + 16);
  b->chunks[2] = load_satured(reinterpret_cast<const uint16_t *>(src) + 32);
  b->chunks[3] = load_satured(reinterpret_cast<const uint16_t *>(src) + 48);
}

// decode 64 bytes and output 48 bytes
void base64_decode_block(char *out, const char *src) {
  uint8x16x4_t str = vld4q_u8((uint8_t *)src);
  uint8x16x3_t outvec;
  outvec.val[0] = vsliq_n_u8(vshrq_n_u8(str.val[1], 4), str.val[0], 2);
  outvec.val[1] = vsliq_n_u8(vshrq_n_u8(str.val[2], 2), str.val[1], 4);
  outvec.val[2] = vsliq_n_u8(str.val[3], str.val[2], 6);
  vst3q_u8((uint8_t *)out, outvec);
}

static size_t compress_block_single(block64 *b, uint64_t mask, char *output) {
  const size_t pos64 = trailing_zeroes(mask);
  const int8_t pos = pos64 & 0xf;

  // Predefine the index vector
#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
  const uint8x16_t v1 = simdutf_make_uint8x16_t(0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                                10, 11, 12, 13, 14, 15);
#else  // SIMDUTF_REGULAR_VISUAL_STUDIO
  const uint8x16_t v1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
#endif // SIMDUTF_REGULAR_VISUAL_STUDIO

  switch (pos64 >> 4) {
  case 0b00: {
    const uint8x16_t v0 = vmovq_n_u8((uint8_t)(pos - 1));
    const uint8x16_t v2 =
        vcgtq_s8(vreinterpretq_s8_u8(v1),
                 vreinterpretq_s8_u8(v0));  // Compare greater than
    const uint8x16_t sh = vsubq_u8(v1, v2); // Subtract
    const uint8x16_t compressed =
        vqtbl1q_u8(b->chunks[0], sh); // Table lookup (shuffle)

    vst1q_u8((uint8_t *)(output + 0 * 16), compressed);
    vst1q_u8((uint8_t *)(output + 1 * 16 - 1), b->chunks[1]);
    vst1q_u8((uint8_t *)(output + 2 * 16 - 1), b->chunks[2]);
    vst1q_u8((uint8_t *)(output + 3 * 16 - 1), b->chunks[3]);
  } break;

  case 0b01: {
    vst1q_u8((uint8_t *)(output + 0 * 16), b->chunks[0]);

    const uint8x16_t v0 = vmovq_n_u8((uint8_t)(pos - 1));
    const uint8x16_t v2 =
        vcgtq_s8(vreinterpretq_s8_u8(v1), vreinterpretq_s8_u8(v0));
    const uint8x16_t sh = vsubq_u8(v1, v2);
    const uint8x16_t compressed = vqtbl1q_u8(b->chunks[1], sh);

    vst1q_u8((uint8_t *)(output + 1 * 16), compressed);
    vst1q_u8((uint8_t *)(output + 2 * 16 - 1), b->chunks[2]);
    vst1q_u8((uint8_t *)(output + 3 * 16 - 1), b->chunks[3]);
  } break;

  case 0b10: {
    vst1q_u8((uint8_t *)(output + 0 * 16), b->chunks[0]);
    vst1q_u8((uint8_t *)(output + 1 * 16), b->chunks[1]);

    const uint8x16_t v0 = vmovq_n_u8((uint8_t)(pos - 1));
    const uint8x16_t v2 =
        vcgtq_s8(vreinterpretq_s8_u8(v1), vreinterpretq_s8_u8(v0));
    const uint8x16_t sh = vsubq_u8(v1, v2);
    const uint8x16_t compressed = vqtbl1q_u8(b->chunks[2], sh);

    vst1q_u8((uint8_t *)(output + 2 * 16), compressed);
    vst1q_u8((uint8_t *)(output + 3 * 16 - 1), b->chunks[3]);
  } break;

  case 0b11: {
    vst1q_u8((uint8_t *)(output + 0 * 16), b->chunks[0]);
    vst1q_u8((uint8_t *)(output + 1 * 16), b->chunks[1]);
    vst1q_u8((uint8_t *)(output + 2 * 16), b->chunks[2]);

    const uint8x16_t v0 = vmovq_n_u8((uint8_t)(pos - 1));
    const uint8x16_t v2 =
        vcgtq_s8(vreinterpretq_s8_u8(v1), vreinterpretq_s8_u8(v0));
    const uint8x16_t sh = vsubq_u8(v1, v2);
    const uint8x16_t compressed = vqtbl1q_u8(b->chunks[3], sh);

    vst1q_u8((uint8_t *)(output + 3 * 16), compressed);
  } break;
  }
  return 63;
}

template <typename T> bool is_power_of_two(T x) { return (x & (x - 1)) == 0; }

template <bool base64_url, bool ignore_garbage, bool default_or_url,
          typename char_type>
full_result
compress_decode_base64(char *dst, const char_type *src, size_t srclen,
                       base64_options options,
                       last_chunk_handling_options last_chunk_options) {
  const uint8_t *to_base64 =
      default_or_url ? tables::base64::to_base64_default_or_url_value
                     : (base64_url ? tables::base64::to_base64_url_value
                                   : tables::base64::to_base64_value);
  auto ri = simdutf::scalar::base64::find_end(src, srclen, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  srclen = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  (void)full_input_length;
  if (srclen == 0) {
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation, 0};
    }
    return {SUCCESS, 0, 0};
  }
  const char_type *const srcinit = src;
  const char *const dstinit = dst;
  const char_type *const srcend = src + srclen;

  constexpr size_t block_size = 10;
  char buffer[block_size * 64];
  char *bufferptr = buffer;
  if (srclen >= 64) {
    const char_type *const srcend64 = src + srclen - 64;
    while (src <= srcend64) {
      block64 b;
      load_block(&b, src);
      src += 64;
      bool error = false;
      uint64_t badcharmask =
          to_base64_mask<base64_url, default_or_url>(&b, &error);
      if (badcharmask) {
        if (error && !ignore_garbage) {
          src -= 64;
          while (src < srcend && scalar::base64::is_eight_byte(*src) &&
                 to_base64[uint8_t(*src)] <= 64) {
            src++;
          }
          if (src < srcend) {
            // should never happen
          }
          return {error_code::INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                  size_t(dst - dstinit)};
        }
      }

      if (badcharmask != 0) {
        // optimization opportunity: check for simple masks like those made of
        // continuous 1s followed by continuous 0s. And masks containing a
        // single bad character.
        if (is_power_of_two(badcharmask)) {
          bufferptr += compress_block_single(&b, badcharmask, bufferptr);
        } else {
          bufferptr += compress_block(&b, badcharmask, bufferptr);
        }
      } else {
        // optimization opportunity: if bufferptr == buffer and mask == 0, we
        // can avoid the call to compress_block and decode directly.
        copy_block(&b, bufferptr);
        bufferptr += 64;
      }
      if (bufferptr >= (block_size - 1) * 64 + buffer) {
        for (size_t i = 0; i < (block_size - 1); i++) {
          base64_decode_block(dst, buffer + i * 64);
          dst += 48;
        }
        std::memcpy(buffer, buffer + (block_size - 1) * 64,
                    64); // 64 might be too much
        bufferptr -= (block_size - 1) * 64;
      }
    }
  }
  char *buffer_start = buffer;
  // Optimization note: if this is almost full, then it is worth our
  // time, otherwise, we should just decode directly.
  int last_block = (int)((bufferptr - buffer_start) % 64);
  if (last_block != 0 && srcend - src + last_block >= 64) {
    while ((bufferptr - buffer_start) % 64 != 0 && src < srcend) {
      uint8_t val = to_base64[uint8_t(*src)];
      *bufferptr = char(val);
      if ((!scalar::base64::is_eight_byte(*src) || val > 64) &&
          !ignore_garbage) {
        return {error_code::INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit)};
      }
      bufferptr += (val <= 63);
      src++;
    }
  }

  for (; buffer_start + 64 <= bufferptr; buffer_start += 64) {
    base64_decode_block(dst, buffer_start);
    dst += 48;
  }
  if ((bufferptr - buffer_start) % 64 != 0) {
    while (buffer_start + 4 < bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
      triple = scalar::u32_swap_bytes(triple);
      std::memcpy(dst, &triple, 4);

      dst += 3;
      buffer_start += 4;
    }
    if (buffer_start + 4 <= bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
      triple = scalar::u32_swap_bytes(triple);
      std::memcpy(dst, &triple, 3);

      dst += 3;
      buffer_start += 4;
    }
    // we may have 1, 2 or 3 bytes left and we need to decode them so let us
    // backtrack
    int leftover = int(bufferptr - buffer_start);
    while (leftover > 0) {
      if (!ignore_garbage) {
        while (to_base64[uint8_t(*(src - 1))] == 64) {
          src--;
        }
      } else {
        while (to_base64[uint8_t(*(src - 1))] >= 64) {
          src--;
        }
      }
      src--;
      leftover--;
    }
  }
  if (src < srcend + equalsigns) {
    full_result r = scalar::base64::base64_tail_decode(
        dst, src, srcend - src, equalsigns, options, last_chunk_options);
    r = scalar::base64::patch_tail_result(
        r, size_t(src - srcinit), size_t(dst - dstinit), equallocation,
        full_input_length, last_chunk_options);
    return r;
  }
  if (equalsigns > 0 && !ignore_garbage) {
    if ((size_t(dst - dstinit) % 3 == 0) ||
        ((size_t(dst - dstinit) % 3) + 1 + equalsigns != 4)) {
      return {INVALID_BASE64_CHARACTER, equallocation, size_t(dst - dstinit)};
    }
  }
  return {SUCCESS, srclen, size_t(dst - dstinit)};
}
/* end file src\arm64\arm_base64.cpp */
/* begin file src\arm64\arm_find.cpp */
simdutf_really_inline const char *util_find(const char *start, const char *end,
                                            char character) noexcept {
  // Handle empty or invalid range
  if (start >= end)
    return end;

  // Process 16 bytes (128 bits) at a time with NEON
  const size_t step = 16;
  uint8x16_t char_vec = vdupq_n_u8(static_cast<uint8_t>(character));

  // Main loop for full 16-byte chunks
  while (size_t(end - start) >= step) {
    uint8x16_t data = vld1q_u8(reinterpret_cast<const uint8_t *>(start));
    uint8x16_t cmp = vceqq_u8(data, char_vec);
    uint64_t mask = vget_lane_u64(
        vreinterpret_u64_u8(vshrn_n_u16(vreinterpretq_u16_u8(cmp), 4)), 0);

    if (mask != 0) {
      // Found a match, return the first one
      int index = trailing_zeroes(mask) / 4; // Each character maps to 4 bits
      return start + index;
    }

    start += step;
  }

  // Handle remaining bytes with scalar loop
  for (; start < end; ++start) {
    if (*start == character) {
      return start;
    }
  }

  return end;
}

simdutf_really_inline const char16_t *util_find(const char16_t *start,
                                                const char16_t *end,
                                                char16_t character) noexcept {
  // Handle empty or invalid range
  if (start >= end)
    return end;

  // Process 8 char16_t (16 bytes, 128 bits) at a time with NEON
  const size_t step = 8;
  uint16x8_t char_vec = vdupq_n_u16(character);

  // Main loop for full 8-element chunks
  while (size_t(end - start) >= step) {
    uint16x8_t data = vld1q_u16(reinterpret_cast<const uint16_t *>(start));
    uint16x8_t cmp = vceqq_u16(data, char_vec);
    uint64_t mask = vget_lane_u64(
        vreinterpret_u64_u16(vshrn_n_u32(vreinterpretq_u32_u16(cmp), 4)), 0);

    if (mask != 0) {
      // Found a match, return the first one
      int index = trailing_zeroes(mask) / 8; // Each character maps to 8 bits
      return start + index;
    }

    start += step;
  }

  // Handle remaining elements with scalar loop
  for (; start < end; ++start) {
    if (*start == character) {
      return start;
    }
  }

  return end;
}
/* end file src\arm64\arm_find.cpp */

} // unnamed namespace
} // namespace arm64
} // namespace simdutf

/* begin file src\generic\buf_block_reader.h */
namespace simdutf {
namespace arm64 {
namespace {

// Walks through a buffer in block-sized increments, loading the last part with
// spaces
template <size_t STEP_SIZE> struct buf_block_reader {
public:
  simdutf_really_inline buf_block_reader(const uint8_t *_buf, size_t _len);
  simdutf_really_inline size_t block_index();
  simdutf_really_inline bool has_full_block() const;
  simdutf_really_inline const uint8_t *full_block() const;
  /**
   * Get the last block, padded with spaces.
   *
   * There will always be a last block, with at least 1 byte, unless len == 0
   * (in which case this function fills the buffer with spaces and returns 0. In
   * particular, if len == STEP_SIZE there will be 0 full_blocks and 1 remainder
   * block with STEP_SIZE bytes and no spaces for padding.
   *
   * @return the number of effective characters in the last block.
   */
  simdutf_really_inline size_t get_remainder(uint8_t *dst) const;
  simdutf_really_inline void advance();

private:
  const uint8_t *buf;
  const size_t len;
  const size_t lenminusstep;
  size_t idx;
};

// Routines to print masks and text for debugging bitmask operations
simdutf_unused static char *format_input_text_64(const uint8_t *text) {
  static char *buf =
      reinterpret_cast<char *>(malloc(sizeof(simd8x64<uint8_t>) + 1));
  for (size_t i = 0; i < sizeof(simd8x64<uint8_t>); i++) {
    buf[i] = int8_t(text[i]) < ' ' ? '_' : int8_t(text[i]);
  }
  buf[sizeof(simd8x64<uint8_t>)] = '\0';
  return buf;
}

// Routines to print masks and text for debugging bitmask operations
simdutf_unused static char *format_input_text(const simd8x64<uint8_t> &in) {
  static char *buf =
      reinterpret_cast<char *>(malloc(sizeof(simd8x64<uint8_t>) + 1));
  in.store(reinterpret_cast<uint8_t *>(buf));
  for (size_t i = 0; i < sizeof(simd8x64<uint8_t>); i++) {
    if (buf[i] < ' ') {
      buf[i] = '_';
    }
  }
  buf[sizeof(simd8x64<uint8_t>)] = '\0';
  return buf;
}

simdutf_unused static char *format_mask(uint64_t mask) {
  static char *buf = reinterpret_cast<char *>(malloc(64 + 1));
  for (size_t i = 0; i < 64; i++) {
    buf[i] = (mask & (size_t(1) << i)) ? 'X' : ' ';
  }
  buf[64] = '\0';
  return buf;
}

template <size_t STEP_SIZE>
simdutf_really_inline
buf_block_reader<STEP_SIZE>::buf_block_reader(const uint8_t *_buf, size_t _len)
    : buf{_buf}, len{_len}, lenminusstep{len < STEP_SIZE ? 0 : len - STEP_SIZE},
      idx{0} {}

template <size_t STEP_SIZE>
simdutf_really_inline size_t buf_block_reader<STEP_SIZE>::block_index() {
  return idx;
}

template <size_t STEP_SIZE>
simdutf_really_inline bool buf_block_reader<STEP_SIZE>::has_full_block() const {
  return idx < lenminusstep;
}

template <size_t STEP_SIZE>
simdutf_really_inline const uint8_t *
buf_block_reader<STEP_SIZE>::full_block() const {
  return &buf[idx];
}

template <size_t STEP_SIZE>
simdutf_really_inline size_t
buf_block_reader<STEP_SIZE>::get_remainder(uint8_t *dst) const {
  if (len == idx) {
    return 0;
  } // memcpy(dst, null, 0) will trigger an error with some sanitizers
  std::memset(dst, 0x20,
              STEP_SIZE); // std::memset STEP_SIZE because it is more efficient
                          // to write out 8 or 16 bytes at once.
  std::memcpy(dst, buf + idx, len - idx);
  return len - idx;
}

template <size_t STEP_SIZE>
simdutf_really_inline void buf_block_reader<STEP_SIZE>::advance() {
  idx += STEP_SIZE;
}

} // unnamed namespace
} // namespace arm64
} // namespace simdutf
/* end file src\generic\buf_block_reader.h */

// other functions

//
// Implementation-specific overrides
//
namespace simdutf {
namespace arm64 {

simdutf_warn_unused result implementation::base64_to_binary(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused result implementation::base64_to_binary(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

size_t implementation::binary_to_base64(const char *input, size_t length,
                                        char *output,
                                        base64_options options) const noexcept {
  return encode_base64(output, input, length, options);
}

const char *implementation::find(const char *start, const char *end,
                                 char character) const noexcept {
  return util_find(start, end, character);
}

const char16_t *implementation::find(const char16_t *start, const char16_t *end,
                                     char16_t character) const noexcept {
  return util_find(start, end, character);
}

} // namespace arm64
} // namespace simdutf

/* begin file src\simdutf\arm64\end.h */
#undef SIMDUTF_SIMD_HAS_BYTEMASK
/* end file src\simdutf\arm64\end.h */
/* end file src\arm64\implementation.cpp */
#endif
#if SIMDUTF_IMPLEMENTATION_FALLBACK
/* begin file src\fallback\implementation.cpp */
/* begin file src\simdutf\fallback\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "fallback"
// #define SIMDUTF_IMPLEMENTATION fallback
/* end file src\simdutf\fallback\begin.h */

namespace simdutf {
namespace fallback {

simdutf_warn_unused result implementation::base64_to_binary(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  return simdutf::scalar::base64::base64_to_binary_details_impl(
      input, length, output, options, last_chunk_options);
}

simdutf_warn_unused result implementation::base64_to_binary(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  return simdutf::scalar::base64::base64_to_binary_details_impl(
      input, length, output, options, last_chunk_options);
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  return simdutf::scalar::base64::base64_to_binary_details_impl(
      input, length, output, options, last_chunk_options);
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  return simdutf::scalar::base64::base64_to_binary_details_impl(
      input, length, output, options, last_chunk_options);
}

size_t implementation::binary_to_base64(const char *input, size_t length,
                                        char *output,
                                        base64_options options) const noexcept {
  return scalar::base64::tail_encode_base64(output, input, length, options);
}

const char *implementation::find(const char *start, const char *end,
                                 char character) const noexcept {
  return std::find(start, end, character);
}

const char16_t *implementation::find(const char16_t *start, const char16_t *end,
                                     char16_t character) const noexcept {
  return std::find(start, end, character);
}

} // namespace fallback
} // namespace simdutf

/* begin file src\simdutf\fallback\end.h */
/* end file src\simdutf\fallback\end.h */
/* end file src\fallback\implementation.cpp */
#endif
#if SIMDUTF_IMPLEMENTATION_ICELAKE
/* begin file src\icelake\implementation.cpp */
#include <tuple>
#include <utility>

/* begin file src\simdutf\icelake\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "icelake"
// #define SIMDUTF_IMPLEMENTATION icelake

#if SIMDUTF_CAN_ALWAYS_RUN_ICELAKE
// nothing needed.
#else
SIMDUTF_TARGET_ICELAKE
#endif

#if SIMDUTF_GCC11ORMORE // workaround for
                        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105593
// clang-format off
SIMDUTF_DISABLE_GCC_WARNING(-Wmaybe-uninitialized)
// clang-format on
#endif // end of workaround
/* end file src\simdutf\icelake\begin.h */
namespace simdutf {
namespace icelake {
namespace {
#ifndef SIMDUTF_ICELAKE_H
  #error "icelake.h must be included"
#endif
using namespace simd;

/* begin file src\icelake\icelake_macros.inl.cpp */
/*
    This upcoming macro (SIMDUTF_ICELAKE_TRANSCODE16) takes 16 + 4 bytes (of a
   UTF-8 string) and loads all possible 4-byte substring into an AVX512
   register.

    For example if we have bytes abcdefgh... we create following 32-bit lanes

    [abcd|bcde|cdef|defg|efgh|...]
     ^                          ^
     byte 0 of reg              byte 63 of reg
*/
/** pshufb
        # lane{0,1,2} have got bytes: [  0,  1,  2,  3,  4,  5,  6,  8,  9, 10,
   11, 12, 13, 14, 15] # lane3 has got bytes:        [ 16, 17, 18, 19,  4,  5,
   6,  8,  9, 10, 11, 12, 13, 14, 15]

        expand_ver2 = [
            # lane 0:
            0, 1, 2, 3,
            1, 2, 3, 4,
            2, 3, 4, 5,
            3, 4, 5, 6,

            # lane 1:
            4, 5, 6, 7,
            5, 6, 7, 8,
            6, 7, 8, 9,
            7, 8, 9, 10,

            # lane 2:
             8,  9, 10, 11,
             9, 10, 11, 12,
            10, 11, 12, 13,
            11, 12, 13, 14,

            # lane 3 order: 13, 14, 15, 16 14, 15, 16, 17, 15, 16, 17, 18, 16,
   17, 18, 19 12, 13, 14, 15, 13, 14, 15,  0, 14, 15,  0,  1, 15,  0,  1,  2,
        ]
*/

#define SIMDUTF_ICELAKE_TRANSCODE16(LANE0, LANE1, MASKED)                      \
  {                                                                            \
    const __m512i merged = _mm512_mask_mov_epi32(LANE0, 0x1000, LANE1);        \
    const __m512i expand_ver2 = _mm512_setr_epi64(                             \
        0x0403020103020100, 0x0605040305040302, 0x0807060507060504,            \
        0x0a09080709080706, 0x0c0b0a090b0a0908, 0x0e0d0c0b0d0c0b0a,            \
        0x000f0e0d0f0e0d0c, 0x0201000f01000f0e);                               \
    const __m512i input = _mm512_shuffle_epi8(merged, expand_ver2);            \
                                                                               \
    __mmask16 leading_bytes;                                                   \
    const __m512i v_0000_00c0 = _mm512_set1_epi32(0xc0);                       \
    const __m512i t0 = _mm512_and_si512(input, v_0000_00c0);                   \
    const __m512i v_0000_0080 = _mm512_set1_epi32(0x80);                       \
    leading_bytes = _mm512_cmpneq_epu32_mask(t0, v_0000_0080);                 \
                                                                               \
    __m512i char_class;                                                        \
    char_class = _mm512_srli_epi32(input, 4);                                  \
    /*  char_class = ((input >> 4) & 0x0f) | 0x80808000 */                     \
    const __m512i v_0000_000f = _mm512_set1_epi32(0x0f);                       \
    const __m512i v_8080_8000 = _mm512_set1_epi32(0x80808000);                 \
    char_class =                                                               \
        _mm512_ternarylogic_epi32(char_class, v_0000_000f, v_8080_8000, 0xea); \
                                                                               \
    const int valid_count = static_cast<int>(count_ones(leading_bytes));       \
    const __m512i utf32 = expanded_utf8_to_utf32(char_class, input);           \
                                                                               \
    const __m512i out = _mm512_mask_compress_epi32(_mm512_setzero_si512(),     \
                                                   leading_bytes, utf32);      \
                                                                               \
    if (UTF32) {                                                               \
      if (MASKED) {                                                            \
        const __mmask16 valid = uint16_t((1 << valid_count) - 1);              \
        _mm512_mask_storeu_epi32((__m512i *)output, valid, out);               \
      } else {                                                                 \
        _mm512_storeu_si512((__m512i *)output, out);                           \
      }                                                                        \
      output += valid_count;                                                   \
    } else {                                                                   \
      if (MASKED) {                                                            \
        output += utf32_to_utf16_masked<big_endian>(                           \
            byteflip, out, valid_count, reinterpret_cast<char16_t *>(output)); \
      } else {                                                                 \
        output += utf32_to_utf16<big_endian>(                                  \
            byteflip, out, valid_count, reinterpret_cast<char16_t *>(output)); \
      }                                                                        \
    }                                                                          \
  }

#define SIMDUTF_ICELAKE_WRITE_UTF16_OR_UTF32(INPUT, VALID_COUNT, MASKED)       \
  {                                                                            \
    if (UTF32) {                                                               \
      if (MASKED) {                                                            \
        const __mmask16 valid_mask = uint16_t((1 << VALID_COUNT) - 1);         \
        _mm512_mask_storeu_epi32((__m512i *)output, valid_mask, INPUT);        \
      } else {                                                                 \
        _mm512_storeu_si512((__m512i *)output, INPUT);                         \
      }                                                                        \
      output += VALID_COUNT;                                                   \
    } else {                                                                   \
      if (MASKED) {                                                            \
        output += utf32_to_utf16_masked<big_endian>(                           \
            byteflip, INPUT, VALID_COUNT,                                      \
            reinterpret_cast<char16_t *>(output));                             \
      } else {                                                                 \
        output +=                                                              \
            utf32_to_utf16<big_endian>(byteflip, INPUT, VALID_COUNT,           \
                                       reinterpret_cast<char16_t *>(output));  \
      }                                                                        \
    }                                                                          \
  }

#define SIMDUTF_ICELAKE_STORE_ASCII(UTF32, utf8, output)                       \
  if (UTF32) {                                                                 \
    const __m128i t0 = _mm512_castsi512_si128(utf8);                           \
    const __m128i t1 = _mm512_extracti32x4_epi32(utf8, 1);                     \
    const __m128i t2 = _mm512_extracti32x4_epi32(utf8, 2);                     \
    const __m128i t3 = _mm512_extracti32x4_epi32(utf8, 3);                     \
    _mm512_storeu_si512((__m512i *)(output + 0 * 16),                          \
                        _mm512_cvtepu8_epi32(t0));                             \
    _mm512_storeu_si512((__m512i *)(output + 1 * 16),                          \
                        _mm512_cvtepu8_epi32(t1));                             \
    _mm512_storeu_si512((__m512i *)(output + 2 * 16),                          \
                        _mm512_cvtepu8_epi32(t2));                             \
    _mm512_storeu_si512((__m512i *)(output + 3 * 16),                          \
                        _mm512_cvtepu8_epi32(t3));                             \
  } else {                                                                     \
    const __m256i h0 = _mm512_castsi512_si256(utf8);                           \
    const __m256i h1 = _mm512_extracti64x4_epi64(utf8, 1);                     \
    if (big_endian) {                                                          \
      _mm512_storeu_si512(                                                     \
          (__m512i *)(output + 0 * 16),                                        \
          _mm512_shuffle_epi8(_mm512_cvtepu8_epi16(h0), byteflip));            \
      _mm512_storeu_si512(                                                     \
          (__m512i *)(output + 2 * 16),                                        \
          _mm512_shuffle_epi8(_mm512_cvtepu8_epi16(h1), byteflip));            \
    } else {                                                                   \
      _mm512_storeu_si512((__m512i *)(output + 0 * 16),                        \
                          _mm512_cvtepu8_epi16(h0));                           \
      _mm512_storeu_si512((__m512i *)(output + 2 * 16),                        \
                          _mm512_cvtepu8_epi16(h1));                           \
    }                                                                          \
  }
/* end file src\icelake\icelake_macros.inl.cpp */
/* begin file src\icelake\icelake_common.inl.cpp */
// file included directly
/**
 * Store the last N bytes of previous followed by 512-N bytes from input.
 */
template <int N> __m512i prev(__m512i input, __m512i previous) {
  static_assert(N <= 32, "N must be no larger than 32");
  const __m512i movemask =
      _mm512_setr_epi32(28, 29, 30, 31, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
  const __m512i rotated = _mm512_permutex2var_epi32(input, movemask, previous);
#if SIMDUTF_GCC8 || SIMDUTF_GCC9
  constexpr int shift = 16 - N; // workaround for GCC8,9
  return _mm512_alignr_epi8(input, rotated, shift);
#else
  return _mm512_alignr_epi8(input, rotated, 16 - N);
#endif // SIMDUTF_GCC8 || SIMDUTF_GCC9
}

template <unsigned idx0, unsigned idx1, unsigned idx2, unsigned idx3>
__m512i shuffle_epi128(__m512i v) {
  static_assert((idx0 >= 0 && idx0 <= 3), "idx0 must be in range 0..3");
  static_assert((idx1 >= 0 && idx1 <= 3), "idx1 must be in range 0..3");
  static_assert((idx2 >= 0 && idx2 <= 3), "idx2 must be in range 0..3");
  static_assert((idx3 >= 0 && idx3 <= 3), "idx3 must be in range 0..3");

  constexpr unsigned shuffle = idx0 | (idx1 << 2) | (idx2 << 4) | (idx3 << 6);
  return _mm512_shuffle_i32x4(v, v, shuffle);
}

template <unsigned idx> constexpr __m512i broadcast_epi128(__m512i v) {
  return shuffle_epi128<idx, idx, idx, idx>(v);
}

simdutf_really_inline __m512i broadcast_128bit_lane(__m128i lane) {
  const __m512i tmp = _mm512_castsi128_si512(lane);

  return broadcast_epi128<0>(tmp);
}
/* end file src\icelake\icelake_common.inl.cpp */

/* begin file src\icelake\icelake_base64.inl.cpp */
// file included directly
/**
 * References and further reading:
 *
 * Wojciech Muła, Daniel Lemire, Base64 encoding and decoding at almost the
 * speed of a memory copy, Software: Practice and Experience 50 (2), 2020.
 * https://arxiv.org/abs/1910.05109
 *
 * Wojciech Muła, Daniel Lemire, Faster Base64 Encoding and Decoding using AVX2
 * Instructions, ACM Transactions on the Web 12 (3), 2018.
 * https://arxiv.org/abs/1704.00605
 *
 * Simon Josefsson. 2006. The Base16, Base32, and Base64 Data Encodings.
 * https://tools.ietf.org/html/rfc4648. (2006). Internet Engineering Task Force,
 * Request for Comments: 4648.
 *
 * Alfred Klomp. 2014a. Fast Base64 encoding/decoding with SSE vectorization.
 * http://www.alfredklomp.com/programming/sse-base64/. (2014).
 *
 * Alfred Klomp. 2014b. Fast Base64 stream encoder/decoder in C99, with SIMD
 * acceleration. https://github.com/aklomp/base64. (2014).
 *
 * Hanson Char. 2014. A Fast and Correct Base 64 Codec. (2014).
 * https://aws.amazon.com/blogs/developer/a-fast-and-correct-base-64-codec/
 *
 * Nick Kopp. 2013. Base64 Encoding on a GPU.
 * https://www.codeproject.com/Articles/276993/Base-Encoding-on-a-GPU. (2013).
 */

struct block64 {
  __m512i chunks[1];
};

template <bool base64_url>
size_t encode_base64(char *dst, const char *src, size_t srclen,
                     base64_options options) {
  // credit: Wojciech Muła
  const uint8_t *input = (const uint8_t *)src;

  uint8_t *out = (uint8_t *)dst;
  static const char *lookup_tbl =
      base64_url
          ? "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"
          : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  const __m512i shuffle_input = _mm512_setr_epi32(
      0x01020001, 0x04050304, 0x07080607, 0x0a0b090a, 0x0d0e0c0d, 0x10110f10,
      0x13141213, 0x16171516, 0x191a1819, 0x1c1d1b1c, 0x1f201e1f, 0x22232122,
      0x25262425, 0x28292728, 0x2b2c2a2b, 0x2e2f2d2e);
  const __m512i lookup =
      _mm512_loadu_si512(reinterpret_cast<const __m512i *>(lookup_tbl));
  const __m512i multi_shifts = _mm512_set1_epi64(UINT64_C(0x3036242a1016040a));
  size_t size = srclen;
  __mmask64 input_mask = 0xffffffffffff; // (1 << 48) - 1
  while (size >= 48) {
    const __m512i v = _mm512_maskz_loadu_epi8(
        input_mask, reinterpret_cast<const __m512i *>(input));
    const __m512i in = _mm512_permutexvar_epi8(shuffle_input, v);
    const __m512i indices = _mm512_multishift_epi64_epi8(multi_shifts, in);
    const __m512i result = _mm512_permutexvar_epi8(indices, lookup);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(out), result);
    out += 64;
    input += 48;
    size -= 48;
  }
  input_mask = ((__mmask64)1 << size) - 1;
  const __m512i v = _mm512_maskz_loadu_epi8(
      input_mask, reinterpret_cast<const __m512i *>(input));
  const __m512i in = _mm512_permutexvar_epi8(shuffle_input, v);
  const __m512i indices = _mm512_multishift_epi64_epi8(multi_shifts, in);
  bool padding_needed =
      (((options & base64_url) == 0) ^
       ((options & base64_reverse_padding) == base64_reverse_padding));
  size_t padding_amount = ((size % 3) > 0) ? (3 - (size % 3)) : 0;
  size_t output_len = ((size + 2) / 3) * 4;
  size_t non_padded_output_len = output_len - padding_amount;
  if (!padding_needed) {
    output_len = non_padded_output_len;
  }
  __mmask64 output_mask = output_len == 64 ? (__mmask64)UINT64_MAX
                                           : ((__mmask64)1 << output_len) - 1;
  __m512i result = _mm512_mask_permutexvar_epi8(
      _mm512_set1_epi8('='), ((__mmask64)1 << non_padded_output_len) - 1,
      indices, lookup);
  _mm512_mask_storeu_epi8(reinterpret_cast<__m512i *>(out), output_mask,
                          result);
  return (size_t)(out - (uint8_t *)dst) + output_len;
}

template <bool base64_url, bool ignore_garbage, bool default_or_url>
static inline uint64_t to_base64_mask(block64 *b, uint64_t *error,
                                      uint64_t input_mask = UINT64_MAX) {
  __m512i input = b->chunks[0];
  const __m512i ascii_space_tbl = _mm512_set_epi8(
      0, 0, 13, 12, 0, 10, 9, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 13, 12, 0, 10,
      9, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 13, 12, 0, 10, 9, 0, 0, 0, 0, 0, 0,
      0, 0, 32, 0, 0, 13, 12, 0, 10, 9, 0, 0, 0, 0, 0, 0, 0, 0, 32);
  __m512i lookup0;
  if (default_or_url) {
    lookup0 = _mm512_set_epi8(
        -128, -128, -128, -128, -128, -128, 61, 60, 59, 58, 57, 56, 55, 54, 53,
        52, 63, -128, 62, -128, 62, -128, -128, -128, -128, -128, -128, -128,
        -128, -128, -128, -1, -128, -128, -128, -128, -128, -128, -128, -128,
        -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -1, -128,
        -128, -1, -1, -128, -128, -128, -128, -128, -128, -128, -128, -1);
  } else if (base64_url) {
    lookup0 = _mm512_set_epi8(
        -128, -128, -128, -128, -128, -128, 61, 60, 59, 58, 57, 56, 55, 54, 53,
        52, -128, -128, 62, -128, -128, -128, -128, -128, -128, -128, -128,
        -128, -128, -128, -128, -1, -128, -128, -128, -128, -128, -128, -128,
        -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -1,
        -128, -128, -1, -1, -128, -128, -128, -128, -128, -128, -128, -128, -1);
  } else {
    lookup0 = _mm512_set_epi8(
        -128, -128, -128, -128, -128, -128, 61, 60, 59, 58, 57, 56, 55, 54, 53,
        52, 63, -128, -128, -128, 62, -128, -128, -128, -128, -128, -128, -128,
        -128, -128, -128, -1, -128, -128, -128, -128, -128, -128, -128, -128,
        -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -1, -128,
        -128, -1, -1, -128, -128, -128, -128, -128, -128, -128, -128, -128);
  }
  __m512i lookup1;
  if (default_or_url) {
    lookup1 = _mm512_set_epi8(
        -128, -128, -128, -128, -128, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42,
        41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, -128,
        63, -128, -128, -128, -128, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15,
        14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -128);
  } else if (base64_url) {
    lookup1 = _mm512_set_epi8(
        -128, -128, -128, -128, -128, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42,
        41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, -128,
        63, -128, -128, -128, -128, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15,
        14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -128);
  } else {
    lookup1 = _mm512_set_epi8(
        -128, -128, -128, -128, -128, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42,
        41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, -128,
        -128, -128, -128, -128, -128, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,
        15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -128);
  }

  const __m512i translated = _mm512_permutex2var_epi8(lookup0, input, lookup1);
  const __m512i combined = _mm512_or_si512(translated, input);
  const __mmask64 mask = _mm512_movepi8_mask(combined) & input_mask;
  if (!ignore_garbage && mask) {
    const __mmask64 spaces =
        _mm512_cmpeq_epi8_mask(_mm512_shuffle_epi8(ascii_space_tbl, input),
                               input) &
        input_mask;
    *error = (mask ^ spaces);
  }
  b->chunks[0] = translated;

  return mask | (~input_mask);
}

static inline void copy_block(block64 *b, char *output) {
  _mm512_storeu_si512(reinterpret_cast<__m512i *>(output), b->chunks[0]);
}

static inline uint64_t compress_block(block64 *b, uint64_t mask, char *output) {
  uint64_t nmask = ~mask;
  __m512i c = _mm512_maskz_compress_epi8(nmask, b->chunks[0]);
  _mm512_storeu_si512(reinterpret_cast<__m512i *>(output), c);
  return _mm_popcnt_u64(nmask);
}

// The caller of this function is responsible to ensure that there are 64 bytes
// available from reading at src. The data is read into a block64 structure.
static inline void load_block(block64 *b, const char *src) {
  b->chunks[0] = _mm512_loadu_si512(reinterpret_cast<const __m512i *>(src));
}

static inline void load_block_partial(block64 *b, const char *src,
                                      __mmask64 input_mask) {
  b->chunks[0] = _mm512_maskz_loadu_epi8(
      input_mask, reinterpret_cast<const __m512i *>(src));
}

// The caller of this function is responsible to ensure that there are 128 bytes
// available from reading at src. The data is read into a block64 structure.
static inline void load_block(block64 *b, const char16_t *src) {
  __m512i m1 = _mm512_loadu_si512(reinterpret_cast<const __m512i *>(src));
  __m512i m2 = _mm512_loadu_si512(reinterpret_cast<const __m512i *>(src + 32));
  __m512i p = _mm512_packus_epi16(m1, m2);
  b->chunks[0] =
      _mm512_permutexvar_epi64(_mm512_setr_epi64(0, 2, 4, 6, 1, 3, 5, 7), p);
}

static inline void load_block_partial(block64 *b, const char16_t *src,
                                      __mmask64 input_mask) {
  __m512i m1 = _mm512_maskz_loadu_epi16((__mmask32)input_mask,
                                        reinterpret_cast<const __m512i *>(src));
  __m512i m2 =
      _mm512_maskz_loadu_epi16((__mmask32)(input_mask >> 32),
                               reinterpret_cast<const __m512i *>(src + 32));
  __m512i p = _mm512_packus_epi16(m1, m2);
  b->chunks[0] =
      _mm512_permutexvar_epi64(_mm512_setr_epi64(0, 2, 4, 6, 1, 3, 5, 7), p);
}

static inline void base64_decode(char *out, __m512i str) {
  const __m512i merge_ab_and_bc =
      _mm512_maddubs_epi16(str, _mm512_set1_epi32(0x01400140));
  const __m512i merged =
      _mm512_madd_epi16(merge_ab_and_bc, _mm512_set1_epi32(0x00011000));
  const __m512i pack = _mm512_set_epi8(
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 61, 62, 56, 57, 58,
      52, 53, 54, 48, 49, 50, 44, 45, 46, 40, 41, 42, 36, 37, 38, 32, 33, 34,
      28, 29, 30, 24, 25, 26, 20, 21, 22, 16, 17, 18, 12, 13, 14, 8, 9, 10, 4,
      5, 6, 0, 1, 2);
  const __m512i shuffled = _mm512_permutexvar_epi8(pack, merged);
  _mm512_mask_storeu_epi8(
      (__m512i *)out, 0xffffffffffff,
      shuffled); // mask would be 0xffffffffffff since we write 48 bytes.
}
// decode 64 bytes and output 48 bytes
static inline void base64_decode_block(char *out, const char *src) {
  base64_decode(out,
                _mm512_loadu_si512(reinterpret_cast<const __m512i *>(src)));
}
static inline void base64_decode_block(char *out, block64 *b) {
  base64_decode(out, b->chunks[0]);
}

template <bool base64_url, bool ignore_garbage, bool default_or_url,
          typename chartype>
full_result
compress_decode_base64(char *dst, const chartype *src, size_t srclen,
                       base64_options options,
                       last_chunk_handling_options last_chunk_options) {

  (void)options;
  const uint8_t *to_base64 =
      default_or_url ? tables::base64::to_base64_default_or_url_value
                     : (base64_url ? tables::base64::to_base64_url_value
                                   : tables::base64::to_base64_value);
  auto ri = simdutf::scalar::base64::find_end(src, srclen, options);
  size_t equallocation = ri.equallocation;
  size_t padding_characters = ri.equalsigns;
  srclen = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  (void)full_input_length;
  if (srclen == 0) {
    if (!ignore_garbage && padding_characters > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation, 0};
    }
    return {SUCCESS, 0, 0};
  }
  const chartype *const srcinit = src;
  const char *const dstinit = dst;
  const chartype *const srcend = src + srclen;

  // figure out why block_size == 2 is sometimes best???
  constexpr size_t block_size = 6;
  char buffer[block_size * 64];
  char *bufferptr = buffer;
  if (srclen >= 64) {
    const chartype *const srcend64 = src + srclen - 64;
    while (src <= srcend64) {
      block64 b;
      load_block(&b, src);
      src += 64;
      uint64_t error = 0;
      uint64_t badcharmask =
          to_base64_mask<base64_url, ignore_garbage, default_or_url>(&b,
                                                                     &error);
      if (!ignore_garbage && error) {
        src -= 64;
        size_t error_offset = _tzcnt_u64(error);
        return {error_code::INVALID_BASE64_CHARACTER,
                size_t(src - srcinit + error_offset), size_t(dst - dstinit)};
      }
      if (badcharmask != 0) {
        // optimization opportunity: check for simple masks like those made of
        // continuous 1s followed by continuous 0s. And masks containing a
        // single bad character.
        bufferptr += compress_block(&b, badcharmask, bufferptr);
      } else if (bufferptr != buffer) {
        copy_block(&b, bufferptr);
        bufferptr += 64;
      } else {
        base64_decode_block(dst, &b);
        dst += 48;
      }
      if (bufferptr >= (block_size - 1) * 64 + buffer) {
        for (size_t i = 0; i < (block_size - 1); i++) {
          base64_decode_block(dst, buffer + i * 64);
          dst += 48;
        }
        std::memcpy(buffer, buffer + (block_size - 1) * 64,
                    64); // 64 might be too much
        bufferptr -= (block_size - 1) * 64;
      }
    }
  }

  int last_block_len = (int)(srcend - src);
  if (last_block_len != 0) {
    __mmask64 input_mask = ((__mmask64)1 << last_block_len) - 1;
    block64 b;
    load_block_partial(&b, src, input_mask);
    uint64_t error = 0;
    uint64_t badcharmask =
        to_base64_mask<base64_url, ignore_garbage, default_or_url>(&b, &error,
                                                                   input_mask);
    if (!ignore_garbage && error) {
      size_t error_offset = _tzcnt_u64(error);
      return {error_code::INVALID_BASE64_CHARACTER,
              size_t(src - srcinit + error_offset), size_t(dst - dstinit)};
    }
    src += last_block_len;
    bufferptr += compress_block(&b, badcharmask, bufferptr);
  }

  char *buffer_start = buffer;
  for (; buffer_start + 64 <= bufferptr; buffer_start += 64) {
    base64_decode_block(dst, buffer_start);
    dst += 48;
  }

  if ((bufferptr - buffer_start) != 0) {
    size_t rem = (bufferptr - buffer_start);
    int idx = rem % 4;
    __mmask64 mask = ((__mmask64)1 << rem) - 1;
    __m512i input = _mm512_maskz_loadu_epi8(mask, buffer_start);
    size_t output_len = (rem / 4) * 3;
    __mmask64 output_mask = mask >> (rem - output_len);
    const __m512i merge_ab_and_bc =
        _mm512_maddubs_epi16(input, _mm512_set1_epi32(0x01400140));
    const __m512i merged =
        _mm512_madd_epi16(merge_ab_and_bc, _mm512_set1_epi32(0x00011000));
    const __m512i pack = _mm512_set_epi8(
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 61, 62, 56, 57, 58,
        52, 53, 54, 48, 49, 50, 44, 45, 46, 40, 41, 42, 36, 37, 38, 32, 33, 34,
        28, 29, 30, 24, 25, 26, 20, 21, 22, 16, 17, 18, 12, 13, 14, 8, 9, 10, 4,
        5, 6, 0, 1, 2);
    const __m512i shuffled = _mm512_permutexvar_epi8(pack, merged);

    // The idea here is that in loose mode,
    // if there is padding at all, it must be used
    // to form 4-wise chunk. However, in loose mode,
    // we do accept no padding at all.
    if (!ignore_garbage &&
        last_chunk_options == last_chunk_handling_options::loose &&
        (idx >= 2) && padding_characters > 0 &&
        ((idx + padding_characters) & 3) != 0) {
      return {INVALID_BASE64_CHARACTER, size_t(src - srcinit),
              size_t(dst - dstinit), true};
    } else

      // The idea here is that in strict mode, we do not want to accept
      // incomplete base64 chunks. So if the chunk was otherwise valid, we
      // return BASE64_INPUT_REMAINDER.
      if (!ignore_garbage &&
          last_chunk_options == last_chunk_handling_options::strict &&
          (idx >= 2) && ((idx + padding_characters) & 3) != 0) {
        // The partial chunk was at src - idx
        _mm512_mask_storeu_epi8((__m512i *)dst, output_mask, shuffled);
        dst += output_len;
        return {BASE64_INPUT_REMAINDER, equallocation, size_t(dst - dstinit)};
      } else
        // If there is a partial chunk with insufficient padding, with
        // stop_before_partial, we need to just ignore it. In "only full" mode,
        // skip the minute there are padding characters.
        if ((last_chunk_options ==
                 last_chunk_handling_options::stop_before_partial &&
             (padding_characters + idx < 4) &&
             (idx >= 2 || padding_characters == 0)) ||
            (last_chunk_options ==
                 last_chunk_handling_options::only_full_chunks &&
             (idx >= 2 || padding_characters == 0))) {
          _mm512_mask_storeu_epi8((__m512i *)dst, output_mask, shuffled);
          dst += output_len;
          // we need to rewind src to before the partial chunk
          size_t characters_to_skip = idx;
          while (characters_to_skip > 0) {
            src--;
            auto c = *src;
            uint8_t code = to_base64[uint8_t(c)];
            if (simdutf::scalar::base64::is_eight_byte(c) && code <= 63) {
              characters_to_skip--;
            }
          }
          return {SUCCESS, size_t(src - srcinit), size_t(dst - dstinit)};
        } else {
          if (idx == 2) {
            if (!ignore_garbage &&
                last_chunk_options == last_chunk_handling_options::strict) {
              uint32_t triple = (uint32_t(bufferptr[-2]) << 3 * 6) +
                                (uint32_t(bufferptr[-1]) << 2 * 6);
              if (triple & 0xffff) {
                _mm512_mask_storeu_epi8((__m512i *)dst, output_mask, shuffled);
                dst += output_len;
                return {BASE64_EXTRA_BITS, size_t(src - srcinit),
                        size_t(dst - dstinit)};
              }
            }
            output_mask = (output_mask << 1) | 1;
            output_len += 1;
            _mm512_mask_storeu_epi8((__m512i *)dst, output_mask, shuffled);
            dst += output_len;
          } else if (idx == 3) {
            if (!ignore_garbage &&
                last_chunk_options == last_chunk_handling_options::strict) {
              uint32_t triple = (uint32_t(bufferptr[-3]) << 3 * 6) +
                                (uint32_t(bufferptr[-2]) << 2 * 6) +
                                (uint32_t(bufferptr[-1]) << 1 * 6);
              if (triple & 0xff) {
                _mm512_mask_storeu_epi8((__m512i *)dst, output_mask, shuffled);
                dst += output_len;
                return {BASE64_EXTRA_BITS, size_t(src - srcinit),
                        size_t(dst - dstinit)};
              }
            }
            output_mask = (output_mask << 2) | 3;
            output_len += 2;
            _mm512_mask_storeu_epi8((__m512i *)dst, output_mask, shuffled);
            dst += output_len;
          } else if (!ignore_garbage && idx == 1 &&
                     (!is_partial(last_chunk_options) ||
                      (is_partial(last_chunk_options) &&
                       padding_characters > 0))) {
            _mm512_mask_storeu_epi8((__m512i *)dst, output_mask, shuffled);
            dst += output_len;
            return {BASE64_INPUT_REMAINDER, size_t(src - srcinit),
                    size_t(dst - dstinit)};
          } else if (!ignore_garbage && idx == 0 && padding_characters > 0) {
            _mm512_mask_storeu_epi8((__m512i *)dst, output_mask, shuffled);
            dst += output_len;
            return {INVALID_BASE64_CHARACTER, equallocation,
                    size_t(dst - dstinit)};
          } else {
            _mm512_mask_storeu_epi8((__m512i *)dst, output_mask, shuffled);
            dst += output_len;
          }
        }

    if (!ignore_garbage && !is_partial(last_chunk_options) &&
        padding_characters > 0) {
      size_t output_count = size_t(dst - dstinit);
      if ((output_count % 3 == 0) ||
          ((output_count % 3) + 1 + padding_characters != 4)) {
        return {INVALID_BASE64_CHARACTER, equallocation, output_count};
      }
    }
    return {SUCCESS, full_input_length, size_t(dst - dstinit)};
  }

  if (!ignore_garbage && padding_characters > 0) {
    if ((size_t(dst - dstinit) % 3 == 0) ||
        ((size_t(dst - dstinit) % 3) + 1 + padding_characters != 4)) {
      return {INVALID_BASE64_CHARACTER, equallocation, size_t(dst - dstinit)};
    }
  }
  return {SUCCESS, srclen, size_t(dst - dstinit)};
}
/* end file src\icelake\icelake_base64.inl.cpp */
/* begin file src\icelake\icelake_find.inl.cpp */
simdutf_really_inline const char *util_find(const char *start, const char *end,
                                            char character) noexcept {
  // Handle empty or invalid range
  if (start >= end)
    return end;

  // Process 64 bytes (512 bits) at a time with AVX-512
  const size_t step = 64;
  __m512i char_vec = _mm512_set1_epi8(character);

  // Main loop for full 64-byte chunks
  while (size_t(end - start) >= step) {
    __m512i data = _mm512_loadu_si512(reinterpret_cast<const __m512i *>(start));
    __mmask64 mask = _mm512_cmpeq_epi8_mask(data, char_vec);

    if (mask != 0) {
      // Found a match, return the first one
      size_t index = _tzcnt_u64(mask);
      return start + index;
    }

    start += step;
  }

  // Handle remaining bytes with masked load
  size_t remaining = end - start;
  if (remaining > 0) {
    // Create a mask for the remaining bytes using shifted 0xFFFFFFFFFFFFFFFF
    __mmask64 load_mask = 0xFFFFFFFFFFFFFFFF >> (64 - remaining);
    __m512i data = _mm512_maskz_loadu_epi8(
        load_mask, reinterpret_cast<const __m512i *>(start));
    __mmask64 match_mask = _mm512_cmpeq_epi8_mask(data, char_vec);

    // Apply load mask to avoid false positives
    match_mask &= load_mask;

    if (match_mask != 0) {
      // Found a match in the remaining bytes
      size_t index = _tzcnt_u64(match_mask);
      return start + index;
    }
  }

  return end;
}

simdutf_really_inline const char16_t *util_find(const char16_t *start,
                                                const char16_t *end,
                                                char16_t character) noexcept {
  // Handle empty or invalid range
  if (start >= end)
    return end;

  // Process 32 char16_t (64 bytes, 512 bits) at a time with AVX-512
  const size_t step = 32;
  __m512i char_vec = _mm512_set1_epi16(character);

  // Main loop for full 32-element chunks
  while (size_t(end - start) >= step) {
    __m512i data = _mm512_loadu_si512(reinterpret_cast<const __m512i *>(start));
    __mmask32 mask = _mm512_cmpeq_epi16_mask(data, char_vec);

    if (mask != 0) {
      // Found a match, return the first one
      size_t index = _tzcnt_u64(mask);
      return start + index;
    }

    start += step;
  }

  // Handle remaining elements with masked load
  size_t remaining = end - start;
  if (remaining > 0) {
    // Create a mask for the remaining elements using shifted 0xFFFFFFFF
    __mmask32 load_mask = 0xFFFFFFFF >> (32 - remaining);
    __m512i data = _mm512_maskz_loadu_epi16(
        load_mask, reinterpret_cast<const __m512i *>(start));
    __mmask32 match_mask = _mm512_cmpeq_epi16_mask(data, char_vec);

    // Apply load mask to avoid false positives
    match_mask &= load_mask;

    if (match_mask != 0) {
      // Found a match in the remaining elements
      size_t index = _tzcnt_u64(match_mask);
      return start + index;
    }
  }

  return end;
}
/* end file src\icelake\icelake_find.inl.cpp */

#include <cstdint>

} // namespace
} // namespace icelake
} // namespace simdutf

namespace simdutf {
namespace icelake {

simdutf_warn_unused result implementation::base64_to_binary(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused result implementation::base64_to_binary(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

size_t implementation::binary_to_base64(const char *input, size_t length,
                                        char *output,
                                        base64_options options) const noexcept {
  if (options & base64_url) {
    return encode_base64<true>(output, input, length, options);
  } else {
    return encode_base64<false>(output, input, length, options);
  }
}

const char *implementation::find(const char *start, const char *end,
                                 char character) const noexcept {
  return util_find(start, end, character);
}
const char16_t *implementation::find(const char16_t *start, const char16_t *end,
                                     char16_t character) const noexcept {
  return util_find(start, end, character);
}

} // namespace icelake
} // namespace simdutf

/* begin file src\simdutf\icelake\end.h */
#if SIMDUTF_CAN_ALWAYS_RUN_ICELAKE
// nothing needed.
#else
SIMDUTF_UNTARGET_REGION
#endif


#if SIMDUTF_GCC11ORMORE // workaround for
                        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105593
SIMDUTF_POP_DISABLE_WARNINGS
#endif // end of workaround
/* end file src\simdutf\icelake\end.h */
/* end file src\icelake\implementation.cpp */
#endif
#if SIMDUTF_IMPLEMENTATION_HASWELL
/* begin file src\haswell\implementation.cpp */
/* begin file src\simdutf\haswell\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "haswell"
// #define SIMDUTF_IMPLEMENTATION haswell
#define SIMDUTF_SIMD_HAS_BYTEMASK 1

#if SIMDUTF_CAN_ALWAYS_RUN_HASWELL
// nothing needed.
#else
SIMDUTF_TARGET_HASWELL
#endif

#if SIMDUTF_GCC11ORMORE // workaround for
                        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105593
// clang-format off
SIMDUTF_DISABLE_GCC_WARNING(-Wmaybe-uninitialized)
// clang-format on
#endif // end of workaround
/* end file src\simdutf\haswell\begin.h */

namespace simdutf {
namespace haswell {
namespace {
#ifndef SIMDUTF_HASWELL_H
  #error "haswell.h must be included"
#endif
using namespace simd;

/* begin file src\haswell\avx2_base64.cpp */
/**
 * References and further reading:
 *
 * Wojciech Muła, Daniel Lemire, Base64 encoding and decoding at almost the
 * speed of a memory copy, Software: Practice and Experience 50 (2), 2020.
 * https://arxiv.org/abs/1910.05109
 *
 * Wojciech Muła, Daniel Lemire, Faster Base64 Encoding and Decoding using AVX2
 * Instructions, ACM Transactions on the Web 12 (3), 2018.
 * https://arxiv.org/abs/1704.00605
 *
 * Simon Josefsson. 2006. The Base16, Base32, and Base64 Data Encodings.
 * https://tools.ietf.org/html/rfc4648. (2006). Internet Engineering Task Force,
 * Request for Comments: 4648.
 *
 * Alfred Klomp. 2014a. Fast Base64 encoding/decoding with SSE vectorization.
 * http://www.alfredklomp.com/programming/sse-base64/. (2014).
 *
 * Alfred Klomp. 2014b. Fast Base64 stream encoder/decoder in C99, with SIMD
 * acceleration. https://github.com/aklomp/base64. (2014).
 *
 * Hanson Char. 2014. A Fast and Correct Base 64 Codec. (2014).
 * https://aws.amazon.com/blogs/developer/a-fast-and-correct-base-64-codec/
 *
 * Nick Kopp. 2013. Base64 Encoding on a GPU.
 * https://www.codeproject.com/Articles/276993/Base-Encoding-on-a-GPU. (2013).
 */

template <bool base64_url>
simdutf_really_inline __m256i lookup_pshufb_improved(const __m256i input) {
  // credit: Wojciech Muła
  __m256i result = _mm256_subs_epu8(input, _mm256_set1_epi8(51));
  const __m256i less = _mm256_cmpgt_epi8(_mm256_set1_epi8(26), input);
  result =
      _mm256_or_si256(result, _mm256_and_si256(less, _mm256_set1_epi8(13)));
  __m256i shift_LUT;
  if (base64_url) {
    shift_LUT = _mm256_setr_epi8(
        'a' - 26, '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
        '0' - 52, '0' - 52, '0' - 52, '0' - 52, '-' - 62, '_' - 63, 'A', 0, 0,

        'a' - 26, '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
        '0' - 52, '0' - 52, '0' - 52, '0' - 52, '-' - 62, '_' - 63, 'A', 0, 0);
  } else {
    shift_LUT = _mm256_setr_epi8(
        'a' - 26, '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
        '0' - 52, '0' - 52, '0' - 52, '0' - 52, '+' - 62, '/' - 63, 'A', 0, 0,

        'a' - 26, '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
        '0' - 52, '0' - 52, '0' - 52, '0' - 52, '+' - 62, '/' - 63, 'A', 0, 0);
  }

  result = _mm256_shuffle_epi8(shift_LUT, result);
  return _mm256_add_epi8(result, input);
}

template <bool isbase64url>
size_t encode_base64(char *dst, const char *src, size_t srclen,
                     base64_options options) {
  // credit: Wojciech Muła
  const uint8_t *input = (const uint8_t *)src;

  uint8_t *out = (uint8_t *)dst;
  const __m256i shuf =
      _mm256_set_epi8(10, 11, 9, 10, 7, 8, 6, 7, 4, 5, 3, 4, 1, 2, 0, 1,

                      10, 11, 9, 10, 7, 8, 6, 7, 4, 5, 3, 4, 1, 2, 0, 1);
  size_t i = 0;
  for (; i + 100 <= srclen; i += 96) {
    const __m128i lo0 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 0));
    const __m128i hi0 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 1));
    const __m128i lo1 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 2));
    const __m128i hi1 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 3));
    const __m128i lo2 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 4));
    const __m128i hi2 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 5));
    const __m128i lo3 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 6));
    const __m128i hi3 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 7));

    __m256i in0 = _mm256_shuffle_epi8(_mm256_set_m128i(hi0, lo0), shuf);
    __m256i in1 = _mm256_shuffle_epi8(_mm256_set_m128i(hi1, lo1), shuf);
    __m256i in2 = _mm256_shuffle_epi8(_mm256_set_m128i(hi2, lo2), shuf);
    __m256i in3 = _mm256_shuffle_epi8(_mm256_set_m128i(hi3, lo3), shuf);

    const __m256i t0_0 = _mm256_and_si256(in0, _mm256_set1_epi32(0x0fc0fc00));
    const __m256i t0_1 = _mm256_and_si256(in1, _mm256_set1_epi32(0x0fc0fc00));
    const __m256i t0_2 = _mm256_and_si256(in2, _mm256_set1_epi32(0x0fc0fc00));
    const __m256i t0_3 = _mm256_and_si256(in3, _mm256_set1_epi32(0x0fc0fc00));

    const __m256i t1_0 =
        _mm256_mulhi_epu16(t0_0, _mm256_set1_epi32(0x04000040));
    const __m256i t1_1 =
        _mm256_mulhi_epu16(t0_1, _mm256_set1_epi32(0x04000040));
    const __m256i t1_2 =
        _mm256_mulhi_epu16(t0_2, _mm256_set1_epi32(0x04000040));
    const __m256i t1_3 =
        _mm256_mulhi_epu16(t0_3, _mm256_set1_epi32(0x04000040));

    const __m256i t2_0 = _mm256_and_si256(in0, _mm256_set1_epi32(0x003f03f0));
    const __m256i t2_1 = _mm256_and_si256(in1, _mm256_set1_epi32(0x003f03f0));
    const __m256i t2_2 = _mm256_and_si256(in2, _mm256_set1_epi32(0x003f03f0));
    const __m256i t2_3 = _mm256_and_si256(in3, _mm256_set1_epi32(0x003f03f0));

    const __m256i t3_0 =
        _mm256_mullo_epi16(t2_0, _mm256_set1_epi32(0x01000010));
    const __m256i t3_1 =
        _mm256_mullo_epi16(t2_1, _mm256_set1_epi32(0x01000010));
    const __m256i t3_2 =
        _mm256_mullo_epi16(t2_2, _mm256_set1_epi32(0x01000010));
    const __m256i t3_3 =
        _mm256_mullo_epi16(t2_3, _mm256_set1_epi32(0x01000010));

    const __m256i input0 = _mm256_or_si256(t1_0, t3_0);
    const __m256i input1 = _mm256_or_si256(t1_1, t3_1);
    const __m256i input2 = _mm256_or_si256(t1_2, t3_2);
    const __m256i input3 = _mm256_or_si256(t1_3, t3_3);

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(out),
                        lookup_pshufb_improved<isbase64url>(input0));
    out += 32;

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(out),
                        lookup_pshufb_improved<isbase64url>(input1));
    out += 32;

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(out),
                        lookup_pshufb_improved<isbase64url>(input2));
    out += 32;
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(out),
                        lookup_pshufb_improved<isbase64url>(input3));
    out += 32;
  }
  for (; i + 28 <= srclen; i += 24) {
    // lo = [xxxx|DDDC|CCBB|BAAA]
    // hi = [xxxx|HHHG|GGFF|FEEE]
    const __m128i lo =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(input + i));
    const __m128i hi =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(input + i + 4 * 3));

    // bytes from groups A, B and C are needed in separate 32-bit lanes
    // in = [0HHH|0GGG|0FFF|0EEE[0DDD|0CCC|0BBB|0AAA]
    __m256i in = _mm256_shuffle_epi8(_mm256_set_m128i(hi, lo), shuf);

    // this part is well commented in encode.sse.cpp

    const __m256i t0 = _mm256_and_si256(in, _mm256_set1_epi32(0x0fc0fc00));
    const __m256i t1 = _mm256_mulhi_epu16(t0, _mm256_set1_epi32(0x04000040));
    const __m256i t2 = _mm256_and_si256(in, _mm256_set1_epi32(0x003f03f0));
    const __m256i t3 = _mm256_mullo_epi16(t2, _mm256_set1_epi32(0x01000010));
    const __m256i indices = _mm256_or_si256(t1, t3);

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(out),
                        lookup_pshufb_improved<isbase64url>(indices));
    out += 32;
  }
  return i / 3 * 4 + scalar::base64::tail_encode_base64((char *)out, src + i,
                                                        srclen - i, options);
}

static inline void compress(__m128i data, uint16_t mask, char *output) {
  if (mask == 0) {
    _mm_storeu_si128(reinterpret_cast<__m128i *>(output), data);
    return;
  }
  // this particular implementation was inspired by work done by @animetosho
  // we do it in two steps, first 8 bytes and then second 8 bytes
  uint8_t mask1 = uint8_t(mask);      // least significant 8 bits
  uint8_t mask2 = uint8_t(mask >> 8); // most significant 8 bits
  // next line just loads the 64-bit values thintable_epi8[mask1] and
  // thintable_epi8[mask2] into a 128-bit register, using only
  // two instructions on most compilers.

  __m128i shufmask = _mm_set_epi64x(tables::base64::thintable_epi8[mask2],
                                    tables::base64::thintable_epi8[mask1]);
  // we increment by 0x08 the second half of the mask
  shufmask =
      _mm_add_epi8(shufmask, _mm_set_epi32(0x08080808, 0x08080808, 0, 0));
  // this is the version "nearly pruned"
  __m128i pruned = _mm_shuffle_epi8(data, shufmask);
  // we still need to put the two halves together.
  // we compute the popcount of the first half:
  int pop1 = tables::base64::BitsSetTable256mul2[mask1];
  // then load the corresponding mask, what it does is to write
  // only the first pop1 bytes from the first 8 bytes, and then
  // it fills in with the bytes from the second 8 bytes + some filling
  // at the end.
  __m128i compactmask = _mm_loadu_si128(reinterpret_cast<const __m128i *>(
      tables::base64::pshufb_combine_table + pop1 * 8));
  __m128i answer = _mm_shuffle_epi8(pruned, compactmask);

  _mm_storeu_si128(reinterpret_cast<__m128i *>(output), answer);
}

// --- decoding -----------------------------------------------

template <typename = void>
simdutf_really_inline void compress(__m256i data, uint32_t mask, char *output) {
  if (mask == 0) {
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(output), data);
    return;
  }
  compress(_mm256_castsi256_si128(data), uint16_t(mask), output);
  compress(_mm256_extracti128_si256(data, 1), uint16_t(mask >> 16),
           output + count_ones(~mask & 0xFFFF));
}

template <typename = void>
simdutf_really_inline void base64_decode(char *out, __m256i str) {
  // credit: aqrit
  const __m256i pack_shuffle =
      _mm256_setr_epi8(2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1,
                       2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1);
  const __m256i t0 = _mm256_maddubs_epi16(str, _mm256_set1_epi32(0x01400140));
  const __m256i t1 = _mm256_madd_epi16(t0, _mm256_set1_epi32(0x00011000));
  const __m256i t2 = _mm256_shuffle_epi8(t1, pack_shuffle);

  // Store the output:
  _mm_storeu_si128((__m128i *)out, _mm256_castsi256_si128(t2));
  _mm_storeu_si128((__m128i *)(out + 12), _mm256_extracti128_si256(t2, 1));
}

template <typename = void>
simdutf_really_inline void base64_decode_block(char *out, const char *src) {
  base64_decode(out,
                _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src)));
  base64_decode(out + 24, _mm256_loadu_si256(
                              reinterpret_cast<const __m256i *>(src + 32)));
}

template <typename = void>
simdutf_really_inline void base64_decode_block_safe(char *out,
                                                    const char *src) {
  base64_decode(out,
                _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src)));
  char buffer[32]; // We enforce safety with a buffer.
  base64_decode(
      buffer, _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src + 32)));
  std::memcpy(out + 24, buffer, 24);
}

// --- decoding - base64 class --------------------------------

class block64 {
  __m256i chunks[2];

public:
  // The caller of this function is responsible to ensure that there are 64
  // bytes available from reading at src.
  simdutf_really_inline block64(const char *src) {
    chunks[0] = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src));
    chunks[1] = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src + 32));
  }

  // The caller of this function is responsible to ensure that there are 128
  // bytes available from reading at src.
  simdutf_really_inline block64(const char16_t *src) {
    const auto m1 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src));
    const auto m2 =
        _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src + 16));
    const auto m3 =
        _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src + 32));
    const auto m4 =
        _mm256_loadu_si256(reinterpret_cast<const __m256i *>(src + 48));

    const auto m1p = _mm256_permute2x128_si256(m1, m2, 0x20);
    const auto m2p = _mm256_permute2x128_si256(m1, m2, 0x31);
    const auto m3p = _mm256_permute2x128_si256(m3, m4, 0x20);
    const auto m4p = _mm256_permute2x128_si256(m3, m4, 0x31);

    chunks[0] = _mm256_packus_epi16(m1p, m2p);
    chunks[1] = _mm256_packus_epi16(m3p, m4p);
  }

  simdutf_really_inline void copy_block(char *output) {
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(output), chunks[0]);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(output + 32), chunks[1]);
  }

  // decode 64 bytes and output 48 bytes
  simdutf_really_inline void base64_decode_block(char *out) {
    base64_decode(out, chunks[0]);
    base64_decode(out + 24, chunks[1]);
  }

  simdutf_really_inline void base64_decode_block_safe(char *out) {
    base64_decode(out, chunks[0]);
    char buffer[32]; // We enforce safety with a buffer.
    base64_decode(buffer, chunks[1]);
    std::memcpy(out + 24, buffer, 24);
  }

  template <bool base64_url, bool ignore_garbage, bool default_or_url>
  simdutf_really_inline uint64_t to_base64_mask(uint64_t *error) {
    uint32_t err0 = 0;
    uint32_t err1 = 0;
    uint64_t m0 = to_base64_mask<base64_url, ignore_garbage, default_or_url>(
        &chunks[0], &err0);
    uint64_t m1 = to_base64_mask<base64_url, ignore_garbage, default_or_url>(
        &chunks[1], &err1);
    if (!ignore_garbage) {
      *error = err0 | ((uint64_t)err1 << 32);
    }
    return m0 | (m1 << 32);
  }

  template <bool base64_url, bool ignore_garbage, bool default_or_url>
  simdutf_really_inline uint32_t to_base64_mask(__m256i *src, uint32_t *error) {
    const __m256i ascii_space_tbl =
        _mm256_setr_epi8(0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x9, 0xa,
                         0x0, 0xc, 0xd, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0,
                         0x0, 0x0, 0x0, 0x9, 0xa, 0x0, 0xc, 0xd, 0x0, 0x0);
    // credit: aqrit
    __m256i delta_asso;
    if (default_or_url) {
      delta_asso = _mm256_setr_epi8(
          0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x11, 0x00, 0x16, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
          0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x16);
    } else if (base64_url) {
      delta_asso = _mm256_setr_epi8(0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0,
                                    0x0, 0x0, 0x0, 0x0, 0xF, 0x0, 0xF, 0x1, 0x1,
                                    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0,
                                    0x0, 0x0, 0xF, 0x0, 0xF);
    } else {
      delta_asso = _mm256_setr_epi8(
          0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x0F, 0x00, 0x0F, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
          0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x0F);
    }

    __m256i delta_values;
    if (default_or_url) {
      delta_values = _mm256_setr_epi8(
          uint8_t(0xBF), uint8_t(0xE0), uint8_t(0xB9), uint8_t(0x13),
          uint8_t(0x04), uint8_t(0xBF), uint8_t(0xBF), uint8_t(0xB9),
          uint8_t(0xB9), uint8_t(0x00), uint8_t(0xFF), uint8_t(0x11),
          uint8_t(0xFF), uint8_t(0xBF), uint8_t(0x10), uint8_t(0xB9),
          uint8_t(0xBF), uint8_t(0xE0), uint8_t(0xB9), uint8_t(0x13),
          uint8_t(0x04), uint8_t(0xBF), uint8_t(0xBF), uint8_t(0xB9),
          uint8_t(0xB9), uint8_t(0x00), uint8_t(0xFF), uint8_t(0x11),
          uint8_t(0xFF), uint8_t(0xBF), uint8_t(0x10), uint8_t(0xB9));
    } else if (base64_url) {
      delta_values = _mm256_setr_epi8(
          0x0, 0x0, 0x0, 0x13, 0x4, uint8_t(0xBF), uint8_t(0xBF), uint8_t(0xB9),
          uint8_t(0xB9), 0x0, 0x11, uint8_t(0xC3), uint8_t(0xBF), uint8_t(0xE0),
          uint8_t(0xB9), uint8_t(0xB9), 0x0, 0x0, 0x0, 0x13, 0x4, uint8_t(0xBF),
          uint8_t(0xBF), uint8_t(0xB9), uint8_t(0xB9), 0x0, 0x11, uint8_t(0xC3),
          uint8_t(0xBF), uint8_t(0xE0), uint8_t(0xB9), uint8_t(0xB9));
    } else {
      delta_values = _mm256_setr_epi8(
          int8_t(0x00), int8_t(0x00), int8_t(0x00), int8_t(0x13), int8_t(0x04),
          int8_t(0xBF), int8_t(0xBF), int8_t(0xB9), int8_t(0xB9), int8_t(0x00),
          int8_t(0x10), int8_t(0xC3), int8_t(0xBF), int8_t(0xBF), int8_t(0xB9),
          int8_t(0xB9), int8_t(0x00), int8_t(0x00), int8_t(0x00), int8_t(0x13),
          int8_t(0x04), int8_t(0xBF), int8_t(0xBF), int8_t(0xB9), int8_t(0xB9),
          int8_t(0x00), int8_t(0x10), int8_t(0xC3), int8_t(0xBF), int8_t(0xBF),
          int8_t(0xB9), int8_t(0xB9));
    }

    __m256i check_asso;
    if (default_or_url) {
      check_asso = _mm256_setr_epi8(
          0x0D, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03,
          0x07, 0x0B, 0x0E, 0x0B, 0x06, 0x0D, 0x01, 0x01, 0x01, 0x01, 0x01,
          0x01, 0x01, 0x01, 0x01, 0x03, 0x07, 0x0B, 0x0E, 0x0B, 0x06);
    } else if (base64_url) {
      check_asso = _mm256_setr_epi8(0xD, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
                                    0x1, 0x3, 0x7, 0xB, 0xE, 0xB, 0x6, 0xD, 0x1,
                                    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x3,
                                    0x7, 0xB, 0xE, 0xB, 0x6);
    } else {
      check_asso = _mm256_setr_epi8(
          0x0D, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03,
          0x07, 0x0B, 0x0B, 0x0B, 0x0F, 0x0D, 0x01, 0x01, 0x01, 0x01, 0x01,
          0x01, 0x01, 0x01, 0x01, 0x03, 0x07, 0x0B, 0x0B, 0x0B, 0x0F);
    }
    __m256i check_values;
    if (default_or_url) {
      check_values = _mm256_setr_epi8(
          uint8_t(0x80), uint8_t(0x80), uint8_t(0x80), uint8_t(0x80),
          uint8_t(0xCF), uint8_t(0xBF), uint8_t(0xD5), uint8_t(0xA6),
          uint8_t(0xB5), uint8_t(0xA1), uint8_t(0x00), uint8_t(0x80),
          uint8_t(0x00), uint8_t(0x80), uint8_t(0x00), uint8_t(0x80),
          uint8_t(0x80), uint8_t(0x80), uint8_t(0x80), uint8_t(0x80),
          uint8_t(0xCF), uint8_t(0xBF), uint8_t(0xD5), uint8_t(0xA6),
          uint8_t(0xB5), uint8_t(0xA1), uint8_t(0x00), uint8_t(0x80),
          uint8_t(0x00), uint8_t(0x80), uint8_t(0x00), uint8_t(0x80));
    } else if (base64_url) {
      check_values = _mm256_setr_epi8(
          uint8_t(0x80), uint8_t(0x80), uint8_t(0x80), uint8_t(0x80),
          uint8_t(0xCF), uint8_t(0xBF), uint8_t(0xB6), uint8_t(0xA6),
          uint8_t(0xB5), uint8_t(0xA1), 0x0, uint8_t(0x80), 0x0, uint8_t(0x80),
          0x0, uint8_t(0x80), uint8_t(0x80), uint8_t(0x80), uint8_t(0x80),
          uint8_t(0x80), uint8_t(0xCF), uint8_t(0xBF), uint8_t(0xB6),
          uint8_t(0xA6), uint8_t(0xB5), uint8_t(0xA1), 0x0, uint8_t(0x80), 0x0,
          uint8_t(0x80), 0x0, uint8_t(0x80));
    } else {
      check_values = _mm256_setr_epi8(
          int8_t(0x80), int8_t(0x80), int8_t(0x80), int8_t(0x80), int8_t(0xCF),
          int8_t(0xBF), int8_t(0xD5), int8_t(0xA6), int8_t(0xB5), int8_t(0x86),
          int8_t(0xD1), int8_t(0x80), int8_t(0xB1), int8_t(0x80), int8_t(0x91),
          int8_t(0x80), int8_t(0x80), int8_t(0x80), int8_t(0x80), int8_t(0x80),
          int8_t(0xCF), int8_t(0xBF), int8_t(0xD5), int8_t(0xA6), int8_t(0xB5),
          int8_t(0x86), int8_t(0xD1), int8_t(0x80), int8_t(0xB1), int8_t(0x80),
          int8_t(0x91), int8_t(0x80));
    }
    const __m256i shifted = _mm256_srli_epi32(*src, 3);
    __m256i delta_hash =
        _mm256_avg_epu8(_mm256_shuffle_epi8(delta_asso, *src), shifted);
    if (default_or_url) {
      delta_hash = _mm256_and_si256(delta_hash, _mm256_set1_epi8(0xf));
    }
    const __m256i check_hash =
        _mm256_avg_epu8(_mm256_shuffle_epi8(check_asso, *src), shifted);
    const __m256i out =
        _mm256_adds_epi8(_mm256_shuffle_epi8(delta_values, delta_hash), *src);
    const __m256i chk =
        _mm256_adds_epi8(_mm256_shuffle_epi8(check_values, check_hash), *src);
    const int mask = _mm256_movemask_epi8(chk);
    if (!ignore_garbage && mask) {
      __m256i ascii_space =
          _mm256_cmpeq_epi8(_mm256_shuffle_epi8(ascii_space_tbl, *src), *src);
      *error = (mask ^ _mm256_movemask_epi8(ascii_space));
    }
    *src = out;
    return (uint32_t)mask;
  }

  simdutf_really_inline uint64_t compress_block(uint64_t mask, char *output) {
    if (is_power_of_two(mask)) {
      return compress_block_single(mask, output);
    }

    uint64_t nmask = ~mask;
    compress(chunks[0], uint32_t(mask), output);
    compress(chunks[1], uint32_t(mask >> 32),
             output + count_ones(nmask & 0xFFFFFFFF));
    return count_ones(nmask);
  }

  simdutf_really_inline size_t compress_block_single(uint64_t mask,
                                                     char *output) {
    const size_t pos64 = trailing_zeroes(mask);
    const int8_t pos = pos64 & 0xf;
    switch (pos64 >> 4) {
    case 0b00: {
      const __m128i lane0 = _mm256_extracti128_si256(chunks[0], 0);
      const __m128i lane1 = _mm256_extracti128_si256(chunks[0], 1);

      const __m128i v0 = _mm_set1_epi8(char(pos - 1));
      const __m128i v1 =
          _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
      const __m128i v2 = _mm_cmpgt_epi8(v1, v0);
      const __m128i sh = _mm_sub_epi8(v1, v2);
      const __m128i compressed = _mm_shuffle_epi8(lane0, sh);

      _mm_storeu_si128((__m128i *)(output + 0 * 16), compressed);
      _mm_storeu_si128((__m128i *)(output + 1 * 16 - 1), lane1);
      _mm256_storeu_si256((__m256i *)(output + 2 * 16 - 1), chunks[1]);
    } break;
    case 0b01: {
      const __m128i lane0 = _mm256_extracti128_si256(chunks[0], 0);
      const __m128i lane1 = _mm256_extracti128_si256(chunks[0], 1);
      _mm_storeu_si128((__m128i *)(output + 0 * 16), lane0);

      const __m128i v0 = _mm_set1_epi8(char(pos - 1));
      const __m128i v1 =
          _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
      const __m128i v2 = _mm_cmpgt_epi8(v1, v0);
      const __m128i sh = _mm_sub_epi8(v1, v2);
      const __m128i compressed = _mm_shuffle_epi8(lane1, sh);

      _mm_storeu_si128((__m128i *)(output + 1 * 16), compressed);
      _mm256_storeu_si256((__m256i *)(output + 2 * 16 - 1), chunks[1]);
    } break;
    case 0b10: {
      const __m128i lane2 = _mm256_extracti128_si256(chunks[1], 0);
      const __m128i lane3 = _mm256_extracti128_si256(chunks[1], 1);

      _mm256_storeu_si256((__m256i *)(output + 0 * 16), chunks[0]);

      const __m128i v0 = _mm_set1_epi8(char(pos - 1));
      const __m128i v1 =
          _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
      const __m128i v2 = _mm_cmpgt_epi8(v1, v0);
      const __m128i sh = _mm_sub_epi8(v1, v2);
      const __m128i compressed = _mm_shuffle_epi8(lane2, sh);

      _mm_storeu_si128((__m128i *)(output + 2 * 16), compressed);
      _mm_storeu_si128((__m128i *)(output + 3 * 16 - 1), lane3);
    } break;
    case 0b11: {
      const __m128i lane2 = _mm256_extracti128_si256(chunks[1], 0);
      const __m128i lane3 = _mm256_extracti128_si256(chunks[1], 1);

      _mm256_storeu_si256((__m256i *)(output + 0 * 16), chunks[0]);
      _mm_storeu_si128((__m128i *)(output + 2 * 16), lane2);

      const __m128i v0 = _mm_set1_epi8(char(pos - 1));
      const __m128i v1 =
          _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
      const __m128i v2 = _mm_cmpgt_epi8(v1, v0);
      const __m128i sh = _mm_sub_epi8(v1, v2);
      const __m128i compressed = _mm_shuffle_epi8(lane3, sh);

      _mm_storeu_si128((__m128i *)(output + 3 * 16), compressed);
    } break;
    }

    return 63;
  }
};
/* end file src\haswell\avx2_base64.cpp */

} // unnamed namespace
} // namespace haswell
} // namespace simdutf

/* begin file src\generic\buf_block_reader.h */
namespace simdutf {
namespace haswell {
namespace {

// Walks through a buffer in block-sized increments, loading the last part with
// spaces
template <size_t STEP_SIZE> struct buf_block_reader {
public:
  simdutf_really_inline buf_block_reader(const uint8_t *_buf, size_t _len);
  simdutf_really_inline size_t block_index();
  simdutf_really_inline bool has_full_block() const;
  simdutf_really_inline const uint8_t *full_block() const;
  /**
   * Get the last block, padded with spaces.
   *
   * There will always be a last block, with at least 1 byte, unless len == 0
   * (in which case this function fills the buffer with spaces and returns 0. In
   * particular, if len == STEP_SIZE there will be 0 full_blocks and 1 remainder
   * block with STEP_SIZE bytes and no spaces for padding.
   *
   * @return the number of effective characters in the last block.
   */
  simdutf_really_inline size_t get_remainder(uint8_t *dst) const;
  simdutf_really_inline void advance();

private:
  const uint8_t *buf;
  const size_t len;
  const size_t lenminusstep;
  size_t idx;
};

// Routines to print masks and text for debugging bitmask operations
simdutf_unused static char *format_input_text_64(const uint8_t *text) {
  static char *buf =
      reinterpret_cast<char *>(malloc(sizeof(simd8x64<uint8_t>) + 1));
  for (size_t i = 0; i < sizeof(simd8x64<uint8_t>); i++) {
    buf[i] = int8_t(text[i]) < ' ' ? '_' : int8_t(text[i]);
  }
  buf[sizeof(simd8x64<uint8_t>)] = '\0';
  return buf;
}

// Routines to print masks and text for debugging bitmask operations
simdutf_unused static char *format_input_text(const simd8x64<uint8_t> &in) {
  static char *buf =
      reinterpret_cast<char *>(malloc(sizeof(simd8x64<uint8_t>) + 1));
  in.store(reinterpret_cast<uint8_t *>(buf));
  for (size_t i = 0; i < sizeof(simd8x64<uint8_t>); i++) {
    if (buf[i] < ' ') {
      buf[i] = '_';
    }
  }
  buf[sizeof(simd8x64<uint8_t>)] = '\0';
  return buf;
}

simdutf_unused static char *format_mask(uint64_t mask) {
  static char *buf = reinterpret_cast<char *>(malloc(64 + 1));
  for (size_t i = 0; i < 64; i++) {
    buf[i] = (mask & (size_t(1) << i)) ? 'X' : ' ';
  }
  buf[64] = '\0';
  return buf;
}

template <size_t STEP_SIZE>
simdutf_really_inline
buf_block_reader<STEP_SIZE>::buf_block_reader(const uint8_t *_buf, size_t _len)
    : buf{_buf}, len{_len}, lenminusstep{len < STEP_SIZE ? 0 : len - STEP_SIZE},
      idx{0} {}

template <size_t STEP_SIZE>
simdutf_really_inline size_t buf_block_reader<STEP_SIZE>::block_index() {
  return idx;
}

template <size_t STEP_SIZE>
simdutf_really_inline bool buf_block_reader<STEP_SIZE>::has_full_block() const {
  return idx < lenminusstep;
}

template <size_t STEP_SIZE>
simdutf_really_inline const uint8_t *
buf_block_reader<STEP_SIZE>::full_block() const {
  return &buf[idx];
}

template <size_t STEP_SIZE>
simdutf_really_inline size_t
buf_block_reader<STEP_SIZE>::get_remainder(uint8_t *dst) const {
  if (len == idx) {
    return 0;
  } // memcpy(dst, null, 0) will trigger an error with some sanitizers
  std::memset(dst, 0x20,
              STEP_SIZE); // std::memset STEP_SIZE because it is more efficient
                          // to write out 8 or 16 bytes at once.
  std::memcpy(dst, buf + idx, len - idx);
  return len - idx;
}

template <size_t STEP_SIZE>
simdutf_really_inline void buf_block_reader<STEP_SIZE>::advance() {
  idx += STEP_SIZE;
}

} // unnamed namespace
} // namespace haswell
} // namespace simdutf
/* end file src\generic\buf_block_reader.h */

// other functions

/* begin file src\generic\base64.h */
/**
 * References and further reading:
 *
 * Wojciech Muła, Daniel Lemire, Base64 encoding and decoding at almost the
 * speed of a memory copy, Software: Practice and Experience 50 (2), 2020.
 * https://arxiv.org/abs/1910.05109
 *
 * Wojciech Muła, Daniel Lemire, Faster Base64 Encoding and Decoding using AVX2
 * Instructions, ACM Transactions on the Web 12 (3), 2018.
 * https://arxiv.org/abs/1704.00605
 *
 * Simon Josefsson. 2006. The Base16, Base32, and Base64 Data Encodings.
 * https://tools.ietf.org/html/rfc4648. (2006). Internet Engineering Task Force,
 * Request for Comments: 4648.
 *
 * Alfred Klomp. 2014a. Fast Base64 encoding/decoding with SSE vectorization.
 * http://www.alfredklomp.com/programming/sse-base64/. (2014).
 *
 * Alfred Klomp. 2014b. Fast Base64 stream encoder/decoder in C99, with SIMD
 * acceleration. https://github.com/aklomp/base64. (2014).
 *
 * Hanson Char. 2014. A Fast and Correct Base 64 Codec. (2014).
 * https://aws.amazon.com/blogs/developer/a-fast-and-correct-base-64-codec/
 *
 * Nick Kopp. 2013. Base64 Encoding on a GPU.
 * https://www.codeproject.com/Articles/276993/Base-Encoding-on-a-GPU. (2013).
 */
namespace simdutf {
namespace haswell {
namespace {
namespace base64 {

/*
    The following template function implements API for Base64 decoding.

    An implementation is responsible for providing the `block64` type and
    associated methods that perform actual conversion. Please refer
    to any vectorized implementation to learn the API of these procedures.
*/
template <bool base64_url, bool ignore_garbage, bool default_or_url,
          typename chartype>
full_result
compress_decode_base64(char *dst, const chartype *src, size_t srclen,
                       base64_options options,
                       last_chunk_handling_options last_chunk_options) {
  const uint8_t *to_base64 =
      default_or_url ? tables::base64::to_base64_default_or_url_value
                     : (base64_url ? tables::base64::to_base64_url_value
                                   : tables::base64::to_base64_value);
  auto ri = simdutf::scalar::base64::find_end(src, srclen, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  srclen = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  (void)full_input_length;
  if (srclen == 0) {
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation, 0};
    }
    return {SUCCESS, 0, 0};
  }
  char *end_of_safe_64byte_zone =
      dst == nullptr
          ? nullptr
          : ((srclen + 3) / 4 * 3 >= 63 ? dst + (srclen + 3) / 4 * 3 - 63
                                        : dst);

  const chartype *const srcinit = src;
  const char *const dstinit = dst;
  const chartype *const srcend = src + srclen;

  constexpr size_t block_size = 6;
  static_assert(block_size >= 2, "block_size must be at least two");
  char buffer[block_size * 64];
  char *bufferptr = buffer;
  if (srclen >= 64) {
    const chartype *const srcend64 = src + srclen - 64;
    while (src <= srcend64) {
      block64 b(src);
      src += 64;
      uint64_t error = 0;
      const uint64_t badcharmask =
          b.to_base64_mask<base64_url, ignore_garbage, default_or_url>(&error);
      if (!ignore_garbage && error) {
        src -= 64;
        const size_t error_offset = trailing_zeroes(error);
        return {error_code::INVALID_BASE64_CHARACTER,
                size_t(src - srcinit + error_offset), size_t(dst - dstinit)};
      }
      if (badcharmask != 0) {
        bufferptr += b.compress_block(badcharmask, bufferptr);
      } else if (bufferptr != buffer) {
        b.copy_block(bufferptr);
        bufferptr += 64;
      } else {
        if (dst >= end_of_safe_64byte_zone) {
          b.base64_decode_block_safe(dst);
        } else {
          b.base64_decode_block(dst);
        }
        dst += 48;
      }
      if (bufferptr >= (block_size - 1) * 64 + buffer) {
        for (size_t i = 0; i < (block_size - 2); i++) {
          base64_decode_block(dst, buffer + i * 64);
          dst += 48;
        }
        if (dst >= end_of_safe_64byte_zone) {
          base64_decode_block_safe(dst, buffer + (block_size - 2) * 64);
        } else {
          base64_decode_block(dst, buffer + (block_size - 2) * 64);
        }
        dst += 48;
        std::memcpy(buffer, buffer + (block_size - 1) * 64,
                    64); // 64 might be too much
        bufferptr -= (block_size - 1) * 64;
      }
    }
  }

  char *buffer_start = buffer;
  // Optimization note: if this is almost full, then it is worth our
  // time, otherwise, we should just decode directly.
  int last_block = (int)((bufferptr - buffer_start) % 64);
  if (last_block != 0 && srcend - src + last_block >= 64) {

    while ((bufferptr - buffer_start) % 64 != 0 && src < srcend) {
      uint8_t val = to_base64[uint8_t(*src)];
      *bufferptr = char(val);
      if (!ignore_garbage &&
          (!scalar::base64::is_eight_byte(*src) || val > 64)) {
        return {error_code::INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit)};
      }
      bufferptr += (val <= 63);
      src++;
    }
  }

  for (; buffer_start + 64 <= bufferptr; buffer_start += 64) {
    if (dst >= end_of_safe_64byte_zone) {
      base64_decode_block_safe(dst, buffer_start);
    } else {
      base64_decode_block(dst, buffer_start);
    }
    dst += 48;
  }
  if ((bufferptr - buffer_start) % 64 != 0) {
    while (buffer_start + 4 < bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
#if !SIMDUTF_IS_BIG_ENDIAN
      triple = scalar::u32_swap_bytes(triple);
#endif
      std::memcpy(dst, &triple, 3);

      dst += 3;
      buffer_start += 4;
    }
    if (buffer_start + 4 <= bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
#if !SIMDUTF_IS_BIG_ENDIAN
      triple = scalar::u32_swap_bytes(triple);
#endif
      std::memcpy(dst, &triple, 3);

      dst += 3;
      buffer_start += 4;
    }
    // we may have 1, 2 or 3 bytes left and we need to decode them so let us
    // backtrack
    int leftover = int(bufferptr - buffer_start);
    while (leftover > 0) {
      if (!ignore_garbage) {
        while (to_base64[uint8_t(*(src - 1))] == 64) {
          src--;
        }
      } else {
        while (to_base64[uint8_t(*(src - 1))] >= 64) {
          src--;
        }
      }
      src--;
      leftover--;
    }
  }
  if (src < srcend + equalsigns) {
    full_result r = scalar::base64::base64_tail_decode(
        dst, src, srcend - src, equalsigns, options, last_chunk_options);
    r = scalar::base64::patch_tail_result(
        r, size_t(src - srcinit), size_t(dst - dstinit), equallocation,
        full_input_length, last_chunk_options);
    return r;
  }
  if (!ignore_garbage && equalsigns > 0) {
    if ((size_t(dst - dstinit) % 3 == 0) ||
        ((size_t(dst - dstinit) % 3) + 1 + equalsigns != 4)) {
      return {INVALID_BASE64_CHARACTER, equallocation, size_t(dst - dstinit)};
    }
  }
  return {SUCCESS, srclen, size_t(dst - dstinit)};
}

} // namespace base64
} // unnamed namespace
} // namespace haswell
} // namespace simdutf
/* end file src\generic\base64.h */
/* begin file src\generic\find.h */
namespace simdutf {
namespace haswell {
namespace {
namespace util {

simdutf_really_inline const char *find(const char *start, const char *end,
                                       char character) noexcept {
  for (; std::distance(start, end) >= 64; start += 64) {
    simd8x64<uint8_t> input(reinterpret_cast<const uint8_t *>(start));
    uint64_t matches = input.eq(uint8_t(character));
    if (matches != 0) {
      // Found a match, return the first one
      int index = trailing_zeroes(matches);
      return start + index;
    }
  }
  return std::find(start, end, character);
}

simdutf_really_inline const char16_t *
find(const char16_t *start, const char16_t *end, char16_t character) noexcept {
  for (; std::distance(start, end) >= 32; start += 32) {
    simd16x32<uint16_t> input(reinterpret_cast<const uint16_t *>(start));
    uint64_t matches = input.eq(uint16_t(character));
    if (matches != 0) {
      // Found a match, return the first one
      int index = trailing_zeroes(matches) / 2;
      return start + index;
    }
  }
  return std::find(start, end, character);
}
} // namespace util
} // namespace
} // namespace haswell
} // namespace simdutf
/* end file src\generic\find.h */

namespace simdutf {
namespace haswell {

simdutf_warn_unused result implementation::base64_to_binary(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused result implementation::base64_to_binary(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

size_t implementation::binary_to_base64(const char *input, size_t length,
                                        char *output,
                                        base64_options options) const noexcept {
  if (options & base64_url) {
    return encode_base64<true>(output, input, length, options);
  } else {
    return encode_base64<false>(output, input, length, options);
  }
}

const char *implementation::find(const char *start, const char *end,
                                 char character) const noexcept {
  return util::find(start, end, character);
}

const char16_t *implementation::find(const char16_t *start, const char16_t *end,
                                     char16_t character) const noexcept {
  return util::find(start, end, character);
}

} // namespace haswell
} // namespace simdutf

/* begin file src\simdutf\haswell\end.h */
#if SIMDUTF_CAN_ALWAYS_RUN_HASWELL
// nothing needed.
#else
SIMDUTF_UNTARGET_REGION
#endif

#undef SIMDUTF_SIMD_HAS_BYTEMASK

#if SIMDUTF_GCC11ORMORE // workaround for
                        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105593
SIMDUTF_POP_DISABLE_WARNINGS
#endif // end of workaround
/* end file src\simdutf\haswell\end.h */
/* end file src\haswell\implementation.cpp */
#endif
#if SIMDUTF_IMPLEMENTATION_PPC64
/* begin file src\ppc64\implementation.cpp */
/* begin file src\simdutf\ppc64\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "ppc64"
// #define SIMDUTF_IMPLEMENTATION ppc64
/* end file src\simdutf\ppc64\begin.h */

/* begin file src\ppc64\ppc64_utf16_to_utf8_tables.h */
// Code generated automatically; DO NOT EDIT
// file generated by scripts/ppc64_convert_utf16_to_utf8.py
#ifndef PPC64_SIMDUTF_UTF16_TO_UTF8_TABLES_H
#define PPC64_SIMDUTF_UTF16_TO_UTF8_TABLES_H

namespace simdutf {
namespace {
namespace tables {
namespace ppc64_utf16_to_utf8 {

#if SIMDUTF_IS_BIG_ENDIAN
// 1 byte for length, 16 bytes for mask
const uint8_t pack_1_2_3_utf8_bytes[256][17] = {
    {12, 1, 0, 16, 3, 2, 18, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80},
    {9, 3, 2, 18, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 0, 16, 3, 2, 18, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 17, 3, 2, 18, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 1, 0, 16, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 0, 16, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 17, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {11, 1, 0, 16, 2, 18, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 2, 18, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 0, 16, 2, 18, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 17, 2, 18, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 1, 0, 16, 19, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 19, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 0, 16, 19, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 17, 19, 5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 1, 0, 16, 3, 2, 18, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 3, 2, 18, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 0, 16, 3, 2, 18, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 17, 3, 2, 18, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 1, 0, 16, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 16, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 17, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 1, 0, 16, 2, 18, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 2, 18, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 2, 18, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 2, 18, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 0, 16, 19, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 19, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 19, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 19, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {11, 1, 0, 16, 3, 2, 18, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 3, 2, 18, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 0, 16, 3, 2, 18, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 17, 3, 2, 18, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 1, 0, 16, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {10, 1, 0, 16, 2, 18, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 2, 18, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 0, 16, 2, 18, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 17, 2, 18, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 1, 0, 16, 19, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 19, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 0, 16, 19, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 17, 19, 4, 20, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 1, 0, 16, 3, 2, 18, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 3, 2, 18, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 0, 16, 3, 2, 18, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 17, 3, 2, 18, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 1, 0, 16, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 1, 0, 16, 2, 18, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 2, 18, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 0, 16, 2, 18, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 17, 2, 18, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 1, 0, 16, 19, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 19, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 19, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 19, 21, 7, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 1, 0, 16, 3, 2, 18, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 3, 2, 18, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 0, 16, 3, 2, 18, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 17, 3, 2, 18, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 1, 0, 16, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 16, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 17, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 1, 0, 16, 2, 18, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 2, 18, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 2, 18, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 2, 18, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 0, 16, 19, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 19, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 19, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 19, 5, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 1, 0, 16, 3, 2, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 3, 2, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 16, 3, 2, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 17, 3, 2, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 1, 0, 16, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {2, 0, 16, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {1, 17, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {5, 1, 0, 16, 2, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 2, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 0, 16, 2, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 17, 2, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 1, 0, 16, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {1, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {3, 0, 16, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {2, 17, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {8, 1, 0, 16, 3, 2, 18, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 3, 2, 18, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 3, 2, 18, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 3, 2, 18, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 1, 0, 16, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 0, 16, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 17, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {7, 1, 0, 16, 2, 18, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 2, 18, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 2, 18, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 2, 18, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 1, 0, 16, 19, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 19, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 16, 19, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 17, 19, 4, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {7, 1, 0, 16, 3, 2, 18, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 3, 2, 18, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 3, 2, 18, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 3, 2, 18, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 1, 0, 16, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {1, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {3, 0, 16, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {2, 17, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {6, 1, 0, 16, 2, 18, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 2, 18, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 16, 2, 18, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 17, 2, 18, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 0, 16, 19, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 19, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {4, 0, 16, 19, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 17, 19, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {11, 1, 0, 16, 3, 2, 18, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 3, 2, 18, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 0, 16, 3, 2, 18, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 17, 3, 2, 18, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 1, 0, 16, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {10, 1, 0, 16, 2, 18, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 2, 18, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 0, 16, 2, 18, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 17, 2, 18, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 1, 0, 16, 19, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 19, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 0, 16, 19, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 17, 19, 5, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 1, 0, 16, 3, 2, 18, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 3, 2, 18, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 3, 2, 18, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 3, 2, 18, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 1, 0, 16, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 0, 16, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 17, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {7, 1, 0, 16, 2, 18, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 2, 18, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 2, 18, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 2, 18, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 1, 0, 16, 19, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 19, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 16, 19, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 17, 19, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {10, 1, 0, 16, 3, 2, 18, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 3, 2, 18, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 0, 16, 3, 2, 18, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 17, 3, 2, 18, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 1, 0, 16, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 1, 0, 16, 2, 18, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 2, 18, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 0, 16, 2, 18, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 17, 2, 18, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 1, 0, 16, 19, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 19, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 19, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 19, 4, 20, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 1, 0, 16, 3, 2, 18, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 3, 2, 18, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 0, 16, 3, 2, 18, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 17, 3, 2, 18, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 1, 0, 16, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 16, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 17, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 1, 0, 16, 2, 18, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 2, 18, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 2, 18, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 2, 18, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 0, 16, 19, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 19, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 19, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 19, 21, 6, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {10, 1, 0, 16, 3, 2, 18, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 3, 2, 18, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 0, 16, 3, 2, 18, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 17, 3, 2, 18, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 1, 0, 16, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 1, 0, 16, 2, 18, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 2, 18, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 0, 16, 2, 18, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 17, 2, 18, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 1, 0, 16, 19, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 19, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 19, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 19, 5, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 0, 16, 3, 2, 18, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 3, 2, 18, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 3, 2, 18, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 3, 2, 18, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 1, 0, 16, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {1, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {3, 0, 16, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {2, 17, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {6, 1, 0, 16, 2, 18, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 2, 18, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 16, 2, 18, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 17, 2, 18, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 0, 16, 19, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 19, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {4, 0, 16, 19, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 17, 19, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {9, 1, 0, 16, 3, 2, 18, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 3, 2, 18, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 0, 16, 3, 2, 18, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 17, 3, 2, 18, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 1, 0, 16, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 16, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 17, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 1, 0, 16, 2, 18, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 2, 18, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 2, 18, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 2, 18, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 0, 16, 19, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 19, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 19, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 19, 4, 20, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 1, 0, 16, 3, 2, 18, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 3, 2, 18, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 16, 3, 2, 18, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 17, 3, 2, 18, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 1, 0, 16, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {4, 0, 16, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 17, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {7, 1, 0, 16, 2, 18, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 2, 18, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 0, 16, 2, 18, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 17, 2, 18, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 1, 0, 16, 19, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 19, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 16, 19, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 17, 19, 21, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
};
#else
// 1 byte for length, 16 bytes for mask
const uint8_t pack_1_2_3_utf8_bytes[256][17] = {
    {12, 0, 1, 17, 2, 3, 19, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80},
    {9, 2, 3, 19, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {11, 1, 17, 2, 3, 19, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80},
    {10, 16, 2, 3, 19, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 0, 1, 17, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 1, 17, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 16, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {11, 0, 1, 17, 3, 19, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 3, 19, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 1, 17, 3, 19, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 16, 3, 19, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 0, 1, 17, 18, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 18, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 1, 17, 18, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 16, 18, 4, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 0, 1, 17, 2, 3, 19, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 2, 3, 19, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 1, 17, 2, 3, 19, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 16, 2, 3, 19, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 1, 17, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 17, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 16, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 0, 1, 17, 3, 19, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 3, 19, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 3, 19, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 3, 19, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 1, 17, 18, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 18, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 18, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 18, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {11, 0, 1, 17, 2, 3, 19, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 2, 3, 19, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 1, 17, 2, 3, 19, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 16, 2, 3, 19, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 0, 1, 17, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {10, 0, 1, 17, 3, 19, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 3, 19, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 1, 17, 3, 19, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 16, 3, 19, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 0, 1, 17, 18, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 18, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 1, 17, 18, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 16, 18, 5, 21, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 0, 1, 17, 2, 3, 19, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 2, 3, 19, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 1, 17, 2, 3, 19, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 16, 2, 3, 19, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 1, 17, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 0, 1, 17, 3, 19, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 3, 19, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 1, 17, 3, 19, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 16, 3, 19, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 0, 1, 17, 18, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 18, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 18, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 18, 20, 6, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 0, 1, 17, 2, 3, 19, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {6, 2, 3, 19, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 1, 17, 2, 3, 19, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 16, 2, 3, 19, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 1, 17, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 17, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 16, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 0, 1, 17, 3, 19, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 3, 19, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 3, 19, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 3, 19, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 1, 17, 18, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 18, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 18, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 18, 4, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 0, 1, 17, 2, 3, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 2, 3, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 17, 2, 3, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 16, 2, 3, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 0, 1, 17, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {2, 1, 17, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {1, 16, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {5, 0, 1, 17, 3, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 3, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 1, 17, 3, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 16, 3, 19, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 0, 1, 17, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {1, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {3, 1, 17, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {2, 16, 18, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {8, 0, 1, 17, 2, 3, 19, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 2, 3, 19, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 2, 3, 19, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 2, 3, 19, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 0, 1, 17, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 1, 17, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 16, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {7, 0, 1, 17, 3, 19, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 3, 19, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 3, 19, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 3, 19, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 0, 1, 17, 18, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 18, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 17, 18, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 16, 18, 5, 21, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {7, 0, 1, 17, 2, 3, 19, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 2, 3, 19, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 2, 3, 19, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 2, 3, 19, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 1, 17, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {1, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {3, 1, 17, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {2, 16, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {6, 0, 1, 17, 3, 19, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 3, 19, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 17, 3, 19, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 16, 3, 19, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 1, 17, 18, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 18, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {4, 1, 17, 18, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 16, 18, 20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {11, 0, 1, 17, 2, 3, 19, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80},
    {8, 2, 3, 19, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {10, 1, 17, 2, 3, 19, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {9, 16, 2, 3, 19, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 0, 1, 17, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {10, 0, 1, 17, 3, 19, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 3, 19, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 1, 17, 3, 19, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 16, 3, 19, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 0, 1, 17, 18, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 18, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 1, 17, 18, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 16, 18, 4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 0, 1, 17, 2, 3, 19, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 2, 3, 19, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 2, 3, 19, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 2, 3, 19, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 0, 1, 17, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {4, 1, 17, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 16, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {7, 0, 1, 17, 3, 19, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 3, 19, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 3, 19, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 3, 19, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 0, 1, 17, 18, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 18, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 17, 18, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 16, 18, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {10, 0, 1, 17, 2, 3, 19, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 2, 3, 19, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 1, 17, 2, 3, 19, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 16, 2, 3, 19, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 1, 17, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 0, 1, 17, 3, 19, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 3, 19, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 1, 17, 3, 19, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 16, 3, 19, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 0, 1, 17, 18, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 18, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 18, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 18, 5, 21, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 0, 1, 17, 2, 3, 19, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 2, 3, 19, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 1, 17, 2, 3, 19, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 16, 2, 3, 19, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 1, 17, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 17, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 16, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 0, 1, 17, 3, 19, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 3, 19, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 3, 19, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 3, 19, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 1, 17, 18, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 18, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 18, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 18, 20, 7, 23, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {10, 0, 1, 17, 2, 3, 19, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {7, 2, 3, 19, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {9, 1, 17, 2, 3, 19, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 16, 2, 3, 19, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 0, 1, 17, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {9, 0, 1, 17, 3, 19, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 3, 19, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 1, 17, 3, 19, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 16, 3, 19, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {8, 0, 1, 17, 18, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 18, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 18, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 18, 4, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 1, 17, 2, 3, 19, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 2, 3, 19, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 2, 3, 19, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 2, 3, 19, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 0, 1, 17, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {1, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {3, 1, 17, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {2, 16, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {6, 0, 1, 17, 3, 19, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 3, 19, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 17, 3, 19, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 16, 3, 19, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 0, 1, 17, 18, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 18, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {4, 1, 17, 18, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 16, 18, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {9, 0, 1, 17, 2, 3, 19, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 2, 3, 19, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 1, 17, 2, 3, 19, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {7, 16, 2, 3, 19, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 0, 1, 17, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 17, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 16, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {8, 0, 1, 17, 3, 19, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 3, 19, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 3, 19, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 3, 19, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 0, 1, 17, 18, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 18, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 18, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 18, 5, 21, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {8, 0, 1, 17, 2, 3, 19, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {5, 2, 3, 19, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {7, 1, 17, 2, 3, 19, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {6, 16, 2, 3, 19, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 0, 1, 17, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {2, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80, 0x80},
    {4, 1, 17, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {3, 16, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {7, 0, 1, 17, 3, 19, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80},
    {4, 3, 19, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {6, 1, 17, 3, 19, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {5, 16, 3, 19, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {6, 0, 1, 17, 18, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {3, 18, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
    {5, 1, 17, 18, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80},
    {4, 16, 18, 20, 22, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
     0x80, 0x80, 0x80},
};
#endif // SIMDUTF_IS_BIG_ENDIAN
} // namespace ppc64_utf16_to_utf8
} // namespace tables
} // unnamed namespace
} // namespace simdutf

#endif // PPC64_SIMDUTF_UTF16_TO_UTF8_TABLES_H
/* end file src\ppc64\ppc64_utf16_to_utf8_tables.h */

namespace simdutf {
namespace ppc64 {
namespace {
#ifndef SIMDUTF_PPC64_H
  #error "ppc64.h must be included"
#endif
using namespace simd;

simdutf_really_inline bool is_ascii(const simd8x64<uint8_t> &input) {
  // careful: 0x80 is not ascii.
  return input.reduce_or().saturating_sub(0b01111111u).bits_not_set_anywhere();
}

simdutf_really_inline simd8<bool>
must_be_2_3_continuation(const simd8<uint8_t> prev2,
                         const simd8<uint8_t> prev3) {
  simd8<uint8_t> is_third_byte =
      prev2.saturating_sub(0xe0u - 0x80); // Only 111_____ will be >= 0x80
  simd8<uint8_t> is_fourth_byte =
      prev3.saturating_sub(0xf0u - 0x80); // Only 1111____ will be >= 0x80
  // Caller requires a bool (all 1's). All values resulting from the subtraction
  // will be <= 64, so signed comparison is fine.
  return simd8<bool>(is_third_byte | is_fourth_byte);
}

/// ErrorReporting describes behaviour of a vectorized procedure regarding error
/// checking
enum class ErrorReporting {
  precise,    // the procedure will report *approximate* or *precise* error
              // position
  at_the_end, // the procedure will only inform about an error after scanning
              // the whole input (or its significant portion)
  none,       // no error checking is done, we assume valid inputs
};

/* begin file src\ppc64\ppc64_base64.cpp */
/*
 * References and further reading:
 *
 * Wojciech Muła, Daniel Lemire, Base64 encoding and decoding at almost the
 * speed of a memory copy, Software: Practice and Experience 50 (2), 2020.
 * https://arxiv.org/abs/1910.05109
 *
 * Wojciech Muła, Daniel Lemire, Faster Base64 Encoding and Decoding using AVX2
 * Instructions, ACM Transactions on the Web 12 (3), 2018.
 * https://arxiv.org/abs/1704.00605
 *
 * Simon Josefsson. 2006. The Base16, Base32, and Base64 Data Encodings.
 * https://tools.ietf.org/html/rfc4648. (2006). Internet Engineering Task Force,
 * Request for Comments: 4648.
 *
 * Alfred Klomp. 2014a. Fast Base64 encoding/decoding with SSE vectorization.
 * http://www.alfredklomp.com/programming/sse-base64/. (2014).
 *
 * Alfred Klomp. 2014b. Fast Base64 stream encoder/decoder in C99, with SIMD
 * acceleration. https://github.com/aklomp/base64. (2014).
 *
 * Hanson Char. 2014. A Fast and Correct Base 64 Codec. (2014).
 * https://aws.amazon.com/blogs/developer/a-fast-and-correct-base-64-codec/
 *
 * Nick Kopp. 2013. Base64 Encoding on a GPU.
 * https://www.codeproject.com/Articles/276993/Base-Encoding-on-a-GPU. (2013).
 *
 * AMD XOP specific: http://0x80.pl/notesen/2016-01-12-sse-base64-encoding.html
 * Altivec has capabilites of AMD XOP (or vice versa): shuffle using 2 vectors
 * and variable shifts, thus this implementation shares some code solution
 * (modulo intrisic function names).
 */

constexpr bool with_base64_std = false;
constexpr bool with_base64_url = true;
constexpr bool with_ignore_errors = true;
constexpr bool with_ignore_garbage = true;
constexpr bool with_strict_checking = false;

// --- encoding -----------------------------------------------

/*
    Procedure translates vector of bytes having 6-bit values
    into ASCII counterparts.
*/
template <bool base64_url>
vector_u8 encoding_translate_6bit_values(const vector_u8 input) {
  // credit: Wojciech Muła
  // reduce  0..51 -> 0
  //        52..61 -> 1 .. 10
  //            62 -> 11
  //            63 -> 12
  auto result = input.saturating_sub(vector_u8::splat(51));

  // distinguish between ranges 0..25 and 26..51:
  //         0 .. 25 -> remains 13
  //        26 .. 51 -> becomes 0
  const auto lt = input < vector_u8::splat(26);
  result = select(as_vector_u8(lt), vector_u8::splat(13), result);

  const auto shift_LUT =
      base64_url ? vector_u8('a' - 26, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
                             '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
                             '0' - 52, '-' - 62, '_' - 63, 'A', 0, 0)
                 : vector_u8('a' - 26, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
                             '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
                             '0' - 52, '+' - 62, '/' - 63, 'A', 0, 0);
  // read shift
  result = result.lookup_16(shift_LUT);

  return input + result;
}

/*
   Procedure expands 12 bytes (4*3 bytes) into 16 bytes,
   each byte stores 6 bits of data
*/
template <typename = void>
simdutf_really_inline vector_u8 encoding_expand_6bit_fields(vector_u8 input) {
#if SIMDUTF_IS_BIG_ENDIAN
  #define indices4(dx) (dx + 0), (dx + 1), (dx + 1), (dx + 2)
  const auto expand_3_to_4 = vector_u8(indices4(0 * 3), indices4(1 * 3),
                                       indices4(2 * 3), indices4(3 * 3));
  #undef indices4

  // input = [........|ccdddddd|bbbbcccc|aaaaaabb] as uint8_t
  //              3        2        1        0
  //
  // in'   = [aaaaaabb|bbbbcccc|bbbbcccc|ccdddddd] as uint32_t
  //              0        1        1        2
  const auto in = as_vector_u32(expand_3_to_4.lookup_16(input));

  // t0    = [00000000|00000000|00000000|00dddddd]
  const auto t0 = in & uint32_t(0x0000003f);

  // t1    = [00000000|00000000|00cccccc|00dddddd]
  const auto t1 = select(uint32_t(0x00003f00), in.shl<2>(), t0);

  // t2    = [00000000|00bbbbbb|00cccccc|00dddddd]
  const auto t2 = select(uint32_t(0x003f0000), in.shr<4>(), t1);

  // t3    = [00aaaaaa|00bbbbbb|00cccccc|00dddddd]
  const auto t3 = select(uint32_t(0x3f000000), in.shr<2>(), t2);

  return as_vector_u8(t3);
#else
  #define indices4(dx) (dx + 1), (dx + 0), (dx + 2), (dx + 1)
  const auto expand_3_to_4 = vector_u8(indices4(0 * 3), indices4(1 * 3),
                                       indices4(2 * 3), indices4(3 * 3));
  #undef indices4

  // input = [........|ccdddddd|bbbbcccc|aaaaaabb] as uint8_t
  //              3        2        1        0
  //
  // in'   = [bbbbcccc|ccdddddd|aaaaaabb|bbbbcccc] as uint32_t
  //              1        2        0        1
  const auto in = as_vector_u32(expand_3_to_4.lookup_16(input));

  // t0    = [00dddddd|00000000|00000000|00000000]
  const auto t0 = in.shl<8>() & uint32_t(0x3f000000);

  // t1    = [00dddddd|00cccccc|00000000|00000000]
  const auto t1 = select(uint32_t(0x003f0000), in.shr<6>(), t0);

  // t2    = [00dddddd|00cccccc|00bbbbbb|00000000]
  const auto t2 = select(uint32_t(0x00003f00), in.shl<4>(), t1);

  // t3    = [00dddddd|00cccccc|00bbbbbb|00aaaaaa]
  const auto t3 = select(uint32_t(0x0000003f), in.shr<10>(), t2);

  return as_vector_u8(t3);
#endif // SIMDUTF_IS_BIG_ENDIAN
}

template <bool isbase64url>
size_t encode_base64(char *dst, const char *src, size_t srclen,
                     base64_options options) {

  const uint8_t *input = (const uint8_t *)src;

  uint8_t *out = (uint8_t *)dst;

  size_t i = 0;
  for (; i + 52 <= srclen; i += 48) {
    const auto in0 = vector_u8::load(input + i + 12 * 0);
    const auto in1 = vector_u8::load(input + i + 12 * 1);
    const auto in2 = vector_u8::load(input + i + 12 * 2);
    const auto in3 = vector_u8::load(input + i + 12 * 3);

    const auto expanded0 = encoding_expand_6bit_fields(in0);
    const auto expanded1 = encoding_expand_6bit_fields(in1);
    const auto expanded2 = encoding_expand_6bit_fields(in2);
    const auto expanded3 = encoding_expand_6bit_fields(in3);

    const auto base64_0 =
        encoding_translate_6bit_values<isbase64url>(expanded0);
    const auto base64_1 =
        encoding_translate_6bit_values<isbase64url>(expanded1);
    const auto base64_2 =
        encoding_translate_6bit_values<isbase64url>(expanded2);
    const auto base64_3 =
        encoding_translate_6bit_values<isbase64url>(expanded3);

    base64_0.store(out);
    out += 16;

    base64_1.store(out);
    out += 16;

    base64_2.store(out);
    out += 16;

    base64_3.store(out);
    out += 16;
  }
  for (; i + 16 <= srclen; i += 12) {
    const auto in = vector_u8::load(input + i);
    const auto expanded = encoding_expand_6bit_fields(in);
    const auto base64 = encoding_translate_6bit_values<isbase64url>(expanded);

    base64.store(out);
    out += 16;
  }

  return i / 3 * 4 + scalar::base64::tail_encode_base64((char *)out, src + i,
                                                        srclen - i, options);
}

// --- decoding -----------------------------------------------

static simdutf_really_inline void compress(const vector_u8 data, uint16_t mask,
                                           char *output) {
  if (mask == 0) {
    data.store(output);
    return;
  }

  // this particular implementation was inspired by work done by @animetosho
  // we do it in two steps, first 8 bytes and then second 8 bytes
  uint8_t mask1 = uint8_t(mask);      // least significant 8 bits
  uint8_t mask2 = uint8_t(mask >> 8); // most significant 8 bits
  // next line just loads the 64-bit values thintable_epi8[mask1] and
  // thintable_epi8[mask2] into a 128-bit register, using only
  // two instructions on most compilers.

#if SIMDUTF_IS_BIG_ENDIAN
  vec_u64_t tmp = {
      tables::base64::thintable_epi8[mask2],
      tables::base64::thintable_epi8[mask1],
  };

  auto shufmask = vector_u8(vec_reve(vec_u8_t(tmp)));

  // we increment by 0x08 the second half of the mask
  shufmask =
      shufmask + vector_u8(0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8);
#else
  vec_u64_t tmp = {
      tables::base64::thintable_epi8[mask1],
      tables::base64::thintable_epi8[mask2],
  };

  auto shufmask = vector_u8(vec_u8_t(tmp));

  // we increment by 0x08 the second half of the mask
  shufmask =
      shufmask + vector_u8(0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8);
#endif // SIMDUTF_IS_BIG_ENDIAN

  // this is the version "nearly pruned"
  const auto pruned = shufmask.lookup_16(data);
  // we still need to put the two halves together.
  // we compute the popcount of the first half:
  const int pop1 = tables::base64::BitsSetTable256mul2[mask1];
  // then load the corresponding mask, what it does is to write
  // only the first pop1 bytes from the first 8 bytes, and then
  // it fills in with the bytes from the second 8 bytes + some filling
  // at the end.
  const auto compactmask =
      vector_u8::load(tables::base64::pshufb_combine_table + pop1 * 8);

  const auto answer = compactmask.lookup_16(pruned);

  answer.store(output);
}

static simdutf_really_inline vector_u8 decoding_pack(vector_u8 input) {
#if SIMDUTF_IS_BIG_ENDIAN
  // in   = [00aaaaaa|00bbbbbb|00cccccc|00dddddd]
  // want = [00000000|aaaaaabb|bbbbcccc|ccdddddd]

  auto in = as_vector_u16(input);
  // t0   = [00??aaaa|aabbbbbb|00??cccc|ccdddddd]
  const auto t0 = in.shr<2>();
  const auto t1 = select(uint16_t(0x0fc0), t0, in);

  // t0   = [00??????|aaaaaabb|bbbbcccc|ccdddddd]
  const auto t2 = as_vector_u32(t1);
  const auto t3 = t2.shr<4>();
  const auto t4 = select(uint32_t(0x00fff000), t3, t2);

  const auto tmp = as_vector_u8(t4);

  const auto shuffle =
      vector_u8(1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15, 0, 0, 0, 0);

  const auto t = shuffle.lookup_16(tmp);

  return t;
#else
  // in   = [00dddddd|00cccccc|00bbbbbb|00aaaaaa]
  // want = [00000000|aaaaaabb|bbbbcccc|ccdddddd]

  auto u = as_vector_u32(input).swap_bytes();

  auto in = vector_u16((vec_u16_t)u.value);
  // t0   = [00??aaaa|aabbbbbb|00??cccc|ccdddddd]
  const auto t0 = in.shr<2>();
  const auto t1 = select(uint16_t(0x0fc0), t0, in);

  // t0   = [00??????|aaaaaabb|bbbbcccc|ccdddddd]
  const auto t2 = as_vector_u32(t1);
  const auto t3 = t2.shr<4>();
  const auto t4 = select(uint32_t(0x00fff000), t3, t2);

  const auto tmp = as_vector_u8(t4);

  const auto shuffle =
      vector_u8(2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, 0, 0, 0, 0);

  const auto t = shuffle.lookup_16(tmp);

  return t;
#endif // SIMDUTF_IS_BIG_ENDIAN
}
static simdutf_really_inline void base64_decode(char *out, vector_u8 input) {
  const auto expanded = decoding_pack(input);
  expanded.store(out);
}

static simdutf_really_inline void base64_decode_block(char *out,
                                                      const char *src) {
  base64_decode(out + 12 * 0, vector_u8::load(src + 0 * 16));
  base64_decode(out + 12 * 1, vector_u8::load(src + 1 * 16));
  base64_decode(out + 12 * 2, vector_u8::load(src + 2 * 16));
  base64_decode(out + 12 * 3, vector_u8::load(src + 3 * 16));
}

static simdutf_really_inline void base64_decode_block_safe(char *out,
                                                           const char *src) {
  base64_decode(out + 12 * 0, vector_u8::load(src + 0 * 16));
  base64_decode(out + 12 * 1, vector_u8::load(src + 1 * 16));
  base64_decode(out + 12 * 2, vector_u8::load(src + 2 * 16));

  char buffer[16];
  base64_decode(buffer, vector_u8::load(src + 3 * 16));
  std::memcpy(out + 36, buffer, 12);
}

// ---base64 decoding::block64 class --------------------------

class block64 {
  simd8x64<uint8_t> b;

public:
  simdutf_really_inline block64(const char *src) : b(load_block(src)) {}
  simdutf_really_inline block64(const char16_t *src) : b(load_block(src)) {}

private:
  // The caller of this function is responsible to ensure that there are 64
  // bytes available from reading at src. The data is read into a block64
  // structure.
  static simdutf_really_inline simd8x64<uint8_t> load_block(const char *src) {
    const auto v0 = vector_u8::load(src + 16 * 0);
    const auto v1 = vector_u8::load(src + 16 * 1);
    const auto v2 = vector_u8::load(src + 16 * 2);
    const auto v3 = vector_u8::load(src + 16 * 3);

    return simd8x64<uint8_t>(v0, v1, v2, v3);
  }

  // The caller of this function is responsible to ensure that there are 128
  // bytes available from reading at src. The data is read into a block64
  // structure.
  static simdutf_really_inline simd8x64<uint8_t>
  load_block(const char16_t *src) {
    const auto m1 = vector_u16::load(src + 8 * 0);
    const auto m2 = vector_u16::load(src + 8 * 1);
    const auto m3 = vector_u16::load(src + 8 * 2);
    const auto m4 = vector_u16::load(src + 8 * 3);
    const auto m5 = vector_u16::load(src + 8 * 4);
    const auto m6 = vector_u16::load(src + 8 * 5);
    const auto m7 = vector_u16::load(src + 8 * 6);
    const auto m8 = vector_u16::load(src + 8 * 7);

    return simd8x64<uint8_t>(vector_u16::pack(m1, m2), vector_u16::pack(m3, m4),
                             vector_u16::pack(m5, m6),
                             vector_u16::pack(m7, m8));
  }

public:
  template <bool base64_url, bool ignore_garbage, bool default_or_url>
  static inline uint16_t to_base64_mask(vector_u8 &src, uint16_t &error) {
    const auto ascii_space_tbl =
        vector_u8(0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x9, 0xa, 0x0,
                  0xc, 0xd, 0x0, 0x0);

    // credit: aqrit
    const auto delta_asso =
        default_or_url
            ? vector_u8(0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x16)
            : vector_u8(0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x0F);

    const auto delta_values =
        default_or_url
            ? vector_u8(0xBF, 0xE0, 0xB9, 0x13, 0x04, 0xBF, 0xBF, 0xB9, 0xB9,
                        0x00, 0xFF, 0x11, 0xFF, 0xBF, 0x10, 0xB9)
            : (base64_url
                   ? vector_u8(0x0, 0x0, 0x0, 0x13, 0x4, 0xBF, 0xBF, 0xB9, 0xB9,
                               0x0, 0x11, 0xC3, 0xBF, 0xE0, 0xB9, 0xB9)
                   : vector_u8(0x00, 0x00, 0x00, 0x13, 0x04, 0xBF, 0xBF, 0xB9,
                               0xB9, 0x00, 0x10, 0xC3, 0xBF, 0xBF, 0xB9, 0xB9));

    const auto check_asso =
        default_or_url
            ? vector_u8(0x0D, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                        0x01, 0x03, 0x07, 0x0B, 0x0E, 0x0B, 0x06)
            : (base64_url
                   ? vector_u8(0xD, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
                               0x3, 0x7, 0xB, 0xE, 0xB, 0x6)
                   : vector_u8(0x0D, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                               0x01, 0x01, 0x03, 0x07, 0x0B, 0x0B, 0x0B, 0x0F));

    const auto check_values =
        default_or_url
            ? vector_u8(0x80, 0x80, 0x80, 0x80, 0xCF, 0xBF, 0xD5, 0xA6, 0xB5,
                        0xA1, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80)
            : (base64_url
                   ? vector_u8(0x80, 0x80, 0x80, 0x80, 0xCF, 0xBF, 0xB6, 0xA6,
                               0xB5, 0xA1, 0x0, 0x80, 0x0, 0x80, 0x0, 0x80)
                   : vector_u8(0x80, 0x80, 0x80, 0x80, 0xCF, 0xBF, 0xD5, 0xA6,
                               0xB5, 0x86, 0xD1, 0x80, 0xB1, 0x80, 0x91, 0x80));

    const auto shifted = src.shr<3>();

    const auto delta_hash = avg(src.lookup_16(delta_asso), shifted);
    const auto check_hash = avg(src.lookup_16(check_asso), shifted);

    const auto out = as_vector_i8(delta_hash.lookup_16(delta_values))
                         .saturating_add(as_vector_i8(src));
    const auto chk = as_vector_i8(check_hash.lookup_16(check_values))
                         .saturating_add(as_vector_i8(src));

    const uint16_t mask = chk.to_bitmask();
    if (!ignore_garbage && mask) {
      const auto ascii = src.lookup_16(ascii_space_tbl);
      const auto ascii_space = (ascii == src);
      error = (mask ^ ascii_space.to_bitmask());
    }
    src = out;

    return mask;
  }

  template <bool base64_url, bool ignore_garbage, bool default_or_url>
  simdutf_really_inline uint64_t to_base64_mask(uint64_t *error) {
    uint16_t err0 = 0;
    uint16_t err1 = 0;
    uint16_t err2 = 0;
    uint16_t err3 = 0;
    uint64_t m0 = to_base64_mask<base64_url, ignore_garbage, default_or_url>(
        b.chunks[0], err0);
    uint64_t m1 = to_base64_mask<base64_url, ignore_garbage, default_or_url>(
        b.chunks[1], err1);
    uint64_t m2 = to_base64_mask<base64_url, ignore_garbage, default_or_url>(
        b.chunks[2], err2);
    uint64_t m3 = to_base64_mask<base64_url, ignore_garbage, default_or_url>(
        b.chunks[3], err3);

    if (!ignore_garbage) {
      *error = (err0) | ((uint64_t)err1 << 16) | ((uint64_t)err2 << 32) |
               ((uint64_t)err3 << 48);
    }
    return m0 | (m1 << 16) | (m2 << 32) | (m3 << 48);
  }

  simdutf_really_inline void copy_block(char *output) {
    b.store(reinterpret_cast<uint8_t *>(output));
  }

  simdutf_really_inline uint64_t compress_block(uint64_t mask, char *output) {
    uint64_t nmask = ~mask;
    compress(b.chunks[0], uint16_t(mask), output);
    compress(b.chunks[1], uint16_t(mask >> 16),
             output + count_ones(nmask & 0xFFFF));
    compress(b.chunks[2], uint16_t(mask >> 32),
             output + count_ones(nmask & 0xFFFFFFFF));
    compress(b.chunks[3], uint16_t(mask >> 48),
             output + count_ones(nmask & 0xFFFFFFFFFFFFULL));
    return count_ones(nmask);
  }

  simdutf_really_inline void base64_decode_block(char *out) {
    base64_decode(out + 12 * 0, b.chunks[0]);
    base64_decode(out + 12 * 1, b.chunks[1]);
    base64_decode(out + 12 * 2, b.chunks[2]);
    base64_decode(out + 12 * 3, b.chunks[3]);
  }

  simdutf_really_inline void base64_decode_block_safe(char *out) {
    base64_decode(out + 12 * 0, b.chunks[0]);
    base64_decode(out + 12 * 1, b.chunks[1]);
    base64_decode(out + 12 * 2, b.chunks[2]);
    char buffer[16];
    base64_decode(buffer, b.chunks[3]);
    std::memcpy(out + 12 * 3, buffer, 12);
  }
};
/* end file src\ppc64\ppc64_base64.cpp */

} // unnamed namespace
} // namespace ppc64
} // namespace simdutf

/* begin file src\generic\base64.h */
/**
 * References and further reading:
 *
 * Wojciech Muła, Daniel Lemire, Base64 encoding and decoding at almost the
 * speed of a memory copy, Software: Practice and Experience 50 (2), 2020.
 * https://arxiv.org/abs/1910.05109
 *
 * Wojciech Muła, Daniel Lemire, Faster Base64 Encoding and Decoding using AVX2
 * Instructions, ACM Transactions on the Web 12 (3), 2018.
 * https://arxiv.org/abs/1704.00605
 *
 * Simon Josefsson. 2006. The Base16, Base32, and Base64 Data Encodings.
 * https://tools.ietf.org/html/rfc4648. (2006). Internet Engineering Task Force,
 * Request for Comments: 4648.
 *
 * Alfred Klomp. 2014a. Fast Base64 encoding/decoding with SSE vectorization.
 * http://www.alfredklomp.com/programming/sse-base64/. (2014).
 *
 * Alfred Klomp. 2014b. Fast Base64 stream encoder/decoder in C99, with SIMD
 * acceleration. https://github.com/aklomp/base64. (2014).
 *
 * Hanson Char. 2014. A Fast and Correct Base 64 Codec. (2014).
 * https://aws.amazon.com/blogs/developer/a-fast-and-correct-base-64-codec/
 *
 * Nick Kopp. 2013. Base64 Encoding on a GPU.
 * https://www.codeproject.com/Articles/276993/Base-Encoding-on-a-GPU. (2013).
 */
namespace simdutf {
namespace ppc64 {
namespace {
namespace base64 {

/*
    The following template function implements API for Base64 decoding.

    An implementation is responsible for providing the `block64` type and
    associated methods that perform actual conversion. Please refer
    to any vectorized implementation to learn the API of these procedures.
*/
template <bool base64_url, bool ignore_garbage, bool default_or_url,
          typename chartype>
full_result
compress_decode_base64(char *dst, const chartype *src, size_t srclen,
                       base64_options options,
                       last_chunk_handling_options last_chunk_options) {
  const uint8_t *to_base64 =
      default_or_url ? tables::base64::to_base64_default_or_url_value
                     : (base64_url ? tables::base64::to_base64_url_value
                                   : tables::base64::to_base64_value);
  auto ri = simdutf::scalar::base64::find_end(src, srclen, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  srclen = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  (void)full_input_length;
  if (srclen == 0) {
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation, 0};
    }
    return {SUCCESS, 0, 0};
  }
  char *end_of_safe_64byte_zone =
      dst == nullptr
          ? nullptr
          : ((srclen + 3) / 4 * 3 >= 63 ? dst + (srclen + 3) / 4 * 3 - 63
                                        : dst);

  const chartype *const srcinit = src;
  const char *const dstinit = dst;
  const chartype *const srcend = src + srclen;

  constexpr size_t block_size = 6;
  static_assert(block_size >= 2, "block_size must be at least two");
  char buffer[block_size * 64];
  char *bufferptr = buffer;
  if (srclen >= 64) {
    const chartype *const srcend64 = src + srclen - 64;
    while (src <= srcend64) {
      block64 b(src);
      src += 64;
      uint64_t error = 0;
      const uint64_t badcharmask =
          b.to_base64_mask<base64_url, ignore_garbage, default_or_url>(&error);
      if (!ignore_garbage && error) {
        src -= 64;
        const size_t error_offset = trailing_zeroes(error);
        return {error_code::INVALID_BASE64_CHARACTER,
                size_t(src - srcinit + error_offset), size_t(dst - dstinit)};
      }
      if (badcharmask != 0) {
        bufferptr += b.compress_block(badcharmask, bufferptr);
      } else if (bufferptr != buffer) {
        b.copy_block(bufferptr);
        bufferptr += 64;
      } else {
        if (dst >= end_of_safe_64byte_zone) {
          b.base64_decode_block_safe(dst);
        } else {
          b.base64_decode_block(dst);
        }
        dst += 48;
      }
      if (bufferptr >= (block_size - 1) * 64 + buffer) {
        for (size_t i = 0; i < (block_size - 2); i++) {
          base64_decode_block(dst, buffer + i * 64);
          dst += 48;
        }
        if (dst >= end_of_safe_64byte_zone) {
          base64_decode_block_safe(dst, buffer + (block_size - 2) * 64);
        } else {
          base64_decode_block(dst, buffer + (block_size - 2) * 64);
        }
        dst += 48;
        std::memcpy(buffer, buffer + (block_size - 1) * 64,
                    64); // 64 might be too much
        bufferptr -= (block_size - 1) * 64;
      }
    }
  }

  char *buffer_start = buffer;
  // Optimization note: if this is almost full, then it is worth our
  // time, otherwise, we should just decode directly.
  int last_block = (int)((bufferptr - buffer_start) % 64);
  if (last_block != 0 && srcend - src + last_block >= 64) {

    while ((bufferptr - buffer_start) % 64 != 0 && src < srcend) {
      uint8_t val = to_base64[uint8_t(*src)];
      *bufferptr = char(val);
      if (!ignore_garbage &&
          (!scalar::base64::is_eight_byte(*src) || val > 64)) {
        return {error_code::INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit)};
      }
      bufferptr += (val <= 63);
      src++;
    }
  }

  for (; buffer_start + 64 <= bufferptr; buffer_start += 64) {
    if (dst >= end_of_safe_64byte_zone) {
      base64_decode_block_safe(dst, buffer_start);
    } else {
      base64_decode_block(dst, buffer_start);
    }
    dst += 48;
  }
  if ((bufferptr - buffer_start) % 64 != 0) {
    while (buffer_start + 4 < bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
#if !SIMDUTF_IS_BIG_ENDIAN
      triple = scalar::u32_swap_bytes(triple);
#endif
      std::memcpy(dst, &triple, 3);

      dst += 3;
      buffer_start += 4;
    }
    if (buffer_start + 4 <= bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
#if !SIMDUTF_IS_BIG_ENDIAN
      triple = scalar::u32_swap_bytes(triple);
#endif
      std::memcpy(dst, &triple, 3);

      dst += 3;
      buffer_start += 4;
    }
    // we may have 1, 2 or 3 bytes left and we need to decode them so let us
    // backtrack
    int leftover = int(bufferptr - buffer_start);
    while (leftover > 0) {
      if (!ignore_garbage) {
        while (to_base64[uint8_t(*(src - 1))] == 64) {
          src--;
        }
      } else {
        while (to_base64[uint8_t(*(src - 1))] >= 64) {
          src--;
        }
      }
      src--;
      leftover--;
    }
  }
  if (src < srcend + equalsigns) {
    full_result r = scalar::base64::base64_tail_decode(
        dst, src, srcend - src, equalsigns, options, last_chunk_options);
    r = scalar::base64::patch_tail_result(
        r, size_t(src - srcinit), size_t(dst - dstinit), equallocation,
        full_input_length, last_chunk_options);
    return r;
  }
  if (!ignore_garbage && equalsigns > 0) {
    if ((size_t(dst - dstinit) % 3 == 0) ||
        ((size_t(dst - dstinit) % 3) + 1 + equalsigns != 4)) {
      return {INVALID_BASE64_CHARACTER, equallocation, size_t(dst - dstinit)};
    }
  }
  return {SUCCESS, srclen, size_t(dst - dstinit)};
}

} // namespace base64
} // unnamed namespace
} // namespace ppc64
} // namespace simdutf
/* end file src\generic\base64.h */
/* begin file src\generic\find.h */
namespace simdutf {
namespace ppc64 {
namespace {
namespace util {

simdutf_really_inline const char *find(const char *start, const char *end,
                                       char character) noexcept {
  for (; std::distance(start, end) >= 64; start += 64) {
    simd8x64<uint8_t> input(reinterpret_cast<const uint8_t *>(start));
    uint64_t matches = input.eq(uint8_t(character));
    if (matches != 0) {
      // Found a match, return the first one
      int index = trailing_zeroes(matches);
      return start + index;
    }
  }
  return std::find(start, end, character);
}

simdutf_really_inline const char16_t *
find(const char16_t *start, const char16_t *end, char16_t character) noexcept {
  for (; std::distance(start, end) >= 32; start += 32) {
    simd16x32<uint16_t> input(reinterpret_cast<const uint16_t *>(start));
    uint64_t matches = input.eq(uint16_t(character));
    if (matches != 0) {
      // Found a match, return the first one
      int index = trailing_zeroes(matches) / 2;
      return start + index;
    }
  }
  return std::find(start, end, character);
}
} // namespace util
} // namespace
} // namespace ppc64
} // namespace simdutf
/* end file src\generic\find.h */

/* begin file src\ppc64\templates.cpp */
/*
    Template `convert_impl` implements generic conversion routine between
    different encodings. Procedure returns the number of written elements,
    or zero in the case of error.

    Parameters:
    * VectorizedConvert - vectorized procedure that returns structure having
      three fields: error_code (err), const Source* (input), Destination*
   (output)
    * ScalarConvert - scalar procedure that carries on conversion of tail
    * Source - type of input char (like char16_t, char)
    * Destination - type of input char
*/
template <typename VectorizedConvert, typename ScalarConvert, typename Source,
          typename Destination>
size_t convert_impl(VectorizedConvert vectorized_convert,
                    ScalarConvert scalar_convert, const Source *buf, size_t len,
                    Destination *output) {
  const auto vr = vectorized_convert(buf, len, output);
  const size_t consumed = vr.input - buf;
  const size_t written = vr.output - output;
  if (vr.err != simdutf::error_code::SUCCESS) {
    if (vr.err == simdutf::error_code::OTHER) {
      // Vectorized procedure detected an error, but does not know
      // exact position. The scalar procedure rescan the portion of
      // input and figure out where the error is located.
      return scalar_convert(vr.input, len - consumed, vr.output);
    }
    return 0;
  }

  if (consumed == len) {
    return written;
  }

  const auto ret = scalar_convert(vr.input, len - consumed, vr.output);
  if (ret == 0) {
    return 0;
  }

  return written + ret;
}

/*
    Template `convert_with_errors_impl` implements generic conversion routine
    between different encodings. Procedure returns a `result` instance ---
    please refer to its documentation for details.

    Parameters:
    * VectorizedConvert - vectorized procedure that returns structure having
      three fields: error_code (err), const Source* (input), Destination*
   (output)
    * ScalarConvert - scalar procedure that carries on conversion of tail
    * Source - type of input char (like char16_t, char)
    * Destination - type of input char
*/
template <typename VectorizedConvert, typename ScalarConvert, typename Source,
          typename Destination>
simdutf::result convert_with_errors_impl(VectorizedConvert vectorized_convert,
                                         ScalarConvert scalar_convert,
                                         const Source *buf, size_t len,
                                         Destination *output) {

  const auto vr = vectorized_convert(buf, len, output);
  const size_t consumed = vr.input - buf;
  const size_t written = vr.output - output;
  if (vr.err != simdutf::error_code::SUCCESS) {
    if (vr.err == simdutf::error_code::OTHER) {
      // Vectorized procedure detected an error, but does not know
      // exact position. The scalar procedure rescan the portion of
      // input and figure out where the error is located.
      auto sr = scalar_convert(vr.input, len - consumed, vr.output);
      sr.count += consumed;
      return sr;
    }
    return simdutf::result(vr.err, consumed);
  }

  if (consumed == len) {
    return simdutf::result(simdutf::error_code::SUCCESS, written);
  }

  simdutf::result sr = scalar_convert(vr.input, len - consumed, vr.output);
  if (sr.is_ok()) {
    sr.count += written;
  } else {
    sr.count += consumed;
  }

  return sr;
}
/* end file src\ppc64\templates.cpp */

#ifdef SIMDUTF_INTERNAL_TESTS
  #if SIMDUTF_FEATURE_BASE64
    #include "ppc64_base64_internal_tests.cpp"
  #endif // SIMDUTF_FEATURE_BASE64
#endif   // SIMDUTF_INTERNAL_TESTS
//
// Implementation-specific overrides
//
namespace simdutf {
namespace ppc64 {

simdutf_warn_unused size_t implementation::maximal_binary_length_from_base64(
    const char *input, size_t length) const noexcept {
  return scalar::base64::maximal_binary_length_from_base64(input, length);
}

simdutf_warn_unused result implementation::base64_to_binary(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused result implementation::base64_to_binary(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

size_t implementation::binary_to_base64(const char *input, size_t length,
                                        char *output,
                                        base64_options options) const noexcept {
  if (options & base64_url) {
    return encode_base64<true>(output, input, length, options);
  } else {
    return encode_base64<false>(output, input, length, options);
  }
}
const char *implementation::find(const char *start, const char *end,
                                 char character) const noexcept {
  return util::find(start, end, character);
}

const char16_t *implementation::find(const char16_t *start, const char16_t *end,
                                     char16_t character) const noexcept {
  return util::find(start, end, character);
}

#ifdef SIMDUTF_INTERNAL_TESTS
std::vector<implementation::TestProcedure>
implementation::internal_tests() const {
  #define entry(proc)                                                          \
    TestProcedure { #proc, proc }
  return {entry(base64_encoding_translate_6bit_values),
          entry(base64_encoding_expand_6bit_fields),
          entry(base64_decoding_valid),
          entry(base64_decoding_invalid_ignore_errors),
          entry(base64url_decoding_invalid_ignore_errors),
          entry(base64_decoding_invalid_strict_errors),
          entry(base64url_decoding_invalid_strict_errors),
          entry(base64_decoding_pack),
          entry(base64_compress)};
  #undef entry
}
#endif

} // namespace ppc64
} // namespace simdutf

/* begin file src\simdutf\ppc64\end.h */
/* end file src\simdutf\ppc64\end.h */
/* end file src\ppc64\implementation.cpp */
#endif
#if SIMDUTF_IMPLEMENTATION_RVV
/* begin file src\rvv\implementation.cpp */
/* begin file src\simdutf\rvv\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "rvv"
// #define SIMDUTF_IMPLEMENTATION rvv

#if SIMDUTF_CAN_ALWAYS_RUN_RVV
// nothing needed.
#else
SIMDUTF_TARGET_RVV
#endif
/* end file src\simdutf\rvv\begin.h */
namespace simdutf {
namespace rvv {
namespace {
#ifndef SIMDUTF_RVV_H
  #error "rvv.h must be included"
#endif

} // unnamed namespace
} // namespace rvv
} // namespace simdutf

//
// Implementation-specific overrides
//
namespace simdutf {
namespace rvv {
/* begin file src\rvv\rvv_helpers.inl.cpp */
template <simdutf_ByteFlip bflip>
simdutf_really_inline static size_t
rvv_utf32_store_utf16_m4(uint16_t *dst, vuint32m4_t utf32, size_t vl,
                         vbool4_t m4even) {
  /* convert [000000000000aaaa|aaaaaabbbbbbbbbb]
   * to      [110111bbbbbbbbbb|110110aaaaaaaaaa] */
  vuint32m4_t sur = __riscv_vsub_vx_u32m4(utf32, 0x10000, vl);
  sur = __riscv_vor_vv_u32m4(__riscv_vsll_vx_u32m4(sur, 16, vl),
                             __riscv_vsrl_vx_u32m4(sur, 10, vl), vl);
  sur = __riscv_vand_vx_u32m4(sur, 0x3FF03FF, vl);
  sur = __riscv_vor_vx_u32m4(sur, 0xDC00D800, vl);
  /* merge 1 byte utf32 and 2 byte sur */
  vbool8_t m4 = __riscv_vmsgtu_vx_u32m4_b8(utf32, 0xFFFF, vl);
  vuint16m4_t utf32_16 = __riscv_vreinterpret_v_u32m4_u16m4(
      __riscv_vmerge_vvm_u32m4(utf32, sur, m4, vl));
  /* compress and store */
  vbool4_t mOut = __riscv_vmor_mm_b4(
      __riscv_vmsne_vx_u16m4_b4(utf32_16, 0, vl * 2), m4even, vl * 2);
  vuint16m4_t vout = __riscv_vcompress_vm_u16m4(utf32_16, mOut, vl * 2);
  vl = __riscv_vcpop_m_b4(mOut, vl * 2);
  __riscv_vse16_v_u16m4(dst, simdutf_byteflip<bflip>(vout, vl), vl);
  return vl;
};
/* end file src\rvv\rvv_helpers.inl.cpp */

/* begin file src\rvv\rvv_length_from.inl.cpp */
/* end file src\rvv\rvv_length_from.inl.cpp */
/* begin file src\rvv\rvv_validate.inl.cpp */
/* end file src\rvv\rvv_validate.inl.cpp */

/* begin file src\rvv\rvv_latin1_to.inl.cpp */
/* end file src\rvv\rvv_latin1_to.inl.cpp */
/* begin file src\rvv\rvv_utf16_to.inl.cpp */
/* end file src\rvv\rvv_utf16_to.inl.cpp */

/* begin file src\rvv\rvv_utf32_to.inl.cpp */
/* end file src\rvv\rvv_utf32_to.inl.cpp */
/* begin file src\rvv\rvv_utf8_to.inl.cpp */
/* end file src\rvv\rvv_utf8_to.inl.cpp */

/* begin file src\rvv\rvv_find.cpp */
simdutf_really_inline const char *util_find(const char *start, const char *end,
                                            char character) noexcept {
  const char *src = start;
  for (size_t len = end - start, vl; len > 0; len -= vl, src += vl) {
    vl = __riscv_vsetvl_e8m8(len);
    vuint8m8_t v = __riscv_vle8_v_u8m8((uint8_t *)src, vl);
    long idx =
        __riscv_vfirst_m_b1(__riscv_vmseq_vx_u8m8_b1(v, character, vl), vl);
    if (idx >= 0)
      return src + idx;
  }
  return end;
}

simdutf_really_inline const char16_t *util_find(const char16_t *start,
                                                const char16_t *end,
                                                char16_t character) noexcept {
  const char16_t *src = start;
  for (size_t len = end - start, vl; len > 0; len -= vl, src += vl) {
    vl = __riscv_vsetvl_e16m8(len);
    vuint16m8_t v = __riscv_vle16_v_u16m8((uint16_t *)src, vl);
    long idx =
        __riscv_vfirst_m_b2(__riscv_vmseq_vx_u16m8_b2(v, character, vl), vl);
    if (idx >= 0)
      return src + idx;
  }
  return end;
}
/* end file src\rvv\rvv_find.cpp */

simdutf_warn_unused result implementation::base64_to_binary(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  return simdutf::scalar::base64::base64_to_binary_details_impl(
      input, length, output, options, last_chunk_options);
}

simdutf_warn_unused result implementation::base64_to_binary(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  return simdutf::scalar::base64::base64_to_binary_details_impl(
      input, length, output, options, last_chunk_options);
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  return simdutf::scalar::base64::base64_to_binary_details_impl(
      input, length, output, options, last_chunk_options);
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  return simdutf::scalar::base64::base64_to_binary_details_impl(
      input, length, output, options, last_chunk_options);
}

size_t implementation::binary_to_base64(const char *input, size_t length,
                                        char *output,
                                        base64_options options) const noexcept {
  return scalar::base64::tail_encode_base64(output, input, length, options);
}

const char *implementation::find(const char *start, const char *end,
                                 char character) const noexcept {
  return util_find(start, end, character);
}

const char16_t *implementation::find(const char16_t *start, const char16_t *end,
                                     char16_t character) const noexcept {
  return util_find(start, end, character);
}

} // namespace rvv
} // namespace simdutf

/* begin file src\simdutf\rvv\end.h */
#if SIMDUTF_CAN_ALWAYS_RUN_RVV
// nothing needed.
#else
SIMDUTF_UNTARGET_REGION
#endif

/* end file src\simdutf\rvv\end.h */
/* end file src\rvv\implementation.cpp */
#endif
#if SIMDUTF_IMPLEMENTATION_WESTMERE
/* begin file src\westmere\implementation.cpp */
/* begin file src\simdutf\westmere\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "westmere"
// #define SIMDUTF_IMPLEMENTATION westmere
#define SIMDUTF_SIMD_HAS_BYTEMASK 1

#if SIMDUTF_CAN_ALWAYS_RUN_WESTMERE
// nothing needed.
#else
SIMDUTF_TARGET_WESTMERE
#endif
/* end file src\simdutf\westmere\begin.h */

namespace simdutf {
namespace westmere {
namespace {
#ifndef SIMDUTF_WESTMERE_H
  #error "westmere.h must be included"
#endif
using namespace simd;

/* begin file src\westmere\sse_base64.cpp */
/**
 * References and further reading:
 *
 * Wojciech Muła, Daniel Lemire, Base64 encoding and decoding at almost the
 * speed of a memory copy, Software: Practice and Experience 50 (2), 2020.
 * https://arxiv.org/abs/1910.05109
 *
 * Wojciech Muła, Daniel Lemire, Faster Base64 Encoding and Decoding using AVX2
 * Instructions, ACM Transactions on the Web 12 (3), 2018.
 * https://arxiv.org/abs/1704.00605
 *
 * Simon Josefsson. 2006. The Base16, Base32, and Base64 Data Encodings.
 * https://tools.ietf.org/html/rfc4648. (2006). Internet Engineering Task Force,
 * Request for Comments: 4648.
 *
 * Alfred Klomp. 2014a. Fast Base64 encoding/decoding with SSE vectorization.
 * http://www.alfredklomp.com/programming/sse-base64/. (2014).
 *
 * Alfred Klomp. 2014b. Fast Base64 stream encoder/decoder in C99, with SIMD
 * acceleration. https://github.com/aklomp/base64. (2014).
 *
 * Hanson Char. 2014. A Fast and Correct Base 64 Codec. (2014).
 * https://aws.amazon.com/blogs/developer/a-fast-and-correct-base-64-codec/
 *
 * Nick Kopp. 2013. Base64 Encoding on a GPU.
 * https://www.codeproject.com/Articles/276993/Base-Encoding-on-a-GPU. (2013).
 */

// --- encoding ----------------------------------------------------

template <bool base64_url> __m128i lookup_pshufb_improved(const __m128i input) {
  // credit: Wojciech Muła
  // reduce  0..51 -> 0
  //        52..61 -> 1 .. 10
  //            62 -> 11
  //            63 -> 12
  __m128i result = _mm_subs_epu8(input, _mm_set1_epi8(51));

  // distinguish between ranges 0..25 and 26..51:
  //         0 .. 25 -> remains 0
  //        26 .. 51 -> becomes 13
  const __m128i less = _mm_cmpgt_epi8(_mm_set1_epi8(26), input);
  result = _mm_or_si128(result, _mm_and_si128(less, _mm_set1_epi8(13)));

  __m128i shift_LUT;
  if (base64_url) {
    shift_LUT = _mm_setr_epi8('a' - 26, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
                              '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
                              '0' - 52, '-' - 62, '_' - 63, 'A', 0, 0);
  } else {
    shift_LUT = _mm_setr_epi8('a' - 26, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
                              '0' - 52, '0' - 52, '0' - 52, '0' - 52, '0' - 52,
                              '0' - 52, '+' - 62, '/' - 63, 'A', 0, 0);
  }

  // read shift
  result = _mm_shuffle_epi8(shift_LUT, result);

  return _mm_add_epi8(result, input);
}

template <bool isbase64url>
size_t encode_base64(char *dst, const char *src, size_t srclen,
                     base64_options options) {
  // credit: Wojciech Muła
  // SSE (lookup: pshufb improved unrolled)
  const uint8_t *input = (const uint8_t *)src;

  uint8_t *out = (uint8_t *)dst;
  const __m128i shuf =
      _mm_set_epi8(10, 11, 9, 10, 7, 8, 6, 7, 4, 5, 3, 4, 1, 2, 0, 1);

  size_t i = 0;
  for (; i + 52 <= srclen; i += 48) {
    __m128i in0 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 0));
    __m128i in1 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 1));
    __m128i in2 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 2));
    __m128i in3 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(input + i + 4 * 3 * 3));

    in0 = _mm_shuffle_epi8(in0, shuf);
    in1 = _mm_shuffle_epi8(in1, shuf);
    in2 = _mm_shuffle_epi8(in2, shuf);
    in3 = _mm_shuffle_epi8(in3, shuf);

    const __m128i t0_0 = _mm_and_si128(in0, _mm_set1_epi32(0x0fc0fc00));
    const __m128i t0_1 = _mm_and_si128(in1, _mm_set1_epi32(0x0fc0fc00));
    const __m128i t0_2 = _mm_and_si128(in2, _mm_set1_epi32(0x0fc0fc00));
    const __m128i t0_3 = _mm_and_si128(in3, _mm_set1_epi32(0x0fc0fc00));

    const __m128i t1_0 = _mm_mulhi_epu16(t0_0, _mm_set1_epi32(0x04000040));
    const __m128i t1_1 = _mm_mulhi_epu16(t0_1, _mm_set1_epi32(0x04000040));
    const __m128i t1_2 = _mm_mulhi_epu16(t0_2, _mm_set1_epi32(0x04000040));
    const __m128i t1_3 = _mm_mulhi_epu16(t0_3, _mm_set1_epi32(0x04000040));

    const __m128i t2_0 = _mm_and_si128(in0, _mm_set1_epi32(0x003f03f0));
    const __m128i t2_1 = _mm_and_si128(in1, _mm_set1_epi32(0x003f03f0));
    const __m128i t2_2 = _mm_and_si128(in2, _mm_set1_epi32(0x003f03f0));
    const __m128i t2_3 = _mm_and_si128(in3, _mm_set1_epi32(0x003f03f0));

    const __m128i t3_0 = _mm_mullo_epi16(t2_0, _mm_set1_epi32(0x01000010));
    const __m128i t3_1 = _mm_mullo_epi16(t2_1, _mm_set1_epi32(0x01000010));
    const __m128i t3_2 = _mm_mullo_epi16(t2_2, _mm_set1_epi32(0x01000010));
    const __m128i t3_3 = _mm_mullo_epi16(t2_3, _mm_set1_epi32(0x01000010));

    const __m128i input0 = _mm_or_si128(t1_0, t3_0);
    const __m128i input1 = _mm_or_si128(t1_1, t3_1);
    const __m128i input2 = _mm_or_si128(t1_2, t3_2);
    const __m128i input3 = _mm_or_si128(t1_3, t3_3);

    _mm_storeu_si128(reinterpret_cast<__m128i *>(out),
                     lookup_pshufb_improved<isbase64url>(input0));
    out += 16;

    _mm_storeu_si128(reinterpret_cast<__m128i *>(out),
                     lookup_pshufb_improved<isbase64url>(input1));
    out += 16;

    _mm_storeu_si128(reinterpret_cast<__m128i *>(out),
                     lookup_pshufb_improved<isbase64url>(input2));
    out += 16;

    _mm_storeu_si128(reinterpret_cast<__m128i *>(out),
                     lookup_pshufb_improved<isbase64url>(input3));
    out += 16;
  }
  for (; i + 16 <= srclen; i += 12) {

    __m128i in = _mm_loadu_si128(reinterpret_cast<const __m128i *>(input + i));

    // bytes from groups A, B and C are needed in separate 32-bit lanes
    // in = [DDDD|CCCC|BBBB|AAAA]
    //
    //      an input triplet has layout
    //      [????????|ccdddddd|bbbbcccc|aaaaaabb]
    //        byte 3   byte 2   byte 1   byte 0    -- byte 3 comes from the next
    //        triplet
    //
    //      shuffling changes the order of bytes: 1, 0, 2, 1
    //      [bbbbcccc|ccdddddd|aaaaaabb|bbbbcccc]
    //           ^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^
    //                  processed bits
    in = _mm_shuffle_epi8(in, shuf);

    // unpacking

    // t0    = [0000cccc|cc000000|aaaaaa00|00000000]
    const __m128i t0 = _mm_and_si128(in, _mm_set1_epi32(0x0fc0fc00));
    // t1    = [00000000|00cccccc|00000000|00aaaaaa]
    //          (c * (1 << 10), a * (1 << 6)) >> 16 (note: an unsigned
    //          multiplication)
    const __m128i t1 = _mm_mulhi_epu16(t0, _mm_set1_epi32(0x04000040));

    // t2    = [00000000|00dddddd|000000bb|bbbb0000]
    const __m128i t2 = _mm_and_si128(in, _mm_set1_epi32(0x003f03f0));
    // t3    = [00dddddd|00000000|00bbbbbb|00000000](
    //          (d * (1 << 8), b * (1 << 4))
    const __m128i t3 = _mm_mullo_epi16(t2, _mm_set1_epi32(0x01000010));

    // res   = [00dddddd|00cccccc|00bbbbbb|00aaaaaa] = t1 | t3
    const __m128i indices = _mm_or_si128(t1, t3);

    _mm_storeu_si128(reinterpret_cast<__m128i *>(out),
                     lookup_pshufb_improved<isbase64url>(indices));
    out += 16;
  }

  return i / 3 * 4 + scalar::base64::tail_encode_base64((char *)out, src + i,
                                                        srclen - i, options);
}

// --- decoding -----------------------------------------------

static simdutf_really_inline void compress(__m128i data, uint16_t mask,
                                           char *output) {
  if (mask == 0) {
    _mm_storeu_si128(reinterpret_cast<__m128i *>(output), data);
    return;
  }

  // this particular implementation was inspired by work done by @animetosho
  // we do it in two steps, first 8 bytes and then second 8 bytes
  uint8_t mask1 = uint8_t(mask);      // least significant 8 bits
  uint8_t mask2 = uint8_t(mask >> 8); // most significant 8 bits
  // next line just loads the 64-bit values thintable_epi8[mask1] and
  // thintable_epi8[mask2] into a 128-bit register, using only
  // two instructions on most compilers.

  __m128i shufmask = _mm_set_epi64x(tables::base64::thintable_epi8[mask2],
                                    tables::base64::thintable_epi8[mask1]);
  // we increment by 0x08 the second half of the mask
  shufmask =
      _mm_add_epi8(shufmask, _mm_set_epi32(0x08080808, 0x08080808, 0, 0));
  // this is the version "nearly pruned"
  __m128i pruned = _mm_shuffle_epi8(data, shufmask);
  // we still need to put the two halves together.
  // we compute the popcount of the first half:
  int pop1 = tables::base64::BitsSetTable256mul2[mask1];
  // then load the corresponding mask, what it does is to write
  // only the first pop1 bytes from the first 8 bytes, and then
  // it fills in with the bytes from the second 8 bytes + some filling
  // at the end.
  __m128i compactmask = _mm_loadu_si128(reinterpret_cast<const __m128i *>(
      tables::base64::pshufb_combine_table + pop1 * 8));
  __m128i answer = _mm_shuffle_epi8(pruned, compactmask);
  _mm_storeu_si128(reinterpret_cast<__m128i *>(output), answer);
}

static simdutf_really_inline void base64_decode(char *out, __m128i str) {
  // credit: aqrit

  const __m128i pack_shuffle =
      _mm_setr_epi8(2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1);

  const __m128i t0 = _mm_maddubs_epi16(str, _mm_set1_epi32(0x01400140));
  const __m128i t1 = _mm_madd_epi16(t0, _mm_set1_epi32(0x00011000));
  const __m128i t2 = _mm_shuffle_epi8(t1, pack_shuffle);
  // Store the output:
  // this writes 16 bytes, but we only need 12.
  _mm_storeu_si128((__m128i *)out, t2);
}

// decode 64 bytes and output 48 bytes
static inline void base64_decode_block(char *out, const char *src) {
  base64_decode(out, _mm_loadu_si128(reinterpret_cast<const __m128i *>(src)));
  base64_decode(out + 12,
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 16)));
  base64_decode(out + 24,
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 32)));
  base64_decode(out + 36,
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 48)));
}

static inline void base64_decode_block_safe(char *out, const char *src) {
  base64_decode(out, _mm_loadu_si128(reinterpret_cast<const __m128i *>(src)));
  base64_decode(out + 12,
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 16)));
  base64_decode(out + 24,
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 32)));
  char buffer[16];
  base64_decode(buffer,
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 48)));
  std::memcpy(out + 36, buffer, 12);
}

// --- decoding - base64 class --------------------------------

class block64 {
  __m128i chunks[4];

public:
  // The caller of this function is responsible to ensure that there are 64
  // bytes available from reading at src.
  simdutf_really_inline block64(const char *src) {
    chunks[0] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(src));
    chunks[1] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 16));
    chunks[2] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 32));
    chunks[3] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 48));
  }

public:
  // The caller of this function is responsible to ensure that there are 128
  // bytes available from reading at src. The data is read into a block64
  // structure.
  simdutf_really_inline block64(const char16_t *src) {
    const auto m1 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(src));
    const auto m2 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 8));
    const auto m3 =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 16));
    const auto m4 =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 24));
    const auto m5 =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 32));
    const auto m6 =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 40));
    const auto m7 =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 48));
    const auto m8 =
        _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + 56));
    chunks[0] = _mm_packus_epi16(m1, m2);
    chunks[1] = _mm_packus_epi16(m3, m4);
    chunks[2] = _mm_packus_epi16(m5, m6);
    chunks[3] = _mm_packus_epi16(m7, m8);
  }

public:
  simdutf_really_inline void copy_block(char *output) {
    _mm_storeu_si128(reinterpret_cast<__m128i *>(output), chunks[0]);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(output + 16), chunks[1]);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(output + 32), chunks[2]);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(output + 48), chunks[3]);
  }

public:
  simdutf_really_inline uint64_t compress_block(uint64_t mask, char *output) {
    if (is_power_of_two(mask)) {
      return compress_block_single(mask, output);
    }

    uint64_t nmask = ~mask;
    compress(chunks[0], uint16_t(mask), output);
    compress(chunks[1], uint16_t(mask >> 16),
             output + count_ones(nmask & 0xFFFF));
    compress(chunks[2], uint16_t(mask >> 32),
             output + count_ones(nmask & 0xFFFFFFFF));
    compress(chunks[3], uint16_t(mask >> 48),
             output + count_ones(nmask & 0xFFFFFFFFFFFFULL));
    return count_ones(nmask);
  }

private:
  simdutf_really_inline size_t compress_block_single(uint64_t mask,
                                                     char *output) {
    const size_t pos64 = trailing_zeroes(mask);
    const int8_t pos = pos64 & 0xf;
    switch (pos64 >> 4) {
    case 0b00: {
      const __m128i v0 = _mm_set1_epi8(char(pos - 1));
      const __m128i v1 =
          _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
      const __m128i v2 = _mm_cmpgt_epi8(v1, v0);
      const __m128i sh = _mm_sub_epi8(v1, v2);
      const __m128i compressed = _mm_shuffle_epi8(chunks[0], sh);

      _mm_storeu_si128((__m128i *)(output + 0 * 16), compressed);
      _mm_storeu_si128((__m128i *)(output + 1 * 16 - 1), chunks[1]);
      _mm_storeu_si128((__m128i *)(output + 2 * 16 - 1), chunks[2]);
      _mm_storeu_si128((__m128i *)(output + 3 * 16 - 1), chunks[3]);
    } break;
    case 0b01: {
      _mm_storeu_si128((__m128i *)(output + 0 * 16), chunks[0]);

      const __m128i v0 = _mm_set1_epi8(char(pos - 1));
      const __m128i v1 =
          _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
      const __m128i v2 = _mm_cmpgt_epi8(v1, v0);
      const __m128i sh = _mm_sub_epi8(v1, v2);
      const __m128i compressed = _mm_shuffle_epi8(chunks[1], sh);

      _mm_storeu_si128((__m128i *)(output + 1 * 16), compressed);
      _mm_storeu_si128((__m128i *)(output + 2 * 16 - 1), chunks[2]);
      _mm_storeu_si128((__m128i *)(output + 3 * 16 - 1), chunks[3]);
    } break;
    case 0b10: {
      _mm_storeu_si128((__m128i *)(output + 0 * 16), chunks[0]);
      _mm_storeu_si128((__m128i *)(output + 1 * 16), chunks[1]);

      const __m128i v0 = _mm_set1_epi8(char(pos - 1));
      const __m128i v1 =
          _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
      const __m128i v2 = _mm_cmpgt_epi8(v1, v0);
      const __m128i sh = _mm_sub_epi8(v1, v2);
      const __m128i compressed = _mm_shuffle_epi8(chunks[2], sh);

      _mm_storeu_si128((__m128i *)(output + 2 * 16), compressed);
      _mm_storeu_si128((__m128i *)(output + 3 * 16 - 1), chunks[3]);
    } break;
    case 0b11: {
      _mm_storeu_si128((__m128i *)(output + 0 * 16), chunks[0]);
      _mm_storeu_si128((__m128i *)(output + 1 * 16), chunks[1]);
      _mm_storeu_si128((__m128i *)(output + 2 * 16), chunks[2]);

      const __m128i v0 = _mm_set1_epi8(char(pos - 1));
      const __m128i v1 =
          _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
      const __m128i v2 = _mm_cmpgt_epi8(v1, v0);
      const __m128i sh = _mm_sub_epi8(v1, v2);
      const __m128i compressed = _mm_shuffle_epi8(chunks[3], sh);

      _mm_storeu_si128((__m128i *)(output + 3 * 16), compressed);
    } break;
    }

    return 63;
  }

public:
  template <bool base64_url, bool ignore_garbage, bool default_or_url>
  simdutf_really_inline uint64_t to_base64_mask(uint64_t *error) {
    uint32_t err0 = 0;
    uint32_t err1 = 0;
    uint32_t err2 = 0;
    uint32_t err3 = 0;
    uint64_t m0 = to_base64_mask<base64_url, ignore_garbage, default_or_url>(
        &chunks[0], &err0);
    uint64_t m1 = to_base64_mask<base64_url, ignore_garbage, default_or_url>(
        &chunks[1], &err1);
    uint64_t m2 = to_base64_mask<base64_url, ignore_garbage, default_or_url>(
        &chunks[2], &err2);
    uint64_t m3 = to_base64_mask<base64_url, ignore_garbage, default_or_url>(
        &chunks[3], &err3);
    if (!ignore_garbage) {
      *error = (err0) | ((uint64_t)err1 << 16) | ((uint64_t)err2 << 32) |
               ((uint64_t)err3 << 48);
    }
    return m0 | (m1 << 16) | (m2 << 32) | (m3 << 48);
  }

private:
  template <bool base64_url, bool ignore_garbage, bool default_or_url>
  simdutf_really_inline uint16_t to_base64_mask(__m128i *src, uint32_t *error) {
    const __m128i ascii_space_tbl =
        _mm_setr_epi8(0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x9, 0xa,
                      0x0, 0xc, 0xd, 0x0, 0x0);
    // credit: aqrit
    __m128i delta_asso;
    if (default_or_url) {
      delta_asso =
          _mm_setr_epi8(0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x16);
    } else if (base64_url) {
      delta_asso = _mm_setr_epi8(0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0,
                                 0x0, 0x0, 0x0, 0x0, 0xF, 0x0, 0xF);
    } else {
      delta_asso =
          _mm_setr_epi8(0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x0F);
    }
    __m128i delta_values;
    if (default_or_url) {
      delta_values = _mm_setr_epi8(
          uint8_t(0xBF), uint8_t(0xE0), uint8_t(0xB9), uint8_t(0x13),
          uint8_t(0x04), uint8_t(0xBF), uint8_t(0xBF), uint8_t(0xB9),
          uint8_t(0xB9), uint8_t(0x00), uint8_t(0xFF), uint8_t(0x11),
          uint8_t(0xFF), uint8_t(0xBF), uint8_t(0x10), uint8_t(0xB9));

    } else if (base64_url) {
      delta_values = _mm_setr_epi8(0x0, 0x0, 0x0, 0x13, 0x4, uint8_t(0xBF),
                                   uint8_t(0xBF), uint8_t(0xB9), uint8_t(0xB9),
                                   0x0, 0x11, uint8_t(0xC3), uint8_t(0xBF),
                                   uint8_t(0xE0), uint8_t(0xB9), uint8_t(0xB9));
    } else {
      delta_values =
          _mm_setr_epi8(int8_t(0x00), int8_t(0x00), int8_t(0x00), int8_t(0x13),
                        int8_t(0x04), int8_t(0xBF), int8_t(0xBF), int8_t(0xB9),
                        int8_t(0xB9), int8_t(0x00), int8_t(0x10), int8_t(0xC3),
                        int8_t(0xBF), int8_t(0xBF), int8_t(0xB9), int8_t(0xB9));
    }
    __m128i check_asso;
    if (default_or_url) {
      check_asso =
          _mm_setr_epi8(0x0D, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                        0x01, 0x03, 0x07, 0x0B, 0x0E, 0x0B, 0x06);
    } else if (base64_url) {
      check_asso = _mm_setr_epi8(0xD, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
                                 0x1, 0x3, 0x7, 0xB, 0xE, 0xB, 0x6);
    } else {
      check_asso =
          _mm_setr_epi8(0x0D, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                        0x01, 0x03, 0x07, 0x0B, 0x0B, 0x0B, 0x0F);
    }
    __m128i check_values;
    if (default_or_url) {
      check_values = _mm_setr_epi8(
          uint8_t(0x80), uint8_t(0x80), uint8_t(0x80), uint8_t(0x80),
          uint8_t(0xCF), uint8_t(0xBF), uint8_t(0xD5), uint8_t(0xA6),
          uint8_t(0xB5), uint8_t(0xA1), uint8_t(0x00), uint8_t(0x80),
          uint8_t(0x00), uint8_t(0x80), uint8_t(0x00), uint8_t(0x80));
    } else if (base64_url) {
      check_values = _mm_setr_epi8(uint8_t(0x80), uint8_t(0x80), uint8_t(0x80),
                                   uint8_t(0x80), uint8_t(0xCF), uint8_t(0xBF),
                                   uint8_t(0xB6), uint8_t(0xA6), uint8_t(0xB5),
                                   uint8_t(0xA1), 0x0, uint8_t(0x80), 0x0,
                                   uint8_t(0x80), 0x0, uint8_t(0x80));
    } else {
      check_values =
          _mm_setr_epi8(int8_t(0x80), int8_t(0x80), int8_t(0x80), int8_t(0x80),
                        int8_t(0xCF), int8_t(0xBF), int8_t(0xD5), int8_t(0xA6),
                        int8_t(0xB5), int8_t(0x86), int8_t(0xD1), int8_t(0x80),
                        int8_t(0xB1), int8_t(0x80), int8_t(0x91), int8_t(0x80));
    }
    const __m128i shifted = _mm_srli_epi32(*src, 3);

    __m128i delta_hash =
        _mm_avg_epu8(_mm_shuffle_epi8(delta_asso, *src), shifted);
    if (default_or_url) {
      delta_hash = _mm_and_si128(delta_hash, _mm_set1_epi8(0xf));
    }
    const __m128i check_hash =
        _mm_avg_epu8(_mm_shuffle_epi8(check_asso, *src), shifted);

    const __m128i out =
        _mm_adds_epi8(_mm_shuffle_epi8(delta_values, delta_hash), *src);
    const __m128i chk =
        _mm_adds_epi8(_mm_shuffle_epi8(check_values, check_hash), *src);
    const int mask = _mm_movemask_epi8(chk);
    if (!ignore_garbage && mask) {
      __m128i ascii_space =
          _mm_cmpeq_epi8(_mm_shuffle_epi8(ascii_space_tbl, *src), *src);
      *error = (mask ^ _mm_movemask_epi8(ascii_space));
    }
    *src = out;
    return (uint16_t)mask;
  }

public:
  simdutf_really_inline void base64_decode_block(char *out) {
    base64_decode(out, chunks[0]);
    base64_decode(out + 12, chunks[1]);
    base64_decode(out + 24, chunks[2]);
    base64_decode(out + 36, chunks[3]);
  }

public:
  simdutf_really_inline void base64_decode_block_safe(char *out) {
    base64_decode(out, chunks[0]);
    base64_decode(out + 12, chunks[1]);
    base64_decode(out + 24, chunks[2]);
    char buffer[16];
    base64_decode(buffer, chunks[3]);
    std::memcpy(out + 36, buffer, 12);
  }
};
/* end file src\westmere\sse_base64.cpp */

} // unnamed namespace
} // namespace westmere
} // namespace simdutf

/* begin file src\generic\buf_block_reader.h */
namespace simdutf {
namespace westmere {
namespace {

// Walks through a buffer in block-sized increments, loading the last part with
// spaces
template <size_t STEP_SIZE> struct buf_block_reader {
public:
  simdutf_really_inline buf_block_reader(const uint8_t *_buf, size_t _len);
  simdutf_really_inline size_t block_index();
  simdutf_really_inline bool has_full_block() const;
  simdutf_really_inline const uint8_t *full_block() const;
  /**
   * Get the last block, padded with spaces.
   *
   * There will always be a last block, with at least 1 byte, unless len == 0
   * (in which case this function fills the buffer with spaces and returns 0. In
   * particular, if len == STEP_SIZE there will be 0 full_blocks and 1 remainder
   * block with STEP_SIZE bytes and no spaces for padding.
   *
   * @return the number of effective characters in the last block.
   */
  simdutf_really_inline size_t get_remainder(uint8_t *dst) const;
  simdutf_really_inline void advance();

private:
  const uint8_t *buf;
  const size_t len;
  const size_t lenminusstep;
  size_t idx;
};

// Routines to print masks and text for debugging bitmask operations
simdutf_unused static char *format_input_text_64(const uint8_t *text) {
  static char *buf =
      reinterpret_cast<char *>(malloc(sizeof(simd8x64<uint8_t>) + 1));
  for (size_t i = 0; i < sizeof(simd8x64<uint8_t>); i++) {
    buf[i] = int8_t(text[i]) < ' ' ? '_' : int8_t(text[i]);
  }
  buf[sizeof(simd8x64<uint8_t>)] = '\0';
  return buf;
}

// Routines to print masks and text for debugging bitmask operations
simdutf_unused static char *format_input_text(const simd8x64<uint8_t> &in) {
  static char *buf =
      reinterpret_cast<char *>(malloc(sizeof(simd8x64<uint8_t>) + 1));
  in.store(reinterpret_cast<uint8_t *>(buf));
  for (size_t i = 0; i < sizeof(simd8x64<uint8_t>); i++) {
    if (buf[i] < ' ') {
      buf[i] = '_';
    }
  }
  buf[sizeof(simd8x64<uint8_t>)] = '\0';
  return buf;
}

simdutf_unused static char *format_mask(uint64_t mask) {
  static char *buf = reinterpret_cast<char *>(malloc(64 + 1));
  for (size_t i = 0; i < 64; i++) {
    buf[i] = (mask & (size_t(1) << i)) ? 'X' : ' ';
  }
  buf[64] = '\0';
  return buf;
}

template <size_t STEP_SIZE>
simdutf_really_inline
buf_block_reader<STEP_SIZE>::buf_block_reader(const uint8_t *_buf, size_t _len)
    : buf{_buf}, len{_len}, lenminusstep{len < STEP_SIZE ? 0 : len - STEP_SIZE},
      idx{0} {}

template <size_t STEP_SIZE>
simdutf_really_inline size_t buf_block_reader<STEP_SIZE>::block_index() {
  return idx;
}

template <size_t STEP_SIZE>
simdutf_really_inline bool buf_block_reader<STEP_SIZE>::has_full_block() const {
  return idx < lenminusstep;
}

template <size_t STEP_SIZE>
simdutf_really_inline const uint8_t *
buf_block_reader<STEP_SIZE>::full_block() const {
  return &buf[idx];
}

template <size_t STEP_SIZE>
simdutf_really_inline size_t
buf_block_reader<STEP_SIZE>::get_remainder(uint8_t *dst) const {
  if (len == idx) {
    return 0;
  } // memcpy(dst, null, 0) will trigger an error with some sanitizers
  std::memset(dst, 0x20,
              STEP_SIZE); // std::memset STEP_SIZE because it is more efficient
                          // to write out 8 or 16 bytes at once.
  std::memcpy(dst, buf + idx, len - idx);
  return len - idx;
}

template <size_t STEP_SIZE>
simdutf_really_inline void buf_block_reader<STEP_SIZE>::advance() {
  idx += STEP_SIZE;
}

} // unnamed namespace
} // namespace westmere
} // namespace simdutf
/* end file src\generic\buf_block_reader.h */

/* begin file src\generic\base64.h */
/**
 * References and further reading:
 *
 * Wojciech Muła, Daniel Lemire, Base64 encoding and decoding at almost the
 * speed of a memory copy, Software: Practice and Experience 50 (2), 2020.
 * https://arxiv.org/abs/1910.05109
 *
 * Wojciech Muła, Daniel Lemire, Faster Base64 Encoding and Decoding using AVX2
 * Instructions, ACM Transactions on the Web 12 (3), 2018.
 * https://arxiv.org/abs/1704.00605
 *
 * Simon Josefsson. 2006. The Base16, Base32, and Base64 Data Encodings.
 * https://tools.ietf.org/html/rfc4648. (2006). Internet Engineering Task Force,
 * Request for Comments: 4648.
 *
 * Alfred Klomp. 2014a. Fast Base64 encoding/decoding with SSE vectorization.
 * http://www.alfredklomp.com/programming/sse-base64/. (2014).
 *
 * Alfred Klomp. 2014b. Fast Base64 stream encoder/decoder in C99, with SIMD
 * acceleration. https://github.com/aklomp/base64. (2014).
 *
 * Hanson Char. 2014. A Fast and Correct Base 64 Codec. (2014).
 * https://aws.amazon.com/blogs/developer/a-fast-and-correct-base-64-codec/
 *
 * Nick Kopp. 2013. Base64 Encoding on a GPU.
 * https://www.codeproject.com/Articles/276993/Base-Encoding-on-a-GPU. (2013).
 */
namespace simdutf {
namespace westmere {
namespace {
namespace base64 {

/*
    The following template function implements API for Base64 decoding.

    An implementation is responsible for providing the `block64` type and
    associated methods that perform actual conversion. Please refer
    to any vectorized implementation to learn the API of these procedures.
*/
template <bool base64_url, bool ignore_garbage, bool default_or_url,
          typename chartype>
full_result
compress_decode_base64(char *dst, const chartype *src, size_t srclen,
                       base64_options options,
                       last_chunk_handling_options last_chunk_options) {
  const uint8_t *to_base64 =
      default_or_url ? tables::base64::to_base64_default_or_url_value
                     : (base64_url ? tables::base64::to_base64_url_value
                                   : tables::base64::to_base64_value);
  auto ri = simdutf::scalar::base64::find_end(src, srclen, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  srclen = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  (void)full_input_length;
  if (srclen == 0) {
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation, 0};
    }
    return {SUCCESS, 0, 0};
  }
  char *end_of_safe_64byte_zone =
      dst == nullptr
          ? nullptr
          : ((srclen + 3) / 4 * 3 >= 63 ? dst + (srclen + 3) / 4 * 3 - 63
                                        : dst);

  const chartype *const srcinit = src;
  const char *const dstinit = dst;
  const chartype *const srcend = src + srclen;

  constexpr size_t block_size = 6;
  static_assert(block_size >= 2, "block_size must be at least two");
  char buffer[block_size * 64];
  char *bufferptr = buffer;
  if (srclen >= 64) {
    const chartype *const srcend64 = src + srclen - 64;
    while (src <= srcend64) {
      block64 b(src);
      src += 64;
      uint64_t error = 0;
      const uint64_t badcharmask =
          b.to_base64_mask<base64_url, ignore_garbage, default_or_url>(&error);
      if (!ignore_garbage && error) {
        src -= 64;
        const size_t error_offset = trailing_zeroes(error);
        return {error_code::INVALID_BASE64_CHARACTER,
                size_t(src - srcinit + error_offset), size_t(dst - dstinit)};
      }
      if (badcharmask != 0) {
        bufferptr += b.compress_block(badcharmask, bufferptr);
      } else if (bufferptr != buffer) {
        b.copy_block(bufferptr);
        bufferptr += 64;
      } else {
        if (dst >= end_of_safe_64byte_zone) {
          b.base64_decode_block_safe(dst);
        } else {
          b.base64_decode_block(dst);
        }
        dst += 48;
      }
      if (bufferptr >= (block_size - 1) * 64 + buffer) {
        for (size_t i = 0; i < (block_size - 2); i++) {
          base64_decode_block(dst, buffer + i * 64);
          dst += 48;
        }
        if (dst >= end_of_safe_64byte_zone) {
          base64_decode_block_safe(dst, buffer + (block_size - 2) * 64);
        } else {
          base64_decode_block(dst, buffer + (block_size - 2) * 64);
        }
        dst += 48;
        std::memcpy(buffer, buffer + (block_size - 1) * 64,
                    64); // 64 might be too much
        bufferptr -= (block_size - 1) * 64;
      }
    }
  }

  char *buffer_start = buffer;
  // Optimization note: if this is almost full, then it is worth our
  // time, otherwise, we should just decode directly.
  int last_block = (int)((bufferptr - buffer_start) % 64);
  if (last_block != 0 && srcend - src + last_block >= 64) {

    while ((bufferptr - buffer_start) % 64 != 0 && src < srcend) {
      uint8_t val = to_base64[uint8_t(*src)];
      *bufferptr = char(val);
      if (!ignore_garbage &&
          (!scalar::base64::is_eight_byte(*src) || val > 64)) {
        return {error_code::INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit)};
      }
      bufferptr += (val <= 63);
      src++;
    }
  }

  for (; buffer_start + 64 <= bufferptr; buffer_start += 64) {
    if (dst >= end_of_safe_64byte_zone) {
      base64_decode_block_safe(dst, buffer_start);
    } else {
      base64_decode_block(dst, buffer_start);
    }
    dst += 48;
  }
  if ((bufferptr - buffer_start) % 64 != 0) {
    while (buffer_start + 4 < bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
#if !SIMDUTF_IS_BIG_ENDIAN
      triple = scalar::u32_swap_bytes(triple);
#endif
      std::memcpy(dst, &triple, 3);

      dst += 3;
      buffer_start += 4;
    }
    if (buffer_start + 4 <= bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
#if !SIMDUTF_IS_BIG_ENDIAN
      triple = scalar::u32_swap_bytes(triple);
#endif
      std::memcpy(dst, &triple, 3);

      dst += 3;
      buffer_start += 4;
    }
    // we may have 1, 2 or 3 bytes left and we need to decode them so let us
    // backtrack
    int leftover = int(bufferptr - buffer_start);
    while (leftover > 0) {
      if (!ignore_garbage) {
        while (to_base64[uint8_t(*(src - 1))] == 64) {
          src--;
        }
      } else {
        while (to_base64[uint8_t(*(src - 1))] >= 64) {
          src--;
        }
      }
      src--;
      leftover--;
    }
  }
  if (src < srcend + equalsigns) {
    full_result r = scalar::base64::base64_tail_decode(
        dst, src, srcend - src, equalsigns, options, last_chunk_options);
    r = scalar::base64::patch_tail_result(
        r, size_t(src - srcinit), size_t(dst - dstinit), equallocation,
        full_input_length, last_chunk_options);
    return r;
  }
  if (!ignore_garbage && equalsigns > 0) {
    if ((size_t(dst - dstinit) % 3 == 0) ||
        ((size_t(dst - dstinit) % 3) + 1 + equalsigns != 4)) {
      return {INVALID_BASE64_CHARACTER, equallocation, size_t(dst - dstinit)};
    }
  }
  return {SUCCESS, srclen, size_t(dst - dstinit)};
}

} // namespace base64
} // unnamed namespace
} // namespace westmere
} // namespace simdutf
/* end file src\generic\base64.h */
/* begin file src\generic\find.h */
namespace simdutf {
namespace westmere {
namespace {
namespace util {

simdutf_really_inline const char *find(const char *start, const char *end,
                                       char character) noexcept {
  for (; std::distance(start, end) >= 64; start += 64) {
    simd8x64<uint8_t> input(reinterpret_cast<const uint8_t *>(start));
    uint64_t matches = input.eq(uint8_t(character));
    if (matches != 0) {
      // Found a match, return the first one
      int index = trailing_zeroes(matches);
      return start + index;
    }
  }
  return std::find(start, end, character);
}

simdutf_really_inline const char16_t *
find(const char16_t *start, const char16_t *end, char16_t character) noexcept {
  for (; std::distance(start, end) >= 32; start += 32) {
    simd16x32<uint16_t> input(reinterpret_cast<const uint16_t *>(start));
    uint64_t matches = input.eq(uint16_t(character));
    if (matches != 0) {
      // Found a match, return the first one
      int index = trailing_zeroes(matches) / 2;
      return start + index;
    }
  }
  return std::find(start, end, character);
}
} // namespace util
} // namespace
} // namespace westmere
} // namespace simdutf
/* end file src\generic\find.h */

//
// Implementation-specific overrides
//

namespace simdutf {
namespace westmere {

simdutf_warn_unused result implementation::base64_to_binary(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused result implementation::base64_to_binary(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return base64::compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return base64::compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return base64::compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return base64::compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

size_t implementation::binary_to_base64(const char *input, size_t length,
                                        char *output,
                                        base64_options options) const noexcept {
  if (options & base64_url) {
    return encode_base64<true>(output, input, length, options);
  } else {
    return encode_base64<false>(output, input, length, options);
  }
}

const char *implementation::find(const char *start, const char *end,
                                 char character) const noexcept {
  return util::find(start, end, character);
}

const char16_t *implementation::find(const char16_t *start, const char16_t *end,
                                     char16_t character) const noexcept {
  return util::find(start, end, character);
}

} // namespace westmere
} // namespace simdutf

/* begin file src\simdutf\westmere\end.h */
#if SIMDUTF_CAN_ALWAYS_RUN_WESTMERE
// nothing needed.
#else
SIMDUTF_UNTARGET_REGION
#endif

#undef SIMDUTF_SIMD_HAS_BYTEMASK
/* end file src\simdutf\westmere\end.h */
/* end file src\westmere\implementation.cpp */
#endif
#if SIMDUTF_IMPLEMENTATION_LSX
/* begin file src\lsx\implementation.cpp */
/* begin file src\simdutf\lsx\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "lsx"
// #define SIMDUTF_IMPLEMENTATION lsx
#define SIMDUTF_SIMD_HAS_UNSIGNED_CMP 1
/* end file src\simdutf\lsx\begin.h */
namespace simdutf {
namespace lsx {
namespace {
#ifndef SIMDUTF_LSX_H
  #error "lsx.h must be included"
#endif
using namespace simd;

/* begin file src\lsx\lsx_base64.cpp */
/**
 * References and further reading:
 *
 * Wojciech Muła, Daniel Lemire, Base64 encoding and decoding at almost the
 * speed of a memory copy, Software: Practice and Experience 50 (2), 2020.
 * https://arxiv.org/abs/1910.05109
 *
 * Wojciech Muła, Daniel Lemire, Faster Base64 Encoding and Decoding using AVX2
 * Instructions, ACM Transactions on the Web 12 (3), 2018.
 * https://arxiv.org/abs/1704.00605
 *
 * Simon Josefsson. 2006. The Base16, Base32, and Base64 Data Encodings.
 * https://tools.ietf.org/html/rfc4648. (2006). Internet Engineering Task Force,
 * Request for Comments: 4648.
 *
 * Alfred Klomp. 2014a. Fast Base64 encoding/decoding with SSE vectorization.
 * http://www.alfredklomp.com/programming/sse-base64/. (2014).
 *
 * Alfred Klomp. 2014b. Fast Base64 stream encoder/decoder in C99, with SIMD
 * acceleration. https://github.com/aklomp/base64. (2014).
 *
 * Hanson Char. 2014. A Fast and Correct Base 64 Codec. (2014).
 * https://aws.amazon.com/blogs/developer/a-fast-and-correct-base-64-codec/
 *
 * Nick Kopp. 2013. Base64 Encoding on a GPU.
 * https://www.codeproject.com/Articles/276993/Base-Encoding-on-a-GPU. (2013).
 */

template <bool isbase64url>
size_t encode_base64(char *dst, const char *src, size_t srclen,
                     base64_options options) {
  // credit: Wojciech Muła
  // SSE (lookup: pshufb improved unrolled)
  const uint8_t *input = (const uint8_t *)src;
  static const char *lookup_tbl =
      isbase64url
          ? "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"
          : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  uint8_t *out = (uint8_t *)dst;

  v16u8 shuf;
  __m128i v_fc0fc00, v_3f03f0, shift_r, shift_l, base64_tbl0, base64_tbl1,
      base64_tbl2, base64_tbl3;
  if (srclen >= 16) {
    shuf = v16u8{1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10};
    v_fc0fc00 = __lsx_vreplgr2vr_w(uint32_t(0x0fc0fc00));
    v_3f03f0 = __lsx_vreplgr2vr_w(uint32_t(0x003f03f0));
    shift_r = __lsx_vreplgr2vr_w(uint32_t(0x0006000a));
    shift_l = __lsx_vreplgr2vr_w(uint32_t(0x00080004));
    base64_tbl0 = __lsx_vld(lookup_tbl, 0);
    base64_tbl1 = __lsx_vld(lookup_tbl, 16);
    base64_tbl2 = __lsx_vld(lookup_tbl, 32);
    base64_tbl3 = __lsx_vld(lookup_tbl, 48);
  }

  size_t i = 0;
  for (; i + 52 <= srclen; i += 48) {
    __m128i in0 =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 0);
    __m128i in1 =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 1);
    __m128i in2 =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 2);
    __m128i in3 =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 3);

    in0 = __lsx_vshuf_b(in0, in0, (__m128i)shuf);
    in1 = __lsx_vshuf_b(in1, in1, (__m128i)shuf);
    in2 = __lsx_vshuf_b(in2, in2, (__m128i)shuf);
    in3 = __lsx_vshuf_b(in3, in3, (__m128i)shuf);

    __m128i t0_0 = __lsx_vand_v(in0, v_fc0fc00);
    __m128i t0_1 = __lsx_vand_v(in1, v_fc0fc00);
    __m128i t0_2 = __lsx_vand_v(in2, v_fc0fc00);
    __m128i t0_3 = __lsx_vand_v(in3, v_fc0fc00);

    __m128i t1_0 = __lsx_vsrl_h(t0_0, shift_r);
    __m128i t1_1 = __lsx_vsrl_h(t0_1, shift_r);
    __m128i t1_2 = __lsx_vsrl_h(t0_2, shift_r);
    __m128i t1_3 = __lsx_vsrl_h(t0_3, shift_r);

    __m128i t2_0 = __lsx_vand_v(in0, v_3f03f0);
    __m128i t2_1 = __lsx_vand_v(in1, v_3f03f0);
    __m128i t2_2 = __lsx_vand_v(in2, v_3f03f0);
    __m128i t2_3 = __lsx_vand_v(in3, v_3f03f0);

    __m128i t3_0 = __lsx_vsll_h(t2_0, shift_l);
    __m128i t3_1 = __lsx_vsll_h(t2_1, shift_l);
    __m128i t3_2 = __lsx_vsll_h(t2_2, shift_l);
    __m128i t3_3 = __lsx_vsll_h(t2_3, shift_l);

    __m128i input0 = __lsx_vor_v(t1_0, t3_0);
    __m128i input0_shuf0 = __lsx_vshuf_b(base64_tbl1, base64_tbl0, input0);
    __m128i input0_shuf1 = __lsx_vshuf_b(base64_tbl3, base64_tbl2,
                                         __lsx_vsub_b(input0, __lsx_vldi(32)));
    __m128i input0_mask = __lsx_vslei_bu(input0, 31);
    __m128i input0_result =
        __lsx_vbitsel_v(input0_shuf1, input0_shuf0, input0_mask);
    __lsx_vst(input0_result, reinterpret_cast<__m128i *>(out), 0);
    out += 16;

    __m128i input1 = __lsx_vor_v(t1_1, t3_1);
    __m128i input1_shuf0 = __lsx_vshuf_b(base64_tbl1, base64_tbl0, input1);
    __m128i input1_shuf1 = __lsx_vshuf_b(base64_tbl3, base64_tbl2,
                                         __lsx_vsub_b(input1, __lsx_vldi(32)));
    __m128i input1_mask = __lsx_vslei_bu(input1, 31);
    __m128i input1_result =
        __lsx_vbitsel_v(input1_shuf1, input1_shuf0, input1_mask);
    __lsx_vst(input1_result, reinterpret_cast<__m128i *>(out), 0);
    out += 16;

    __m128i input2 = __lsx_vor_v(t1_2, t3_2);
    __m128i input2_shuf0 = __lsx_vshuf_b(base64_tbl1, base64_tbl0, input2);
    __m128i input2_shuf1 = __lsx_vshuf_b(base64_tbl3, base64_tbl2,
                                         __lsx_vsub_b(input2, __lsx_vldi(32)));
    __m128i input2_mask = __lsx_vslei_bu(input2, 31);
    __m128i input2_result =
        __lsx_vbitsel_v(input2_shuf1, input2_shuf0, input2_mask);
    __lsx_vst(input2_result, reinterpret_cast<__m128i *>(out), 0);
    out += 16;

    __m128i input3 = __lsx_vor_v(t1_3, t3_3);
    __m128i input3_shuf0 = __lsx_vshuf_b(base64_tbl1, base64_tbl0, input3);
    __m128i input3_shuf1 = __lsx_vshuf_b(base64_tbl3, base64_tbl2,
                                         __lsx_vsub_b(input3, __lsx_vldi(32)));
    __m128i input3_mask = __lsx_vslei_bu(input3, 31);
    __m128i input3_result =
        __lsx_vbitsel_v(input3_shuf1, input3_shuf0, input3_mask);
    __lsx_vst(input3_result, reinterpret_cast<__m128i *>(out), 0);
    out += 16;
  }
  for (; i + 16 <= srclen; i += 12) {

    __m128i in = __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 0);

    // bytes from groups A, B and C are needed in separate 32-bit lanes
    // in = [DDDD|CCCC|BBBB|AAAA]
    //
    //      an input triplet has layout
    //      [????????|ccdddddd|bbbbcccc|aaaaaabb]
    //        byte 3   byte 2   byte 1   byte 0    -- byte 3 comes from the next
    //        triplet
    //
    //      shuffling changes the order of bytes: 1, 0, 2, 1
    //      [bbbbcccc|ccdddddd|aaaaaabb|bbbbcccc]
    //           ^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^
    //                  processed bits
    in = __lsx_vshuf_b(in, in, (__m128i)shuf);

    // unpacking
    // t0    = [0000cccc|cc000000|aaaaaa00|00000000]
    __m128i t0 = __lsx_vand_v(in, v_fc0fc00);
    // t1    = [00000000|00cccccc|00000000|00aaaaaa]
    //          ((c >> 6),  (a >> 10))
    __m128i t1 = __lsx_vsrl_h(t0, shift_r);

    // t2    = [00000000|00dddddd|000000bb|bbbb0000]
    __m128i t2 = __lsx_vand_v(in, v_3f03f0);
    // t3    = [00dddddd|00000000|00bbbbbb|00000000]
    //          ((d << 8), (b << 4))
    __m128i t3 = __lsx_vsll_h(t2, shift_l);

    // res   = [00dddddd|00cccccc|00bbbbbb|00aaaaaa] = t1 | t3
    __m128i indices = __lsx_vor_v(t1, t3);

    __m128i indices_shuf0 = __lsx_vshuf_b(base64_tbl1, base64_tbl0, indices);
    __m128i indices_shuf1 = __lsx_vshuf_b(
        base64_tbl3, base64_tbl2, __lsx_vsub_b(indices, __lsx_vldi(32)));
    __m128i indices_mask = __lsx_vslei_bu(indices, 31);
    __m128i indices_result =
        __lsx_vbitsel_v(indices_shuf1, indices_shuf0, indices_mask);

    __lsx_vst(indices_result, reinterpret_cast<__m128i *>(out), 0);
    out += 16;
  }

  return i / 3 * 4 + scalar::base64::tail_encode_base64((char *)out, src + i,
                                                        srclen - i, options);
}

static inline void compress(__m128i data, uint16_t mask, char *output) {
  if (mask == 0) {
    __lsx_vst(data, reinterpret_cast<__m128i *>(output), 0);
    return;
  }
  // this particular implementation was inspired by work done by @animetosho
  // we do it in two steps, first 8 bytes and then second 8 bytes
  uint8_t mask1 = uint8_t(mask);      // least significant 8 bits
  uint8_t mask2 = uint8_t(mask >> 8); // most significant 8 bits
  // next line just loads the 64-bit values thintable_epi8[mask1] and
  // thintable_epi8[mask2] into a 128-bit register, using only
  // two instructions on most compilers.

  v2u64 shufmask = {tables::base64::thintable_epi8[mask1],
                    tables::base64::thintable_epi8[mask2]};

  // we increment by 0x08 the second half of the mask
  v4u32 hi = {0, 0, 0x08080808, 0x08080808};
  __m128i shufmask1 = __lsx_vadd_b((__m128i)shufmask, (__m128i)hi);

  // this is the version "nearly pruned"
  __m128i pruned = __lsx_vshuf_b(data, data, shufmask1);
  // we still need to put the two halves together.
  // we compute the popcount of the first half:
  int pop1 = tables::base64::BitsSetTable256mul2[mask1];
  // then load the corresponding mask, what it does is to write
  // only the first pop1 bytes from the first 8 bytes, and then
  // it fills in with the bytes from the second 8 bytes + some filling
  // at the end.
  __m128i compactmask =
      __lsx_vld(reinterpret_cast<const __m128i *>(
                    tables::base64::pshufb_combine_table + pop1 * 8),
                0);
  __m128i answer = __lsx_vshuf_b(pruned, pruned, compactmask);

  __lsx_vst(answer, reinterpret_cast<__m128i *>(output), 0);
}

struct block64 {
  __m128i chunks[4];
};

template <bool base64_url, bool default_or_url>
static inline uint16_t to_base64_mask(__m128i *src, bool *error) {
  const v16u8 ascii_space_tbl = {0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                                 0x0,  0x9, 0xa, 0x0, 0xc, 0xd, 0x0, 0x0};
  // credit: aqrit
  /*
  '0'(0x30)-'9'(0x39) => delta_values_index = 4
  'A'(0x41)-'Z'(0x5a) => delta_values_index = 4/5/12(4+8)
  'a'(0x61)-'z'(0x7a) => delta_values_index = 6/7/14(6+8)
  '+'(0x2b)           => delta_values_index = 3
  '/'(0x2f)           => delta_values_index = 2+8 = 10
  '-'(0x2d)           => delta_values_index = 2+8 = 10
  '_'(0x5f)           => delta_values_index = 5+8 = 13
  */
  v16u8 delta_asso;
  if (default_or_url) {
    delta_asso = v16u8{0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x16};
  } else {
    delta_asso = v16u8{0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
                       0x0, 0x0, 0x0, 0x0, 0x0, 0xF, 0x0, 0xF};
  }
  v16i8 delta_values;
  if (default_or_url) {
    delta_values =
        v16i8{int8_t(0xBF), int8_t(0xE0), int8_t(0xB9), int8_t(0x13),
              int8_t(0x04), int8_t(0xBF), int8_t(0xBF), int8_t(0xB9),
              int8_t(0xB9), int8_t(0x00), int8_t(0xFF), int8_t(0x11),
              int8_t(0xFF), int8_t(0xBF), int8_t(0x10), int8_t(0xB9)};
  } else if (base64_url) {
    delta_values =
        v16i8{int8_t(0x00), int8_t(0x00), int8_t(0x00), int8_t(0x13),
              int8_t(0x04), int8_t(0xBF), int8_t(0xBF), int8_t(0xB9),
              int8_t(0xB9), int8_t(0x00), int8_t(0x11), int8_t(0xC3),
              int8_t(0xBF), int8_t(0xE0), int8_t(0xB9), int8_t(0xB9)};
  } else {
    delta_values =
        v16i8{int8_t(0x00), int8_t(0x00), int8_t(0x00), int8_t(0x13),
              int8_t(0x04), int8_t(0xBF), int8_t(0xBF), int8_t(0xB9),
              int8_t(0xB9), int8_t(0x00), int8_t(0x10), int8_t(0xC3),
              int8_t(0xBF), int8_t(0xBF), int8_t(0xB9), int8_t(0xB9)};
  }

  v16u8 check_asso;
  if (default_or_url) {
    check_asso = v16u8{0x0D, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                       0x01, 0x01, 0x03, 0x07, 0x0B, 0x0E, 0x0B, 0x06};
  } else if (base64_url) {
    check_asso = v16u8{0x0D, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                       0x01, 0x01, 0x03, 0x07, 0x0B, 0x06, 0x0B, 0x12};
  } else {
    check_asso = v16u8{0x0D, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                       0x01, 0x01, 0x03, 0x07, 0x0B, 0x0B, 0x0B, 0x0F};
  }

  v16i8 check_values;
  if (default_or_url) {
    check_values =
        v16i8{int8_t(0x80), int8_t(0x80), int8_t(0x80), int8_t(0x80),
              int8_t(0xCF), int8_t(0xBF), int8_t(0xD5), int8_t(0xA6),
              int8_t(0xB5), int8_t(0xA1), int8_t(0x00), int8_t(0x80),
              int8_t(0x00), int8_t(0x80), int8_t(0x00), int8_t(0x80)};
  } else if (base64_url) {
    check_values = v16i8{int8_t(0x0),  int8_t(0x80), int8_t(0x80), int8_t(0x80),
                         int8_t(0xCF), int8_t(0xBF), int8_t(0xD3), int8_t(0xA6),
                         int8_t(0xB5), int8_t(0x86), int8_t(0xD0), int8_t(0x80),
                         int8_t(0xB0), int8_t(0x80), int8_t(0x0),  int8_t(0x0)};
  } else {
    check_values =
        v16i8{int8_t(0x80), int8_t(0x80), int8_t(0x80), int8_t(0x80),
              int8_t(0xCF), int8_t(0xBF), int8_t(0xD5), int8_t(0xA6),
              int8_t(0xB5), int8_t(0x86), int8_t(0xD1), int8_t(0x80),
              int8_t(0xB1), int8_t(0x80), int8_t(0x91), int8_t(0x80)};
  }

  const __m128i shifted = __lsx_vsrli_b(*src, 3);
  __m128i asso_index = __lsx_vand_v(*src, __lsx_vldi(0xF));
  const __m128i delta_hash =
      __lsx_vavgr_bu(__lsx_vshuf_b((__m128i)delta_asso, (__m128i)delta_asso,
                                   (__m128i)asso_index),
                     shifted);
  const __m128i check_hash =
      __lsx_vavgr_bu(__lsx_vshuf_b((__m128i)check_asso, (__m128i)check_asso,
                                   (__m128i)asso_index),
                     shifted);

  const __m128i out =
      __lsx_vsadd_b(__lsx_vshuf_b((__m128i)delta_values, (__m128i)delta_values,
                                  (__m128i)delta_hash),
                    *src);
  const __m128i chk =
      __lsx_vsadd_b(__lsx_vshuf_b((__m128i)check_values, (__m128i)check_values,
                                  (__m128i)check_hash),
                    *src);
  unsigned int mask = __lsx_vpickve2gr_hu(__lsx_vmskltz_b(chk), 0);
  if (mask) {
    __m128i ascii_space = __lsx_vseq_b(__lsx_vshuf_b((__m128i)ascii_space_tbl,
                                                     (__m128i)ascii_space_tbl,
                                                     (__m128i)asso_index),
                                       *src);
    *error |=
        (mask != __lsx_vpickve2gr_hu(__lsx_vmskltz_b((__m128i)ascii_space), 0));
  }

  *src = out;
  return (uint16_t)mask;
}

template <bool base64_url, bool default_or_url>
static inline uint64_t to_base64_mask(block64 *b, bool *error) {
  *error = 0;
  uint64_t m0 =
      to_base64_mask<base64_url, default_or_url>(&b->chunks[0], error);
  uint64_t m1 =
      to_base64_mask<base64_url, default_or_url>(&b->chunks[1], error);
  uint64_t m2 =
      to_base64_mask<base64_url, default_or_url>(&b->chunks[2], error);
  uint64_t m3 =
      to_base64_mask<base64_url, default_or_url>(&b->chunks[3], error);
  return m0 | (m1 << 16) | (m2 << 32) | (m3 << 48);
}

static inline void copy_block(block64 *b, char *output) {
  __lsx_vst(b->chunks[0], reinterpret_cast<__m128i *>(output), 0);
  __lsx_vst(b->chunks[1], reinterpret_cast<__m128i *>(output), 16);
  __lsx_vst(b->chunks[2], reinterpret_cast<__m128i *>(output), 32);
  __lsx_vst(b->chunks[3], reinterpret_cast<__m128i *>(output), 48);
}

static inline uint64_t compress_block(block64 *b, uint64_t mask, char *output) {
  uint64_t nmask = ~mask;
  uint64_t count =
      __lsx_vpickve2gr_d(__lsx_vpcnt_h(__lsx_vreplgr2vr_d(nmask)), 0);
  uint16_t *count_ptr = (uint16_t *)&count;
  compress(b->chunks[0], uint16_t(mask), output);
  compress(b->chunks[1], uint16_t(mask >> 16), output + count_ptr[0]);
  compress(b->chunks[2], uint16_t(mask >> 32),
           output + count_ptr[0] + count_ptr[1]);
  compress(b->chunks[3], uint16_t(mask >> 48),
           output + count_ptr[0] + count_ptr[1] + count_ptr[2]);
  return count_ones(nmask);
}

// The caller of this function is responsible to ensure that there are 64 bytes
// available from reading at src. The data is read into a block64 structure.
static inline void load_block(block64 *b, const char *src) {
  b->chunks[0] = __lsx_vld(reinterpret_cast<const __m128i *>(src), 0);
  b->chunks[1] = __lsx_vld(reinterpret_cast<const __m128i *>(src), 16);
  b->chunks[2] = __lsx_vld(reinterpret_cast<const __m128i *>(src), 32);
  b->chunks[3] = __lsx_vld(reinterpret_cast<const __m128i *>(src), 48);
}

// The caller of this function is responsible to ensure that there are 128 bytes
// available from reading at src. The data is read into a block64 structure.
static inline void load_block(block64 *b, const char16_t *src) {
  __m128i m1 = __lsx_vld(reinterpret_cast<const __m128i *>(src), 0);
  __m128i m2 = __lsx_vld(reinterpret_cast<const __m128i *>(src), 16);
  __m128i m3 = __lsx_vld(reinterpret_cast<const __m128i *>(src), 32);
  __m128i m4 = __lsx_vld(reinterpret_cast<const __m128i *>(src), 48);
  __m128i m5 = __lsx_vld(reinterpret_cast<const __m128i *>(src), 64);
  __m128i m6 = __lsx_vld(reinterpret_cast<const __m128i *>(src), 80);
  __m128i m7 = __lsx_vld(reinterpret_cast<const __m128i *>(src), 96);
  __m128i m8 = __lsx_vld(reinterpret_cast<const __m128i *>(src), 112);
  b->chunks[0] = __lsx_vssrlni_bu_h(m2, m1, 0);
  b->chunks[1] = __lsx_vssrlni_bu_h(m4, m3, 0);
  b->chunks[2] = __lsx_vssrlni_bu_h(m6, m5, 0);
  b->chunks[3] = __lsx_vssrlni_bu_h(m8, m7, 0);
}

static inline void base64_decode(char *out, __m128i str) {
  __m128i t0 = __lsx_vor_v(
      __lsx_vslli_w(str, 26),
      __lsx_vslli_w(__lsx_vand_v(str, lsx_splat_u32(0x0000FF00)), 12));
  __m128i t1 = __lsx_vsrli_w(__lsx_vand_v(str, lsx_splat_u32(0x003F0000)), 2);
  __m128i t2 = __lsx_vor_v(t0, t1);
  __m128i t3 = __lsx_vor_v(t2, __lsx_vsrli_w(str, 16));
  const v16u8 pack_shuffle = {3, 2,  1,  7,  6, 5, 11, 10,
                              9, 15, 14, 13, 0, 0, 0,  0};
  t3 = __lsx_vshuf_b(t3, t3, (__m128i)pack_shuffle);

  // Store the output:
  // we only need 12.
  __lsx_vstelm_d(t3, out, 0, 0);
  __lsx_vstelm_w(t3, out + 8, 0, 2);
}
// decode 64 bytes and output 48 bytes
static inline void base64_decode_block(char *out, const char *src) {
  base64_decode(out, __lsx_vld(reinterpret_cast<const __m128i *>(src), 0));
  base64_decode(out + 12,
                __lsx_vld(reinterpret_cast<const __m128i *>(src), 16));
  base64_decode(out + 24,
                __lsx_vld(reinterpret_cast<const __m128i *>(src), 32));
  base64_decode(out + 36,
                __lsx_vld(reinterpret_cast<const __m128i *>(src), 48));
}
static inline void base64_decode_block_safe(char *out, const char *src) {
  base64_decode_block(out, src);
}
static inline void base64_decode_block(char *out, block64 *b) {
  base64_decode(out, b->chunks[0]);
  base64_decode(out + 12, b->chunks[1]);
  base64_decode(out + 24, b->chunks[2]);
  base64_decode(out + 36, b->chunks[3]);
}
static inline void base64_decode_block_safe(char *out, block64 *b) {
  base64_decode_block(out, b);
}

template <bool base64_url, bool ignore_garbage, bool default_or_url,
          typename char_type>
full_result
compress_decode_base64(char *dst, const char_type *src, size_t srclen,
                       base64_options options,
                       last_chunk_handling_options last_chunk_options) {
  const uint8_t *to_base64 =
      default_or_url ? tables::base64::to_base64_default_or_url_value
                     : (base64_url ? tables::base64::to_base64_url_value
                                   : tables::base64::to_base64_value);
  auto ri = simdutf::scalar::base64::find_end(src, srclen, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  srclen = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  (void)full_input_length;
  if (srclen == 0) {
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation, 0};
    }
    return {SUCCESS, 0, 0};
  }
  const char_type *const srcinit = src;
  const char *const dstinit = dst;
  const char_type *const srcend = src + srclen;

  constexpr size_t block_size = 10;
  char buffer[block_size * 64];
  char *bufferptr = buffer;
  if (srclen >= 64) {
    const char_type *const srcend64 = src + srclen - 64;
    while (src <= srcend64) {
      block64 b;
      load_block(&b, src);
      src += 64;
      bool error = false;
      uint64_t badcharmask =
          to_base64_mask<base64_url, default_or_url>(&b, &error);
      if (badcharmask) {
        if (error && !ignore_garbage) {
          src -= 64;
          while (src < srcend && scalar::base64::is_eight_byte(*src) &&
                 to_base64[uint8_t(*src)] <= 64) {
            src++;
          }
          if (src < srcend) {
            // should never happen
          }
          return {error_code::INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                  size_t(dst - dstinit)};
        }
      }

      if (badcharmask != 0) {
        // optimization opportunity: check for simple masks like those made of
        // continuous 1s followed by continuous 0s. And masks containing a
        // single bad character.
        bufferptr += compress_block(&b, badcharmask, bufferptr);
      } else {
        // optimization opportunity: if bufferptr == buffer and mask == 0, we
        // can avoid the call to compress_block and decode directly.
        copy_block(&b, bufferptr);
        bufferptr += 64;
      }
      if (bufferptr >= (block_size - 1) * 64 + buffer) {
        for (size_t i = 0; i < (block_size - 1); i++) {
          base64_decode_block(dst, buffer + i * 64);
          dst += 48;
        }
        std::memcpy(buffer, buffer + (block_size - 1) * 64,
                    64); // 64 might be too much
        bufferptr -= (block_size - 1) * 64;
      }
    }
  }
  char *buffer_start = buffer;
  // Optimization note: if this is almost full, then it is worth our
  // time, otherwise, we should just decode directly.
  int last_block = (int)((bufferptr - buffer_start) % 64);
  if (last_block != 0 && srcend - src + last_block >= 64) {
    while ((bufferptr - buffer_start) % 64 != 0 && src < srcend) {
      uint8_t val = to_base64[uint8_t(*src)];
      *bufferptr = char(val);
      if ((!scalar::base64::is_eight_byte(*src) || val > 64) &&
          !ignore_garbage) {
        return {error_code::INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit)};
      }
      bufferptr += (val <= 63);
      src++;
    }
  }

  for (; buffer_start + 64 <= bufferptr; buffer_start += 64) {
    base64_decode_block(dst, buffer_start);
    dst += 48;
  }
  if ((bufferptr - buffer_start) % 64 != 0) {
    while (buffer_start + 4 < bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
      triple = scalar::u32_swap_bytes(triple);
      std::memcpy(dst, &triple, 4);

      dst += 3;
      buffer_start += 4;
    }
    if (buffer_start + 4 <= bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
      triple = scalar::u32_swap_bytes(triple);
      std::memcpy(dst, &triple, 3);

      dst += 3;
      buffer_start += 4;
    }
    // we may have 1, 2 or 3 bytes left and we need to decode them so let us
    // backtrack
    int leftover = int(bufferptr - buffer_start);
    while (leftover > 0) {
      if (!ignore_garbage) {
        while (to_base64[uint8_t(*(src - 1))] == 64) {
          src--;
        }
      } else {
        while (to_base64[uint8_t(*(src - 1))] >= 64) {
          src--;
        }
      }
      src--;
      leftover--;
    }
  }
  if (src < srcend + equalsigns) {
    full_result r = scalar::base64::base64_tail_decode(
        dst, src, srcend - src, equalsigns, options, last_chunk_options);
    r = scalar::base64::patch_tail_result(
        r, size_t(src - srcinit), size_t(dst - dstinit), equallocation,
        full_input_length, last_chunk_options);
    return r;
  }
  if (equalsigns > 0 && !ignore_garbage) {
    if ((size_t(dst - dstinit) % 3 == 0) ||
        ((size_t(dst - dstinit) % 3) + 1 + equalsigns != 4)) {
      return {INVALID_BASE64_CHARACTER, equallocation, size_t(dst - dstinit)};
    }
  }
  return {SUCCESS, srclen, size_t(dst - dstinit)};
}
/* end file src\lsx\lsx_base64.cpp */
/* begin file src\lsx\lsx_find.cpp */
simdutf_really_inline const char *util_find(const char *start, const char *end,
                                            char character) noexcept {
  if (start >= end)
    return end;

  const int step = 16;
  __m128i char_vec = __lsx_vreplgr2vr_b(static_cast<uint8_t>(character));

  while (end - start >= step) {
    __m128i data = __lsx_vld(reinterpret_cast<const __m128i *>(start), 0);
    __m128i cmp = __lsx_vseq_b(data, char_vec);
    if (__lsx_bnz_v(cmp)) {
      uint16_t mask =
          static_cast<uint16_t>(__lsx_vpickve2gr_hu(__lsx_vmsknz_b(cmp), 0));
      return start + trailing_zeroes(mask);
    }

    start += step;
  }

  // Handle remaining bytes with scalar loop
  for (; start < end; ++start) {
    if (*start == character) {
      return start;
    }
  }

  return end;
}

simdutf_really_inline const char16_t *util_find(const char16_t *start,
                                                const char16_t *end,
                                                char16_t character) noexcept {
  if (start >= end)
    return end;

  const int step = 8;
  __m128i char_vec = __lsx_vreplgr2vr_h(static_cast<uint16_t>(character));

  while (end - start >= step) {
    __m128i data = __lsx_vld(reinterpret_cast<const __m128i *>(start), 0);
    __m128i cmp = __lsx_vseq_h(data, char_vec);
    if (__lsx_bnz_v(cmp)) {
      uint16_t mask =
          static_cast<uint16_t>(__lsx_vpickve2gr_hu(__lsx_vmsknz_b(cmp), 0));
      return start + trailing_zeroes(mask) / 2;
    }

    start += step;
  }

  // Handle remaining elements with scalar loop
  for (; start < end; ++start) {
    if (*start == character) {
      return start;
    }
  }

  return end;
}
/* end file src\lsx\lsx_find.cpp */

} // namespace
} // namespace lsx
} // namespace simdutf

/* begin file src\generic\buf_block_reader.h */
namespace simdutf {
namespace lsx {
namespace {

// Walks through a buffer in block-sized increments, loading the last part with
// spaces
template <size_t STEP_SIZE> struct buf_block_reader {
public:
  simdutf_really_inline buf_block_reader(const uint8_t *_buf, size_t _len);
  simdutf_really_inline size_t block_index();
  simdutf_really_inline bool has_full_block() const;
  simdutf_really_inline const uint8_t *full_block() const;
  /**
   * Get the last block, padded with spaces.
   *
   * There will always be a last block, with at least 1 byte, unless len == 0
   * (in which case this function fills the buffer with spaces and returns 0. In
   * particular, if len == STEP_SIZE there will be 0 full_blocks and 1 remainder
   * block with STEP_SIZE bytes and no spaces for padding.
   *
   * @return the number of effective characters in the last block.
   */
  simdutf_really_inline size_t get_remainder(uint8_t *dst) const;
  simdutf_really_inline void advance();

private:
  const uint8_t *buf;
  const size_t len;
  const size_t lenminusstep;
  size_t idx;
};

// Routines to print masks and text for debugging bitmask operations
simdutf_unused static char *format_input_text_64(const uint8_t *text) {
  static char *buf =
      reinterpret_cast<char *>(malloc(sizeof(simd8x64<uint8_t>) + 1));
  for (size_t i = 0; i < sizeof(simd8x64<uint8_t>); i++) {
    buf[i] = int8_t(text[i]) < ' ' ? '_' : int8_t(text[i]);
  }
  buf[sizeof(simd8x64<uint8_t>)] = '\0';
  return buf;
}

// Routines to print masks and text for debugging bitmask operations
simdutf_unused static char *format_input_text(const simd8x64<uint8_t> &in) {
  static char *buf =
      reinterpret_cast<char *>(malloc(sizeof(simd8x64<uint8_t>) + 1));
  in.store(reinterpret_cast<uint8_t *>(buf));
  for (size_t i = 0; i < sizeof(simd8x64<uint8_t>); i++) {
    if (buf[i] < ' ') {
      buf[i] = '_';
    }
  }
  buf[sizeof(simd8x64<uint8_t>)] = '\0';
  return buf;
}

simdutf_unused static char *format_mask(uint64_t mask) {
  static char *buf = reinterpret_cast<char *>(malloc(64 + 1));
  for (size_t i = 0; i < 64; i++) {
    buf[i] = (mask & (size_t(1) << i)) ? 'X' : ' ';
  }
  buf[64] = '\0';
  return buf;
}

template <size_t STEP_SIZE>
simdutf_really_inline
buf_block_reader<STEP_SIZE>::buf_block_reader(const uint8_t *_buf, size_t _len)
    : buf{_buf}, len{_len}, lenminusstep{len < STEP_SIZE ? 0 : len - STEP_SIZE},
      idx{0} {}

template <size_t STEP_SIZE>
simdutf_really_inline size_t buf_block_reader<STEP_SIZE>::block_index() {
  return idx;
}

template <size_t STEP_SIZE>
simdutf_really_inline bool buf_block_reader<STEP_SIZE>::has_full_block() const {
  return idx < lenminusstep;
}

template <size_t STEP_SIZE>
simdutf_really_inline const uint8_t *
buf_block_reader<STEP_SIZE>::full_block() const {
  return &buf[idx];
}

template <size_t STEP_SIZE>
simdutf_really_inline size_t
buf_block_reader<STEP_SIZE>::get_remainder(uint8_t *dst) const {
  if (len == idx) {
    return 0;
  } // memcpy(dst, null, 0) will trigger an error with some sanitizers
  std::memset(dst, 0x20,
              STEP_SIZE); // std::memset STEP_SIZE because it is more efficient
                          // to write out 8 or 16 bytes at once.
  std::memcpy(dst, buf + idx, len - idx);
  return len - idx;
}

template <size_t STEP_SIZE>
simdutf_really_inline void buf_block_reader<STEP_SIZE>::advance() {
  idx += STEP_SIZE;
}

} // unnamed namespace
} // namespace lsx
} // namespace simdutf
/* end file src\generic\buf_block_reader.h */

//
// Implementation-specific overrides
//
namespace simdutf {
namespace lsx {

simdutf_warn_unused result implementation::base64_to_binary(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused result implementation::base64_to_binary(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

size_t implementation::binary_to_base64(const char *input, size_t length,
                                        char *output,
                                        base64_options options) const noexcept {
  if (options & base64_url) {
    return encode_base64<true>(output, input, length, options);
  } else {
    return encode_base64<false>(output, input, length, options);
  }
}
const char *implementation::find(const char *start, const char *end,
                                 char character) const noexcept {
  return util_find(start, end, character);
}

const char16_t *implementation::find(const char16_t *start, const char16_t *end,
                                     char16_t character) const noexcept {
  return util_find(start, end, character);
}

} // namespace lsx
} // namespace simdutf

/* begin file src\simdutf\lsx\end.h */
#undef SIMDUTF_SIMD_HAS_UNSIGNED_CMP
/* end file src\simdutf\lsx\end.h */
/* end file src\lsx\implementation.cpp */
#endif
#if SIMDUTF_IMPLEMENTATION_LASX
/* begin file src\lasx\implementation.cpp */
/* begin file src\simdutf\lasx\begin.h */
// redefining SIMDUTF_IMPLEMENTATION to "lasx"
// #define SIMDUTF_IMPLEMENTATION lasx
#define SIMDUTF_SIMD_HAS_UNSIGNED_CMP 1
/* end file src\simdutf\lasx\begin.h */
namespace simdutf {
namespace lasx {
namespace {
#ifndef SIMDUTF_LASX_H
  #error "lasx.h must be included"
#endif
using namespace simd;

/* begin file src\lasx\lasx_base64.cpp */
/**
 * References and further reading:
 *
 * Wojciech Muła, Daniel Lemire, Base64 encoding and decoding at almost the
 * speed of a memory copy, Software: Practice and Experience 50 (2), 2020.
 * https://arxiv.org/abs/1910.05109
 *
 * Wojciech Muła, Daniel Lemire, Faster Base64 Encoding and Decoding using AVX2
 * Instructions, ACM Transactions on the Web 12 (3), 2018.
 * https://arxiv.org/abs/1704.00605
 *
 * Simon Josefsson. 2006. The Base16, Base32, and Base64 Data Encodings.
 * https://tools.ietf.org/html/rfc4648. (2006). Internet Engineering Task Force,
 * Request for Comments: 4648.
 *
 * Alfred Klomp. 2014a. Fast Base64 encoding/decoding with SSE vectorization.
 * http://www.alfredklomp.com/programming/sse-base64/. (2014).
 *
 * Alfred Klomp. 2014b. Fast Base64 stream encoder/decoder in C99, with SIMD
 * acceleration. https://github.com/aklomp/base64. (2014).
 *
 * Hanson Char. 2014. A Fast and Correct Base 64 Codec. (2014).
 * https://aws.amazon.com/blogs/developer/a-fast-and-correct-base-64-codec/
 *
 * Nick Kopp. 2013. Base64 Encoding on a GPU.
 * https://www.codeproject.com/Articles/276993/Base-Encoding-on-a-GPU. (2013).
 */

template <bool isbase64url>
size_t encode_base64(char *dst, const char *src, size_t srclen,
                     base64_options options) {
  // credit: Wojciech Muła
  // SSE (lookup: pshufb improved unrolled)
  const uint8_t *input = (const uint8_t *)src;
  static const char *lookup_tbl =
      isbase64url
          ? "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"
          : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  uint8_t *out = (uint8_t *)dst;

  v32u8 shuf;
  __m256i v_fc0fc00, v_3f03f0, shift_r, shift_l, base64_tbl0, base64_tbl1,
      base64_tbl2, base64_tbl3;
  if (srclen >= 28) {
    shuf = v32u8{1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10,
                 1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10};

    v_fc0fc00 = __lasx_xvreplgr2vr_w(uint32_t(0x0fc0fc00));
    v_3f03f0 = __lasx_xvreplgr2vr_w(uint32_t(0x003f03f0));
    shift_r = __lasx_xvreplgr2vr_w(uint32_t(0x0006000a));
    shift_l = __lasx_xvreplgr2vr_w(uint32_t(0x00080004));
    base64_tbl0 = ____m256i(__lsx_vld(lookup_tbl, 0));
    base64_tbl1 = ____m256i(__lsx_vld(lookup_tbl, 16));
    base64_tbl2 = ____m256i(__lsx_vld(lookup_tbl, 32));
    base64_tbl3 = ____m256i(__lsx_vld(lookup_tbl, 48));
  }
  size_t i = 0;
  for (; i + 100 <= srclen; i += 96) {
    __m128i in0_lo =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 0);
    __m128i in0_hi =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 1);
    __m128i in1_lo =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 2);
    __m128i in1_hi =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 3);
    __m128i in2_lo =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 4);
    __m128i in2_hi =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 5);
    __m128i in3_lo =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 6);
    __m128i in3_hi =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 7);

    __m256i in0 = lasx_set_q(in0_hi, in0_lo);
    __m256i in1 = lasx_set_q(in1_hi, in1_lo);
    __m256i in2 = lasx_set_q(in2_hi, in2_lo);
    __m256i in3 = lasx_set_q(in3_hi, in3_lo);

    in0 = __lasx_xvshuf_b(in0, in0, (__m256i)shuf);
    in1 = __lasx_xvshuf_b(in1, in1, (__m256i)shuf);
    in2 = __lasx_xvshuf_b(in2, in2, (__m256i)shuf);
    in3 = __lasx_xvshuf_b(in3, in3, (__m256i)shuf);

    __m256i t0_0 = __lasx_xvand_v(in0, v_fc0fc00);
    __m256i t0_1 = __lasx_xvand_v(in1, v_fc0fc00);
    __m256i t0_2 = __lasx_xvand_v(in2, v_fc0fc00);
    __m256i t0_3 = __lasx_xvand_v(in3, v_fc0fc00);

    __m256i t1_0 = __lasx_xvsrl_h(t0_0, shift_r);
    __m256i t1_1 = __lasx_xvsrl_h(t0_1, shift_r);
    __m256i t1_2 = __lasx_xvsrl_h(t0_2, shift_r);
    __m256i t1_3 = __lasx_xvsrl_h(t0_3, shift_r);

    __m256i t2_0 = __lasx_xvand_v(in0, v_3f03f0);
    __m256i t2_1 = __lasx_xvand_v(in1, v_3f03f0);
    __m256i t2_2 = __lasx_xvand_v(in2, v_3f03f0);
    __m256i t2_3 = __lasx_xvand_v(in3, v_3f03f0);

    __m256i t3_0 = __lasx_xvsll_h(t2_0, shift_l);
    __m256i t3_1 = __lasx_xvsll_h(t2_1, shift_l);
    __m256i t3_2 = __lasx_xvsll_h(t2_2, shift_l);
    __m256i t3_3 = __lasx_xvsll_h(t2_3, shift_l);

    __m256i input0 = __lasx_xvor_v(t1_0, t3_0);
    __m256i input0_shuf0 = __lasx_xvshuf_b(base64_tbl1, base64_tbl0, input0);
    __m256i input0_shuf1 = __lasx_xvshuf_b(
        base64_tbl3, base64_tbl2, __lasx_xvsub_b(input0, __lasx_xvldi(32)));
    __m256i input0_mask = __lasx_xvslei_bu(input0, 31);
    __m256i input0_result =
        __lasx_xvbitsel_v(input0_shuf1, input0_shuf0, input0_mask);
    __lasx_xvst(input0_result, reinterpret_cast<__m256i *>(out), 0);
    out += 32;

    __m256i input1 = __lasx_xvor_v(t1_1, t3_1);
    __m256i input1_shuf0 = __lasx_xvshuf_b(base64_tbl1, base64_tbl0, input1);
    __m256i input1_shuf1 = __lasx_xvshuf_b(
        base64_tbl3, base64_tbl2, __lasx_xvsub_b(input1, __lasx_xvldi(32)));
    __m256i input1_mask = __lasx_xvslei_bu(input1, 31);
    __m256i input1_result =
        __lasx_xvbitsel_v(input1_shuf1, input1_shuf0, input1_mask);
    __lasx_xvst(input1_result, reinterpret_cast<__m256i *>(out), 0);
    out += 32;

    __m256i input2 = __lasx_xvor_v(t1_2, t3_2);
    __m256i input2_shuf0 = __lasx_xvshuf_b(base64_tbl1, base64_tbl0, input2);
    __m256i input2_shuf1 = __lasx_xvshuf_b(
        base64_tbl3, base64_tbl2, __lasx_xvsub_b(input2, __lasx_xvldi(32)));
    __m256i input2_mask = __lasx_xvslei_bu(input2, 31);
    __m256i input2_result =
        __lasx_xvbitsel_v(input2_shuf1, input2_shuf0, input2_mask);
    __lasx_xvst(input2_result, reinterpret_cast<__m256i *>(out), 0);
    out += 32;

    __m256i input3 = __lasx_xvor_v(t1_3, t3_3);
    __m256i input3_shuf0 = __lasx_xvshuf_b(base64_tbl1, base64_tbl0, input3);
    __m256i input3_shuf1 = __lasx_xvshuf_b(
        base64_tbl3, base64_tbl2, __lasx_xvsub_b(input3, __lasx_xvldi(32)));
    __m256i input3_mask = __lasx_xvslei_bu(input3, 31);
    __m256i input3_result =
        __lasx_xvbitsel_v(input3_shuf1, input3_shuf0, input3_mask);
    __lasx_xvst(input3_result, reinterpret_cast<__m256i *>(out), 0);
    out += 32;
  }
  for (; i + 28 <= srclen; i += 24) {

    __m128i in_lo = __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 0);
    __m128i in_hi =
        __lsx_vld(reinterpret_cast<const __m128i *>(input + i), 4 * 3 * 1);

    __m256i in = lasx_set_q(in_hi, in_lo);

    // bytes from groups A, B and C are needed in separate 32-bit lanes
    // in = [DDDD|CCCC|BBBB|AAAA]
    //
    //      an input triplet has layout
    //      [????????|ccdddddd|bbbbcccc|aaaaaabb]
    //        byte 3   byte 2   byte 1   byte 0    -- byte 3 comes from the next
    //        triplet
    //
    //      shuffling changes the order of bytes: 1, 0, 2, 1
    //      [bbbbcccc|ccdddddd|aaaaaabb|bbbbcccc]
    //           ^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^
    //                  processed bits
    in = __lasx_xvshuf_b(in, in, (__m256i)shuf);

    // unpacking
    // t0    = [0000cccc|cc000000|aaaaaa00|00000000]
    __m256i t0 = __lasx_xvand_v(in, v_fc0fc00);
    // t1    = [00000000|00cccccc|00000000|00aaaaaa]
    //          ((c >> 6),  (a >> 10))
    __m256i t1 = __lasx_xvsrl_h(t0, shift_r);

    // t2    = [00000000|00dddddd|000000bb|bbbb0000]
    __m256i t2 = __lasx_xvand_v(in, v_3f03f0);
    // t3    = [00dddddd|00000000|00bbbbbb|00000000]
    //          ((d << 8), (b << 4))
    __m256i t3 = __lasx_xvsll_h(t2, shift_l);

    // res   = [00dddddd|00cccccc|00bbbbbb|00aaaaaa] = t1 | t3
    __m256i indices = __lasx_xvor_v(t1, t3);

    __m256i indices_shuf0 = __lasx_xvshuf_b(base64_tbl1, base64_tbl0, indices);
    __m256i indices_shuf1 = __lasx_xvshuf_b(
        base64_tbl3, base64_tbl2, __lasx_xvsub_b(indices, __lasx_xvldi(32)));
    __m256i indices_mask = __lasx_xvslei_bu(indices, 31);
    __m256i indices_result =
        __lasx_xvbitsel_v(indices_shuf1, indices_shuf0, indices_mask);
    __lasx_xvst(indices_result, reinterpret_cast<__m256i *>(out), 0);
    out += 32;
  }

  return i / 3 * 4 + scalar::base64::tail_encode_base64((char *)out, src + i,
                                                        srclen - i, options);
}

static inline void compress(__m128i data, uint16_t mask, char *output) {
  if (mask == 0) {
    __lsx_vst(data, reinterpret_cast<__m128i *>(output), 0);
    return;
  }
  // this particular implementation was inspired by work done by @animetosho
  // we do it in two steps, first 8 bytes and then second 8 bytes
  uint8_t mask1 = uint8_t(mask);      // least significant 8 bits
  uint8_t mask2 = uint8_t(mask >> 8); // most significant 8 bits
  // next line just loads the 64-bit values thintable_epi8[mask1] and
  // thintable_epi8[mask2] into a 128-bit register, using only
  // two instructions on most compilers.

  v2u64 shufmask = {tables::base64::thintable_epi8[mask1],
                    tables::base64::thintable_epi8[mask2]};

  // we increment by 0x08 the second half of the mask
  const v4u32 hi = {0, 0, 0x08080808, 0x08080808};
  __m128i shufmask1 = __lsx_vadd_b((__m128i)shufmask, (__m128i)hi);

  // this is the version "nearly pruned"
  __m128i pruned = __lsx_vshuf_b(data, data, shufmask1);
  // we still need to put the two halves together.
  // we compute the popcount of the first half:
  int pop1 = tables::base64::BitsSetTable256mul2[mask1];
  // then load the corresponding mask, what it does is to write
  // only the first pop1 bytes from the first 8 bytes, and then
  // it fills in with the bytes from the second 8 bytes + some filling
  // at the end.
  __m128i compactmask =
      __lsx_vld(reinterpret_cast<const __m128i *>(
                    tables::base64::pshufb_combine_table + pop1 * 8),
                0);
  __m128i answer = __lsx_vshuf_b(pruned, pruned, compactmask);

  __lsx_vst(answer, reinterpret_cast<__m128i *>(output), 0);
}

struct block64 {
  __m256i chunks[2];
};

template <bool base64_url, bool default_or_url>
static inline uint32_t to_base64_mask(__m256i *src, bool *error) {
  __m256i ascii_space_tbl =
      ____m256i((__m128i)v16u8{0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                               0x9, 0xa, 0x0, 0xc, 0xd, 0x0, 0x0});
  // credit: aqrit
  __m256i delta_asso;
  if (default_or_url) {
    delta_asso =
        ____m256i((__m128i)v16u8{0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0,
                                 0x0, 0x0, 0x0, 0x0, 0x11, 0x0, 0x16});
  } else {
    delta_asso =
        ____m256i((__m128i)v16u8{0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0,
                                 0x0, 0x0, 0x0, 0x0, 0xF, 0x0, 0xF});
  }
  __m256i delta_values;
  if (default_or_url) {
    delta_values = ____m256i(
        (__m128i)v16i8{int8_t(0xBF), int8_t(0xE0), int8_t(0xB9), int8_t(0x13),
                       int8_t(0x04), int8_t(0xBF), int8_t(0xBF), int8_t(0xB9),
                       int8_t(0xB9), int8_t(0x00), int8_t(0xFF), int8_t(0x11),
                       int8_t(0xFF), int8_t(0xBF), int8_t(0x10), int8_t(0xB9)});
  } else if (base64_url) {
    delta_values = ____m256i(
        (__m128i)v16i8{int8_t(0x00), int8_t(0x00), int8_t(0x00), int8_t(0x13),
                       int8_t(0x04), int8_t(0xBF), int8_t(0xBF), int8_t(0xB9),
                       int8_t(0xB9), int8_t(0x00), int8_t(0x11), int8_t(0xC3),
                       int8_t(0xBF), int8_t(0xE0), int8_t(0xB9), int8_t(0xB9)});
  } else {
    delta_values = ____m256i(
        (__m128i)v16i8{int8_t(0x00), int8_t(0x00), int8_t(0x00), int8_t(0x13),
                       int8_t(0x04), int8_t(0xBF), int8_t(0xBF), int8_t(0xB9),
                       int8_t(0xB9), int8_t(0x00), int8_t(0x10), int8_t(0xC3),
                       int8_t(0xBF), int8_t(0xBF), int8_t(0xB9), int8_t(0xB9)});
  }

  __m256i check_asso;
  if (default_or_url) {
    check_asso = ____m256i((__m128i)v16u8{0x0D, 0x01, 0x01, 0x01, 0x01, 0x01,
                                          0x01, 0x01, 0x01, 0x01, 0x03, 0x07,
                                          0x0B, 0x0E, 0x0B, 0x06});

  } else if (base64_url) {
    check_asso = ____m256i((__m128i)v16u8{0x0D, 0x01, 0x01, 0x01, 0x01, 0x01,
                                          0x01, 0x01, 0x01, 0x01, 0x03, 0x07,
                                          0x0B, 0x06, 0x0B, 0x12});
  } else {
    check_asso = ____m256i((__m128i)v16u8{0x0D, 0x01, 0x01, 0x01, 0x01, 0x01,
                                          0x01, 0x01, 0x01, 0x01, 0x03, 0x07,
                                          0x0B, 0x0B, 0x0B, 0x0F});
  }

  __m256i check_values;
  if (default_or_url) {

    check_values = ____m256i(
        (__m128i)v16i8{int8_t(0x80), int8_t(0x80), int8_t(0x80), int8_t(0x80),
                       int8_t(0xCF), int8_t(0xBF), int8_t(0xD5), int8_t(0xA6),
                       int8_t(0xB5), int8_t(0xA1), int8_t(0x00), int8_t(0x80),
                       int8_t(0x00), int8_t(0x80), int8_t(0x00), int8_t(0x80)});
  } else if (base64_url) {
    check_values = ____m256i(
        (__m128i)v16i8{int8_t(0x0), int8_t(0x80), int8_t(0x80), int8_t(0x80),
                       int8_t(0xCF), int8_t(0xBF), int8_t(0xD3), int8_t(0xA6),
                       int8_t(0xB5), int8_t(0x86), int8_t(0xD0), int8_t(0x80),
                       int8_t(0xB0), int8_t(0x80), int8_t(0x0), int8_t(0x0)});
  } else {
    check_values = ____m256i(
        (__m128i)v16i8{int8_t(0x80), int8_t(0x80), int8_t(0x80), int8_t(0x80),
                       int8_t(0xCF), int8_t(0xBF), int8_t(0xD5), int8_t(0xA6),
                       int8_t(0xB5), int8_t(0x86), int8_t(0xD1), int8_t(0x80),
                       int8_t(0xB1), int8_t(0x80), int8_t(0x91), int8_t(0x80)});
  }

  __m256i shifted = __lasx_xvsrli_b(*src, 3);
  __m256i asso_index = __lasx_xvand_v(*src, __lasx_xvldi(0xF));
  __m256i delta_hash = __lasx_xvavgr_bu(
      __lasx_xvshuf_b(delta_asso, delta_asso, asso_index), shifted);
  __m256i check_hash = __lasx_xvavgr_bu(
      __lasx_xvshuf_b(check_asso, check_asso, asso_index), shifted);

  __m256i out = __lasx_xvsadd_b(
      __lasx_xvshuf_b(delta_values, delta_values, delta_hash), *src);
  __m256i chk = __lasx_xvsadd_b(
      __lasx_xvshuf_b(check_values, check_values, check_hash), *src);
  __m256i chk_ltz = __lasx_xvmskltz_b(chk);
  unsigned int mask = __lasx_xvpickve2gr_wu(chk_ltz, 0);
  mask = mask | (__lsx_vpickve2gr_hu(lasx_extracti128_hi(chk_ltz), 0) << 16);
  if (mask) {
    __m256i ascii_space = __lasx_xvseq_b(
        __lasx_xvshuf_b(ascii_space_tbl, ascii_space_tbl, asso_index), *src);
    __m256i ascii_space_ltz = __lasx_xvmskltz_b(ascii_space);
    unsigned int ascii_space_mask = __lasx_xvpickve2gr_wu(ascii_space_ltz, 0);
    ascii_space_mask =
        ascii_space_mask |
        (__lsx_vpickve2gr_hu(lasx_extracti128_hi(ascii_space_ltz), 0) << 16);
    *error |= (mask != ascii_space_mask);
  }

  *src = out;
  return (uint32_t)mask;
}

template <bool base64_url, bool default_or_url>
static inline uint64_t to_base64_mask(block64 *b, bool *error) {
  *error = 0;
  uint64_t m0 =
      to_base64_mask<base64_url, default_or_url>(&b->chunks[0], error);
  uint64_t m1 =
      to_base64_mask<base64_url, default_or_url>(&b->chunks[1], error);
  return m0 | (m1 << 32);
}

static inline void copy_block(block64 *b, char *output) {
  __lasx_xvst(b->chunks[0], reinterpret_cast<__m256i *>(output), 0);
  __lasx_xvst(b->chunks[1], reinterpret_cast<__m256i *>(output), 32);
}

static inline uint64_t compress_block(block64 *b, uint64_t mask, char *output) {
  uint64_t nmask = ~mask;
  uint64_t count =
      __lsx_vpickve2gr_d(__lsx_vpcnt_h(__lsx_vreplgr2vr_d(nmask)), 0);
  uint16_t *count_ptr = (uint16_t *)&count;
  compress(lasx_extracti128_lo(b->chunks[0]), uint16_t(mask), output);
  compress(lasx_extracti128_hi(b->chunks[0]), uint16_t(mask >> 16),
           output + count_ptr[0]);
  compress(lasx_extracti128_lo(b->chunks[1]), uint16_t(mask >> 32),
           output + count_ptr[0] + count_ptr[1]);
  compress(lasx_extracti128_hi(b->chunks[1]), uint16_t(mask >> 48),
           output + count_ptr[0] + count_ptr[1] + count_ptr[2]);
  return count_ones(nmask);
}

// The caller of this function is responsible to ensure that there are 64 bytes
// available from reading at src. The data is read into a block64 structure.
static inline void load_block(block64 *b, const char *src) {
  b->chunks[0] = __lasx_xvld(reinterpret_cast<const __m256i *>(src), 0);
  b->chunks[1] = __lasx_xvld(reinterpret_cast<const __m256i *>(src), 32);
}

// The caller of this function is responsible to ensure that there are 128 bytes
// available from reading at src. The data is read into a block64 structure.
static inline void load_block(block64 *b, const char16_t *src) {
  __m256i m1 = __lasx_xvld(reinterpret_cast<const __m256i *>(src), 0);
  __m256i m2 = __lasx_xvld(reinterpret_cast<const __m256i *>(src), 32);
  __m256i m3 = __lasx_xvld(reinterpret_cast<const __m256i *>(src), 64);
  __m256i m4 = __lasx_xvld(reinterpret_cast<const __m256i *>(src), 96);
  b->chunks[0] = __lasx_xvpermi_d(__lasx_xvssrlni_bu_h(m2, m1, 0), 0b11011000);
  b->chunks[1] = __lasx_xvpermi_d(__lasx_xvssrlni_bu_h(m4, m3, 0), 0b11011000);
}

static inline void base64_decode(char *out, __m256i str) {
  __m256i t0 = __lasx_xvor_v(
      __lasx_xvslli_w(str, 26),
      __lasx_xvslli_w(__lasx_xvand_v(str, lasx_splat_u32(0x0000ff00)), 12));
  __m256i t1 =
      __lasx_xvsrli_w(__lasx_xvand_v(str, lasx_splat_u32(0x003f0000)), 2);
  __m256i t2 = __lasx_xvor_v(t0, t1);
  __m256i t3 = __lasx_xvor_v(t2, __lasx_xvsrli_w(str, 16));
  __m256i pack_shuffle = ____m256i(
      (__m128i)v16u8{3, 2, 1, 7, 6, 5, 11, 10, 9, 15, 14, 13, 0, 0, 0, 0});
  t3 = __lasx_xvshuf_b(t3, t3, (__m256i)pack_shuffle);

  // Store the output:
  __lsx_vst(lasx_extracti128_lo(t3), out, 0);
  __lsx_vst(lasx_extracti128_hi(t3), out, 12);
}
// decode 64 bytes and output 48 bytes
static inline void base64_decode_block(char *out, const char *src) {
  base64_decode(out, __lasx_xvld(reinterpret_cast<const __m256i *>(src), 0));
  base64_decode(out + 24,
                __lasx_xvld(reinterpret_cast<const __m256i *>(src), 32));
}

static inline void base64_decode_block_safe(char *out, const char *src) {
  base64_decode(out, __lasx_xvld(reinterpret_cast<const __m256i *>(src), 0));
  char buffer[32];
  base64_decode(buffer,
                __lasx_xvld(reinterpret_cast<const __m256i *>(src), 32));
  std::memcpy(out + 24, buffer, 24);
}

static inline void base64_decode_block(char *out, block64 *b) {
  base64_decode(out, b->chunks[0]);
  base64_decode(out + 24, b->chunks[1]);
}
static inline void base64_decode_block_safe(char *out, block64 *b) {
  base64_decode(out, b->chunks[0]);
  char buffer[32];
  base64_decode(buffer, b->chunks[1]);
  std::memcpy(out + 24, buffer, 24);
}

template <bool base64_url, bool ignore_garbage, bool default_or_url,
          typename chartype>
full_result
compress_decode_base64(char *dst, const chartype *src, size_t srclen,
                       base64_options options,
                       last_chunk_handling_options last_chunk_options) {
  const uint8_t *to_base64 =
      default_or_url ? tables::base64::to_base64_default_or_url_value
                     : (base64_url ? tables::base64::to_base64_url_value
                                   : tables::base64::to_base64_value);
  auto ri = simdutf::scalar::base64::find_end(src, srclen, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  srclen = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  (void)full_input_length;
  if (srclen == 0) {
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation, 0};
    }
    return {SUCCESS, 0, 0};
  }
  char *end_of_safe_64byte_zone =
      (srclen + 3) / 4 * 3 >= 63 ? dst + (srclen + 3) / 4 * 3 - 63 : dst;

  const chartype *const srcinit = src;
  const char *const dstinit = dst;
  const chartype *const srcend = src + srclen;

  constexpr size_t block_size = 6;
  static_assert(block_size >= 2, "block_size must be at least two");
  char buffer[block_size * 64];
  char *bufferptr = buffer;
  if (srclen >= 64) {
    const chartype *const srcend64 = src + srclen - 64;
    while (src <= srcend64) {
      block64 b;
      load_block(&b, src);
      src += 64;
      bool error = false;
      uint64_t badcharmask =
          to_base64_mask<base64_url, default_or_url>(&b, &error);
      if (error && !ignore_garbage) {
        src -= 64;
        while (src < srcend && scalar::base64::is_eight_byte(*src) &&
               to_base64[uint8_t(*src)] <= 64) {
          src++;
        }
        return {error_code::INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit)};
      }
      if (badcharmask != 0) {
        // optimization opportunity: check for simple masks like those made of
        // continuous 1s followed by continuous 0s. And masks containing a
        // single bad character.
        bufferptr += compress_block(&b, badcharmask, bufferptr);
      } else if (bufferptr != buffer) {
        copy_block(&b, bufferptr);
        bufferptr += 64;
      } else {
        if (dst >= end_of_safe_64byte_zone) {
          base64_decode_block_safe(dst, &b);
        } else {
          base64_decode_block(dst, &b);
        }
        dst += 48;
      }
      if (bufferptr >= (block_size - 1) * 64 + buffer) {
        for (size_t i = 0; i < (block_size - 2); i++) {
          base64_decode_block(dst, buffer + i * 64);
          dst += 48;
        }
        if (dst >= end_of_safe_64byte_zone) {
          base64_decode_block_safe(dst, buffer + (block_size - 2) * 64);
        } else {
          base64_decode_block(dst, buffer + (block_size - 2) * 64);
        }
        dst += 48;
        std::memcpy(buffer, buffer + (block_size - 1) * 64,
                    64); // 64 might be too much
        bufferptr -= (block_size - 1) * 64;
      }
    }
  }

  char *buffer_start = buffer;
  // Optimization note: if this is almost full, then it is worth our
  // time, otherwise, we should just decode directly.
  int last_block = (int)((bufferptr - buffer_start) % 64);
  if (last_block != 0 && srcend - src + last_block >= 64) {

    while ((bufferptr - buffer_start) % 64 != 0 && src < srcend) {
      uint8_t val = to_base64[uint8_t(*src)];
      *bufferptr = char(val);
      if ((!scalar::base64::is_eight_byte(*src) || val > 64) &&
          !ignore_garbage) {
        return {error_code::INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit)};
      }
      bufferptr += (val <= 63);
      src++;
    }
  }

  for (; buffer_start + 64 <= bufferptr; buffer_start += 64) {
    if (dst >= end_of_safe_64byte_zone) {
      base64_decode_block_safe(dst, buffer_start);
    } else {
      base64_decode_block(dst, buffer_start);
    }
    dst += 48;
  }
  if ((bufferptr - buffer_start) % 64 != 0) {
    while (buffer_start + 4 < bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
      triple = scalar::u32_swap_bytes(triple);
      std::memcpy(dst, &triple, 4);

      dst += 3;
      buffer_start += 4;
    }
    if (buffer_start + 4 <= bufferptr) {
      uint32_t triple = ((uint32_t(uint8_t(buffer_start[0])) << 3 * 6) +
                         (uint32_t(uint8_t(buffer_start[1])) << 2 * 6) +
                         (uint32_t(uint8_t(buffer_start[2])) << 1 * 6) +
                         (uint32_t(uint8_t(buffer_start[3])) << 0 * 6))
                        << 8;
      triple = scalar::u32_swap_bytes(triple);
      std::memcpy(dst, &triple, 3);

      dst += 3;
      buffer_start += 4;
    }
    // we may have 1, 2 or 3 bytes left and we need to decode them so let us
    // backtrack
    int leftover = int(bufferptr - buffer_start);
    while (leftover > 0) {
      if (!ignore_garbage) {
        while (to_base64[uint8_t(*(src - 1))] == 64) {
          src--;
        }
      } else {
        while (to_base64[uint8_t(*(src - 1))] >= 64) {
          src--;
        }
      }
      src--;
      leftover--;
    }
  }
  if (src < srcend + equalsigns) {
    full_result r = scalar::base64::base64_tail_decode(
        dst, src, srcend - src, equalsigns, options, last_chunk_options);
    r = scalar::base64::patch_tail_result(
        r, size_t(src - srcinit), size_t(dst - dstinit), equallocation,
        full_input_length, last_chunk_options);
    return r;
  }
  if (equalsigns > 0 && !ignore_garbage) {
    if ((size_t(dst - dstinit) % 3 == 0) ||
        ((size_t(dst - dstinit) % 3) + 1 + equalsigns != 4)) {
      return {INVALID_BASE64_CHARACTER, equallocation, size_t(dst - dstinit)};
    }
  }
  return {SUCCESS, srclen, size_t(dst - dstinit)};
}
/* end file src\lasx\lasx_base64.cpp */
/* begin file src\lasx\lasx_find.cpp */
simdutf_really_inline const char *util_find(const char *start, const char *end,
                                            char character) noexcept {
  if (start >= end)
    return end;

  const int step = 32;
  __m256i char_vec = __lasx_xvreplgr2vr_b(static_cast<uint16_t>(character));

  while (end - start >= step) {
    __m256i data = __lasx_xvld(reinterpret_cast<const __m256i *>(start), 0);
    __m256i cmp = __lasx_xvseq_b(data, char_vec);
    if (__lasx_xbnz_v(cmp)) {
      __m256i res = __lasx_xvmsknz_b(cmp);
      uint32_t mask0 = __lasx_xvpickve2gr_wu(res, 0);
      uint32_t mask1 = __lasx_xvpickve2gr_wu(res, 4);
      uint32_t mask = (mask0 | (mask1 << 16));
      return start + trailing_zeroes(mask);
    }

    start += step;
  }

  // Handle remaining bytes with scalar loop
  for (; start < end; ++start) {
    if (*start == character) {
      return start;
    }
  }

  return end;
}

simdutf_really_inline const char16_t *util_find(const char16_t *start,
                                                const char16_t *end,
                                                char16_t character) noexcept {
  if (start >= end)
    return end;

  const int step = 16;
  __m256i char_vec = __lasx_xvreplgr2vr_h(static_cast<uint16_t>(character));

  while (end - start >= step) {
    __m256i data = __lasx_xvld(reinterpret_cast<const __m256i *>(start), 0);
    __m256i cmp = __lasx_xvseq_h(data, char_vec);
    if (__lasx_xbnz_v(cmp)) {
      __m256i res = __lasx_xvmsknz_b(cmp);
      uint32_t mask0 = __lasx_xvpickve2gr_wu(res, 0);
      uint32_t mask1 = __lasx_xvpickve2gr_wu(res, 4);
      uint32_t mask = (mask0 | (mask1 << 16));
      return start + trailing_zeroes(mask) / 2;
    }

    start += step;
  }

  // Handle remaining elements with scalar loop
  for (; start < end; ++start) {
    if (*start == character) {
      return start;
    }
  }

  return end;
}
/* end file src\lasx\lasx_find.cpp */

} // namespace
} // namespace lasx
} // namespace simdutf

/* begin file src\generic\buf_block_reader.h */
namespace simdutf {
namespace lasx {
namespace {

// Walks through a buffer in block-sized increments, loading the last part with
// spaces
template <size_t STEP_SIZE> struct buf_block_reader {
public:
  simdutf_really_inline buf_block_reader(const uint8_t *_buf, size_t _len);
  simdutf_really_inline size_t block_index();
  simdutf_really_inline bool has_full_block() const;
  simdutf_really_inline const uint8_t *full_block() const;
  /**
   * Get the last block, padded with spaces.
   *
   * There will always be a last block, with at least 1 byte, unless len == 0
   * (in which case this function fills the buffer with spaces and returns 0. In
   * particular, if len == STEP_SIZE there will be 0 full_blocks and 1 remainder
   * block with STEP_SIZE bytes and no spaces for padding.
   *
   * @return the number of effective characters in the last block.
   */
  simdutf_really_inline size_t get_remainder(uint8_t *dst) const;
  simdutf_really_inline void advance();

private:
  const uint8_t *buf;
  const size_t len;
  const size_t lenminusstep;
  size_t idx;
};

// Routines to print masks and text for debugging bitmask operations
simdutf_unused static char *format_input_text_64(const uint8_t *text) {
  static char *buf =
      reinterpret_cast<char *>(malloc(sizeof(simd8x64<uint8_t>) + 1));
  for (size_t i = 0; i < sizeof(simd8x64<uint8_t>); i++) {
    buf[i] = int8_t(text[i]) < ' ' ? '_' : int8_t(text[i]);
  }
  buf[sizeof(simd8x64<uint8_t>)] = '\0';
  return buf;
}

// Routines to print masks and text for debugging bitmask operations
simdutf_unused static char *format_input_text(const simd8x64<uint8_t> &in) {
  static char *buf =
      reinterpret_cast<char *>(malloc(sizeof(simd8x64<uint8_t>) + 1));
  in.store(reinterpret_cast<uint8_t *>(buf));
  for (size_t i = 0; i < sizeof(simd8x64<uint8_t>); i++) {
    if (buf[i] < ' ') {
      buf[i] = '_';
    }
  }
  buf[sizeof(simd8x64<uint8_t>)] = '\0';
  return buf;
}

simdutf_unused static char *format_mask(uint64_t mask) {
  static char *buf = reinterpret_cast<char *>(malloc(64 + 1));
  for (size_t i = 0; i < 64; i++) {
    buf[i] = (mask & (size_t(1) << i)) ? 'X' : ' ';
  }
  buf[64] = '\0';
  return buf;
}

template <size_t STEP_SIZE>
simdutf_really_inline
buf_block_reader<STEP_SIZE>::buf_block_reader(const uint8_t *_buf, size_t _len)
    : buf{_buf}, len{_len}, lenminusstep{len < STEP_SIZE ? 0 : len - STEP_SIZE},
      idx{0} {}

template <size_t STEP_SIZE>
simdutf_really_inline size_t buf_block_reader<STEP_SIZE>::block_index() {
  return idx;
}

template <size_t STEP_SIZE>
simdutf_really_inline bool buf_block_reader<STEP_SIZE>::has_full_block() const {
  return idx < lenminusstep;
}

template <size_t STEP_SIZE>
simdutf_really_inline const uint8_t *
buf_block_reader<STEP_SIZE>::full_block() const {
  return &buf[idx];
}

template <size_t STEP_SIZE>
simdutf_really_inline size_t
buf_block_reader<STEP_SIZE>::get_remainder(uint8_t *dst) const {
  if (len == idx) {
    return 0;
  } // memcpy(dst, null, 0) will trigger an error with some sanitizers
  std::memset(dst, 0x20,
              STEP_SIZE); // std::memset STEP_SIZE because it is more efficient
                          // to write out 8 or 16 bytes at once.
  std::memcpy(dst, buf + idx, len - idx);
  return len - idx;
}

template <size_t STEP_SIZE>
simdutf_really_inline void buf_block_reader<STEP_SIZE>::advance() {
  idx += STEP_SIZE;
}

} // unnamed namespace
} // namespace lasx
} // namespace simdutf
/* end file src\generic\buf_block_reader.h */

//
// Implementation-specific overrides
//
namespace simdutf {
namespace lasx {

simdutf_warn_unused result implementation::base64_to_binary(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused result implementation::base64_to_binary(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

simdutf_warn_unused full_result implementation::base64_to_binary_details(
    const char16_t *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) const noexcept {
  if (options & base64_default_or_url) {
    if (options == base64_options::base64_default_or_url_accept_garbage) {
      return compress_decode_base64<false, true, true>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, true>(
          output, input, length, options, last_chunk_options);
    }
  } else if (options & base64_url) {
    if (options == base64_options::base64_url_accept_garbage) {
      return compress_decode_base64<true, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<true, false, false>(
          output, input, length, options, last_chunk_options);
    }
  } else {
    if (options == base64_options::base64_default_accept_garbage) {
      return compress_decode_base64<false, true, false>(
          output, input, length, options, last_chunk_options);
    } else {
      return compress_decode_base64<false, false, false>(
          output, input, length, options, last_chunk_options);
    }
  }
}

size_t implementation::binary_to_base64(const char *input, size_t length,
                                        char *output,
                                        base64_options options) const noexcept {
  if (options & base64_url) {
    return encode_base64<true>(output, input, length, options);
  } else {
    return encode_base64<false>(output, input, length, options);
  }
}

const char *implementation::find(const char *start, const char *end,
                                 char character) const noexcept {
  return util_find(start, end, character);
}

const char16_t *implementation::find(const char16_t *start, const char16_t *end,
                                     char16_t character) const noexcept {
  return util_find(start, end, character);
}

} // namespace lasx
} // namespace simdutf

/* begin file src\simdutf\lasx\end.h */
#undef SIMDUTF_SIMD_HAS_UNSIGNED_CMP
/* end file src\simdutf\lasx\end.h */
/* end file src\lasx\implementation.cpp */
#endif

SIMDUTF_POP_DISABLE_WARNINGS
/* end file src\simdutf.cpp */
