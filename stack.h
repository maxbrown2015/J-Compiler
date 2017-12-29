/*
 * stack.h : Declares functions used to implement a LIFO stack
 */


typedef struct stack_tag {
   int if_count;
  struct stack_tag *next;
} stack;

stack *push (stack *head, int count);
stack *pop (stack *head);
stack* clearStack(stack *head);
void printStack(stack *head);