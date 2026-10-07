#!/bin/sh
# =============================================================================
# Banco de pruebas de la librería TM1638
#
# Ejecuta los tests bajo sim65 (simulador 6502 de cc65), comprobando:
#   - Aislamiento de pines: el driver no altera los bits ajenos del puerto
#   - Control propio: el driver sí manda en CLK/DIO/STB
#   - Configuración de DIO: solo cambia el bit 1
#   - Compatibilidad de protocolo: mismos estados de CLK/DIO/STB que antes
#
# Requiere cl65 y sim65 en el PATH, o definir CC65_BIN.
#
# Uso:
#   sh tests/run_tests.sh
#   CC65_BIN=/d/cc65/bin sh tests/run_tests.sh
# =============================================================================

CC65_BIN=${CC65_BIN:-/d/cc65/bin}
CL65="$CC65_BIN/cl65.exe"
SIM65="$CC65_BIN/sim65.exe"

if [ ! -x "$CL65" ]; then
    CL65="$CC65_BIN/cl65"
    SIM65="$CC65_BIN/sim65"
fi

if [ ! -x "$CL65" ]; then
    echo "ERROR: no se encuentra cl65 en $CC65_BIN"
    echo "Defina CC65_BIN con la ruta a los binarios de cc65."
    exit 1
fi

cd "$(dirname "$0")" || exit 1

CFLAGS="-t sim6502 -O -I ../include"
FALLOS=0

ejecutar() {
    nombre=$1
    fuente=$2
    printf '%-40s' "$nombre"
    if ! "$CL65" $CFLAGS -o "$nombre.prg" "$fuente" ../src/tm1638.c 2>/dev/null; then
        echo "ERROR DE COMPILACION"
        FALLOS=$((FALLOS + 1))
        return
    fi
    "$SIM65" "$nombre.prg"
    codigo=$?
    if [ "$codigo" -eq 0 ]; then
        echo "OK"
    else
        echo "FALLO (codigo $codigo)"
        FALLOS=$((FALLOS + 1))
    fi
}

echo "=== Banco de pruebas TM1638 (sim65) ==="
echo ""
ejecutar "test_port_isolation"  "test_port_isolation.c"
ejecutar "test_control_bits"    "test_control_bits.c"
ejecutar "test_dio_config"      "test_dio_config.c"
ejecutar "test_display_onoff"   "test_display_onoff.c"
echo ""

rm -f test_port_isolation.prg test_control_bits.prg test_dio_config.prg test_display_onoff.prg

if [ "$FALLOS" -eq 0 ]; then
    echo "RESULTADO: todos los tests pasan"
    exit 0
fi

echo "RESULTADO: $FALLOS suite(s) con fallos"
exit 1
