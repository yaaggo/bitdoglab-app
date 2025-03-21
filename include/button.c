#include "button.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/timer.h"

static volatile button_event current_event = BUTTON_NONE;

static absolute_time_t last_press_time_a = 0;
static absolute_time_t last_press_time_b = 0;
static absolute_time_t last_press_time_joystick = 0;

static void gpio_callback(uint gpio, uint32_t events) {
    absolute_time_t now = get_absolute_time();

    if (gpio == BUTTON_A_PIN) {
        if (absolute_time_diff_us(last_press_time_a, now) < DEBOUNCE_MS * 1000) {
            return;
        }
        last_press_time_a = now;
        current_event = BUTTON_A;
    }

    if (gpio == BUTTON_B_PIN) {
        if (absolute_time_diff_us(last_press_time_b, now) < DEBOUNCE_MS * 1000) {
            return;
        }
        last_press_time_b = now;
        current_event = BUTTON_B;
    }

    if (gpio == BUTTON_JOYSTICK_PIN) {
        if (absolute_time_diff_us(last_press_time_joystick, now) < DEBOUNCE_MS * 1000) {
            return;
        }
        last_press_time_joystick = now;
        current_event = BUTTON_JOYSTICK;
    }
}

void button_init(void) {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    gpio_init(BUTTON_JOYSTICK_PIN);
    gpio_set_dir(BUTTON_JOYSTICK_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_JOYSTICK_PIN);

    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BUTTON_JOYSTICK_PIN, GPIO_IRQ_EDGE_FALL, true);
}

button_event button_get_event(void) {
    return current_event;
}

void button_clear_event(void) {
    current_event = BUTTON_NONE;
}