all: compile

# The compiler to use
CC := /bin/g++-10
VER := #-std=c++11

# Target file
TARGET := test

# File sources
SOURCES := main.cpp rawmode.cpp
HEADERS := buff.h winConf.h menu.h


compile:
	@echo "Compilando..."
	${CC} ${SOURCES} ${HEADERS} ${VER} -o ${TARGET}
	@echo "Terminado."
