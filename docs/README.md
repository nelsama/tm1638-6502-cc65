# Librería TM1638 v2.0 🚀

Librería profesional y completa para módulos TM1638 con características avanzadas, optimizada para CC65/C89 y sistemas 6502.

## ✨ Características Avanzadas v2.0

- ✅ **Anti-Ghosting**: Eliminación completa de segmentos fantasma
- ✅ **Control de Brillo Inteligente**: Gestión automática y persistente del brillo
- ✅ **API Simplificada**: Funciones todo-en-uno para uso inmediato
- ✅ **Mapeo QYF-TM1638**: Soporte completo y probado para hardware QYF-TM1638
- ✅ **Aislamiento de pines**: Solo controla los bits CLK/DIO/STB, sin afectar otras señales del puerto
- ✅ **Modular**: Funciones separadas para máxima flexibilidad
- ✅ **Robusta**: Inicialización resistente a estados residuales
- ✅ **Probada**: Banco de pruebas automático bajo simulador sim65

## 📁 Archivos

- **`tm1638.c`** - Implementación principal (~700 líneas, completamente optimizada)
- **`MANUAL_TM1638.md`** - Manual completo con ejemplos y referencia completa
- **`ANTI_GHOSTING_NOTES.md`** - Documentación técnica anti-ghosting
- **`PIN_ISOLATION_AND_NOISE.md`** - Aislamiento de pines y reducción de ruido en audio
- **`tm1638_working.backup`** - Backup de versión anterior (histórico)

## 🚀 Uso Ultra-Rápido (API v2.0)

```c
#include "../../include/tm1638.h"

void main(void) {
    /* ========== CONFIGURACIÓN (3 LÍNEAS) ========== */
    CONF_PORT_SALIDA = 0b00000000;        // TM1638 como salidas (puerto en exclusiva)
    tm1638_init();                        // Inicialización robusta anti-ghosting
    tm1638_set_brightness(4);             // Brillo medio persistente
    
    /* ========== USO INMEDIATO ========== */
    tm1638_show_text(" HOLA   ");         // Mostrar texto (súper simple!)
    tm1638_delay(2000);
    tm1638_show_number(12345);            // Mostrar número (auto-alineado)
    
    /* ========== TECLADO ========== */
    while(1) {
        uint8_t tecla = tm1638_get_key_pressed();  // QYF-TM1638 (1-16)
        if (tecla > 0) {
            tm1638_show_number(tecla);    // Mostrar tecla presionada
        }
        tm1638_delay(100);               // Anti-rebote automático
    }
}
```

### Funciones Todo-en-Uno Más Usadas:
```c
tm1638_init();                         // Inicialización (OBLIGATORIO)
tm1638_show_text("MENSAJE ");          // Texto ASCII (8 chars)
tm1638_show_number(12345678);          // Número entero
tm1638_show_hex(hex_array);            // Hexadecimal
tm1638_clear_display();                // Limpiar display
tm1638_set_brightness(0-7);            // Brillo (0=mín, 7=máx)
tm1638_display_off();                  // Apagar pantalla (conserva datos)
tm1638_display_on();                   // Encender con brillo actual
uint8_t key = tm1638_get_key_pressed(); // Teclado QYF-TM1638
```

### Configuración del puerto según tu hardware

La línea `CONF_PORT_SALIDA` que configura CLK/STB como salida admite dos formas:

```c
/* Si el puerto 0xC000 es exclusivo del TM1638 */
CONF_PORT_SALIDA = 0b00000000;

/* Si el puerto se comparte con audio, LEDs u otras señales */
CONF_PORT_SALIDA &= ~TM1638_PINS_MASK;   /* solo los bits 0-2 como salida */
```

El driver solo modifica los bits 0-2 de `PORT_SALIDA`, así que compartir ese byte es seguro. Ver [`MANUAL_TM1638.md`](MANUAL_TM1638.md) para el detalle completo.

## 📚 Documentación

Para documentación completa, ejemplos avanzados y referencia de todas las funciones, consulta:

**[MANUAL_TM1638.md](MANUAL_TM1638.md)**

## 📊 Comparación v1.0 vs v2.0

| Característica | v1.0 | v2.0 |
|----------------|------|------|
| **API** | Manual completa | ✅ Todo-en-uno + Manual |
| **Anti-Ghosting** | Básico | ✅ Completa eliminación |
| **Control Brillo** | Básico | ✅ Inteligente y persistente |
| **Inicialización** | Manual | ✅ Robusta automática |
| **QYF-TM1638** | Genérico | ✅ Soporte específico probado |
| **Modularidad** | Limitada | ✅ Completamente modular |
| **Líneas de código** | ~500 | ~700 optimizadas |

## 🎯 Casos de Uso

- **✅ Principiantes**: API simplificada, listo en 3 líneas
- **✅ Expertos**: API modular completa para control total  
- **✅ QYF-TM1638**: Soporte específico y probado
- **✅ Proyectos 6502**: Optimizado para arquitectura 6502
- **✅ FPGA**: Perfecto para implementaciones FPGA

---

Parte del proyecto **Micro6502** - Sistema 6502 en FPGA