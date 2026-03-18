
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../tftlib/tft.h"
#include "sprites/sprite_17_2.h"
#include "afio.h"
#include "exti.h"
#include "pfic.h"
#include "keypad.h"
#include "interrupts.h"
#include "systick.h"
#include "vector_table.h"
#include "keyboard.h"
#define RGB565(r, g, b) ((((r) & 0x1F) << 11) | (((g) & 0x3F) << 5) | ((b) & 0x1F))
#define COLOR_BLACK  0x0000
#define COLOR_WHITE  0xFFFF
#define COLOR_RED    0xF800
#define COLOR_GREEN  0x07E0
#define COLOR_BLUE   0x001F



#define SOLUTION 1

typedef struct {
    int16_t x;
    int16_t y;
} Speed;

typedef struct {
    int16_t x;
    int16_t y;
} Point;

typedef struct {
    uint32_t size;
    Point position;
    Speed speed;

} Square;

typedef struct {
    Point point;
    Speed speed;
    int numTurned;
} TurningPoint;

TurningPoint turningPoints[100];
volatile int num_points = 0;

Square worm[50] = {{20, {240, 160}, {20, 0}},
    {20, {220, 160}, {20, 0}}};
int wormLength = 2;

int center_x = 240;
int center_y = 160;
Speed speed = {20,0};
uint8_t input;
volatile uint8_t spawnNewApple;
Point applePos;
volatile uint8_t lost = 0;


__attribute__((interrupt("machine")))
void systick_handler() {
    input = get_keyboard_button();
    Point prev_positions[50];

    for (int i = 0; i < wormLength; i++) {
        prev_positions[i] = worm[i].position;
    }

    // move head
    tft_rect(worm[0].position.x, worm[0].position.y, worm[0].size, worm[0].size, COLOR_BLACK, 1);
    worm[0].position.x += worm[0].speed.x;
    worm[0].position.y += worm[0].speed.y;
    tft_rect(worm[0].position.x, worm[0].position.y, worm[0].size, worm[0].size, COLOR_WHITE, 1);

    // move body
    for (int i = 1; i < wormLength; i++) {
        tft_rect(worm[i].position.x, worm[i].position.y, worm[i].size, worm[i].size, COLOR_BLACK, 1);
        worm[i].position = prev_positions[i - 1];
        tft_rect(worm[i].position.x, worm[i].position.y, worm[i].size, worm[i].size, COLOR_WHITE, 1);
    }
    if (worm[0].position.x == applePos.x && worm[0].position.y == applePos.y) {
        spawnNewApple = 1;
        Square newSegment;
        newSegment.position = prev_positions[wormLength - 1];
        newSegment.speed = worm[wormLength - 1].speed;
        newSegment.size = 20;

        worm[wormLength] = newSegment;
        wormLength++;
    }
    for (int n = 1; n < wormLength; n++) {
        if (worm[0].position.x == worm[n].position.x &&
            worm[0].position.y == worm[n].position.y) {
            printf("You lose! Score : %d \n", wormLength - 2);
            lost = 1;
        }
    }
    if (worm[0].position.x > 480 || worm[0].position.x < 0 || worm[0].position.y > 320 || worm[0].position.y < 0) {
        printf("You lose! Score : %d \n", wormLength - 2);
        lost = 1;
    }
    STK->SR = 1;

}

__attribute__((interrupt("machine")))
void exti_handler() {
    //WASD-esque layout for comfortable inputs
    if (input == 1) {
        //up
        worm[0].speed.y = -20;
        worm[0].speed.x = 0;
    }
    else if (input == 4) {
        //left
        worm[0].speed.x = -20;
        worm[0].speed.y = 0;
    }
    else if (input == 5) {
        //down
        worm[0].speed.y = 20;
        worm[0].speed.x = 0;
    }
    else if (input == 6) {
        //right
        worm[0].speed.x = 20;
        worm[0].speed.y = 0;
    }
    else {
        return;
    }
    num_points += 1;
    *EXTI_INTFR = (1<<(input%4));
}

int main(void)
{
    init_systick();
    init_keyboard();
    init_vector_table();
    init_interrupts();
    tft_init();
    tft_rect(0, 0, 479, 319, COLOR_BLACK, 1);
    *EXTI_INTFR = 0b1111;
    uint8_t deleted = 0;
    applePos.x = (rand() % 24)*20;
    applePos.y = (rand() % 16)*20;
    tft_rect(applePos.x, applePos.y, 10, 10, COLOR_RED, 1);

    spawnNewApple = 0;
    
    
    systick_start();
    while(1) {

        if (spawnNewApple) {
            applePos.x = (rand() % 24)*20;
            applePos.y = (rand() % 16)*20;
            tft_rect(applePos.x, applePos.y, 10, 10, COLOR_RED, 1);
            spawnNewApple = 0;
        }
        
        if (lost) {
            STK -> CTLR &= ~1;
            break;
        }


    }

}