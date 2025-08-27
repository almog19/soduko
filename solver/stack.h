/*
קובץ הגדרת header לקובץ
אב טיפוס של הפונקציות
*/

#ifndef STACK_H//בודק האם מוגדר, כאשר לא מוגדר להגדור
#define STACK_H

typedef struct Stack Stack;

Stack* stack_create(int capacity);

int stack_push(Stack* stack, void* item);
void* stack_pop(Stack* stack);

int stack_empty(Stack* stack);
int stack_full(Stack* stack);
int stack_size(Stack* stack);

void stack_free(Stack* stack);
#endif
