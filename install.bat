@echo off
REM ============================================================================
REM Script de instalación automática de Librería TM1638 v2.0
REM Uso: install.bat C:\ruta\a\tu\proyecto
REM ============================================================================

if "%1"=="" (
    echo Error: Debe especificar la ruta del proyecto destino
    echo Uso: install.bat C:\ruta\a\tu\proyecto
    pause
    exit /b 1
)

set DEST_DIR=%1
set LIB_NAME=TM1638 v2.0

echo ============================================================================
echo INSTALANDO LIBRERIA %LIB_NAME%
echo ============================================================================
echo Destino: %DEST_DIR%
echo.

REM Verificar que existe el directorio destino
if not exist "%DEST_DIR%" (
    echo Creando directorio destino: %DEST_DIR%
    mkdir "%DEST_DIR%"
)

REM Crear estructura de directorios
echo Creando estructura de directorios...
if not exist "%DEST_DIR%\libs" mkdir "%DEST_DIR%\libs"
if not exist "%DEST_DIR%\libs\tm1638" mkdir "%DEST_DIR%\libs\tm1638"
if not exist "%DEST_DIR%\include" mkdir "%DEST_DIR%\include"

REM Copiar archivos principales
echo Copiando archivos de la libreria...
copy "src\tm1638.c" "%DEST_DIR%\libs\tm1638\" >nul
if %errorlevel% neq 0 (
    echo Error: No se pudo copiar tm1638.c
    pause
    exit /b 1
)

copy "include\tm1638.h" "%DEST_DIR%\include\" >nul
if %errorlevel% neq 0 (
    echo Error: No se pudo copiar tm1638.h
    pause
    exit /b 1
)

REM Copiar documentación (opcional)
if exist "docs\*.md" (
    echo Copiando documentacion...
    copy "docs\*.md" "%DEST_DIR%\libs\tm1638\" >nul
)

REM Copiar librería compilada si existe
if exist "build\libtm1638.a" (
    echo Copiando libreria compilada...
    copy "build\libtm1638.a" "%DEST_DIR%\libs\tm1638\" >nul
)

REM Copiar ejemplos (opcional)
if exist "examples\*.c" (
    if not exist "%DEST_DIR%\examples" mkdir "%DEST_DIR%\examples"
    echo Copiando ejemplos...
    copy "examples\*.c" "%DEST_DIR%\examples\" >nul
)

echo.
echo ============================================================================
echo INSTALACION COMPLETADA EXITOSAMENTE
echo ============================================================================
echo.
echo Archivos instalados:
echo   %DEST_DIR%\libs\tm1638\tm1638.c
echo   %DEST_DIR%\include\tm1638.h
if exist "%DEST_DIR%\libs\tm1638\*.md" echo   %DEST_DIR%\libs\tm1638\*.md ^(documentacion^)
if exist "%DEST_DIR%\libs\tm1638\libtm1638.a" echo   %DEST_DIR%\libs\tm1638\libtm1638.a ^(libreria compilada^)
if exist "%DEST_DIR%\examples\*.c" echo   %DEST_DIR%\examples\*.c ^(ejemplos^)
echo.
echo PARA USAR EN TU PROYECTO:
echo.
echo 1. Agregar a tu Makefile:
echo    CFLAGS += -I include/
echo    SOURCES += libs/tm1638/tm1638.c
echo.
echo    O bien ^(si copiaste la libreria compilada^):
echo    LIBS += -L libs/tm1638/ -l tm1638
echo.
echo 2. En tu codigo C:
echo    #include "tm1638.h"
echo.
echo 3. Inicializar:
echo    tm1638_init^(^);
echo    tm1638_show_text^(" HOLA   "^);
echo.
echo Ver documentacion completa en: %DEST_DIR%\libs\tm1638\MANUAL_TM1638.md
echo.
pause