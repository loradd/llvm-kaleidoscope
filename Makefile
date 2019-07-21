SOURCES = $(shell find src/ast src/lexer src/logger src/parser src/utils -name '*.cpp')
HEADERS = $(shell find src/ast src/lexer src/logger src/parser src/utils -name '*.h')
OBJ = ${SOURCES:.cpp=.o}

CC = llvm-g++
CFLAGS = -stdlib=libc++ -std=c++17 -g -O3
LLVMFLAGS = `llvm-config --cxxflags --ldflags --system-libs --libs core`
# --libs all

.PHONY: main

main: src/main.cpp ${OBJ}
	${CC} ${CFLAGS} ${LLVMFLAGS} ${OBJ} $< -o $@ 

clean:
	rm -r ${OBJ}

%.o: %.cpp ${HEADERS}
	${CC} ${CFLAGS} ${LLVMFLAGS} -c $< -o $@ 

run : build
	./main
