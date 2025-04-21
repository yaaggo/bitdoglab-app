#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

/*
* Como funciona a troca de dados
* 1. o meste gera os pulsos de clock no SCL
* 2. dados são enviados e recebidos na linha SDA, sincronizados com o clock
* 3. Só um dispositivo fala por vez na SDA, evitando colisões
*/

#define SDA_PIN 14 // serial data -> linha de dados por onde passa os bits
#define SCL_PIN 15 // serial clock -> linha de clock, que marca o ritmo da comunicação

#define I2C_PORT i2c1


#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64


typedef struct {
    uint8_t buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
    bool initialized;
} display;

void display_init(display *display);
void display_update(display *display);
void display_clear(display *display);
void display_shutdown(display *display);

void display_draw_pixel(int x, int y, bool on, display *display);
void display_draw_line(int x0, int y0, int x1, int y1, bool on, display *display);
void display_draw_char(int x, int y, char c, bool on, display *display);
void display_draw_string(int x, int y, const char *str, bool on, display *display);
void display_draw_rectangle(int x0, int y0, int x1, int y1, bool filled, bool on, display *display);
void display_draw_circle(int xc, int yc, int radius, bool filled, bool on, display *display);
void display_draw_bitmap( int x, int y, const uint8_t *bitmap, int w, int h, int rotation, bool on, display *display);


#endif