#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

#define BUZZER_A_PIN 21
#define BUZZER_B_PIN 10

void buzzer_init(uint8_t gpio);
void buzzer_turn_on(uint8_t gpio, uint16_t freq);
void buzzer_turn_off(uint8_t gpio);
void buzzer_beep(uint8_t gpio, uint16_t freq, uint16_t duration_ms);

#endif