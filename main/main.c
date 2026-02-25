/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

const int LED_PIN = 16;
const int BUTTON_PIN = 15;
const int step_pinA = 18;
const int step_pinB = 19;
const int step_pinC = 20;
const int step_pinD = 21;

// 28BYJ-48: 4096 steps per full rotation (half-step mode)
const int STEPS_PER_ROTATION = 4096;

// Step sequence for 28BYJ-48 (half-step, 8 positions per full cycle)
int steps[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};

void rotate_one_turn() {
    printf("Rotating 360 degrees...\n");
    for (int i = 0; i < STEPS_PER_ROTATION; i++) {
        int step = i % 8;
        gpio_put(step_pinA, steps[step][0]);
        gpio_put(step_pinB, steps[step][1]);
        gpio_put(step_pinC, steps[step][2]);
        gpio_put(step_pinD, steps[step][3]);
        sleep_ms(5);  // 5ms per step
        
        // Debug output every 512 steps
        if (i % 512 == 0) {
            printf("Step %d\n", i);
        }
    }
    // Turn off all pins after rotation
    gpio_put(step_pinA, 0);
    gpio_put(step_pinB, 0);
    gpio_put(step_pinC, 0);
    gpio_put(step_pinD, 0);
    printf("Rotation complete!\n");
}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    
    gpio_init(step_pinA);
    gpio_set_dir(step_pinA, GPIO_OUT);
    gpio_init(step_pinB);
    gpio_set_dir(step_pinB, GPIO_OUT);
    gpio_init(step_pinC);
    gpio_set_dir(step_pinC, GPIO_OUT);
    gpio_init(step_pinD);
    gpio_set_dir(step_pinD, GPIO_OUT);
    
    while (true) {
        if (!gpio_get(BUTTON_PIN)) {
            sleep_ms(20);  // Debounce
            if (!gpio_get(BUTTON_PIN)) {  // Confirm button is pressed
                gpio_put(LED_PIN, 1);
                rotate_one_turn();
                gpio_put(LED_PIN, 0);
                
                // Wait for button release
                while (!gpio_get(BUTTON_PIN)) {
                    sleep_ms(10);
                }
                sleep_ms(50);  // Debounce release
            }
        }
        sleep_ms(10);
    }
}
