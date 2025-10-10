# Manual Completo de la Librería TM1638

## Descripción General

Librería profesional TM1638 para sistemas 6502/CC65 con características avanzadas y API simplificada. Completamente optimizada para el hardware QYF-TM1638.

### ✨ Características Avanzadas (v2.0):
- ✅ **Anti-Ghosting**: Eliminación completa de segmentos fantasma
- ✅ **Control de Brillo Inteligente**: Gestión automática y persistente
- ✅ **API Simplificada**: Funciones todo-en-uno para uso inmediato
- ✅ **Mapeo QYF-TM1638**: Soporte completo y probado para QYF-TM1638
- ✅ **Modular**: Funciones separadas para máxima flexibilidad
- ✅ **Robusta**: Inicialización y limpieza resistente a estados residuales
- ✅ **Completa**: Display, teclado, LEDs, y control de brillo
- ✅ **Compatible**: CC65/C89 estándar, optimizada para 6502

---

## � Inicio Rápido (API Simplificada)

### Configuración Básica (5 líneas de código):
```c
#include "../include/tm1638.h"

void main(void) {
    /* Configuración mínima */
    CONF_PORT_SALIDA = 0b00000000;        // TM1638 como salidas
    
    /* Inicialización automática anti-ghosting */
    tm1638_init();                        // ¡Ya está listo para usar!
    tm1638_set_brightness(4);             // Brillo medio opcional
    
    /* Uso inmediato */
    tm1638_show_text(" HOLA   ");         // Mostrar texto
    tm1638_delay(2000);
    tm1638_show_number(12345);            // Mostrar número
    
    /* Leer teclas */
    while(1) {
        uint8_t tecla = tm1638_get_key_pressed();
        if (tecla > 0) {
            tm1638_show_number(tecla);    // Mostrar número de tecla
        }
        tm1638_delay(100);               // Anti-rebote
    }
}
```

### Funciones Todo-en-Uno (Más Usadas):
```c
// ==================== INICIALIZACIÓN ====================
tm1638_init();                         // Inicialización robusta (OBLIGATORIO)
tm1638_set_brightness(0-7);            // Brillo persistente (0=mín, 7=máx)

// ==================== DISPLAY SIMPLIFICADO ====================
tm1638_show_text("MENSAJE ");          // Texto ASCII (8 chars, auto-limpia)
tm1638_show_number(12345678);          // Número entero (auto-alineado derecha)
tm1638_show_hex(hex_array);            // Hexadecimal (8 dígitos)
tm1638_clear_display();                // Limpiar (mantiene brillo)

// ==================== TECLADO QYF-TM1638 ====================
uint8_t tecla = tm1638_get_key_pressed();     // Una tecla (1-16, 0=ninguna)
uint8_t n = tm1638_get_all_keys_pressed(arr); // Múltiples teclas simultáneas
```

---

## 🔧 Configuración Avanzada

### Inclusión de la librería:
```c
#include "../include/tm1638.h"
```

### Definiciones de hardware requeridas:
```c
#define PORT_SALIDA         (*(volatile uint8_t*)0xC000)
#define PORT_SALIDA_LED     (*(volatile uint8_t*)0xC001)
#define CONF_PORT_SALIDA    (*(volatile uint8_t*)0xC002)
#define CONF_PORT_SALIDA_LED (*(volatile uint8_t*)0xC003)
```

### Configuración inicial en main():
```c
void main(void) {
    /* Configuración de puertos */
    CONF_PORT_SALIDA_LED = 0b11111111;    
    PORT_SALIDA_LED = 0b00000000;            
    CONF_PORT_SALIDA = 0b00000000;        /* TM1638 como salidas */

    /* Configurar brillo inicial (opcional) */
    tm1638_set_brightness(4);             /* Brillo medio */

    /* Tu código aquí... */
}
```

---

## 📺 Funciones de Display

### 1. `tm1638_encode_ascii()`
**Propósito**: Convierte texto ASCII a formato 7-segmentos.

```c
void tm1638_encode_ascii(const char* chars, uint8_t* segments, uint8_t digits_len);
```

**Parámetros**:
- `chars`: String ASCII de entrada
- `segments`: Array de salida con códigos 7-segmentos
- `digits_len`: Número de caracteres a codificar

**Ejemplo**:
```c
uint8_t segments[8];
tm1638_encode_ascii("HOLA    ", segments, 8);
```

**Caracteres soportados**: Espacios (32) hasta ~ (126), incluyendo:
- Números: `0-9`
- Letras: `A-Z`, `a-z` (algunas con limitaciones del display)
- Símbolos: `- + . , : ; ? ! = < > [ ] ( ) _ / \ @ # $ % & * '`

---

### 2. `tm1638_encode_hex()`
**Propósito**: Convierte dígitos hexadecimales a formato 7-segmentos.

```c
void tm1638_encode_hex(const uint8_t* digits, uint8_t* segments, uint8_t digits_len);
```

**Parámetros**:
- `digits`: Array con valores hex (0x0-0xF)
- `segments`: Array de salida con códigos 7-segmentos
- `digits_len`: Número de dígitos a codificar

**Ejemplo**:
```c
uint8_t hex_digits[] = {0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x0, 0x1};
uint8_t segments[8];
tm1638_encode_hex(hex_digits, segments, 8);
```

---

### 3. `tm1638_digits_common_anode()`
**Propósito**: Convierte datos de segmentos a formato grid para common anode.

```c
void tm1638_digits_common_anode(const uint8_t* digits, uint8_t* grids, uint8_t digits_len);
```

**Parámetros**:
- `digits`: Array con datos de segmentos
- `grids`: Array de salida con datos de grid
- `digits_len`: Número de dígitos

**Ejemplo**:
```c
uint8_t segments[8];
uint8_t grids[8];
tm1638_encode_ascii("12345678", segments, 8);
tm1638_digits_common_anode(segments, grids, 8);
```

---

### 4. `tm1638_display()`
**Propósito**: Muestra datos en el display con brillo por defecto.

```c
void tm1638_display(const uint8_t* grids);
```

**Parámetros**:
- `grids`: Array de 8 bytes con datos de grid

**Ejemplo completo**:
```c
uint8_t segments[8];
uint8_t grids[8];

tm1638_encode_ascii("CONTADOR", segments, 8);
tm1638_digits_common_anode(segments, grids, 8);
tm1638_display(grids);
```

---

### 5. `tm1638_display_with_brightness()`
**Propósito**: Muestra datos en el display con brillo personalizado.

```c
void tm1638_display_with_brightness(const uint8_t* grids, uint8_t brightness);
```

**Parámetros**:
- `grids`: Array de 8 bytes con datos de grid
- `brightness`: Nivel de brillo (0-7)

**Ejemplo**:
```c
uint8_t segments[8];
uint8_t grids[8];

tm1638_encode_ascii("BRILLO  ", segments, 8);
tm1638_digits_common_anode(segments, grids, 8);
tm1638_display_with_brightness(grids, 2);  /* Brillo bajo */
```

---

### 6. `tm1638_set_brightness()`
**Propósito**: Configura el brillo del display.

```c
void tm1638_set_brightness(uint8_t brightness);
```

**Parámetros**:
- `brightness`: Nivel de brillo (0-7)

**Niveles de brillo**:
- `0` = 12.5% (muy tenue)
- `1` = 25% (tenue)
- `2` = 37.5% (bajo)
- `3` = 50% (medio-bajo)
- `4` = 62.5% (medio) ⭐ **Recomendado**
- `5` = 75% (medio-alto)
- `6` = 87.5% (alto)
- `7` = 100% (máximo)

**Ejemplo**:
```c
tm1638_set_brightness(3);  /* Configurar brillo medio-bajo */
```

---

### 7. `tm1638_clear_display()`
**Propósito**: Limpia completamente el display.

```c
void tm1638_clear_display(void);
```

**Ejemplo**:
```c
tm1638_clear_display();  /* Apagar todos los segmentos */
```

---

## ⌨️ Funciones de Teclado

### Mapeo de teclas QYF-TM1638:
```
[ 1] [ 2] [ 3] [ 4]
[ 5] [ 6] [ 7] [ 8]
[ 9] [10] [11] [12]
[13] [14] [15] [16]
```

### 1. `tm1638_get_key_pressed()`
**Propósito**: Obtiene el número de la primera tecla presionada.

```c
uint8_t tm1638_get_key_pressed(void);
```

**Retorna**:
- `1-16`: Número de tecla presionada
- `0`: No hay teclas presionadas

**Ejemplo**:
```c
uint8_t tecla = tm1638_get_key_pressed();
if (tecla > 0) {
    /* Procesar tecla presionada */
    printf("Tecla presionada: %d\n", tecla);
}
```

---

### 2. `tm1638_get_all_keys_pressed()`
**Propósito**: Obtiene todas las teclas presionadas simultáneamente.

```c
uint8_t tm1638_get_all_keys_pressed(uint8_t* pressed_keys);
```

**Parámetros**:
- `pressed_keys`: Array de 16 elementos (1=presionada, 0=no presionada)

**Retorna**:
- Número total de teclas presionadas

**Ejemplo**:
```c
uint8_t teclas_presionadas[16];
uint8_t total_teclas = tm1638_get_all_keys_pressed(teclas_presionadas);

if (total_teclas > 0) {
    for (int i = 0; i < 16; i++) {
        if (teclas_presionadas[i]) {
            printf("Tecla %d está presionada\n", i + 1);
        }
    }
}
```

---

### 3. `tm1638_read_keys()`
**Propósito**: Lee los datos crudos de las teclas (nivel avanzado).

```c
uint32_t tm1638_read_keys(void);
```

**Retorna**:
- Valor de 32 bits con el estado crudo de las teclas

**Nota**: Esta función es para uso avanzado. Normalmente usa `tm1638_get_key_pressed()`.

---

## 🔧 Funciones Auxiliares

### `tm1638_delay()`
**Propósito**: Genera delays para timing y efectos.

```c
void tm1638_delay(volatile uint16_t delay_counter);
```

**Parámetros**:
- `delay_counter`: Número de iteraciones de delay

**Ejemplo**:
```c
tm1638_delay(5000);  /* Pausa de aproximadamente 0.5 segundos */
```

---

## 💡 Ejemplos Prácticos

### Ejemplo 1: Mostrar texto simple
```c
void mostrar_mensaje(char* mensaje) {
    uint8_t segments[8];
    uint8_t grids[8];
    
    tm1638_encode_ascii(mensaje, segments, 8);
    tm1638_digits_common_anode(segments, grids, 8);
    tm1638_display(grids);
}

void main(void) {
    /* Configuración inicial */
    CONF_PORT_SALIDA = 0b00000000;
    tm1638_set_brightness(4);
    
    /* Mostrar mensaje */
    mostrar_mensaje("HOLA    ");
    tm1638_delay(8000);
    
    mostrar_mensaje("MUNDO   ");
    tm1638_delay(8000);
}
```

---

### Ejemplo 2: Contador simple
```c
void mostrar_numero(uint16_t numero) {
    char texto[9];
    uint8_t segments[8];
    uint8_t grids[8];
    
    /* Convertir número a string con formato */
    sprintf(texto, "%8d", numero);
    
    tm1638_encode_ascii(texto, segments, 8);
    tm1638_digits_common_anode(segments, grids, 8);
    tm1638_display(grids);
}

void main(void) {
    uint16_t contador = 0;
    
    /* Configuración inicial */
    CONF_PORT_SALIDA = 0b00000000;
    tm1638_set_brightness(5);
    
    while(1) {
        mostrar_numero(contador);
        contador++;
        tm1638_delay(5000);  /* Incrementar cada 0.5 segundos */
    }
}
```

---

### Ejemplo 3: Teclado interactivo
```c
void main(void) {
    uint8_t tecla;
    uint8_t segments[8];
    uint8_t grids[8];
    char mensaje[9];
    
    /* Configuración inicial */
    CONF_PORT_SALIDA = 0b00000000;
    tm1638_set_brightness(4);
    
    /* Mostrar mensaje inicial */
    tm1638_encode_ascii("PRESIONA", segments, 8);
    tm1638_digits_common_anode(segments, grids, 8);
    tm1638_display(grids);
    
    while(1) {
        tecla = tm1638_get_key_pressed();
        
        if (tecla > 0) {
            /* Mostrar número de tecla presionada */
            sprintf(mensaje, "TECLA %2d", tecla);
            tm1638_encode_ascii(mensaje, segments, 8);
            tm1638_digits_common_anode(segments, grids, 8);
            tm1638_display(grids);
            
            /* Esperar a que se suelte la tecla */
            while(tm1638_get_key_pressed() != 0) {
                tm1638_delay(50);
            }
            
            /* Pausa */
            tm1638_delay(3000);
            
            /* Volver al mensaje inicial */
            tm1638_encode_ascii("PRESIONA", segments, 8);
            tm1638_digits_common_anode(segments, grids, 8);
            tm1638_display(grids);
        }
        
        tm1638_delay(100);
    }
}
```

---

### Ejemplo 4: Control de brillo dinámico
```c
void efecto_brillo(char* mensaje) {
    uint8_t segments[8];
    uint8_t grids[8];
    uint8_t i;
    
    /* Preparar mensaje */
    tm1638_encode_ascii(mensaje, segments, 8);
    tm1638_digits_common_anode(segments, grids, 8);
    
    /* Efecto de brillo creciente */
    for (i = 0; i <= 7; i++) {
        tm1638_display_with_brightness(grids, i);
        tm1638_delay(3000);
    }
    
    /* Efecto de brillo decreciente */
    for (i = 7; i > 0; i--) {
        tm1638_display_with_brightness(grids, i);
        tm1638_delay(3000);
    }
}

void main(void) {
    /* Configuración inicial */
    CONF_PORT_SALIDA = 0b00000000;
    
    while(1) {
        efecto_brillo("BRILLO  ");
        tm1638_delay(5000);
    }
}
```

---

## 📝 Consejos y Mejores Prácticas

### ✅ Recomendaciones:
1. **Brillo inicial**: Usa nivel 4-5 para un balance entre visibilidad y consumo
2. **Delays**: Usa `tm1638_delay(100)` en loops para evitar saturar el CPU
3. **Detección de teclas**: Siempre verifica que la tecla se haya soltado antes de procesar la siguiente
4. **Strings**: Asegúrate de que los strings tengan exactamente 8 caracteres (rellenar con espacios)

### ⚠️ Precauciones:
1. **Configuración de puertos**: Siempre configura los puertos antes de usar las funciones
2. **Variables C89**: Declara todas las variables al inicio de las funciones
3. **Bounds checking**: Los niveles de brillo se limitan automáticamente a 0-7
4. **Memory**: Los arrays deben tener el tamaño correcto (8 elementos para display)

### 🔧 Debugging:
- Si el display no muestra nada: Verifica la configuración de puertos
- Si las teclas no responden: Asegúrate de que el mapeo sea para QYF-TM1638
- Si el texto se ve extraño: Algunos caracteres ASCII tienen representación limitada en 7-segmentos

---

## 📚 Referencia Rápida

### Funciones más utilizadas:
```c
/* Configuración */
tm1638_set_brightness(4);

/* Mostrar texto */
tm1638_encode_ascii("TEXTO   ", segments, 8);
tm1638_digits_common_anode(segments, grids, 8);
tm1638_display(grids);

/* Leer tecla */
uint8_t tecla = tm1638_get_key_pressed();

/* Limpiar */
tm1638_clear_display();

/* Delay */
tm1638_delay(5000);
```

### Constantes útiles:
```c
#define BRILLO_MINIMO    0
#define BRILLO_BAJO      2
#define BRILLO_MEDIO     4
#define BRILLO_ALTO      6
#define BRILLO_MAXIMO    7

#define DELAY_CORTO     1000
#define DELAY_MEDIO     5000
#define DELAY_LARGO    10000
```

---

**© 2025 - Manual de la Librería TM1638 para CC65/C89**