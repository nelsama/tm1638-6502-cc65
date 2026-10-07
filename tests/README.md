# Banco de pruebas TM1638

Pruebas automáticas de la librería, ejecutadas sobre el simulador `sim65` que
viene con CC65. Verifican propiedades del driver que no son visibles a simple
vista pero que afectan al hardware real.

## Ejecutar

```bash
make test
```

O directamente:

```bash
sh tests/run_tests.sh
```

Si `cl65` y `sim65` no están en el PATH, indica dónde están:

```bash
CC65_BIN=/ruta/a/cc65/bin sh tests/run_tests.sh
```

Requiere CC65 2.19 o superior (el target `sim6502` y `sim65` deben existir).

## Qué se prueba

### `test_port_isolation.c` — Aislamiento de pines

La prueba principal. Comprueba que **ninguna** operación del driver modifica
los bits 3-7 del puerto de salida, que pertenecen a otras señales (audio,
LEDs, etc.). Se verifica con varios patrones de partida (`0xA8`, `0xF8`,
`0x00`) sobre todas las funciones que tocan el puerto.

Cubre `tm1638_write_bit`, `tm1638_write_byte`, `tm1638_send_cmd`,
`tm1638_display`, `tm1638_set_brightness`, `tm1638_clear_display`,
`tm1638_init`, `tm1638_show_text`, `tm1638_show_number` y `tm1638_delay`.

### `test_control_bits.c` — Control de los bits propios

Control negativo del anterior. Si el driver no escribiera nada, el test de
aislamiento pasaría trivialmente. Este verifica que **sí** controla CLK, DIO y
STB, y que tras `write_byte` CLK queda alto (estado final del protocolo).

### `test_dio_config.c` — Configuración de DIO

Verifica que `TM_DIO_CONFIG_INPUT()` y `TM_DIO_CONFIG_OUTPUT()` solo cambian
el bit 1 del registro de configuración, preservando el resto. DIO es
bidireccional (entrada para el teclado, salida para escribir).

### `test_display_onoff.c` — Encendido y apagado

Verifica que `tm1638_display_off()` y `tm1638_display_on()` no perturban los
bits ajenos del puerto, y que STB queda alto tras ambas. Incluye una secuencia
repetida de apagado/encendido.

## Nota sobre el entorno de simulación

En el target `sim6502`, las direcciones `0xC000` y `0xC002` caen dentro del
espacio de datos del propio programa de prueba, así que el runtime de CC65
puede sobrescribirlas. Por eso las comprobaciones sobre `CONF_PORT_SALIDA` se
hacen en `test_dio_config.c`, que usa un programa mínimo sin colisiones.

En el FPGA real esas direcciones son registros de hardware que nadie más toca.

## Cómo interpretar los resultados

El script muestra `OK` o `FALLO` por cada suite. Además, cada programa devuelve
como **código de salida el número de comprobaciones fallidas** (0 = todo
correcto), lo que permite usarlos en integración continua.

Para ver qué comprobación concreta falla:

```bash
cd tests
CC65_BIN=/ruta/a/cc65/bin/cl65.exe -t sim6502 -O -I ../include \
    -o test.prg test_port_isolation.c ../src/tm1638.c
sim65 --trace test.prg > trace.txt
grep "8D 00 03" trace.txt        # escrituras a la dirección de registro de fallos
```

Cada comprobación tiene un identificador numérico; los ids registrados en
`$0300` durante la traza son los que fallaron.

## Añadir pruebas

Los archivos de prueba siguen un patrón simple: escriben en `PORT_SALIDA` un
patrón de bits ajenos, llaman a una función del driver, y comprueban que el
patrón sobrevivió. Para añadir una suite nueva, crea el `.c` en `tests/` y
añade una llamada a `ejecutar` en `run_tests.sh`.
