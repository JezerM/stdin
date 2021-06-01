all: compile

# The compiler to use
CC := g++
VER := -std=c++14
FLAGS := -pthread -lopenal

# Target file
TARGET := tasky

# File sources
SOURCES := ${wildcard src/*.cpp}
HEADERS := -I headers/
OBJECTS := $(patsubst src/%.cpp, obj/%.o, $(SOURCES))

# Debug
DEBUG := debugf

${OBJECTS}: obj/%.o : src/%.cpp
	${CC} ${VER} ${HEADERS} -c $< -o $@ ${FLAGS}

compile: build ${OBJECTS}
	${CC} ${VER} ${HEADERS} obj/*.o -o ${TARGET} ${FLAGS}

clean:
	rm -rf ${OBJECTS} ${DEBUG} obj

${DEBUG} : src/*.cpp
	${CC} -g ${SOURCES} ${HEADERS} -o ${DEBUG} ${VER} ${FLAGS}

debug: ${DEBUG}
	@gdb ${DEBUG}

run:
	@./${TARGET}

build:
	@mkdir -p obj

# For Code:Blocks
Debug: ${DEBUG}

Release: compile
