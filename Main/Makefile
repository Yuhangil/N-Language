export PATH := /usr/local/opt/llvm/bin:$(PATH)

all:
	clang++ -c lexer.cpp
	clang++ `llvm-config --cxxflags --ldflags --system-libs --libs all` -std=c++14 -c parser.cpp
	clang++ `llvm-config --cxxflags --ldflags --system-libs --libs all` -std=c++14 -c codegen.cpp
	clang++ `llvm-config --cxxflags --ldflags --system-libs --libs all` -o parser parser.o lexer.o codegen.o

env:
	export PATH
	echo $(PATH)