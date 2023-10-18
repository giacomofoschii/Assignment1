/*********
 * 
 * LED BOARD
 * 
 * This module implements the Led Board,
 * containing the 4 leds.
 * 
 */
#ifndef __LED_BOARD__
#define __LED_BOARD__
#define NLEDS 4

#include "Arduino.h"

void init_led_board();
void reset_led_board();
void light_led_board();

void turn_off_pattern(uint8_t pattern[NLEDS], long delay);
void turn_on_led(int nled);

void reset_pulsing();
void go_on_pulsing();
void go_on_lose();

/* for testing */ 
void test_led_board();

#endif
