
jc: token.o tokenWriter.o jcompiler.c stack.o
	clang -o jc token.o stack.o tokenWriter.o jcompiler.c 

token: token.h token.c
	clang -c token.h token.c

tokenWriter: tokenWriter.h tokenWriter.c
	clang -c tokenWriter.h tokenWriter.c

stack: stack.h stack.c
	clang -c stack.h stack.c
	
clean: 
	rm jc *.o

