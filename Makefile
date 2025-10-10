# =============================================================================
# Makefile para Librería TM1638 Standalone
# Compatible con CC65 para arquitectura 6502
# =============================================================================

# Compilador y herramientas
CC = cl65
AR = ar65
CFLAGS = -t none -O --cpu 6502 -I include/

# Directorios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
DOCS_DIR = docs
EXAMPLES_DIR = examples

# Archivos fuente
SOURCES = $(SRC_DIR)/tm1638.c
OBJECTS = $(BUILD_DIR)/tm1638.o
LIBRARY = $(BUILD_DIR)/libtm1638.a

# Target por defecto
all: $(LIBRARY)

# Crear directorio de build
$(BUILD_DIR):
	@echo "Creando directorio build..."
	@mkdir $(BUILD_DIR) 2>nul || echo "Directorio build ya existe"

# Compilar archivo objeto
$(BUILD_DIR)/tm1638.o: $(SRC_DIR)/tm1638.c $(INCLUDE_DIR)/tm1638.h | $(BUILD_DIR)
	@echo "Compilando tm1638.c..."
	$(CC) $(CFLAGS) -c $(SRC_DIR)/tm1638.c -o $(BUILD_DIR)/tm1638.o

# Crear librería estática
$(LIBRARY): $(OBJECTS)
	@echo "Creando librería libtm1638.a..."
	$(AR) a $(LIBRARY) $(OBJECTS)
	@echo "Librería TM1638 creada exitosamente: $(LIBRARY)"

# Compilar ejemplos
examples: $(LIBRARY)
	@echo "Compilando ejemplos..."
	@if exist $(EXAMPLES_DIR)\*.c ($(CC) $(CFLAGS) -L $(BUILD_DIR) -l tm1638 $(EXAMPLES_DIR)\*.c -C fpga.cfg -o $(BUILD_DIR)\example.bin) else echo "No hay ejemplos para compilar"

# Limpiar archivos generados
clean:
	@echo "Limpiando archivos generados..."
	@if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
	@echo "Limpieza completada."

# Mostrar información de la librería
info:
	@echo "=== LIBRERIA TM1638 STANDALONE ==="
	@echo "Version: 2.0"
	@echo "Arquitectura: 6502 (CC65)"
	@echo "Archivos:"
	@echo "  - $(SRC_DIR)/tm1638.c (implementación)"
	@echo "  - $(INCLUDE_DIR)/tm1638.h (header)"
	@echo "  - $(DOCS_DIR)/ (documentación)"
	@echo "  - $(EXAMPLES_DIR)/ (ejemplos)"
	@echo ""
	@echo "Comandos disponibles:"
	@echo "  make all      - Compilar librería"
	@echo "  make examples - Compilar ejemplos"
	@echo "  make clean    - Limpiar archivos"
	@echo "  make install  - Instalar en proyecto"
	@echo "  make info     - Mostrar esta información"

# Instalar librería en otro proyecto (uso: make install DEST_DIR=ruta_proyecto)
install:
	@if not defined DEST_DIR (echo "Error: Especificar DEST_DIR=ruta_proyecto" && exit /b 1)
	@echo "Instalando librería TM1638 en $(DEST_DIR)..."
	@if not exist "$(DEST_DIR)" mkdir "$(DEST_DIR)"
	@if not exist "$(DEST_DIR)/libs" mkdir "$(DEST_DIR)/libs"
	@if not exist "$(DEST_DIR)/libs/tm1638" mkdir "$(DEST_DIR)/libs/tm1638"
	@if not exist "$(DEST_DIR)/include" mkdir "$(DEST_DIR)/include"
	@copy "$(SRC_DIR)\tm1638.c" "$(DEST_DIR)\libs\tm1638\" >nul
	@copy "$(INCLUDE_DIR)\tm1638.h" "$(DEST_DIR)\include\" >nul
	@if exist $(BUILD_DIR)\libtm1638.a copy "$(BUILD_DIR)\libtm1638.a" "$(DEST_DIR)\libs\tm1638\" >nul
	@if exist $(DOCS_DIR)\*.md copy "$(DOCS_DIR)\*.md" "$(DEST_DIR)\libs\tm1638\" >nul
	@echo "Instalación completada en $(DEST_DIR)"
	@echo ""
	@echo "Para usar en su proyecto, agregar al Makefile:"
	@echo "  CFLAGS += -I include/"
	@echo "  LIBS += libs/tm1638/tm1638.c"
	@echo "  # O bien:"
	@echo "  LIBS += -L libs/tm1638/ -l tm1638"

# Crear paquete distribuible
package: $(LIBRARY)
	@echo "Creando paquete distribuible..."
	@set PKG_NAME=tm1638_v2.0_6502
	@if exist $(PKG_NAME) rmdir /S /Q $(PKG_NAME)
	@mkdir $(PKG_NAME)
	@mkdir $(PKG_NAME)\src
	@mkdir $(PKG_NAME)\include  
	@mkdir $(PKG_NAME)\docs
	@mkdir $(PKG_NAME)\build
	@copy "$(SRC_DIR)\*" "$(PKG_NAME)\src\" >nul
	@copy "$(INCLUDE_DIR)\*" "$(PKG_NAME)\include\" >nul
	@copy "$(DOCS_DIR)\*" "$(PKG_NAME)\docs\" >nul
	@copy "$(BUILD_DIR)\*" "$(PKG_NAME)\build\" >nul
	@copy "Makefile" "$(PKG_NAME)\" >nul
	@copy "README.md" "$(PKG_NAME)\" >nul 2>nul || echo "README.md no encontrado"
	@echo "Paquete creado: $(PKG_NAME)/"

# Targets que no son archivos
.PHONY: all clean info install examples package