/**
 * @file basic_example.c
 * @brief Ejemplo básico de uso de la librería TM1638 v2.0
 * 
 * Este ejemplo demuestra el uso básico de la API simplificada de TM1638.
 * Perfecto para principiantes y prototipado rápido.
 */

#include "../include/tm1638.h"

/* Definiciones específicas del hardware - AJUSTAR SEGÚN TU PROYECTO */
#define CONF_PORT_SALIDA_LED    (*(volatile uint8_t*)0xC004)  /* Ejemplo: puerto LEDs */
#define PORT_SALIDA_LED         (*(volatile uint8_t*)0xC005)  /* Ejemplo: datos LEDs */

void main(void) {
    uint8_t key_pressed, last_key;
    uint16_t counter;
    
    /* ==================== CONFIGURACIÓN HARDWARE ==================== */
    /* NOTA: Ajustar estas líneas según tu mapa de puertos específico */
    CONF_PORT_SALIDA_LED = 0b11111111;    /* LEDs como salida (si tienes) */
    PORT_SALIDA_LED = 0b00000000;         /* LEDs apagados inicialmente */
    
    /* ==================== INICIALIZACIÓN SÚPER FÁCIL ==================== */
    tm1638_init();                        /* ¡Una línea para todo! */
    tm1638_set_brightness(4);             /* Brillo al 50% (0-7) */
    
    /* ==================== DEMO DE FUNCIONES ==================== */
    /* Mostrar mensaje de bienvenida */
    tm1638_show_text(" HOLA   ");         /* Texto automático */
    tm1638_delay(2000);                   /* Esperar 2 segundos */
    
    /* Mostrar número */
    tm1638_show_number(12345);            /* Número automático */
    tm1638_delay(2000);
    
    /* Limpiar display (mantiene brillo) */
    tm1638_clear_display();
    tm1638_delay(1000);
    
    /* ==================== CONTADOR INTERACTIVO ==================== */
    counter = 0;
    last_key = 0;
    
    while(1) {
        /* Leer teclado (1-16, 0=ninguna) */
        key_pressed = tm1638_get_key_pressed();
        
        if (key_pressed > 0 && key_pressed != last_key) {
            /* Tecla nueva presionada */
            switch(key_pressed) {
                case 1:  /* Tecla 1: Incrementar */
                    counter++;
                    break;
                case 2:  /* Tecla 2: Decrementar */
                    if (counter > 0) counter--;
                    break;
                case 3:  /* Tecla 3: Reset */
                    counter = 0;
                    break;
                case 4:  /* Tecla 4: Brillo + */
                    tm1638_set_brightness(6);
                    break;
                case 5:  /* Tecla 5: Brillo - */
                    tm1638_set_brightness(2);
                    break;
                default: /* Otras teclas: mostrar número de tecla */
                    counter = key_pressed;
                    break;
            }
            
            /* Mostrar valor actual */
            tm1638_show_number(counter);
            last_key = key_pressed;
            
        } else if (key_pressed == 0) {
            /* No hay tecla presionada */
            last_key = 0;
        }
        
        /* Pausa anti-rebote */
        tm1638_delay(100);
    }
}

/*
 * INSTRUCCIONES DE USO:
 * 
 * 1. Copiar este archivo a tu proyecto
 * 2. Ajustar las definiciones de puertos según tu hardware
 * 3. Agregar tm1638.c y tm1638.h a tu proyecto
 * 4. Compilar con CC65
 * 
 * CONTROLES:
 * - Tecla 1: Incrementar contador
 * - Tecla 2: Decrementar contador  
 * - Tecla 3: Reset contador
 * - Tecla 4: Brillo alto
 * - Tecla 5: Brillo bajo
 * - Otras: Mostrar número de tecla
 */