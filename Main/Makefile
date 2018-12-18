export PATH := /usr/local/opt/llvm/bin:$(PATH)

CFLAGS = -Wno-unknown-warning-option -std=c++14 `llvm-config --cxxflags --ldflags --system-libs --libs all` -Qunused-arguments -Wno-potentially-evaluated-expression -Werror -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include

all:
	clang++ $(CFLAGS) -c lexer.cpp
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