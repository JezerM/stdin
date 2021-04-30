all: compile

# The compiler to use
CC := g++
VER := #-std=c++11
FLAGS := -pthread

# Target file
TARGET := test

# File sources
SOURCES := main.cpp rawmode.cpp behavior.cpp buff.cpp timer.cpp
HEADERS := buff.h winConf.h menu.h

compile:
	@echo "Compilando..."
	${CC} ${SOURCES} ${HEADERS} -o ${TARGET} ${VER} ${FLAGS}
	@echo "Terminado."

run:
	@./${TARGET}
