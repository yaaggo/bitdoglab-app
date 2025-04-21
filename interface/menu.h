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

void menu(display *);
void menu_initial_handle(uint8_t page, uint8_t cursor, display *dp);
void menu_led_handle(display *dp);
void menu_matrix_handle(display *dp);
void menu_joystick_handle(display *dp);
void menu_buzzer_handle(display *dp);
void menu_mic_handle(display *dp);
void menu_display_handle(display *dp);

#endif