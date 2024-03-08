#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lfsr.h"

void lfsr_calculate(uint16_t *reg) {
    uint32_t position_0, position_2, position_3, position_5, left;
    position_0 = *reg & 1;
    position_2 = (*reg & (1 << 2)) >> 2;
    position_3 = (*reg & (1 << 3)) >> 3;
    position_5 = (*reg & (1 << 5)) >> 5;
    left = position_0 ^ position_2 ^ position_3 ^ position_5;

    *reg = (*reg >> 1) | (left << 15);
}

