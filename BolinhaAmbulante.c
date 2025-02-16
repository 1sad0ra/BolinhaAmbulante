#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h" // joystick
#include "hardware/pio.h" //neopixel
#include "hardware/clocks.h" //neopixel


// ***************************** neopixel ***************************** //

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

// Definição de pixel GRB
struct pixel_t {
 uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

/**
* Inicializa a máquina PIO para controle da matriz de LEDs.
*/
void npInit(uint pin) {

 // Cria programa PIO.
 uint offset = pio_add_program(pio0, &ws2818b_program);
 np_pio = pio0;

 // Toma posse de uma máquina PIO.
 sm = pio_claim_unused_sm(np_pio, false);
 if (sm < 0) {
   np_pio = pio1;
   sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
 }

 // Inicia programa na máquina PIO obtida.
 ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

 // Limpa buffer de pixels.
 for (uint i = 0; i < LED_COUNT; ++i) {
   leds[i].R = 0;
   leds[i].G = 0;
   leds[i].B = 0;
 }
}

/**
* Atribui uma cor RGB a um LED.
*/
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
 leds[index].R = r;
 leds[index].G = g;
 leds[index].B = b;
}

/**
* Limpa o buffer de pixels.
*/
void npClear() {
 for (uint i = 0; i < LED_COUNT; ++i)
   npSetLED(i, 0, 0, 0);
}

/**
* Escreve os dados do buffer nos LEDs.
*/
void npWrite() {
 // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
 for (uint i = 0; i < LED_COUNT; ++i) {
   pio_sm_put_blocking(np_pio, sm, leds[i].G);
   pio_sm_put_blocking(np_pio, sm, leds[i].R);
   pio_sm_put_blocking(np_pio, sm, leds[i].B);
 }
}

/**
 * Converte as coordenadas (x, y) na matriz 5x5 para o índice da fila linear.
 * 
 * @param x A coluna (0 a 4).
 * @param y A linha (0 a 4).
 * @return O índice correspondente na fila (0 a 24).
 */
int getIndex(int x, int y) {
    // Se a linha for par (1, 3), percorremos da direita para a esquerda.
    // Se a linha for ímpar (0, 2, 4), percorremos da esquerda para a direita.
    if (y % 2 == 0) {
        return y * 5 + (4 - x); // Linha par (direita para esquerda).
    } else {
        return y * 5 + x; // Linha ímpar (esquerda para direita).
    }
}


// ***************************** Fim ***************************** //

int main(){
    // Inicializa entradas e saídas.
    stdio_init_all();

    /*joystick*/
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);

    /*neopixel*/
    npInit(LED_PIN); // Inicializa a matriz de LEDs.
    npClear(); // Limpa os LEDs.

    // Definir variáveis antes do loop principal
    const uint bar_width = 5; // largura da barra
    const uint adc_max = (1 << 12) - 1; // o valor máximo que o ADC pode retornar
    uint bar_x_pos, bar_y_pos;

    while (1) {
        /*joystick*/
        adc_select_input(0); // eixo Y do joystick
        uint adc_y_raw = adc_read();
        adc_select_input(1); // eixo X do joystick
        uint adc_x_raw = adc_read();

        // Calcula as posições de X e Y
        bar_x_pos = adc_x_raw * bar_width / adc_max; // calcula a posição x do "cursor"
        bar_y_pos = adc_y_raw * bar_width / adc_max; // calcula a posição y do "cursor"

        // Exibe a posição do joystick no terminal
        printf("\nX: %d, Y: %d", bar_x_pos, bar_y_pos);

        // Desenhar LEDs

        npSetLED(getIndex(bar_x_pos, bar_y_pos), 255,0,255); // Define o LED de índice (x,y) para Fuchsia / Magenta.
        npWrite(); // Escreve os dados nos LEDs.

        sleep_ms(2); // Faz uma pequena pausa
        npClear(); // Apaga todos os LEDs da matriz.

    }
}