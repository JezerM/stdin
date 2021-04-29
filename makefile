all: compile

# The compiler to use
CC := g++
VER := -std=c++14
FLAGS := -pthread

# Target file
TARGET := test

# File sources
SOURCES := main.cpp rawmode.cpp behavior.cpp buff.cpp
HEADERS := buff.h winConf.h menu.h

compile:
	@echo "Compilando..."
	${CC} ${SOURCES} ${HEADERS} -o ${TARGET} ${VER} ${FLAGS} -I mingw-std-threads/
	@echo "Terminado."

run:
	@./${TARGET}
