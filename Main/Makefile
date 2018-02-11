export PATH := /usr/local/opt/llvm/bin:$(PATH)

all:
	clang++ -c lexer.cpp
	clang++ `llvm-config --cxxflags --ldflags --system-libs --libs core native` -std=c++14 -c parser.cpp
	clang++ `llvm-config --cxxflags --ldflags --system-libs --libs core native` -std=c++14 -c codegen.cpp
	clang++ `llvm-config --cxxflags --ldflags --system-libs --libs core native` -o parser parser.o lexer.o codegen.o

env:
	export PATH
	echo $(PATH)