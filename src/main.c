#include <stdio.h>
#include <pico/stdlib.h>
#include "interface/menu.h"
#include "include/joystick.h"
#include "include/button.h"
#include "include/led.h"

display dp;

void setup() {
    display_init(&dp);
    joystick_init(JOYSTICK_X_PIN, JOYSTICK_Y_PIN);
    button_init();
    led_init(LED_RED_PIN);
    led_init(LED_GREEN_PIN);
    led_init(LED_BLUE_PIN);
}

int main() {
    stdio_init_all();
    setup();

    while(true) {
        display_clear(&dp);

        menu(&dp);
        
        display_update(&dp);
        sleep_ms(100);
    }

}