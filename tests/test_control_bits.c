/**
 * @file test_control_bits.c
 * @brief Test del control de los bits propios del driver.
 *
 * Verifica que el driver efectivamente controla CLK/DIO/STB (control negativo:
 * si esto fallara, los tests de aislamiento no probarían nada, porque un driver
 * que no escribe nada trivialmente "no molesta" a los demás bits).
 */

#include "../include/tm1638.h"

void tm1638_write_bit(uint8_t bit);
void tm1638_write_byte(uint8_t data);
void TM_DIO_CONFIG_OUTPUT(void);

#define PATRON_AJENO   0xA8
#define MASK_AJENOS    0xF8

int main(void) {
    /* El driver debe poder poner CLK a 1 */
    PORT_SALIDA = PATRON_AJENO;
    TM_DIO_CONFIG_OUTPUT();
    tm1638_write_bit(1);
    if ((PORT_SALIDA & 0x01) != 0x01) return 1;         /* CLK no subio */
    if ((PORT_SALIDA & MASK_AJENOS) != PATRON_AJENO) return 2;

    /* El driver debe poder poner CLK a 0 durante la fase baja del pulso.
       NOTA: tm1638_write_bit(bit) SIEMPRE termina con CLK en alto, porque el
       parametro 'bit' es el DATO para DIO, no el nivel de CLK: la funcion
       completa el pulso (CLK bajo -> CLK alto). Por eso aqui se comprueba el
       nivel de DIO (que refleja el dato) y que CLK quedo alto tras el pulso. */
    PORT_SALIDA = (uint8_t)(PATRON_AJENO | 0x01);
    tm1638_write_bit(0);
    if ((PORT_SALIDA & 0x01) != 0x01) return 3;         /* CLK alto tras el pulso */
    if ((PORT_SALIDA & 0x02) != 0x00) return 4;         /* DIO = 0 (dato enviado) */
    if ((PORT_SALIDA & MASK_AJENOS) != PATRON_AJENO) return 11;

    /* Tras write_byte, CLK queda alto (estado final del protocolo) */
    PORT_SALIDA = PATRON_AJENO;
    tm1638_write_byte(0x00);
    if ((PORT_SALIDA & 0x01) != 0x01) return 5;
    if ((PORT_SALIDA & MASK_AJENOS) != PATRON_AJENO) return 6;

    PORT_SALIDA = PATRON_AJENO;
    tm1638_write_byte(0xFF);
    if ((PORT_SALIDA & 0x01) != 0x01) return 7;
    if ((PORT_SALIDA & MASK_AJENOS) != PATRON_AJENO) return 8;

    /* El driver debe controlar STB a traves de las secuencias de display */
    {
        uint8_t g[8];
        uint8_t i;
        for (i = 0; i < 8; i++) g[i] = 0x00;
        PORT_SALIDA = PATRON_AJENO;
        tm1638_display(g);
        if ((PORT_SALIDA & 0x04) != 0x04) return 9;     /* STB debe acabar alto */
        if ((PORT_SALIDA & MASK_AJENOS) != PATRON_AJENO) return 10;
    }

    return 0;   /* todo correcto */
}
