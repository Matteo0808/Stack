# Stack

Hello, GitHub! Here is my Data Structure Stack!

It's my realisation of Stack, based on principle LIFO(Last in first out).

This repository contains the code for a program that ensures stack's work and contains canonical stack functions.


## Function list

1. StackConstructor(stack_t *stk, ssize_t capacity)
2. StackDestructor(stack_t *stk)
3. StackPush(stack_t *stk, ssize_t elem)
4. StackPop(stack_t *stk, ssize_t *elem)
5. StackVerify(stack_t *stk)
6. StackDump(stack_t *stk)
7. StackPrintf(stack_t *stk)


## How to work with stack?

1. At first, to create stack variable, you need to use function **"StackConstructor"**
```c++
    stack_t *stk;
    stk = StackConstructor(stk, capacity);
```
**OR**
```c++
    stack_t *stk = StackConstructor(stk, capacity);
```


2. If you want to add an element in your stack, you need to use function **"StackPush"**
```c++
    StackPush(stk, element);
```
Don't worry, if the number of elements exceeds capacity, **"StackPush"** will automatically resize capacity 2 times more.


3. If you want to get an element from your stack, you need to use function **"StackPop"**
```c++
    ssize_t elem;
    StackPop(stk, &elem);
``` 
If the number of elements is 4 times less than capacity, **"StackPop"** will automatically resize capacity 2 times less.


4. If you want to check your stack for errors, you need to use function **"StackVerify"**
```c++
    ErrorSuccess error = StackVerify(stk);
```
ErrorSuccess is enum: 

```
    enum ErrorSuccess{
        RETURN_SUCCESS = 0,
        RETURN_ERROR = 1
    };
```

If stack is ok, **"StackVerify"** returns 0, so you can check it this way:

```c++
    ErrorSuccess error = StackVerify(stk);
    if(error){
        printf("stack isn't okay:(");
    }
```
**"StackVerify"** calls **"StackCheckErrors"** that returns error code.
And if there is at least 1 error, **"StackVerify"** calls **"StackDump"**.

Ofcourse **"StackVerify"** is in every function, before and after the body of function.


5. If you want to take all information about stack, you need to use **"StackDump"**
```c++
    StackDump(stk);
```

6. If you want to show your stack status, you need to use **"StackPrintf"**
```c++
    StackPrintf(stk);
```

7. In the end, you need to destroy your stack with function **"StackDestructor"**
```c++
    StackDestructor(stk);
```


## Features:

1. StackConstructor allocate memory for stack. So it's possible to reallocate array.
2. Maximal level of protection:
    1. struct declarated after main, so you can't directly accessed a structure field.
    2. canary protection, so you can register attack from left or from right.
    3. hash protection, it counts the control sum by algorithm DJB.
3. Every function has a wrapper, becouse every function without th wrapper has call file, call function and call line in arguments. 
4. Every function print a message in console if there is an error, and full stack info in log file
5. Super puper code style\)




<details>
<summary>Includes:</summary>

<stdio.h>\
<string.h>\
<assert.h>

</details>