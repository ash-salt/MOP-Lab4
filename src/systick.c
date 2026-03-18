#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "systick.h"



void init_systick() {
    STK->SR = 1;
    STK->CMPLR = 720000;
    STK->CMPHR = 0;
    STK->CTLR = 0b101110;

}

void systick_start() {
    STK->CTLR |= 1;
}