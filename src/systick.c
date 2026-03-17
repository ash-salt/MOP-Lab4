#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct {
    uint32_t CTLR;
    uint32_t SR;
    uint64_t CNT;
    uint64_t CMP;
} SysTick;

SysTick* systick = (SysTick*) 0xE000F000;

void init_systick() {
    systick->SR = 1;
    systick->CTLR = 0b101110;
    systick->CMP = 72000000;

}