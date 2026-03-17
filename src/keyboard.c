
#include <stdint.h>
#include <stdio.h>

#define SOLUTION 1
///////////////////////////////////////////////////////////////////////////////
// Assignment 1
// ===========================================================================
// Write a function that configures pin 11 of GPIO port D as input with 
// pull-down resistor enabled.
///////////////////////////////////////////////////////////////////////////////

// TODO: Macro definitions here
#define GPIO_D_BASE  0x40011400
#define GPIO_D_CFGLR ((volatile uint32_t *)0x40011400)
#define GPIO_D_CFGHR ((volatile uint32_t *)0x40011404)
#define GPIO_D_INDR  ((volatile uint32_t *)0x40011408)
#define GPIO_D_OUTDR ((volatile uint32_t *)0x4001140C)
#define GPIO_D_BSHR  ((volatile uint32_t *)0x40011410)
#define GPIO_D_BCR   ((volatile uint32_t *)0x40011414)

void assignment_1(void)
{
    *GPIO_D_CFGHR &= ~(0xF000);
    *GPIO_D_CFGHR |= (0x8000);
    *GPIO_D_OUTDR &= ~(1<<11);
}

///////////////////////////////////////////////////////////////////////////////
// Assignment 2
// ===========================================================================
// Write a more general function that configures a pin of GPIO port D as
// input with pull up or down or floating.
// The function should be able to modify CFGLR and CFGHR if needed. 
///////////////////////////////////////////////////////////////////////////////

void gpio_d_set_pin_input( 
    int pin,    // The pin to configure (0-15)
    int mode    // 0 = PULLDOWN, 1 = PULLUP, 2 = FLOATING 
    )
{   
    int altpin;
    volatile uint32_t * cfg;
    if (pin>=8) {
        altpin = pin - 8;
        cfg = GPIO_D_CFGHR;
    }
    else {
        altpin = pin;
        cfg = GPIO_D_CFGLR;
    }
    *cfg &= ~(0xF << (altpin*4));
    if (mode == 2) {
        *cfg |= (0x4 << (altpin * 4));
    }
    else {
        *cfg |= (0x8 << (altpin * 4));
        if (mode == 0) {
            *GPIO_D_OUTDR &= ~(1<<pin);
        }
        else {
            *GPIO_D_OUTDR |= (1<<pin);
        }

    }
}

///////////////////////////////////////////////////////////////////////////////
// Assignment 3
// ===========================================================================
// Write a function that configures a pin of GPIO port D as
// output with either push-pull or open-drain mode.
// The pin should be configured as output at 2 MHz.
///////////////////////////////////////////////////////////////////////////////
void gpio_d_set_pin_output( 
    int pin,    // The pin to configure
    int mode    // 0 = PUSHPULL, 1 = OPENDRAIN
    )
{
    int altpin;
    volatile uint32_t * cfg;
    if (pin>=8) {
        altpin = pin - 8;
        cfg = GPIO_D_CFGHR;
    }
    else {
        altpin = pin;
        cfg = GPIO_D_CFGLR;
    }
    *cfg &= ~(0xF << (altpin*4));
    if (mode == 0) {
        *cfg |= (0x2 << (altpin * 4));
    }
    else {
        *cfg |= (0x6 << (altpin * 4));
    }
}

///////////////////////////////////////////////////////////////////////////////
// Assignment 4
// ===========================================================================
// Write a function that sets one pin of GPIO port D high.
// The function should use bit set/reset register (BSHR)
// 
// EDIT: At the time you downloaded this code, there is a bug in the simulator
//       that means the BSHR register does not work properly. 
//       Use the OUTDR instead, but make sure you do not overwrite
//       any other pin.
///////////////////////////////////////////////////////////////////////////////
void gpio_d_set_pin_high(int pin)
{
    *GPIO_D_OUTDR |= (1<<pin);
}

///////////////////////////////////////////////////////////////////////////////
// Assignment 5
// ===========================================================================
// Write a function that sets one pin of GPIO port D low.
// The function should use the bit set/reset registers (BSHR or BCR)
//
// EDIT: At the time you downloaded this code, there is a bug in the simulator
//       that means the BSHR register does not work properly. 
//       Use the OUTDR instead, but make sure you do not overwrite
//       any other pin.
///////////////////////////////////////////////////////////////////////////////
void gpio_d_set_pin_low(int pin)
{   
    *GPIO_D_OUTDR &= ~(1<<pin);  
}    

///////////////////////////////////////////////////////////////////////////////
// Assignment 6
// ===========================================================================
// Assume that the keyboard is connected to GPIO port D pins 0-7
// Write a function that returns the number of the button on the keyboard: 
//
// /-------------------\
// |  0 |  1 |  2 |  3 |
// |----|----|----|----|    
// |  4 |  5 |  6 |  7 |
// |----|----|----|----|
// |  8 |  9 | 10 | 11 |
// |----|----|----|----|
// | 12 | 13 | 14 | 15 |
// \-------------------/
// 
// Follow the instructions in comments
// You can use the functions you have written above
///////////////////////////////////////////////////////////////////////////////
int get_keyboard_button(void)
{
    // Configure the row-selection pins (pins 4-7) as output, open drain, at 2 MHz
    gpio_d_set_pin_output(4, 1);
    gpio_d_set_pin_output(5, 1);
    gpio_d_set_pin_output(6, 1);
    gpio_d_set_pin_output(7, 1);

    // Configure the row-read pins (pins 0-3) as input with pull-up resistors

    gpio_d_set_pin_input(0,1);
    gpio_d_set_pin_input(1,1);
    gpio_d_set_pin_input(2,1);
    gpio_d_set_pin_input(3,1);

    // Deactivate all rows by setting pins the row selection pins  4-7 high
    // (remember that they are active low, so 0 selects a row)

    gpio_d_set_pin_high(4);
    gpio_d_set_pin_high(5);
    gpio_d_set_pin_high(6);
    gpio_d_set_pin_high(7);

    for(int row = 0; row < 4; row++) {
        // Activate the current row by setting the corresponding pin low

        gpio_d_set_pin_low(row+4);

        // Read the row (buttons 0-3, 4-7, 8-11, or 12-15) as the lower 4 bits of INDR 
        if (((*GPIO_D_INDR) & 0xF) != 0xF) {
            if (((*GPIO_D_INDR) & 0xF) == 0b1110) {
                return 4*row;
            }
            else if (((*GPIO_D_INDR) & 0xF) == 0b1101) {
                return 4*row + 1;
            }
            else if (((*GPIO_D_INDR) & 0xF) == 0b1011) {
                return 4*row + 2;
            }
            else if (((*GPIO_D_INDR) & 0xF) == 0b0111) {
                return 4*row + 3;
            }
        }

        gpio_d_set_pin_high(row+4);

        // Check if any button in this row is pressed (low)

        //   and, if so, return the number of the button

        // Deactivate the current row by setting the corresponding pin high
    }
    return -1; // No button pressed    return -1;  
}