
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
    uint16_t x;
    uint16_t y;
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


__attribute__((interrupt("machine")))
void systick_handler() {
    for (int i = 0; i < wormLength; i++) {
        for (int j = 0; j < num_points; j++) {
            if (worm[i].position.x == turningPoints[j].point.x && worm[i].position.y == turningPoints[j].point.y) {
                worm[i].speed = turningPoints[j].speed;
                turningPoints[j].numTurned += 1;
            }
        }
        tft_rect(worm[i].position.x, worm[i].position.y, worm[i].size, worm[i].size, COLOR_BLACK, 1);
        worm[i].position.x += worm[i].speed.x;
        worm[i].position.y += worm[i].speed.y;
        tft_rect(worm[i].position.x, worm[i].position.y, worm[i].size, worm[i].size, COLOR_WHITE, 1);
    }
    STK->SR = 1;

}

__attribute__((interrupt("machine")))
void exti_handler() {
    uint8_t input = keypad();
    //WASD-esque layout for comfortable inputs
    TurningPoint p;
    p.point.x = worm[0].position.x;
    p.point.y = worm[0].position.y;
    p.numTurned = 0;
    if (input == 1) {
        //up
        p.speed.y = -20;
        p.speed.x = 0;
    }
    else if (input == 4) {
        //left
        p.speed.x = -20;
        p.speed.y = 0;
    }
    else if (input == 5) {
        //down
        p.speed.y = 20;
        p.speed.x = 0;
    }
    else if (input == 6) {
        //right
        p.speed.x = 20;
        p.speed.y = 0;
    }
    turningPoints[num_points] = p;
    worm[0].speed = p.speed;
    num_points += 1;

    *EXTI_INTFR = 0xFFFFFFFF;
    
}

int main(void)
{
    init_systick();
    
    init_vector_table();
    init_interrupts();
    tft_init();
    tft_rect(0, 0, 479, 319, COLOR_BLACK, 1);

    
    
    systick_start();
    while(1) {
        for (int k = 0; k < num_points; k++) {
            if (turningPoints[k].numTurned == wormLength) {
                for (int a = k + 1; a < num_points; a++) {
                    turningPoints[a-1] = turningPoints[a];
                }
                num_points -= 1;
            }
            
        }
    }

}