all: compile

# The compiler to use
CC := g++
VER := -std=c++14
FLAGS := -pthread -lopenal

# Target file
TARGET := tasky

# File sources
SOURCES := -I src/ src/*.cpp
HEADERS := -I headers/ -I mingw-std-threads/

compile:
	@echo "Compilando en \"${TARGET}\"..."
	${CC} ${SOURCES} ${HEADERS} -o ${TARGET} ${VER} ${FLAGS}
	@echo "Terminado."

debug: debug.compile debug.run

debug.compile:
	@echo "Compilando para el debug..."
	${CC} -g ${SOURCES} ${HEADERS} -o debug ${VER} ${FLAGS}
	@echo "Compilado."

debug.run:
	@echo "Ejecutando gdb..."
	gdb debug

run:
	@./${TARGET}


# For Code:Blocks
Debug: debug.compile

Release: compile
