#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "matrix.h"
#include "ws2818b.pio.h"

// pio principal que será usado
static PIO np_pio;

// variavel para guardar uma state machine (máquina de estado)
static uint sm;

rgb_led leds[LED_COUNT];

uint32_t letters[27] = {
    0xE8C7F1,           // A
    0x1E8FA3E,          // B
    0xF0C02F,           // C
    0x1E8C63E,          // D
    0x1F0FC3F,          // E
    0x1F0F830,          // F
    0xF0CE2F,           // G
    0x118FE31,          // H
    0xE2108E,           // I
    0x18462E,           // J
    0x114F131,          // K
    0x100C03F,          // L
    0x1BAD631,          // M
    0x119D731,          // N
    0xE8C62E,           // O
    0x1E8F830,          // P
    0xE8C52D,           // Q
    0x1E8FA31,          // R
    0xF0BA1E,           // S
    0x1F21084,          // T
    0x118C62E,          // U
    0x118C544,          // V
    0x11AD6AA,          // W
    0x1151151,          // X
    0x1151084,          // Y
    0x1F8185F,          // Z
    0x0000000,          // NULL
};

uint32_t numbers[10] = {
    0xE5294E,           // 0
    0x43108E,           // 1
    0xE4384E,           // 2
    0xE4390E,           // 3
    0xA53902,           // 4
    0xE1390E,           // 5
    0xE1394E,           // 6
    0xE50902,           // 7
    0xE5394E,           // 8
    0xE5390E,           // 9
};

// função para inicializar uma matriz de leds
void matrix_init(uint8_t pin) {
    // aqui adciona o programa ws2818b ao pio0
    // essa função que foi gerada pelo .pio
    // o programa pio define o protocolo de comunicação com os leds (preciso, com temporização exata)
    // o offset vai ser onde o programa foi carregado na memória do pio
    uint offset = pio_add_program(pio0, &ws2818b_program);

    // define que o pio principal a ser usado é a pio0 (pode ser trocada se não houver espaço)
    np_pio = pio0;

    // ele tenta pegar uma state machine livre na pio0
    // se não encontrar (sm < 0), tenta na pio1
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true);
    }

    // inicializa o programa do protocolo ws2818b na pio e state machine escolhida
    // define também qual pino gpio será usado para saída de dados dos leds
    ws2818b_program_init(np_pio, sm, offset, pin);

    // zera todos os leds, deixando todos desligados
    for (uint i = 0; i < LED_COUNT; ++i) {
        //leds[i].activate = 0;
        leds[i].r = 0;
        leds[i].g = 0;
        leds[i].b = 0;
    }

    // envia o conteudo do vetor de leds para a matriz de leds
    matrix_update(leds);

    // da uma pausa para garantir que os dados foram enviados
    sleep_ms(10);
}

void matrix_set_led(uint8_t index, rgb_led color) {
    if (index >= LED_COUNT) return; // Verifica se o índice é válido
    //leds[index].activate = (color.r || color.g || color.b);
    leds[index].r = color.r;
    leds[index].g = color.g;
    leds[index].b = color.b;
}

void matrix_set_led_horizontally(uint8_t index, rgb_led color) {
    uint8_t row = index / LED_LINE;
    uint8_t col = index % LED_LINE;

    if (row % 2 == 0) col = LED_LINE - 1 - col;

    index = (LED_LINE - 1 - row) * LED_LINE + col;

    matrix_set_led(index, color);
}

void matrix_set_led_xy(uint8_t x, uint8_t y, rgb_led color) {
    if (x >= LED_LINE || y >= LED_LINE) return; // Verifica se x e y são válidos

    int index = (y % 2 == 0) ? (LED_LINE - x - 1 + y * LED_LINE) : (x + y * LED_LINE);
    matrix_set_led(index, color);
}

void matrix_clear(rgb_led *leds) {
    for (uint8_t i = 0; i < LED_COUNT; ++i) {
        matrix_set_led(i, (rgb_led){0, 0, 0});
    }
}

void matrix_update() {
    for (uint8_t i = 0; i < LED_COUNT; ++i) {
        uint32_t color = COLOR_HEX(leds[i].r, leds[i].g, leds[i].b);
        pio_sm_put_blocking(np_pio, sm, color);
    }
    // sleep_us(200);
}

void matrix_letter(char letter, rgb_led color) {
    uint32_t configure = letters[letter-'A'];
    for(int i = 0; i < LED_COUNT; i++) {
        if(configure & 1) matrix_set_led(i, color);

        configure = configure >> 1;
    }
}

void matrix_number(uint8_t number, rgb_led color) {
    uint32_t configure = numbers[number];
    for(int i = 0; i < LED_COUNT; i++) {
        if(configure & 1) matrix_set_led(i, color);
        else matrix_set_led(i, COLOR_RGB(0, 0, 0));
        configure = configure >> 1;
    }
}

void matrix_string(char *buffer, rgb_led color) {
    for (int i = 0; buffer[i]; i++) {
        char letter = buffer[i] & ~32; // Converte para maiúsculo

        if (letter >= 'A' && letter <= 'Z') {
            matrix_clear(leds);
            matrix_letter(letter, color);
            matrix_update(leds);
            sleep_ms(500);
        }
    }
}

void matrix_rainbow_wave() {
    const int rainbow_steps = 360;
    const int wave_speed = 5;
    const int delay_ms = 50;
    int wave_offset = 0;

    while (true) {
        for (int y = 0; y < LED_LINE; y++) {
            for (int x = 0; x < LED_LINE; x++) {
                int hue = (wave_offset + (x * 20)) % rainbow_steps;
                rgb_led color = hue_to_rgb(hue);
                matrix_set_led_xy(x, y, color);
            }
        }

        matrix_update(leds);
        wave_offset = (wave_offset + wave_speed) % rainbow_steps;
        sleep_ms(delay_ms);
    }
}

rgb_led hue_to_rgb(int hue) {
    float r, g, b;
    int sector = hue / 60;
    float fractional = (hue % 60) / 60.0;
    float p = 0, q, t;

    q = 1 - fractional;
    t = fractional;

    switch (sector) {
        case 0: r = 1; g = t; b = p; break;
        case 1: r = q; g = 1; b = p; break;
        case 2: r = p; g = 1; b = t; break;
        case 3: r = p; g = q; b = 1; break;
        case 4: r = t; g = p; b = 1; break;
        case 5: r = 1; g = p; b = q; break;
        default: r = g = b = 0; break;
    }

    return (rgb_led) {
        .r = (uint8_t)(r * 255),
        .g = (uint8_t)(g * 255),
        .b = (uint8_t)(b * 255)
    };
}