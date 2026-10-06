#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DEBUG_MODE
#define CANARY_PROTECTION

#ifdef DEBUG_MODE
#define DEBUG(...) __VA_ARGS__
#else
#define DEBUG(...)
#endif


const ssize_t fiasco = 0xF1A5C0;    //15836608
const ssize_t minCapacity = 5;

enum stack_status{
    NOT_INITIALIZED = 0,
    INITIALIZED = 1,
    DESTROYED = 2
};

enum ErrorSucess{
    RETURN_SUCCESS = 0,
    RETURN_ERROR = 1
};

enum errorList{
    STACK_IS_OK = 0,
    STACK_WASNT_INITIALIZED = 1,
    STACK_WAS_DESTROYED_YET = 2,
    NULL_PTR_ON_DATA = 3,
    NULL_PTR_ON_STACK = 4,
    SIZE_OVER_CAPACITY = 5,
    CAPACITY_LOWER_MINIMAL = 6,
    CAPACITY_BELOW_ZERO = 7
    #ifdef CANARY_PROTECTION
    , CANARY_LEFT_ATTACKED = 8,
    CANARY_RIGHT_ATTACKED = 9
    #endif
};

struct stack_t;

stack_t *StackConstructor(ssize_t capacity
                      DEBUG(, const char *crName, const char *crFile,
                      const char *crFunc, ssize_t crLine));
void StackDestructor(stack_t *stk);

ErrorSucess StackVerify(stack_t *stk, const char *verFile, const char *verFunc, ssize_t verLine);
errorList StackCheckErrors(stack_t *stk);
void StackPrintf(stack_t *stk);
void StackDump(stack_t *stk, errorList error, const char *dmpFile, const char *dmpFunc, ssize_t dmpLine);
const char *GetLineStatus(stack_status status);
const char *GetLineError(errorList status);


void StackPush(stack_t *stk, ssize_t elem);
void StackPop(stack_t *stk, ssize_t *elem);

void StackResizeUp(stack_t *stk);
void StackResizeDown(stack_t *stk);

//TODO - resizedown
///@note done

//TODO - strdump
///@note completed

//TODO - define debug
///@note completed

//TODO - verificator - in progress

//TODO - verificator for constructor
//ANCHOR - ASK DED
//NOTE - nahui nado

//TODO - message in console: look in log file - in progress

//TODO - struct after main 
//ANCHOR - ASK DED

//TODO - canary protection - in progress

//TODO - hash protection

//TODO - malloc protection

/// @brief now for ull only 
int main(){

    DEBUG(printf("debug0\n");)

    stack_t *stk = StackConstructor(minCapacity DEBUG(, "stk", __FILE__, __FUNCTION__, __LINE__));


    DEBUG(printf("debug1\n");)

    //StackDump(stk, __FUNCTION__,__FILE__,__LINE__);

    for(ssize_t i = 0; i < 11; i++){
        StackPush(stk, i + 1);
        StackPrintf(stk);
    }

    DEBUG(printf("debug2\n");)

    //StackDump(stk, __FUNCTION__,__FILE__,__LINE__);
    ssize_t pop = 0;
    for(ssize_t i = 0; i < 11; i++){
        StackPop(stk, &pop);
        printf("POP = [%lld]\t", pop);
        StackPrintf(stk);
    }

    DEBUG(printf("debug3\n");)

    //StackDump(stk, __FUNCTION__,__FILE__,__LINE__);

    StackDestructor(stk);

    DEBUG(printf("debug4\n");)

    return 0;
}

struct stack_t{
    DEBUG(
        const char *createName;
        const char *createFile;
        const char *createFunc;
        ssize_t createLine;
    )
    #ifdef CANARY_PROTECTION
    ssize_t canaryLeft;
    ssize_t canaryRight;
    #endif
    ssize_t* data;
    ssize_t size;
    ssize_t capacity;
    stack_status status = NOT_INITIALIZED;
};



stack_t *StackConstructor(ssize_t capacity
                      DEBUG(, const char *crName, const char *crFile,
                      const char *crFunc, ssize_t crLine)){

    stack_t *stk;

    DEBUG(
        stk->createName = crName;
        stk->createFile = crFile;
        stk->createFunc = crFunc;
        stk->createLine = crLine;
    )
    
    //printf("pizda\n");
    
    #ifdef CANARY_PROTECTION
    ssize_t *array = (ssize_t *)calloc(capacity + 2, sizeof(ssize_t));
    stk->canaryLeft = array[0];
    stk->canaryRight = array[capacity + 1];
    stk->data = array + 1;
    #else
    stk->data = (ssize_t *)calloc(capacity, sizeof(ssize_t));
    #endif

    stk->capacity = capacity;
    stk->size = 0;
    stk->status = INITIALIZED;

    ErrorSucess err = RETURN_SUCCESS;
    err = StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
    if(err){
        printf("Error in StackConstructor(), check log file");
        return NULL;
    }
    return stk;
}


ErrorSucess StackVerify(stack_t *stk, const char *verFile, const char *verFunc, ssize_t verLine){
    errorList error = StackCheckErrors(stk);
    if(error){
        StackDump(stk, error, verFile, verFunc, verLine);
        return RETURN_ERROR;
    }
    return RETURN_SUCCESS;
}

errorList StackCheckErrors(stack_t *stk){
    if(stk == NULL){
        return NULL_PTR_ON_STACK;
    }
    if(stk->status == NOT_INITIALIZED){
        return STACK_WASNT_INITIALIZED;
    }
    if(stk->status == DESTROYED){
        return STACK_WAS_DESTROYED_YET;
    }
    if(stk->data == NULL){
        return NULL_PTR_ON_DATA;
    }
    if(stk->capacity < minCapacity){
        return CAPACITY_LOWER_MINIMAL;
    }
    if(stk->capacity <= 0){
        return CAPACITY_BELOW_ZERO;
    }
    if(stk->size > stk->capacity){
        return SIZE_OVER_CAPACITY;
    }
    #ifdef CANARY_PROTECTION
    if(stk->data[-1] != stk->canaryRight){
        return CANARY_LEFT_ATTACKED;
    }
    if(stk->data[stk->capacity] != stk->canaryLeft){
        return CANARY_RIGHT_ATTACKED;
    }
    else{
        return STACK_IS_OK;
    }
    #endif
}





void StackDump(stack_t *stk, errorList error, const char *dmpFile, const char *dmpFunc, ssize_t dmpLine){
    
    FILE *fp = fopen("log.txt", "a");

    fputs("Hello! StackDump is working!\n", fp);
    fprintf(fp, "Function was called by %s() at %s:%lld\n", dmpFunc, dmpFile, dmpLine);

    if(error == NULL_PTR_ON_STACK){
        fputs("StackDump detected critical error: ptr on stack variable = 0\n", fp);
        fputs("Program won't be working correctly\n", fp);
        // fputs("I'm sorry, but i've started emergency program termination\n", fp);
        // fclose(fp);
        // abort();
    }
    
    

    fprintf(fp, "stack_t %s (located at %p) created by %s() at %s:%lld\n{\n",
            stk->createName, stk, stk->createFunc, stk->createFile, stk->createLine);
    fprintf(fp, "stack_status = %s\n", GetLineStatus(stk->status));
    fprintf(fp, "StackDump was called with error code: %s\n", GetLineError(error));
    fprintf(fp, "\tsize = %lld\n\tcapacity = %lld\n\tdata = %p\n\t{\n", stk->size, stk->capacity, stk);

    if(error == NULL_PTR_ON_DATA){
        fputs("Sorry, can't show elements of stack, cause ptr on stack = 0" , fp);
        fclose(fp);
    } 
    for(ssize_t index = 0; index < stk->capacity; index++){
        if(index < stk->size){
            fprintf(fp, "\t\t*[%lld] = %lld\n", index, stk->data[index]);
        }
        else{
            fprintf(fp, "\t\t [%lld] = %lld\n", index, stk->data[index]);
        }
    }
    fputs("\t}\n}\n\n\n", fp);
    fclose(fp);
}




const char *GetLineStatus(stack_status status){
    if(status == NOT_INITIALIZED){
        return "NOT_INITIALIZED";
    }
    else if(status == INITIALIZED){
        return "INITIALIZED";
    }
    else if(status == DESTROYED){
        return "DESTROYED";
    }
    else{
        return NULL;
    }
}

const char *GetLineError(errorList error){
    if(error == STACK_IS_OK){
        return "STACK_IS_OK";
    }
    if(error == STACK_WASNT_INITIALIZED){
        return "STACK_WASNT_INITIALIZED";
    }
    else if(error == STACK_WAS_DESTROYED_YET){
        return "STACK_WAS_DESTROYED_YET";
    }
    else if(error == NULL_PTR_ON_DATA){
        return "NULL_PTR_ON_DATA";
    }
    else if(error == NULL_PTR_ON_STACK){
        return "NULL_PTR_ON_STACK";
    }
    else if(error == SIZE_OVER_CAPACITY){
        return "SIZE_OVER_CAPACITY";
    }
    else if(error == CAPACITY_LOWER_MINIMAL){
        return "CAPACITY_LOWER_MINIMAL";
    }
    else if(error == CAPACITY_BELOW_ZERO){
        return "CAPACITY_BELOW_ZERO";
    }
    #ifdef CANARY_PROTECTION
    else if(error == CANARY_LEFT_ATTACKED){
        return "CANARY_LEFT_ATTACKED";
    }
    else if(error == CANARY_RIGHT_ATTACKED){
        return "CANARY_RIGHT_ATTACKED";
    }
    #endif
    else{
        return NULL;
    }
}


void StackPush(stack_t *stk, ssize_t elem){
    ErrorSucess err = RETURN_SUCCESS;
    err = StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
    printf("push\n");
    if(stk->size < stk->capacity){
        stk->data[stk->size] = elem;
        stk->size++;
    }
    else{
        StackResizeUp(stk);
        stk->data[stk->size] = elem;
        stk->size++;
    }
    err = StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
    if(err){
        printf("Error in StackPush(), check log file");
    }
}

void StackPop(stack_t *stk, ssize_t *elem){
    ErrorSucess err = RETURN_SUCCESS;
    err = StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
    assert(stk != NULL);
    assert(stk->data != NULL);

    if(stk->size > 0){
        stk->size--;
        if(4 * stk->size <= stk->capacity && stk->capacity > minCapacity){
            StackResizeDown(stk);
        }
        err = StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
        *elem = stk->data[stk->size];
        if(err){
            printf("Error in StackPop(), check log file");
        }
    }
    else{
        printf("Stack is empty:((\n");
        err = StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
        
        if(err){
            printf("Error in StackPop(), check log file");
        }
    }
}


void StackDestructor(stack_t *stk){
    ErrorSucess err = RETURN_SUCCESS;
    err = StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
    
    for(ssize_t index = 0; index < stk->capacity; index++){
        stk->data[index] = fiasco;
    }
    
    free(stk->data);
    stk->data = NULL;
    stk->size = fiasco;
    stk->capacity = fiasco;
    stk->status = DESTROYED;

    if(err){
        printf("Error in StackDestructor(), check log file");
    }   
}




void StackResizeUp(stack_t *stk){
    StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
    #ifdef CANARY_PROTECTION
    void *temp = realloc((stk->data - 1), sizeof(stk->data[0]) * ((stk->capacity + 1)* 2));
    if(temp == NULL){
        printf("It wasn't possible to increase stack((\n");
        StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
        return;
    }
    stk->data = (ssize_t *)temp + 1;
    stk->capacity *= 2;
    stk->canaryLeft = stk->data[0];
    stk->canaryRight = stk->data[stk->capacity + 1]; 
    #else
    void *temp = realloc((stk->data), sizeof(stk->data[0]) * (stk->capacity* 2));
    if(temp == NULL){
        printf("It wasn't possible to increase stack((\n");
        StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
        return;
    }
    stk->data = (ssize_t *)temp;
    stk->capacity *= 2;
    #endif
    StackVerify(stk, __FILE__, __FUNCTION__, __LINE__); 
}

void StackResizeDown(stack_t *stk){
    StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
    #ifdef CANARY_PROTECTION
    void *temp = realloc((stk->data - 1), sizeof(stk->data[0]) * ((stk->capacity + 1) / 2));
    if(temp == NULL){
        printf("It wasn't possible to increase stack((\n");
        StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
        return;
    }
    stk->data = (ssize_t *)temp + 1;
    stk->capacity /= 2;
    stk->canaryLeft = stk->data[0];
    stk->canaryRight = stk->data[stk->capacity + 1];
    #else
    void *temp = realloc(stk->data, sizeof(stk->data[0]) * (stk->capacity / 2));
    if(temp == NULL){
        printf("It wasn't possible to reduce stack((\n");
        StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
        return;
    }
    stk->data = (ssize_t *)temp;
    stk->capacity /= 2;
    #endif
    StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
}




void StackPrintf(stack_t *stk){
    StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
    printf("printf\n");
    printf("\n===================================================================================\n");
    printf("size = %lld\tcapacity = %lld\n", stk->size, stk->capacity);
    if (stk->size == 0){
        printf("Stack is empty:(\n");
    }
    for(ssize_t i = 0; i < stk->size; i++){
        printf("[%lld]\t", stk->data[i]);
    }
    printf("\n===================================================================================\n");
    StackVerify(stk, __FILE__, __FUNCTION__, __LINE__);
}