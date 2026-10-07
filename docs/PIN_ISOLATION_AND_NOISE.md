# Aislamiento de Pines y Ruido en Audio

## Resumen

El driver TM1638 **solo controla los bits 0 (CLK), 1 (DIO) y 2 (STB)** del
puerto de salida. Cualquier otro bit del mismo puerto —audio, video, LEDs,
señales de control— conserva intacto el valor que tenía.

Esto es importante en sistemas donde un solo byte de puerto comparte el TM1638
con otras señales, algo habitual en implementaciones FPGA con 6502.

## El problema original

La versión anterior volcaba el byte completo del puerto en cada operación:

```c
PORT_SALIDA = TM_CLK_LOW();   /* escribía los 8 bits */
```

Como el driver no conoce los bits que no le pertenecen, en la práctica
**reescribía las patas ajenas con cada flanco de reloj**. En un puerto de
8 bits que comparta el TM1638 con una salida de audio, eso significa que cada
bit enviado al display generaba actividad eléctrica en el camino de audio,
lo que se traduce en un zumbido audible en el altavoz.

Medido con el simulador `sim65`, el driver llegaba a escribir el patrón ajeno
**26 veces** durante una sola secuencia de operaciones.

## La solución

Un read-modify-write que preserva los bits que no son del driver:

```c
void tm1638_apply_output(void) {
    uint8_t resto;

    resto = (uint8_t)(PORT_SALIDA & (uint8_t)~TM_ALL_MASK);
    PORT_SALIDA = (uint8_t)(resto | (tmp_salida & TM_ALL_MASK));
}
```

Todas las operaciones del driver pasan por esta función, que lee el valor
actual del puerto, sustituye únicamente los 3 bits de control y lo reescribe.
Ningún otro bit se ve afectado.

### Coste

| Concepto | Impacto |
|---|---|
| ROM | +24 bytes (~0.8% de la librería) |
| RAM | **0 bytes** |
| Velocidad | Despreciable; además se omiten escrituras redundantes |

El diseño reutiliza la variable existente `tmp_salida` en lugar de añadir
estado nuevo, por eso la RAM no cambia.

### Compatibilidad

El cambio es **transparente para aplicaciones existentes**:

- Las firmas de todas las funciones públicas no cambian.
- Los 35 símbolos exportados se conservan, incluido `tmp_salida`.
- La secuencia de estados de CLK/DIO/STB que recibe el TM1638 es idéntica:
  el conjunto de valores usados (`00, 01, 02, 03, 05, 07`) es el mismo antes
  y después. Solo se omiten escrituras repetidas que no alteraban ningún bit.

La única diferencia observable es que los bits 3-7 del puerto ya no se
modifican, que es precisamente el objetivo.

## Ruido en el parlante

El aislamiento de pines elimina la causa principal de inyección de ruido,
pero si el zumbido persiste hay un segundo factor a considerar: **la velocidad
de conmutación**.

### `timing_delay`

El parámetro `timing_delay`, definido en `tm1638.h`, controla las pausas entre
flancos:

```c
#define timing_delay 0
```

Con el valor por defecto `0`, las líneas de control conmutan cada pocos
ciclos de CPU. En un 6502 a 12-25 MHz eso produce actividad en el rango de
los megahercios, cuyos armónicos pueden caer en la banda audible y
demodularse en la etapa de audio.

El TM1638 no necesita esa velocidad: funciona con holgura en el rango de
cientos de kHz. Aumentar `timing_delay` reduce la frecuencia de conmutación
y, con ella, el ruido inyectado.

### Cómo ajustarlo sin romper otras aplicaciones

El valor está protegido para que cada proyecto pueda sobrescribirlo sin
modificar la librería:

```c
/* Opción 1: en el Makefile, solo para tu proyecto */
CFLAGS += -Dtiming_delay=12

/* Opción 2: antes de incluir el header */
#define timing_delay 12
#include "tm1638.h"
```

Las aplicaciones que no definan nada mantienen el comportamiento original
(`0`), de modo que nadie se ve afectado por un cambio de temporización.

Valores de partida sugeridos: `8` a `20`. El límite práctico depende de la
frecuencia de CPU y de qué tan rápido refresques el display.

### Apagar el display durante el audio

La medida más directa: apagar el módulo mientras suena el audio elimina por
completo su conmutación en los momentos críticos. El TM1638 tiene un comando
específico para esto (`0x80`), expuesto como dos funciones:

```c
tm1638_display_off();   /* apaga el display */
/* ... reproducir audio ... */
tm1638_display_on();    /* enciende con el brillo que tenía */
```

Los datos en memoria **se conservan**: al reencender aparece de nuevo el
contenido anterior, sin necesidad de repintarlo. No confundir con
`tm1638_clear_display()`, que sí borra el contenido.

Ejemplo de uso alrededor de un fragmento de audio:

```c
tm1638_show_text(" PLAY   ");
tm1638_delay(500);

tm1638_display_off();
reproducir_audio();        /* sin ruido del display */
tm1638_display_on();

tm1638_show_text(" DONE   ");
```

Coste: 37 bytes de ROM, 0 de RAM, 0 de stack.

### Otras medidas

- **Configura el puerto de forma no invasiva.** En lugar de escribir el
  registro de configuración completo, modifica solo los bits del TM1638:

  ```c
  CONF_PORT_SALIDA &= ~TM1638_PINS_MASK;   /* los 3 bits del TM1638 como salida */
  ```

  Escribir `CONF_PORT_SALIDA = 0b00000000` afecta a las patas de otras
  señales que compartan ese byte.

- **Refresca solo cuando cambie el contenido.** `tm1638_display()` reenvía
  los 16 bytes completos en cada llamada. Llamarlo dentro de un lazo genera
  tráfico continuo e innecesario.

- **A nivel de hardware**, una resistencia en serie de 100 Ω en CLK/DIO/STB
  y una alimentación separada para el módulo TM1638 reducen el acoplamiento.
  Conviene recordar que el propio módulo QYF-TM1638, al multiplexar 8 dígitos,
  es una fuente de ruido de alimentación por sí mismo.

### Combinar medidas

Las tres palancas actúan sobre factores distintos, así que conviene probarlas
por separado para saber cuál pesa más en cada montaje:

| Medida | Actúa sobre | Requiere recompilar |
|---|---|---|
| Aislamiento de pines | Patas ajenas del puerto | No (ya aplicado) |
| `display_off` / `display_on` | Conmutación durante el audio | No |
| `timing_delay` | Frecuencia de conmutación | Sí (`-D`) |
| Hardware (serie, alimentación) | Acoplamiento físico | — |

## Verificación

El aislamiento está cubierto por un banco de pruebas que se ejecuta bajo el
simulador `sim65`:

```bash
make test
```

Las pruebas comprueban que:

- Ninguna operación del driver altera los bits 3-7 del puerto, con distintos
  patrones de partida (`0xA8`, `0xF8`, `0x00`).
- El driver sí controla correctamente sus propios bits (control negativo;
  sin él, un driver que no escribiera nada pasaría las pruebas trivialmente).
- La configuración de DIO solo modifica el bit 1.

Para comparar el protocolo entre versiones de la librería, la traza completa
de accesos al puerto puede capturarse con:

```bash
sim65 --trace programa.prg
```

## Historial

| Versión | Cambio |
|---|---|
| v2.0 | Volcado del byte completo del puerto (bits ajenos afectados) |
| v2.1 | Read-modify-write: solo se controlan los bits 0-2 |
