# 📦 Librería TM1638 v2.0 - Standalone

**Librería profesional TM1638 para 6502/CC65 completamente portable y reutilizable**

[![CC65](https://img.shields.io/badge/Compiler-CC65-blue.svg)](https://cc65.github.io/)
[![6502](https://img.shields.io/badge/CPU-6502-green.svg)](https://en.wikipedia.org/wiki/MOS_Technology_6502)
[![C89](https://img.shields.io/badge/Standard-C89-orange.svg)](https://en.wikipedia.org/wiki/ANSI_C)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## 🚀 Instalación Rápida (3 pasos)

### 1️⃣ Descargar Librería
```bash
# Opción A: Compilar librería
make all

# Opción B: Instalar en proyecto existente
make install DEST_DIR=C:\mi_proyecto_6502
```

### 2️⃣ Agregar a tu Proyecto
```makefile
# En tu Makefile:
CFLAGS += -I include/
SOURCES += libs/tm1638/tm1638.c

# O usar librería compilada:
LIBS += -L libs/tm1638/ -l tm1638
```

### 3️⃣ Usar en tu Código
```c
#include "tm1638.h"

void main(void) {
    tm1638_init();                    // Una línea = todo configurado
    tm1638_show_text(" HOLA   ");     // Texto automático
    tm1638_show_number(12345);        // Número automático
    
    while(1) {
        uint8_t key = tm1638_get_key_pressed();
        if (key > 0) tm1638_show_number(key);
    }
}
```

## 📁 Estructura de la Librería

```
lib_tm1638_standalone/
├── src/
│   └── tm1638.c                 # ✅ Implementación completa (700+ líneas)
├── include/
│   └── tm1638.h                 # ✅ Header con todas las funciones
├── docs/
│   ├── README.md               # ✅ Documentación de la librería
│   ├── MANUAL_TM1638.md        # ✅ Manual completo de usuario
│   └── ANTI_GHOSTING_NOTES.md  # ✅ Notas técnicas anti-ghosting
├── examples/
│   ├── basic_example.c         # ✅ Ejemplo para principiantes
│   └── advanced_example.c      # ✅ Ejemplo API modular
├── Makefile                    # ✅ Compilación y gestión
└── README.md                   # ✅ Esta documentación
```

## 🎯 Características v2.0

| Característica | Estado | Descripción |
|----------------|--------|-------------|
| **🚫 Anti-Ghosting** | ✅ COMPLETO | Eliminación total de segmentos fantasma |
| **💡 Brillo Inteligente** | ✅ COMPLETO | Persistente, gestión automática |
| **🔧 API Dual** | ✅ COMPLETO | Simple para principiantes + Modular para expertos |
| **🎮 QYF-TM1638** | ✅ COMPLETO | Mapeo específico del hardware probado |
| **⚡ C89 Estricto** | ✅ COMPLETO | Compatible CC65, sin warnings |
| **📦 Portable** | ✅ COMPLETO | Fácil integración en cualquier proyecto |
| **📚 Documentado** | ✅ COMPLETO | Manual completo con ejemplos |

## 🎮 APIs Disponibles

### 👶 API Simplificada (Principiantes)
```c
// ==================== TODO-EN-UNO ====================
tm1638_init();                          // Inicialización robusta
tm1638_set_brightness(4);               // Brillo persistente (0-7)
tm1638_show_text(" HOLA   ");           // Texto automático (8 chars)
tm1638_show_number(12345);              // Número automático
tm1638_clear_display();                 // Limpiar (mantiene brillo)

// ==================== TECLADO ====================
uint8_t key = tm1638_get_key_pressed(); // Leer tecla (1-16, 0=ninguna)
uint8_t keys[16];
uint8_t n = tm1638_get_all_keys_pressed(keys); // Múltiples teclas
```

### 🥷 API Modular (Expertos)
```c
// ==================== CONTROL GRANULAR ====================
uint8_t segments[8], grids[8];
tm1638_encode_ascii8("MENSAJE ", segments);    // Solo codificar
tm1638_number_to_segments8(12345, segments);   // Solo convertir
tm1638_digits_common_anode8(segments, grids);  // Solo procesar
tm1638_display(grids);                         // Solo mostrar
```

## 📚 Documentación Completa

| Documento | Descripción | Para quién |
|-----------|-------------|------------|
| [`docs/README.md`](docs/README.md) | Documentación técnica de la librería | Desarrolladores |
| [`docs/MANUAL_TM1638.md`](docs/MANUAL_TM1638.md) | Manual completo con ejemplos | Todos los usuarios |
| [`docs/ANTI_GHOSTING_NOTES.md`](docs/ANTI_GHOSTING_NOTES.md) | Notas técnicas anti-ghosting | Técnicos avanzados |
| [`examples/basic_example.c`](examples/basic_example.c) | Ejemplo básico funcional | Principiantes |
| [`examples/advanced_example.c`](examples/advanced_example.c) | Ejemplo API modular | Usuarios avanzados |

## 🔧 Comandos Disponibles

```bash
# Información de la librería
make info

# Compilar librería
make all

# Compilar ejemplos
make examples

# Instalar en proyecto
make install DEST_DIR=ruta_a_tu_proyecto

# Crear paquete distribuible
make package

# Limpiar archivos generados
make clean
```

## 🚀 Casos de Uso

### ✅ Perfecto para:
- **Proyectos 6502**: Optimizado específicamente
- **FPGA**: Tang Nano 9K y similares  
- **Prototipado rápido**: API simplificada
- **Sistemas industriales**: API modular completa
- **Educación**: Ejemplos paso a paso
- **Reutilización**: Fácil integración

### 🎯 Hardware Soportado:
- **CPU**: 6502 @ cualquier frecuencia
- **Compilador**: CC65 (C89/C90 estricto)
- **Display**: TM1638 (especialmente QYF-TM1638)
- **FPGA**: Cualquier implementación 6502

## ⚡ Integración en Tu Proyecto

### Método 1: Código Fuente Directo
1. Copiar `src/tm1638.c` a tu carpeta `libs/`
2. Copiar `include/tm1638.h` a tu carpeta `include/`
3. Agregar al Makefile: `SOURCES += libs/tm1638.c`

### Método 2: Librería Compilada
1. Ejecutar `make all` en esta carpeta
2. Copiar `build/libtm1638.a` a tu proyecto
3. Agregar al Makefile: `LIBS += -L libs/ -l tm1638`

### Método 3: Instalación Automática
```bash
make install DEST_DIR=C:\tu_proyecto_6502
```

## 🔬 Especificaciones Técnicas

- **Líneas de código**: 700+ optimizadas
- **Estándar**: C89/C90 ANSI completo
- **Memoria**: Optimizada para 6502
- **Timing**: Calibrado para displays reales
- **Anti-ghosting**: Técnica multi-pass probada
- **Brillo**: 8 niveles (0-7) con persistencia
- **Teclado**: 16 teclas con mapeo QYF-TM1638

## 📞 Soporte

### Issues Conocidos
- ✅ **Ninguno reportado en v2.0**

### Contribuir
1. Fork del repositorio
2. Crear branch para feature
3. Seguir estilo C89 estricto  
4. Documentar cambios
5. Pull request con tests

---

**TM1638 v2.0** - La librería más completa y profesional para 6502 🚀

[![GitHub](https://img.shields.io/badge/GitHub-Micro6502-black.svg)](https://github.com/tu-usuario/micro6502)
[![Docs](https://img.shields.io/badge/Docs-Completas-green.svg)](docs/MANUAL_TM1638.md)