/**
 * @file test_dio_config.c
 * @brief Test aislado de TM_DIO_CONFIG_INPUT / TM_DIO_CONFIG_OUTPUT.
 *
 * Comprueba de forma independiente, sin interferencia de otros tests, que
 * la configuración de DIO solo modifica el bit 1 y respeta los bits ajenos.
 */

#include "../include/tm1638.h"

void TM_DIO_CONFIG_INPUT(void);
void TM_DIO_CONFIG_OUTPUT(void);

int main(void) {
    /* Caso A: arrancar con DIO a 0 para poder observar la transicion a 1.
       Patron ajeno 0xA8, pero con el bit 1 limpio -> 0xA8 ya tiene bit1=0. */
    CONF_PORT_SALIDA = 0xA8;   /* 0b10101000 : bit DIO = 0 (salida) */
    TM_DIO_CONFIG_INPUT();
    if ((CONF_PORT_SALIDA & 0x02) != 0x02) return 1;   /* DIO debe ser entrada */
    if ((CONF_PORT_SALIDA & 0xF8) != 0xA8) return 2;   /* bits ajenos intactos */

    /* Caso B: volver a salida */
    TM_DIO_CONFIG_OUTPUT();
    if ((CONF_PORT_SALIDA & 0x02) != 0x00) return 3;   /* DIO debe ser salida */
    if ((CONF_PORT_SALIDA & 0xF8) != 0xA8) return 4;   /* bits ajenos intactos */

    /* Caso C: desde un patron con bit DIO ya a 1 */
    CONF_PORT_SALIDA = 0xFA;   /* 0b11111010 : bit DIO = 1 */
    TM_DIO_CONFIG_INPUT();
    if ((CONF_PORT_SALIDA & 0x02) != 0x02) return 5;
    if ((CONF_PORT_SALIDA & 0xF8) != 0xF8) return 6;

    TM_DIO_CONFIG_OUTPUT();
    if ((CONF_PORT_SALIDA & 0x02) != 0x00) return 7;
    if ((CONF_PORT_SALIDA & 0xF8) != 0xF8) return 8;

    /* Caso D: todos los bits ajenos a 0 */
    CONF_PORT_SALIDA = 0x00;
    TM_DIO_CONFIG_INPUT();
    if (CONF_PORT_SALIDA != 0x02) return 9;

    TM_DIO_CONFIG_OUTPUT();
    if (CONF_PORT_SALIDA != 0x00) return 10;

    return 0;   /* todo correcto */
}
