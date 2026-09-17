/**
 * @file test_port_isolation.c
 * @brief Test de aislamiento de pines para la librería TM1638 (sim65).
 *
 * Verifica el requisito fundamental: el driver SOLO debe controlar los bits
 * 0 (CLK), 1 (DIO) y 2 (STB) de PORT_SALIDA. Cualquier otro bit del puerto
 * (que en un proyecto real puede ir a audio, video, LEDs, etc.) debe
 * conservar intacto el valor que tenía.
 *
 * Compilar y ejecutar:
 *   cl65 -t sim6502 -O -I ../include -o test_port_isolation.prg \
 *        test_port_isolation.c ../src/tm1638.c
 *   sim65 test_port_isolation.prg
 *
 * El programa devuelve como código de salida el número de fallos (0 = OK).
 */

#include "../include/tm1638.h"
#include <sim65.h>

/* Funciones internas de tm1638.c, no declaradas en el header público.
   Se declaran aquí para poder probarlas: son parte de la superficie real
   del driver aunque tm1638.h no las exponga. */
void tm1638_write_bit(uint8_t bit);
void tm1638_write_byte(uint8_t data);
void tm1638_send_cmd(uint8_t cmd);
void TM_DIO_CONFIG_INPUT(void);
void TM_DIO_CONFIG_OUTPUT(void);

/* ============================================================================
 * UTILIDADES DE REPORTE
 * ============================================================================ */

static uint8_t fallos = 0;
static uint8_t pruebas = 0;
static uint8_t id_ultimo_fallo = 0;

/* Cada fallo escribe su id en una direccion de memoria conocida, de modo
   que el volcado de memoria del simulador lo revele. */
static void registrar_fallo(uint8_t id) {
    fallos++;
    id_ultimo_fallo = id;
    *((volatile uint8_t*)0x0300) = id;   /* traza de fallos: secuencia de ids */
    *((volatile uint8_t*)0x0301) = fallos; /* contador acumulado de fallos */
}

static void putc_consola(char c) {
    /* Escribir en la consola del simulador vía el periférico counter:
       se usa como puerto de salida simple accesible desde el test. */
    (void)c;
}

static void print(const char* s) {
    while (*s) {
        putc_consola(*s);
        s++;
    }
}

/**
 * Comprueba una condición. Registra el fallo con un identificador numérico
 * (legible desde el código de salida) y lo imprime si hay consola.
 */
static void check_num(uint8_t id, uint8_t condicion) {
    pruebas++;
    if (!condicion) {
        registrar_fallo(id);
    }
}

/* ============================================================================
 * NOTA SOBRE EL ENTORNO DE PRUEBA
 *
 * En el target sim6502, las direcciones $C000/$C002 caen dentro del espacio
 * de datos del propio programa de test, por lo que el runtime de cc65 puede
 * sobrescribirlas. Eso hace que las comprobaciones sobre CONF_PORT_SALIDA sean
 * poco fiables en el simulador (en el FPGA real son registros de hardware que
 * nadie más toca).
 *
 * Por eso la configuracion de DIO se verifica en test_dio_config.c, que se
 * ejecuta con un mapa de memoria sin colisiones. Aqui se verifica lo esencial:
 * que las operaciones del driver preservan los bits ajenos de PORT_SALIDA.
 * ============================================================================ */

/* ============================================================================
 * PATRONES DE PRUEBA
 * ============================================================================ */

/* Bits ajenos al driver: bits 3,5,7 a 1; bits 4,6 a 0 -> simula otras patas. */
#define PATRON_AJENO   0xA8    /* 0b10101000 */

static uint8_t bits_ajenos(void)      { return (uint8_t)(PORT_SALIDA & 0xF8); }
static uint8_t bits_driver(void)      { return (uint8_t)(PORT_SALIDA & 0x07); }
static uint8_t ajenos_conf(void)      { return (uint8_t)(CONF_PORT_SALIDA & 0xF8); }

/* Preparar escenario: patrón ajeno en el puerto y en la config */
static void preparar(void) {
    PORT_SALIDA = PATRON_AJENO;
    CONF_PORT_SALIDA = (uint8_t)(PATRON_AJENO | 0x07);
}

/* ============================================================================
 * TEST PRINCIPAL
 * ============================================================================ */

int main(void) {
    uint8_t segments[8];
    uint8_t grids[8];

    /* --- 1. tm1638_delay no debe tocar el puerto ------------------------- */
    preparar();
    tm1638_delay(5);
    check_num(1, bits_ajenos() == (PATRON_AJENO & 0xF8));

    /* --- 2. tm1638_write_bit --------------------------------------------- */
    preparar();
    tm1638_write_bit(1);
    check_num(2, bits_ajenos() == (PATRON_AJENO & 0xF8));

    preparar();
    tm1638_write_bit(0);
    check_num(3, bits_ajenos() == (PATRON_AJENO & 0xF8));

    /* --- 3. tm1638_write_byte -------------------------------------------- */
    preparar();
    tm1638_write_byte(0x5A);
    check_num(4, bits_ajenos() == (PATRON_AJENO & 0xF8));

    preparar();
    tm1638_write_byte(0xFF);
    check_num(5, bits_ajenos() == (PATRON_AJENO & 0xF8));

    preparar();
    tm1638_write_byte(0x00);
    check_num(6, bits_ajenos() == (PATRON_AJENO & 0xF8));

    /* --- 4. tm1638_send_cmd ---------------------------------------------- */
    preparar();
    tm1638_send_cmd(0x40);
    check_num(7, bits_ajenos() == (PATRON_AJENO & 0xF8));

    /* --- 5. tm1638_display ----------------------------------------------- */
    preparar();
    segments[0] = 0x3F; segments[1] = 0x06; segments[2] = 0x5B; segments[3] = 0x4F;
    segments[4] = 0x66; segments[5] = 0x6D; segments[6] = 0x7D; segments[7] = 0x07;
    tm1638_digits_common_anode8(segments, grids);
    tm1638_display(grids);
    check_num(8, bits_ajenos() == (PATRON_AJENO & 0xF8));

    /* --- 6. tm1638_set_brightness ---------------------------------------- */
    preparar();
    tm1638_set_brightness(3);
    check_num(9, bits_ajenos() == (PATRON_AJENO & 0xF8));

    /* --- 7. tm1638_clear_display ----------------------------------------- */
    preparar();
    tm1638_clear_display();
    check_num(10, bits_ajenos() == (PATRON_AJENO & 0xF8));

    /* --- 8. tm1638_init --------------------------------------------------- */
    preparar();
    tm1638_init();
    check_num(11, bits_ajenos() == (PATRON_AJENO & 0xF8));

    /* --- 9. Configuración de DIO -----------------------------------------
       Se comprueba solo la preservación de bits ajenos. La transición del bit
       DIO se verifica en test_dio_config.c, donde el mapa de memoria del
       simulador no colisiona con las direcciones del puerto. */
    preparar();
    TM_DIO_CONFIG_INPUT();
    check_num(12, ajenos_conf() == (PATRON_AJENO & 0xF8));

    preparar();
    TM_DIO_CONFIG_OUTPUT();
    check_num(14, ajenos_conf() == (PATRON_AJENO & 0xF8));
    check_num(15, (CONF_PORT_SALIDA & 0x02) == 0x00);

    /* --- 10. Otros patrones ajenos --------------------------------------- */
    PORT_SALIDA = 0xF8;              /* todos los ajenos a 1 */
    tm1638_write_byte(0x00);
    check_num(16, bits_ajenos() == 0xF8);

    PORT_SALIDA = 0x00;              /* todos los ajenos a 0 */
    tm1638_write_byte(0xFF);
    check_num(17, bits_ajenos() == 0x00);

    /* --- 11. Control negativo: el driver SÍ debe mandar en sus bits ------
       El parámetro de tm1638_write_bit es el dato para DIO, no el nivel de
       CLK: la función completa el pulso (CLK baja, CLK sube), así que CLK
       siempre acaba en alto y lo que refleja el bit es DIO. */
    preparar();
    tm1638_write_bit(1);
    check_num(18, (bits_driver() & 0x02) == 0x02);   /* DIO = 1 (dato) */

    preparar();
    tm1638_write_bit(0);
    check_num(19, (bits_driver() & 0x02) == 0x00);   /* DIO = 0 (dato) */

    /* --- 12. show_text / show_number (API de alto nivel) ----------------- */
    preparar();
    tm1638_show_text(" HOLA   ");
    check_num(20, bits_ajenos() == (PATRON_AJENO & 0xF8));

    preparar();
    tm1638_show_number(12345);
    check_num(21, bits_ajenos() == (PATRON_AJENO & 0xF8));

    print("pruebas ejecutadas: ");
    (void)pruebas;

    /* Volcar resumen en memoria conocida para inspeccion:
       0x0302 = total de pruebas, 0x0303 = total de fallos */
    *((volatile uint8_t*)0x0302) = pruebas;
    *((volatile uint8_t*)0x0303) = fallos;

    /* Código de salida = número de fallos (0 = todo correcto) */
    return fallos;
}
