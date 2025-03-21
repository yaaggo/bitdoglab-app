#include "buzzer.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"

void buzzer_init(uint8_t gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_enabled(slice_num, false);
}

void buzzer_turn_on(uint8_t gpio, uint16_t freq) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint clock_div = 100; // testar entre 4 e 100 para ver qual fica melhor
    uint sys_clock = clock_get_hz(clk_sys);
    uint wrap = sys_clock / (clock_div * freq) - 1;

    pwm_set_clkdiv(slice_num, clock_div);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_gpio_level(gpio, wrap / 2);
    pwm_set_enabled(slice_num, true);
}

void buzzer_turn_off(uint8_t gpio) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_enabled(slice_num, false);
}

void buzzer_beep(uint8_t gpio, uint16_t freq, uint16_t duration_ms) {
    buzzer_turn_on(gpio, freq);
    sleep_ms(duration_ms);
    buzzer_turn_off(gpio);
}
