#include <stdio.h>
#include <pico/stdlib.h>
#include "interface/menu.h"

display dp;
menu_states menu_actual_state = MENU_INITIAL;


uint8_t menu_initial_cursor = 2;
uint8_t menu_initial_page = 1;

void setup() {
    display_init(&dp);
}

int main() {
    stdio_init_all();
    setup();

    display_draw_circle(64, 32, 30, true, true, &dp);

    while(true) {
        switch(menu_actual_state) {
            case MENU_INITIAL:
                menu_initial_handle(
                    menu_initial_page, 
                    menu_initial_cursor, 
                    &dp
                );
            break;
        }
        sleep_ms(50);
        display_update(&dp);
    }
}