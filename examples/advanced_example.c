/**
 * @file advanced_example.c
 * @brief Ejemplo avanzado usando la API modular de TM1638 v2.0
 * 
 * Este ejemplo demuestra el uso de la API modular para control completo.
 * Ideal para usuarios avanzados que necesitan máxima flexibilidad.
 */

#include "../include/tm1638.h"

/* Definiciones específicas del hardware - AJUSTAR SEGÚN TU PROYECTO */
#define CONF_PORT_SALIDA_LED    (*(volatile uint8_t*)0xC004)  /* Ejemplo: puerto LEDs */
#define PORT_SALIDA_LED         (*(volatile uint8_t*)0xC005)  /* Ejemplo: datos LEDs */

void demo_api_modular(void);
void demo_custom_characters(void);
void demo_multiple_keys(void);

void main(void) {
    /* ==================== CONFIGURACIÓN HARDWARE ==================== */
    CONF_PORT_SALIDA_LED = 0b11111111;    /* LEDs como salida */
    PORT_SALIDA_LED = 0b00000000;         /* LEDs apagados */
    
    /* ==================== INICIALIZACIÓN ==================== */
    tm1638_init();
    tm1638_set_brightness(5);             /* Brillo 62.5% */
    
    /* ==================== DEMOS AVANZADOS ==================== */
    tm1638_show_text(" ADV.API");
    tm1638_delay(2000);
    
    demo_api_modular();
    demo_custom_characters();
    demo_multiple_keys();
    
    /* Bucle infinito */
    while(1) {
        tm1638_show_text(" FINISH ");
        tm1638_delay(1000);
        tm1638_clear_display();
        tm1638_delay(1000);
    }
}

/**
 * Demuestra el uso de la API modular para control granular
 */
void demo_api_modular(void) {
    uint8_t segments[8];
    uint8_t grids[8];
    uint16_t numbers[] = {1234, 5678, 9012, 3456};
    uint8_t i;
    
    tm1638_show_text("MODULAR ");
    tm1638_delay(1500);
    
    /* Mostrar números usando API modular */
    for (i = 0; i < 4; i++) {
        /* Paso 1: Convertir número a segmentos */
        tm1638_number_to_segments8(numbers[i], segments);
        
        /* Paso 2: Convertir segmentos a grids (common anode) */
        tm1638_digits_common_anode8(segments, grids);
        
        /* Paso 3: Mostrar en display (respeta brillo actual) */
        tm1638_display(grids);
        
        tm1638_delay(1000);
    }
}

/**
 * Demuestra la creación de caracteres personalizados
 */
void demo_custom_characters(void) {
    uint8_t segments[8];
    uint8_t grids[8];
    uint8_t i;
    
    tm1638_show_text(" CUSTOM ");
    tm1638_delay(1500);
    
    /* Crear patrón personalizado (ejemplo: barras animadas) */
    uint8_t patterns[] = {
        0b00000001,  /* Segmento A */
        0b00000010,  /* Segmento B */
        0b00000100,  /* Segmento C */
        0b00001000,  /* Segmento D */
        0b00010000,  /* Segmento E */
        0b00100000,  /* Segmento F */
        0b01000000,  /* Segmento G */
        0b00000000   /* Apagado */
    };
    
    /* Animar patrón en todos los dígitos */
    for (i = 0; i < 8; i++) {
        /* Llenar todos los segmentos con el mismo patrón */
        segments[0] = segments[1] = segments[2] = segments[3] = 
        segments[4] = segments[5] = segments[6] = segments[7] = patterns[i];
        
        /* Convertir y mostrar */
        tm1638_digits_common_anode8(segments, grids);
        tm1638_display(grids);
        tm1638_delay(200);
    }
}

/**
 * Demuestra la lectura de múltiples teclas simultáneas
 */
void demo_multiple_keys(void) {
    uint8_t keys[16];
    uint8_t num_keys, i;
    uint16_t key_combination;
    
    tm1638_show_text("KEYS.ALL");
    tm1638_delay(1500);
    tm1638_clear_display();
    
    /* Esperar a que se presionen teclas */
    while(1) {
        num_keys = tm1638_get_all_keys_pressed(keys);
        
        if (num_keys > 0) {
            /* Calcular combinación de teclas */
            key_combination = 0;
            for (i = 0; i < num_keys; i++) {
                key_combination += keys[i];
            }
            
            /* Mostrar suma de teclas presionadas */
            tm1638_show_number(key_combination);
            
            /* Si se presionan más de 3 teclas, salir del demo */
            if (num_keys > 3) {
                tm1638_delay(2000);
                break;
            }
        } else {
            /* No hay teclas, mostrar número de teclas disponibles */
            tm1638_show_number(16);
        }
        
        tm1638_delay(100);
    }
}

/*
 * CARACTERÍSTICAS DEMOSTRADAS:
 * 
 * 1. API MODULAR:
 *    - tm1638_number_to_segments8(): Conversión separada
 *    - tm1638_digits_common_anode8(): Procesamiento separado
 *    - tm1638_display(): Visualización separada
 * 
 * 2. CARACTERES PERSONALIZADOS:
 *    - Control directo de segmentos individuales
 *    - Animaciones personalizadas
 *    - Patrones arbitrarios
 * 
 * 3. TECLADO AVANZADO:
 *    - Lectura de múltiples teclas simultáneas
 *    - Procesamiento de combinaciones
 *    - Detección de patrones complejos
 * 
 * VENTAJAS DE LA API MODULAR:
 * - Control granular de cada paso
 * - Reutilización de conversiones
 * - Optimización de rendimiento
 * - Flexibilidad máxima para casos especiales
 */