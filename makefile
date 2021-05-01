all: compile

# The compiler to use
CC := g++
VER := -std=c++14
FLAGS := -pthread

# Target file
TARGET := test

# File sources
SOURCES := src/ src/*.cpp
HEADERS := headers/

compile:
	@echo "Compilando..."
	${CC} -I ${SOURCES} -I ${HEADERS} -o ${TARGET} ${VER} ${FLAGS} -I mingw-std-threads/
	@echo "Terminado."

run:
	@./${TARGET}
