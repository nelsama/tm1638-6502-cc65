/**
 * @file tm1638.h
 * @brief Header file for TM1638 display and keyboard module interface.
 *
 * Interface para el control completo del módulo TM1638, incluyendo display de 7-segmentos
 * y teclado de 16 teclas. Compatible con CC65/C89.
 *
 * Funcionalidades:
 *   - Codificación de caracteres ASCII y hexadecimales
 *   - Control de display con common anode
 *   - Lectura de teclado con mapeo específico para QYF-TM1638
 *   - Funciones de utilidad para limpieza y control
 *
 * @author Nelson Figueroa
 * @date 2025
 */
#ifndef TM1638_H
#define TM1638_H

#include <stdint.h>

/* ============================================================================
 * DEFINICIONES Y CONSTANTES
 * ============================================================================ */

// Definiciones de puertos para TM1638 (solo si no están ya definidas)
#ifndef PORT_SALIDA
#define PORT_SALIDA         (*(volatile uint8_t*)0xC000)
#endif

#ifndef CONF_PORT_SALIDA
#define CONF_PORT_SALIDA    (*(volatile uint8_t*)0xC002)
#endif



/* Parámetro de timing para delays del TM1638.
 *
 * Controla la pausa entre flancos de CLK/DIO/STB. Con 0 la conmutación es muy
 * rápida (pocos ciclos de CPU), lo que puede inyectar ruido audible en sistemas
 * donde el puerto comparte pines con audio. El TM1638 tolera velocidades mucho
 * menores: subir este valor (por ejemplo 8-20) reduce la frecuencia de
 * conmutación y con ella el ruido.
 *
 * Se deja en 0 por defecto para no alterar el rendimiento de las aplicaciones
 * existentes. Cada proyecto puede sobrescribirlo sin tocar la librería:
 *   - En el Makefile:  CFLAGS += -Dtiming_delay=12
 *   - Antes del include:
 *         #define timing_delay 12
 *         #include "tm1638.h"
 *
 * Ver docs/PIN_ISOLATION_AND_NOISE.md para más detalle.
 */
#ifndef timing_delay
#define timing_delay 0
#endif

/* Bits del puerto que pertenecen al TM1638 (CLK, DIO, STB).
 *
 * En binario para que sea evidente qué pata ocupa cada bit, de acuerdo con el
 * mapeo por defecto:
 *
 *     bit 2 1 0
 *         0b0 0 0 0 0 1 1 1
 *           | | | | | | | +-- CLK
 *           | | | | | | +---- DIO
 *           | | | | | +------ STB
 *           | | | | +-------- libres (audio, LEDs, etc.)
 *
 * Uso con puertos compartidos:
 *     CONF_PORT_SALIDA &= ~TM1638_PINS_MASK;
 *
 * En lugar de escribir el registro completo, lo que afectaría a las demás patas.
 */
#define TM1638_PINS_MASK  0b00000111

/* TODO: auto-configurar CLK/STB en tm1638_init().
 *
 * Hoy la librería solo alterna el bit DIO en CONF_PORT_SALIDA (es bidireccional:
 * entrada para el teclado, salida para escribir), y deja que la aplicación
 * configure CLK y STB como salida. Si el usuario olvida esa línea, el display
 * no responde.
 *
 * Sería más robusto que tm1638_init() hiciera:
 *     CONF_PORT_SALIDA &= ~(TM_CLK_MASK | TM_STB_MASK);
 *
 * Es idempotente para las apps que ya configuran el puerto, pero cambia el
 * binario de la librería: una app que a propósito tuviera CLK/STB como entrada
 * se vería afectada. Pendiente de decidir si se asume ese riesgo.
 *
 * Ver docs/PIN_ISOLATION_AND_NOISE.md
 */

/* ============================================================================
 * FUNCIONES DE CODIFICACIÓN Y DISPLAY
 * ============================================================================ */

// Funciones básicas de comunicación
void tm1638_delay(volatile uint16_t delay_counter2);
void tm1638_init(void);

// Funciones de codificación (versión completa con longitud personalizable)
void tm1638_encode_hex(const uint8_t* digits, uint8_t* segments, uint8_t digits_len);
void tm1638_encode_ascii(const char* chars, uint8_t* segments, uint8_t digits_len);
void tm1638_encode_ascii_raw(const char* chars, uint8_t* segments, uint8_t digits_len);
void tm1638_digits_common_anode(const uint8_t* digits, uint8_t* grids, uint8_t digits_len);

// Funciones de codificación simplificadas (asumen 8 dígitos automáticamente)
void tm1638_encode_hex8(const uint8_t* digits, uint8_t* segments);
void tm1638_encode_ascii8(const char* chars, uint8_t* segments);
void tm1638_encode_ascii_raw8(const char* chars, uint8_t* segments);
void tm1638_number_to_segments8(uint32_t number, uint8_t* segments);
void tm1638_digits_common_anode8(const uint8_t* digits, uint8_t* grids);

// Funciones súper simplificadas (todo-en-uno)
void tm1638_show_text(const char* text);
void tm1638_show_text_raw(const char* text);
void tm1638_show_hex(const uint8_t* hex_digits);
void tm1638_show_number(uint32_t number);

// Funciones de display
void tm1638_display(const uint8_t* grids);
void tm1638_display_with_brightness(const uint8_t* grids, uint8_t brightness);
void tm1638_clear_display(void);
void tm1638_set_brightness(uint8_t brightness);

/* ============================================================================
 * FUNCIONES DE TECLADO
 * ============================================================================ */

// Funciones de lectura de teclas
uint32_t tm1638_read_keys(void);
uint8_t tm1638_get_key_pressed(void);
uint8_t tm1638_get_all_keys_pressed(uint8_t* pressed_keys);

// Funciones auxiliares (normalmente no necesarias para el usuario final)
uint8_t tm1638_read_byte(void);

#endif // TM1638_H