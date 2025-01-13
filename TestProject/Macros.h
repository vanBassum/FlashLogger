#pragma once


#define UINT32_TO_BYTES(value) \
    (static_cast<uint8_t>((value >> 0) & 0xFF)), \
    (static_cast<uint8_t>((value >> 8) & 0xFF)), \
    (static_cast<uint8_t>((value >> 16) & 0xFF)), \
    (static_cast<uint8_t>((value >> 24) & 0xFF))
