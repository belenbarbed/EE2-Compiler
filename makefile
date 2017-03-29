CPPFLAGS += -W -Wall -g -std=gnu++11
CPPFLAGS += -std=c++11 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I include


src/parser.tab.cpp src/parser.tab.hpp : src/parser.y
	bison -v -d src/parser.y -o src/parser.tab.cpp

src/lexer.yy.cpp : src/lexer.flex src/parser.tab.hpp
	flex -o src/lexer.yy.cpp  src/lexer.flex

##########################################################################################################

bin/c_parser : src/c_parser.o src/parser.tab.o src/lexer.yy.o src/parser.tab.o src/ast.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/c_parser $^

##########################################################################################################

bin/c_compiler : src/c_compiler.o src/parser.tab.o src/lexer.yy.o src/parser.tab.o src/ast.o src/context.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/c_compiler $^

##########################################################################################################



##########################################################################################################

clean :
	rm src/*.yy.cpp
	rm src/*.[oa]
	rm src/*.tab.[ch]pp
	rm src/*.output
	rm bin/*

.PHONY: src/parser.y src/lexer.flex src/ast.cpp src/context.cpp
