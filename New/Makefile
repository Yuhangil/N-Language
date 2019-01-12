export PATH := /usr/local/opt/llvm/bin:$(PATH)

CFLAGS = -Wno-unknown-warning-option `llvm-config --cxxflags --ldflags --system-libs --libs all` -std=c++14 -Qunused-arguments -Wno-potentially-evaluated-expression -Werror

all:
	clang++ $(CFLAGS) -c Lexer.cpp
	clang++ $(CFLAGS) -c main.cpp
	clang++ $(CFLAGS) -o lexer lexer.o main.o

clean:
	rm *.o

env:
	export PATH
	echo $(PATH)