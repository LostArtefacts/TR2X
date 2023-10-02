#pragma once

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    uint8_t opcode; // must be 0xE9
    uint32_t offset;
} JMP;
#pragma pack(pop)

void InjectImpl(bool enable, void (*from)(void), void (*to)(void));

#define VAR_U_(address, type) (*(type *)(address))
#define VAR_I_(address, type, value) (*(type *)(address))
#define ARRAY_(address, type, length) (*(type(*) length)(address))
#define INJECT(enable, from, to)                                               \
    {                                                                          \
        InjectImpl(enable, (void (*)(void))from, (void (*)(void))to);          \
    }
