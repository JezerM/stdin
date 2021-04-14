all: compile

# The compiler to use
CC := g++

# Target file
TARGET := test

# File sources
SOURCES := main.cpp rawmode.cpp
HEADERS := buffer.h winConf.h menu.h


compile:
	@echo "Compilando..."
	${CC} ${SOURCES} ${HEADERS} -o ${TARGET}
	@echo "Terminado."
