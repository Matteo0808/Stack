#include <stdio.h>
#include <string.h>
#include <assert.h>

const int fiasco = 0xF1A5C0;    //15836608
const size_t minCapacity = 5;

struct stack_t{
    int* data;
    size_t size;
    size_t capacity;

};

void StackPrintf(stack_t stk);
void StackDump(stack_t stk);

void StackConstructor(stack_t *stk, size_t capacity);
void StackDestructor(stack_t *stk);

void StackPush(stack_t *stk, int elem);
int StackPop(stack_t *stk);

void StackResizeUp(stack_t *stk);
void StackResizeDown(stack_t *stk);

//TODO - resizedown
//TODO - strdump
//TODO - canary protection
//TODO - hash protection
//TODO - malloc protection

/// @brief now for ints only 
int main(){

    stack_t stk = {};

    StackPrintf(stk);

    StackConstructor(&stk, minCapacity);

    for(int i = 0; i < 11; i++){
        StackPush(&stk, i + 1);
        StackPrintf(stk);
    }

    for(int i = 0; i < 11; i++){
        // printf("pop[i] = [%d]\n", i);
        printf("POP = [%d]\t", StackPop(&stk));
        StackPrintf(stk);
    }

    StackDestructor(&stk);

    printf("\nsize after destructor: [%d]\n", stk.size);
    printf("capacity after destroy: [%d]\n", stk.capacity);

    return 0;
}

void StackPrintf(stack_t stk){
    printf("\n===================================================================================\n");
    printf("size = %d\tcapacity = %d\n", stk.size, stk.capacity);
    if (stk.size == 0){
        printf("Stack is empty:(\n");
    }
    for(int i = 0; i < stk.size; i++){
        printf("[%d]\t", stk.data[i]);
    }
    printf("\n==================================================================================\n");
}

void StackConstructor(stack_t *stk, size_t capacity){
    assert(stk != NULL);

    stk->data = (int *)calloc(capacity, sizeof(int));
    stk->capacity = capacity;
    stk->size = 0;
}

void StackPush(stack_t *stk, int elem){
    assert(stk != NULL);
    assert(stk->data != NULL);

    if(stk->size < stk->capacity){
        stk->data[stk->size] = elem;
        stk->size++;
    }
    else{
        StackResizeUp(stk);
        stk->data[stk->size] = elem;
        stk->size++;
    }
}

int StackPop(stack_t *stk){
    assert(stk != NULL);
    assert(stk->data != NULL);

    if(stk->size > 0){
        stk->size--;
        if(4 * stk->size <= stk->capacity && stk->capacity > minCapacity){
            StackResizeDown(stk);
        }
        return stk->data[stk->size];
    }
    else{
        printf("Stack is empty:((\n");
        return 0;
    }
}

void StackResizeUp(stack_t *stk){
    void *temp = realloc(stk->data, sizeof(stk->data[0]) * (stk->capacity * 2));
    if(temp == NULL){
        printf("It wasn't possible to increase stack((\n");
        return;
    }
    stk->data = (int *)temp;
    stk->capacity *= 2; 
}

void StackResizeDown(stack_t *stk){
    void *temp = realloc(stk->data, sizeof(stk->data[0]) * (stk->capacity / 2));
    if(temp == NULL){
        printf("It wasn't possible to reduce stack((\n");
        return;
    }
    stk->data = (int *)temp;
    stk->capacity /= 2; 
}


void StackDestructor(stack_t *stk){
    free(stk->data);
    stk->data = NULL;
    stk->size = fiasco;
    stk->capacity = fiasco;    
}