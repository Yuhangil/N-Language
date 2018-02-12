export PATH := /usr/local/opt/llvm/bin:$(PATH)

CFLAGS = `llvm-config --cxxflags --ldflags --system-libs --libs all` -std=c++14 -Qunused-arguments -Wno-potentially-evaluated-expression -Werror

all:
	clang++ -std=c++14 -c lexer.cpp
	clang++ $(CFLAGS) -c parser.cpp
	clang++ $(CFLAGS) -c codegen.cpp
	clang++ $(CFLAGS) -c main.cpp
	clang++ $(CFLAGS) -o parser parser.o lexer.o codegen.o main.o
	rm *.o

clean:
	rm *.o

env:
	export PATH
	echo $(PATH)