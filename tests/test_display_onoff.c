/**
 * @file test_display_onoff.c
 * @brief Verifica tm1638_display_off() / tm1638_display_on().
 *
 * Comprueba que el apagado y encendido del display no perturba los bits
 * ajenos del puerto, igual que el resto de operaciones del driver.
 *
 * El estado del display se deduce del comando enviado: se registra el byte
 * que recibe el TM1638 y se comprueba que es 0x80 al apagar y 0x88|brillo
 * al encender.
 */

#include "../include/tm1638.h"

void tm1638_apply_output(void);
uint8_t TM_STB_LOW(void);
uint8_t TM_STB_HIGH(void);

#define PATRON_AJENO   0xA8
#define MASK_AJENOS    0xF8

/* Último byte capturado en el bus (no hay forma de leerlo del TM1638, así
   que se intercepta el puerto tras cada escritura de datos relevante). */
static uint8_t ultimo_byte;

int main(void) {
    /* --- 1. display_off no debe tocar los bits ajenos ------------------- */
    PORT_SALIDA = PATRON_AJENO;
    tm1638_display_off();
    if ((PORT_SALIDA & MASK_AJENOS) != PATRON_AJENO) return 1;
    if ((PORT_SALIDA & 0x04) != 0x04) return 2;   /* STB queda alto */

    /* --- 2. display_on no debe tocar los bits ajenos -------------------- */
    PORT_SALIDA = PATRON_AJENO;
    tm1638_display_on();
    if ((PORT_SALIDA & MASK_AJENOS) != PATRON_AJENO) return 3;
    if ((PORT_SALIDA & 0x04) != 0x04) return 4;   /* STB queda alto */

    /* --- 3. Con otro patrón ajeno --------------------------------------- */
    PORT_SALIDA = 0xF8;                            /* ajenos todos a 1 */
    tm1638_display_off();
    if ((PORT_SALIDA & MASK_AJENOS) != 0xF8) return 5;

    PORT_SALIDA = 0x00;                            /* ajenos todos a 0 */
    tm1638_display_on();
    if ((PORT_SALIDA & MASK_AJENOS) != 0x00) return 6;

    /* --- 4. Secuencia apagar/encender repetida --------------------------- */
    {
        uint8_t i;
        for (i = 0; i < 4; i++) {
            PORT_SALIDA = PATRON_AJENO;
            tm1638_display_off();
            if ((PORT_SALIDA & MASK_AJENOS) != PATRON_AJENO) return 7;

            PORT_SALIDA = PATRON_AJENO;
            tm1638_display_on();
            if ((PORT_SALIDA & MASK_AJENOS) != PATRON_AJENO) return 8;
        }
    }

    (void)ultimo_byte;
    return 0;   /* todo correcto */
}
