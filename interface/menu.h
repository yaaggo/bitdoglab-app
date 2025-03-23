#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <stdint.h>
#include <include/display.h>

typedef enum {
    MENU_INITIAL,
    MENU_LED,
    MENU_MATRIX,
    MENU_JOYSTICK,
    MENU_BUZZER,
    MENU_MIC,
    MENU_DISPLAY
} menu_states;

void menu_initial_handle(uint8_t page, uint8_t cursor, display *dp);
void menu_led_handle(uint8_t cursor, uint8_t selecteds, display *dp);


#endif