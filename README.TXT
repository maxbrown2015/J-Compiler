README.TXT

CIS240 JCompiler - Maxwell Brown 

In this directory is a token class, a tokenWriter class, a stack class, and a main class (jcompiler.c).

JCompiler takes in a single j file argument and reads it line by line. Using strtok(), the parser breaks the lines up into string token delimited by whitespace. With each token, there is a call to token.c's initializeToken, which initializes a token struct that contains the necessary information for writing. A subsequent call to tokenWriter's write function generates the required assembly for the instruction. The program ends after the input file feofs.


token.o -> this class creates and initializes the token and throws an error if the string is not a legal token in the j language.

tokenWriter.o -> this class takes a token (initialized in token.c) and handles it accordingly. For arithmatic and logical operations, assembly is generated which pops two items off the stack and performs the given operation. For commands, these involve manipulating the stack. Given a function identifier, it is either a function header (if it follows 'defun') or a function call. For comparison statements, the assembly branches if false to a label which adds 0 to the stack or adds true and branches past the false label. 

Handling if statements -> To handle if statements, tokenWriter.c has a global stack with each node containing two fields, the number of ifs in the program and a pointer to the next element in the stack. When an if is encounter, a new node containing the current if count is pushed onto the stack. When an endif is encountered, the head of the stack is popped off. This is to ensure unqiue labels for branching in LC4. This is similar to the open/closed parantheses problem. 

Handling literals -> If a literal is >256 or <-256, tokenWriter.c sets the upper 8 bits using HICONST and the lower 8 bits using CONST.

stack.o -> This class is nearly identical to the one used in the RPN calculator, just with a single integer field.