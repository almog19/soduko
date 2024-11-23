#include "stack.h"//הכללה של ה header
#include <stdlib.h>
#include <stdio.h>
//מיישם את הפונקציות של המחסנית
struct Stack{
    void** items;
    int capacity;
    int top;
};

Stack* stack_create(int capacity){
    Stack* stack = (Stack*) malloc(sizeof(Stack));
    stack->items = (void**) malloc(sizeof(void*) * capacity);
    stack->capacity = capacity;
    stack->top = -1;
    return stack;
}

int stack_push(Stack* stack, void* item){
    if(stack_full(stack)){printf("stack is full");return 0;}//not good(false)
    stack->items[++stack->top] = item;
    return 1;//good(true)
}

void* stack_pop(Stack* stack){
    if(stack_empty(stack)){return NULL;}
    return stack->items[stack->top--];
}

int stack_empty(Stack* stack){
    return stack->top == -1;
}

int stack_full(Stack* stack){
    return ((stack->top) == (stack->capacity-1));
}

int stack_size(Stack* stack){
    return stack->top+1;
}

void stack_free(Stack* stack){
    free(stack->items);
    free(stack);
}