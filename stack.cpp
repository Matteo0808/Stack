#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DEBUG_MODE
#define CANARY_PROTECTION
//#define HASH_PROTECTION

#ifdef DEBUG_MODE
#define DEBUG(...) __VA_ARGS__
#else
#define DEBUG(...)
#endif

#define StackConstructor(capacity, ...) _StackConstructor(capacity, DEBUG(__VA_ARGS__,) __FILE__, __FUNCTION__, __LINE__)
#define StackVerify(stk) _StackVerify(stk, __FILE__, __FUNCTION__, __LINE__)
#define StackPrintf(stk) _StackPrintf(stk, __FILE__, __FUNCTION__, __LINE__)
#define StackPush(stk, elem) _StackPush(stk, elem, __FILE__, __FUNCTION__, __LINE__)
#define StackPop(stk, elem) _StackPop(stk, elem, __FILE__, __FUNCTION__, __LINE__)
#define StackDestructor(stk) _StackDestructor(stk, __FILE__, __FUNCTION__, __LINE__)

#define STKDUMP(stk) StackDump(stk, STACK_IS_OK, __FILE__, __FUNCTION__, __LINE__)

const ssize_t fiasco = 0xF1A5C0;    //15836608
const ssize_t minCapacity = 5;

#ifdef CANARY_PROTECTION
const ssize_t canaryL = 48099263;   //10110111011110111110111111
const ssize_t canaryR = 86118464;  //101001000100001000001000000
#endif

enum stack_status{
    NOT_INITIALIZED = 0,
    INITIALIZED = 1,
    DESTROYED = 2
};

enum ErrorSuccess{
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
    #ifdef HASH_PROTECTION
    , HASH_WAS_CHANGED = 10
    #endif
};

struct stack_t;

stack_t *_StackConstructor(ssize_t capacity,
                      DEBUG( const char *crName,) const char *crFile,
                      const char *crFunc, ssize_t crLine);
void _StackDestructor(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine);

ErrorSuccess _StackVerify(stack_t *stk, const char *verFile, const char *verFunc, ssize_t verLine);
errorList StackCheckErrors(stack_t *stk);
void _StackPrintf(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine);
void StackDump(stack_t *stk, errorList error, const char *dmpFile, const char *dmpFunc, ssize_t dmpLine);
const char *GetLineStatus(stack_status status);
const char *GetLineError(errorList status);


void _StackPush(stack_t *stk, ssize_t elem, const char *callFile, const char *callFunc, ssize_t callLine);
void _StackPop(stack_t *stk, ssize_t *elem, const char *callFile, const char *callFunc, ssize_t callLine);

void StackResizeUp(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine);
void StackResizeDown(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine);

#ifdef HASH_PROTECTION
ssize_t StackHashCounter(stack_t *stk);
#endif

//TODO - resizedown
//NOTE - done

//TODO - strdump
//NOTE - completed

//TODO - define debug
//NOTE - completed

//TODO - verificator
//NOTE - completed

//TODO - verificator for constructor
//ANCHOR - ASK DED
//NOTE - nahui nado

//TODO - message in console: look in log file
//NOTE - completed

//TODO - struct after main 
//ANCHOR - ASK DED
//completed

//TODO - canary protection - in progress
//NOTE - completed

//TODO - hash protection

//TODO - switchi
//NOTE - completed 

//TODO - code style
//NOTE - completed

//TODO - obertki
//NOTE completed


/// @brief now for lld only 
int main(){

    DEBUG(printf("debug0\n");)

    stack_t *stk = StackConstructor(minCapacity, "stk");

    
    DEBUG(printf("debug1\n");)

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
    
    DEBUG(const char *createName;)
        
    const char *createFile;
    const char *createFunc;
    ssize_t createLine;
    
    #ifdef CANARY_PROTECTION

        ssize_t *canaryLeft;
        ssize_t* data;
        ssize_t *canaryRight;

    #else

        ssize_t *data;

    #endif

    #ifdef HASH_PROTECTION

        ssize_t hash;

    #endif

    ssize_t size;
    ssize_t capacity;
    stack_status status = NOT_INITIALIZED;
};

#ifdef HASH_PROTECTION
ssize_t StackHashCounter(stack_t *stk){
    stk->hash = 0;    // чтобы не учитывать поле

    char *temp = (char *)stk->data;
    size_t hash = 5381;
    
    for(int i = 0; i < sizeof(stack_t); i++){
        hash = hash + (hash << 5) + *temp;
    }
    return hash;
}
#endif

stack_t *_StackConstructor(ssize_t capacity,
                      DEBUG( const char *crName,) const char *crFile,
                      const char *crFunc, ssize_t crLine){

    stack_t *stk  = (stack_t *)calloc(1, sizeof(stack_t));
    assert(stk != NULL);

    DEBUG(stk->createName = crName;)

    stk->createFile = crFile;
    stk->createFunc = crFunc;
    stk->createLine = crLine;
    
    //printf("pizda\n");
    #ifdef HASH_PROCTECTION

        stk->hash = StackHashCounter(stk);

    #endif

    #ifdef CANARY_PROTECTION

        ssize_t *temp = (ssize_t *)calloc(capacity + 2, sizeof(ssize_t));
        assert(temp != NULL);

        stk->canaryLeft = temp;
        *(stk->canaryLeft) = canaryL;
        //printf("canaryLeft = %lld\n", *(stk->canaryLeft));

        stk->canaryRight = temp + (capacity + 1);
        *(stk->canaryRight) = canaryR;
        //printf("canaryRight = %lld\n", *(stk->canaryRight));

        stk->data = temp + 1;

    #else

        stk->data = (ssize_t *)calloc(capacity, sizeof(ssize_t));
    
        #endif

    stk->capacity = capacity;
    stk->size = 0;
    stk->status = INITIALIZED;

    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, crFile, crFunc, crLine);
    
    if(err){
        printf("Error in StackConstructor(), check log file\n");
        return NULL;
    }

    return stk;
}


ErrorSuccess _StackVerify(stack_t *stk, const char *verFile, const char *verFunc, ssize_t verLine){
    
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
    if(stk->size > stk->capacity + 1){
        return SIZE_OVER_CAPACITY;
    }

    #ifdef CANARY_PROTECTION

        if(*(stk->canaryLeft) != canaryL){
            return CANARY_LEFT_ATTACKED;
        }
        if(*(stk->canaryRight) != canaryR){
            return CANARY_RIGHT_ATTACKED;
        }

    #endif

    #ifdef HASH_PROTECTION

        if(stk->hash != StackHashCounter(stk)){
            return HASH_WAS_CHANGED;
        }

    #endif

    else{
        return STACK_IS_OK;
    }
}





void StackDump(stack_t *stk, errorList error, const char *dmpFile, const char *dmpFunc, ssize_t dmpLine){
    
    FILE *fp = fopen("log.log", "a");

    fputs("Hello! StackDump is working!\n", fp);
    fprintf(fp, "Function was called by %s() at %s:%lld\n", dmpFunc, dmpFile, dmpLine);
    
    if(error){
        fprintf(fp, "StackDump was called with error code: %s\n", GetLineError(error));
    }
    // fclose(fp);
    // return;

    DEBUG(printf("file0\n");)

    if(error == NULL_PTR_ON_STACK){
        fputs("StackDump detected critical error: ptr on stack variable = 0\n", fp);
        fputs("Program won't be working correctly\n", fp);
        
        return;
    }

    DEBUG(printf("file1\n");)

    if(stk->status == NOT_INITIALIZED){
        fputs("Stack wasn't initialized\n", fp);
        
        return;
    }

    DEBUG(printf("file2\n");)

    #ifdef DEBUG_MODE
    
        fprintf(fp, "stack_t \"%s\" (located at %p) created by %s() at %s:%lld\n{\n",
                stk->createName, stk, stk->createFunc, stk->createFile, stk->createLine);
    #else

        fprintf(fp, "stack_t (located at %p) created by %s() at %s:%lld\n{\n",
                stk, stk->createFunc, stk->createFile, stk->createLine);
    
    #endif

    fprintf(fp, "\tstack_status = %s\n", GetLineStatus(stk->status));
    #ifdef CANARY_PROTECTION
            
        fprintf(fp, "\tLeft Canary: = %lld at [%p]\n", *(stk->canaryLeft), stk->canaryLeft);
        fprintf(fp, "\tRight Canary: = %lld at [%p]\n", *(stk->canaryRight), stk->canaryRight);

    #endif
    //fprintf(fp, "StackDump was called with error code: %s\n", GetLineError(error));
    fprintf(fp, "\tsize = %lld\n\tcapacity = %lld\n\tdata = %p\n\t{\n", stk->size, stk->capacity, stk);


    #ifdef HASH_PROTECTION
            
        fprintf(fp, "Hash sum = %lld", stk->hash);

    #endif

    DEBUG(printf("file3\n");)

    if(error == NULL_PTR_ON_DATA){
        fputs("Sorry, can't show elements of stack, cause ptr on stack = 0" , fp);
        fclose(fp);
    }

    DEBUG(printf("file4\n");)

    for(ssize_t index = 0; index < stk->capacity; index++){

        if(index < stk->size){
            fprintf(fp, "\t\t[%p]\t*[%lld] = %lld\n", stk->data + index, index, stk->data[index]);
        }
        else{
            fprintf(fp, "\t\t[%p]\t [%lld] = %lld\n", stk->data + index, index, stk->data[index]);
        }
    }

    fputs("\t}\n}\n\n\n", fp);
    fclose(fp);
}




const char *GetLineStatus(stack_status status){
    
    switch(status){

        case NOT_INITIALIZED:
            return "NOT_INITIALIZED";

        case INITIALIZED:
            return "INITIALIZED";

        case DESTROYED:
            return "DESTROYED";

        default:
            return NULL;
    }
}

const char *GetLineError(errorList error){
    
    switch(error){
    
        case STACK_IS_OK:
            return "STACK_IS_OK";

        case STACK_WASNT_INITIALIZED:
            return "STACK_WASNT_INITIALIZED";

        case STACK_WAS_DESTROYED_YET:
            return "STACK_WAS_DESTROYED_YET";

        case NULL_PTR_ON_DATA:
            return "NULL_PTR_ON_DATA";

        case NULL_PTR_ON_STACK:
            return "NULL_PTR_ON_STACK";

        case SIZE_OVER_CAPACITY:
            return "SIZE_OVER_CAPACITY";

        case CAPACITY_LOWER_MINIMAL:
            return "CAPACITY_LOWER_MINIMAL";

        case CAPACITY_BELOW_ZERO:
            return "CAPACITY_BELOW_ZERO";

        #ifdef CANARY_PROTECTION

            case CANARY_LEFT_ATTACKED:
                return "CANARY_LEFT_ATTACKED";

            case CANARY_RIGHT_ATTACKED:
                return "CANARY_RIGHT_ATTACKED";

        #endif

        #ifdef HASH_PROTECTION

            case HASH_WAS_CHANGED:
                return "HASH_WAS_CHANGED";

        #endif
        default:
            return NULL;
    }
}


void _StackPush(stack_t *stk, ssize_t elem, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, callFile, callFunc, callLine);
    
    if(stk->size < stk->capacity){
        stk->data[stk->size] = elem;
        stk->size++;
    }
    else{
        StackResizeUp(stk, callFile, callFunc, callLine);

        stk->data[stk->size] = elem;
        stk->size++;
    }

    err = _StackVerify(stk, callFile, callFunc, callLine);

    if(err){
        printf("Error in StackPush(), check log file\n");
    }
}

void _StackPop(stack_t *stk, ssize_t *elem, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;
    
    err = _StackVerify(stk, callFile, callFunc, callLine);

    if(stk->size > 0){
        stk->size--;

        if(4 * stk->size <= stk->capacity && stk->capacity > minCapacity){
            StackResizeDown(stk, callFile, callFunc, callLine);
        }
        err = _StackVerify(stk, callFile, callFunc, callLine);

        *elem = stk->data[stk->size];

        if(err){
            printf("Error in StackPop(), check log file\n");
        }
    }
    else{
        printf("Stack is empty:((\n");

        err = _StackVerify(stk, callFile, callFunc, callLine);
        
        if(err){
            printf("Error in StackPop(), check log file\n");
        }
    }
}


void _StackDestructor(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, callFile, callFunc, callLine);
    
    for(ssize_t index = 0; index < stk->capacity; index++){
        stk->data[index] = fiasco;
    }

    #ifdef CANARY_PROTECTION

        stk->data[-1] = fiasco;
        stk->data[stk->capacity + 1];
        free(stk->data - 1);
        stk->data = NULL;

    #else

        free(stk->data);
        stk->data = NULL;

    #endif

    stk->size = fiasco;
    stk->capacity = fiasco;
    stk->status = DESTROYED;

    if(err){
        printf("Error in StackDestructor(), check log file\n");
    }   
}



void StackResizeUp(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, callFile, callFunc, callLine);
    
    #ifdef CANARY_PROTECTION
    
        ssize_t *temp = (ssize_t *)realloc((stk->data - 1), sizeof(stk->data[0]) * ((stk->capacity + 1)* 2));
        
        if(temp == NULL){
            printf("It isn't possible to increase stack((\n");

            err = _StackVerify(stk, callFile, callFunc, callLine);

            if(err){
                printf("Error in StackResizeUp(), check log file\n");
            }  
        
            return;
        }

        stk->capacity *= 2;

        stk->canaryLeft = temp;
        *(stk->canaryLeft) = canaryL;

        stk->canaryRight = temp + (stk->capacity + 1);
        *(stk->canaryRight) = canaryR;
        STKDUMP(stk);

        stk->data = temp + 1;

    #else

        void *temp = realloc((stk->data), sizeof(stk->data[0]) * (stk->capacity* 2));

        if(temp == NULL){
            printf("It wasn't possible to increase stack((\n");

            err = _StackVerify(stk, callFile, callFunc, callLine);

            if(err){
                printf("Error in StackResizeUp(), check log file\n");
            }  

            return;
        }
        stk->data = (ssize_t *)temp;
        stk->capacity *= 2;

    #endif

    err = _StackVerify(stk, callFile, callFunc, callLine);
    
    if(err){
        printf("Error in StackResizeUp(), check log file\n");
    }
}

void StackResizeDown(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, callFile, callFunc, callLine);
    
    #ifdef CANARY_PROTECTION

        ssize_t *temp = (ssize_t *)realloc((stk->data - 1), sizeof(stk->data[0]) * ((stk->capacity + 1) / 2));
        if(temp == NULL){
            printf("It wasn't possible to increase stack((\n");

            err = _StackVerify(stk, callFile, callFunc, callLine);

            if(err){
                printf("Error in StackResizeDown(), check log file\n");
            }
            return;
        }

        stk->capacity /= 2;

        stk->canaryLeft = temp;
        *(stk->canaryLeft) = canaryL;

        stk->canaryRight = temp + (stk->capacity + 1);
        *(stk->canaryRight) = canaryR;
        STKDUMP(stk);
        
        stk->data = temp + 1;

    #else

        void *temp = realloc(stk->data, sizeof(stk->data[0]) * (stk->capacity / 2));
        if(temp == NULL){
            printf("It wasn't possible to reduce stack((\n");

            err = _StackVerify(stk, callFile, callFunc, callLine);

            if(err){
                printf("Error in StackResizeDown(), check log file\n");
            }

            return;
        }

        stk->data = (ssize_t *)temp;
        stk->capacity /= 2;

    #endif

    err = _StackVerify(stk, callFile, callFunc, callLine);

    if(err){
        printf("Error in StackResizeDown(), check log file\n");
    }

    return;
}



void _StackPrintf(stack_t *stk, const char *callFile, const char *callFunc, ssize_t callLine){
    
    ErrorSuccess err = RETURN_SUCCESS;

    err = _StackVerify(stk, callFile, callFunc, callLine);

    
    printf("\n===================================================================================\n");
    
    #ifdef CANARY_PROTECTION
    
        printf("%p\t%p\n", &(stk->canaryLeft), &(stk->canaryRight));

    #endif

    printf("size = %lld\tcapacity = %lld\n", stk->size, stk->capacity);
    
    if (stk->size == 0){
        printf("Stack is empty:(\n");
    }
    
    for(ssize_t i = 0; i < stk->size; i++){
        printf("[%lld]\t", stk->data[i]);
    }

    printf("\n===================================================================================\n");
    
    err = _StackVerify(stk, callFile, callFunc, callLine);

    if(err){
        printf("Error in StackPrintf(), check log file\n");
    }

    return;
}