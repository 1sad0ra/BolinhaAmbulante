# Controle de Matriz de LEDs com Joystick usando Raspberry Pi Pico

Este projeto implementa o controle de uma matriz de LEDs Neopixel utilizando um joystick analógico com a placa Raspberry Pi Pico.

## Hardware Utilizado
- Raspberry Pi Pico
- Joystick analógico (2 eixos)
- Matriz de LEDs Neopixel (5x5)
- Resistores e fiação necessária

## Descrição do Projeto
O código utiliza a biblioteca `pico/stdlib.h` para entrada e saída padrão, além das bibliotecas `hardware/adc.h` para leitura do joystick e `hardware/pio.h` para controle da matriz de LEDs. O joystick permite mover um cursor representado por um LED aceso na matriz Neopixel.

### Funcionamento
1. O joystick é conectado às entradas analógicas do Raspberry Pi Pico.
2. A matriz de LEDs é inicializada e controlada por um programa PIO.
3. O joystick controla a posição de um LED aceso na matriz.
4. O LED correspondente ao cursor é atualizado dinamicamente conforme a movimentação do joystick.

## Código Principal
O código é estruturado da seguinte maneira:
- **Inicialização:** Configura os periféricos do Raspberry Pi Pico.
- **Funções para controle dos LEDs:** Inicialização da matriz, atribuição de cores e atualização dos LEDs.
- **Loop principal:** Leitura da posição do joystick e atualização da matriz Neopixel.

```c
int main(){
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
    
    npInit(LED_PIN); // Inicializa a matriz de LEDs.
    npClear(); // Limpa os LEDs.

    while (1) {
        adc_select_input(0);
        uint adc_y_raw = adc_read();
        adc_select_input(1);
        uint adc_x_raw = adc_read();

        uint bar_x_pos = adc_x_raw * 5 / 4095;
        uint bar_y_pos = adc_y_raw * 5 / 4095;

        printf("\nX: %d, Y: %d", bar_x_pos, bar_y_pos);

        npSetLED(getIndex(bar_x_pos, bar_y_pos), 255,0,255);
        npWrite();
        sleep_ms(2);
        npClear();
    }
}
```

## Como Executar
1. Configure o ambiente de desenvolvimento para o Raspberry Pi Pico.
2. Compile e carregue o código na placa.
3. Conecte o joystick e a matriz de LEDs.
4. Observe o movimento do cursor na matriz de LEDs ao manipular o joystick.
<video src="BolinhaFuncionando.mp4" controls width="600"></video>
   
## Referências
- [Documentação Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/)

