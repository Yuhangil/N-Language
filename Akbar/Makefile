all:
	clang++ -c lexer.cpp
	clang++ `llvm-config --cxxflags --ldflags --system-libs --libs core mcjit native` --std=c++14 -c parser.cpp
	clang++ parser.o lexer.o
