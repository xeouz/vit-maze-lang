all: lang run

lang.o:
	@cd out && clang -c ../main.cc ../lang.cc ../interpret.cc ../parse.cc ../lex.cc ../ast.cc

lang: lang.o
	@clang -lstdc++ -lm out/main.o out/lang.o out/interpret.o out/parse.o out/lex.o out/ast.o -o out/main

run:
	@echo ---
	cd out && ./main

clean:
	@rm out/main.o out/lang.o out/interpret.o out/parse.o out/lex.o out/ast.o
	@rm out/main