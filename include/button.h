#ifndef BUTTON_H
#define BUTTON_H

#define BUTTON_A_PIN        5
#define BUTTON_B_PIN        6
#define BUTTON_JOYSTICK_PIN 22

#define DEBOUNCE_MS         200

typedef enum {
    BUTTON_NONE,
    BUTTON_A,
    BUTTON_B,
    BUTTON_JOYSTICK
} button_event;

// inicializa as gpio e as interrupções
void button_init(void);

// le o ultimo evento registrado
button_event button_get_event(void);

// limpa o evento atual
void button_clear_event(void);

#endif