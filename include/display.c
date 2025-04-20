#include "display.h"
#include "font.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <string.h>

// static void i2c_init_custom();
// static void ssd1306_send_command(uint8_t command);

// essa é uma função estatica para inicializar a comunicação i2c
static void i2c_init_custom() {
    // esta usando 400khz pq o display suporta comunicação standart e fast
    // a standart tem velocidade máxima de 100khz
    // a fast tem velocidade máxima de 400khz
    // no caso, estamos usando a fast
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    // é importante ativar o pull up ja que o sda e o scl são open-drain
    // os dispositivos conseguem puxar o fio para gnd
    // eles não conseguem forçar ele para o VCC
    // dessa forma, o pull up faz esse papel, deixando em nível lógico alto
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

// envia um comando para o display ssd1306 via i2c
static void ssd1306_send_command(uint8_t command) {
    // é um array de 2 byts para enviar pela i2c
    // - 0x00 é um prefixo de comando, indica que o proximo byte é um comando
    // - command é o comando real que queremos enviar para o display
    uint8_t data[] = {0x00, command};

    // essa função recebe uma sequencia de dados
    // - I2C_PORT é qual a porta i2c do rp2040 está sendo usada
    // - 0x3C é o endereço i2c do ssd1306
    // - data é os bytes que queremos enviar
    // - false envair stop condition no final, encerrando a transmissão
    i2c_write_blocking(I2C_PORT, 0x3C, data, sizeof(data), false);
}
// inicializa tudo do display
void display_init(display *display) {
    if (display->initialized) return;

    i2c_init_custom();
    // garante que o display esteja desligado antes de configurar
    ssd1306_send_command(0xAE); // display OFF

    // define a frequência do driver interno
    ssd1306_send_command(0xD5); // Set display Clock Divide Ratio
    ssd1306_send_command(0x80); // Frequência padrão

    // define quantas linhas verticais serão usadas (3F == 63 -> 64 linhas)
    ssd1306_send_command(0xA8); // Set Multiplex Ratio
    ssd1306_send_command(0x3F); // Multiplex para 64 linhas

    // não desloca o conteúdo verticalmente
    ssd1306_send_command(0xD3); // Set display Offset
    ssd1306_send_command(0x00); // Sem deslocamento

    // começa a desenhar a partir da linha 0
    ssd1306_send_command(0x40); // Set display Start Line para 0

    // liga a bomba de carga interna, necessário para gerar tensão do oled
    ssd1306_send_command(0x8D); // Ativa Charge Pump
    ssd1306_send_command(0x14); // Habilita
    
    // escreve horizontalmente na ram do display
    ssd1306_send_command(0x20); // Define modo de endereçamento
    ssd1306_send_command(0x00); // Modo horizontal

    // inverte a ordem das colunas
    ssd1306_send_command(0xA1); // Segment Re-map (coluna 127 mapeada para SEG0)
    
    // inverte a ordem das linhas (espelha horizontalmente)
    ssd1306_send_command(0xC8); // COM Output Scan Direction (invertido)
    
    // define a configuração do hardware de linhas
    ssd1306_send_command(0xDA); // Set COM Pins Hardware Configuration
    ssd1306_send_command(0x12); // Configuração padrão
    
    // define o brilho dos pixels (127 sendo o meio termo)
    ssd1306_send_command(0x81); // Define contraste
    ssd1306_send_command(0x7F); // Valor de contraste (127)

    // controla tempo de carga do capacitor oled
    ssd1306_send_command(0xD9); // Define período de pré-carga
    ssd1306_send_command(0xF1); // Valor padrão

    // tensão usada quando pixel está desligado
    ssd1306_send_command(0xDB); // Define nível de deseleção VCOMH
    ssd1306_send_command(0x40); // Nível padrão

    // só mostra pixels se a ram estiver preenchida
    ssd1306_send_command(0xA4); // Define display como "seguindo o conteúdo da RAM"
    
    // branco sobre fundo preto 
    ssd1306_send_command(0xA6); // display em modo normal (não invertido)
    
    // finalmente liga o display depois da configuração
    ssd1306_send_command(0xAF); // display ON

    //zera o buffer que representa a tela inteira
    memset(display->buffer, 0, sizeof(display->buffer));

    // marca como inicializado
    display->initialized = true;
}

// envia o conteúdo do display->buffer inteiro pro display, página por página
// cada página == 128 bytes
void display_update(display *display) {
    // como cada page tem 128 colunas, e cada coluna é um inteiro de 8 bytes
    for (uint8_t page = 0; page < 8; page++) {

        ssd1306_send_command(0xB0 + page);
        ssd1306_send_command(0x00);
        ssd1306_send_command(0x10);

        uint8_t data[129];
        // o prefixo 0x40 indica ao diplay que vem dados, e não comandos
        data[0] = 0x40;

        // coloca os dados da page imediatamente depois do local onde ta armazenado o comando
        memcpy(&data[1], &display->buffer[page * DISPLAY_WIDTH], DISPLAY_WIDTH);

        // escrevendo no display
        i2c_write_blocking(I2C_PORT, 0x3C, data, sizeof(data), false);
    }
}

// limpa o buffer do display
void display_clear(display *display) {
    memset(display->buffer, 0, sizeof(display->buffer));
}

// desenha ou apaga um pixel no display
void display_draw_pixel(int x, int y, bool on, display *display) {
    // se o ponto estiver fora dos limites do display retorna
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) return; 
    // o calculo de x + (y / 8) * DISPLAY_WIDTH é feito para colocar no byte coreto
    // e o (1 << (y % 8)) coloca o bit correto, dentro do byte avaliado
    if (on)
        display->buffer[x + (y / 8) * DISPLAY_WIDTH] |= (1 << (y % 8));
    else
        display->buffer[x + (y / 8) * DISPLAY_WIDTH] &= ~(1 << (y % 8));
}

// algoritmo de Bresenham
void display_draw_line(int x0, int y0, int x1, int y1, bool on, display *display) {
    // calcula a distância horizontal entre os pontos
    // define o sentido do passo no eixo x (direita ou esquerda)
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;

    // calcula a distância vertical entre os pontos (negativa para facilitar o algoritmo)
    // define o sentido do passo no eixo y (cima ou baixo)
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;

    // erro inicial (soma das distâncias)
    int err = dx + dy, e2;

    while (1) {
        // desenha um pixel na posição atual
        display_draw_pixel(x0, y0, on, display);

        // se chegou ao ponto final, encerra o laço
        if (x0 == x1 && y0 == y1) break;

        // calcula o dobro do erro atual
        e2 = 2 * err;

        // se o erro dobrado for maior ou igual a dy, avança no eixo x
        if (e2 >= dy)  err += dy, x0 += sx;

        // se o erro dobrado for menor ou igual a dx, avança no eixo y
        if (e2 <= dx)  err += dx, y0 += sy;
    }
}

// escreve um caracter no buffer do display
void display_draw_char(int x, int y, char c, bool on, display *display) {
    if (c < 0x20 || c > 0x7F) return;

    int index = c - 0x20;

    for (int col = 0; col < 8; col++) {
        uint8_t line = FONTS[index][col];
        
        for (int row = 0; row < 8; row++) {
            if (line & (1 << row)) {
                display_draw_pixel(x + col, y + row, on, display);
            }
        }
    }
}

// escreve uma string no buffer do display
void display_draw_string(int x, int y, const char *str, bool on, display *display) {
    while (*str) {
        if (x + 8 > 128) break;

        display_draw_char(x, y, *str, on, display);
        x += 8;
        str++;
    }
}

void display_draw_rectangle(int x0, int y0, int x1, int y1, bool filled, bool on, display *display) {
    // ajusta os valores para caberem na tela de forma "circular"
    x0 = (x0 + DISPLAY_WIDTH) % DISPLAY_WIDTH;
    y0 = (y0 + DISPLAY_HEIGHT) % DISPLAY_HEIGHT;
    x1 = (x1 + DISPLAY_WIDTH) % DISPLAY_WIDTH;
    y1 = (y1 + DISPLAY_HEIGHT) % DISPLAY_HEIGHT;

    // se for um retângulo preenchido
    if (filled) {
        for (int y = y0; y != (y1 + 1) % DISPLAY_HEIGHT; y = (y + 1) % DISPLAY_HEIGHT) {
            for (int x = x0; x != (x1 + 1) % DISPLAY_WIDTH; x = (x + 1) % DISPLAY_WIDTH) {
                display_draw_pixel(x, y, on, display);
            }
        }
    } else {
        // desenha apenas as bordas do retângulo
        display_draw_line(x0, y0, x1, y0, on, display); // linha superior
        display_draw_line(x0, y1, x1, y1, on, display); // linha inferior
        display_draw_line(x0, y0, x0, y1, on, display); // linha esquerda
        display_draw_line(x1, y0, x1, y1, on, display); // linha direita
    }
}

// implementação do Midpoint Circle Algorithm
void display_draw_circle(int xc, int yc, int radius, bool filled, bool on, display *display) {
    int x = 0;
    int y = radius;
    int d = 1 - radius;

    while (y >= x) {
        if (filled) {
            for (int i = xc - x; i <= xc + x; i++) {
                display_draw_pixel(i, yc + y, on, display);
                display_draw_pixel(i, yc - y, on, display);
            }
            for (int i = xc - y; i <= xc + y; i++) {
                display_draw_pixel(i, yc + x, on, display);
                display_draw_pixel(i, yc - x, on, display);
            }
        } else {
            display_draw_pixel(xc + x, yc + y, on, display);
            display_draw_pixel(xc - x, yc + y, on, display);
            display_draw_pixel(xc + x, yc - y, on, display);
            display_draw_pixel(xc - x, yc - y, on, display);
            display_draw_pixel(xc + y, yc + x, on, display);
            display_draw_pixel(xc - y, yc + x, on, display);
            display_draw_pixel(xc + y, yc - x, on, display);
            display_draw_pixel(xc - y, yc - x, on, display);
        }

        x++;

        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
    }
}

void display_draw_bitmap(int x, int y, const uint8_t *bitmap, int w, int h, int rotation, bool on, display *display) {
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            int src_x = i, src_y = j;
            int dst_x = x, dst_y = y;

            switch (rotation) {
                case 1:
                    dst_x = x + j;
                    dst_y = y + (w - 1 - i);
                    break;
                case 2:
                    dst_x = x + (w - 1 - i);
                    dst_y = y + (h - 1 - j);
                    break;
                case 3:
                    dst_x = x + (h - 1 - j);
                    dst_y = y + i;
                    break;
                default: // 0 graus (normal)
                    dst_x = x + i;
                    dst_y = y + j;
                    break;
            }

            // garantir que os pixels não saiam da tela
            if (dst_x >= 0 && dst_x < DISPLAY_WIDTH && dst_y >= 0 && dst_y < DISPLAY_HEIGHT) {
                // obter o bit correto do bitmap original
                int byte_index = (src_y / 8) * w + src_x;
                int bit_index = src_y % 8;

                if (bitmap[byte_index] & (1 << bit_index)) {
                    display_draw_pixel(dst_x, dst_y, on, display);
                }
            }
        }
    }
}

// apaga e desliga o display
void display_shutdown(display *display) {

    display_clear(display);
    display_update(display);

    ssd1306_send_command(0xAE); // display OFF

    ssd1306_send_command(0x8D); // charge Pump
    ssd1306_send_command(0x10); // desativa

    i2c_deinit(I2C_PORT);
    gpio_set_function(SDA_PIN, GPIO_FUNC_NULL);
    gpio_set_function(SCL_PIN, GPIO_FUNC_NULL);

    display->initialized = false;
}