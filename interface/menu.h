#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <stdint.h>
#include <include/display.h>

typedef enum {
    MENU_INITIAL_1,
    MENU_INITIAL_2,
    MENU_LED,
    MENU_MATRIX,
    MENU_JOYSTICK,
    MENU_BUZZER,
    MENU_MIC,
    MENU_DISPLAY
} menu_states;

typedef struct {
    bool selected;
    uint8_t x, y;
    const uint8_t idle[128];
    const uint8_t selected[128];
} button;

void menu_update(menu_states current_menu, display *dp);
void menu_initial_draw(uint8_t page, uint8_t selected_button, display *dp);

#endif