#include "value.h"
#include "linkedlist.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "talloc.h"
#include "parser.h"
#include "interpreter.h"
#include "tokenizer.h"

//print function to display the tree. 
void print(Value *tree) {
   switch (tree->type)  {
     case INT_TYPE: {
        printf("%i \n", tree -> i);
        break;
     }
    case DOUBLE_TYPE: {
         printf("%f \n", tree -> d);
         break;
     }
    case BOOL_TYPE: {
        if(!strcmp(tree->s, "#t")){
           printf("#t\n");
        }
        else{
           printf("#f\n");
        }
        break;
     }  
    case STR_TYPE: {
      
         printf("%s \n", tree -> s);
         break;
     }
     case NULL_TYPE:{
        printf("()\n"); 
        break;
     }
     case VOID_TYPE:{
        //printf("()\n"); 
        break;
     }
     case SYMBOL_TYPE:{
         printf("%s \n", tree->s);
         break;
     }
     case CLOSURE_TYPE:{
       printf("#<procedure>\n");
       break;
     }
     case DOT_TYPE:{
       printf("."); 
       break;
     }

     default: 
        // printf("missing print function\n");
        // printf("Syntax error\n");
        break;
    }
  }

void evaluationError(){
    printf("Evaluation error\n"); 
    texit(1);
}

int length_frame(Frame *frame) {
  int count = 0;
  while (frame != NULL) {
    count++;
    frame = frame->parent;
  }
  return count;
}

//function to check the number of arguments in a special form 
void numArgsChecker(Value *args, int numArgs){
    int counter = 0; 
    if (args -> type != CONS_TYPE){
      evaluationError();
    }
    while(!isNull(args)){
      counter++; 
      args = cdr(args);
    }
    if(counter != numArgs){
      evaluationError(); 
    }
}

/*
 * Adds a binding between the given name
 * and the input function. Used to add
 * bindings for primitive funtions to the top-level
 * bindings list.
 */
void bind(char *name, Value *(*function)(Value *), Frame *frame) {

  Value *symbol = makeNull(); 
  symbol -> type = STR_TYPE; 
  char *text = name;
  symbol ->s = talloc(sizeof(char)*(strlen(text) + 1));
  strcpy(symbol -> s, name);

  Value *primValue = makeNull(); 
  primValue ->type = PRIMITIVE_TYPE; 

  primValue -> pf = function; 

  Value *newBinding = makeNull(); 
  newBinding = cons(primValue, newBinding); 
  newBinding = cons(symbol, newBinding); 
  frame -> bindings = cons(newBinding, frame ->bindings); 
}


Value *primitiveAdd(Value *args) {
  //printf("IN primitive add Func \n");
  Value* res = makeNull();
  int counter = 0; 
  bool hasDouble = false; 
  double result = 0; 

  while(!isNull(args)){
    counter++; 
    //if types are not int or double, throw error
    if(car(args)->type != INT_TYPE && car(args)->type != DOUBLE_TYPE){
      evaluationError();
    }
    else if(car(args)->type == INT_TYPE){
      result = car(args) -> i + result; 
    }
    else if(car(args)->type == DOUBLE_TYPE){
      result = car(args) -> d + result; 
      hasDouble = true; 
    }
    args = cdr(args); 
  }
//if there is just (+)
  if(counter == 0){
    res->type = INT_TYPE; 
    int temp = (int) result;
    res->i = 0; 
  }
  //these cases convert between int and double since this function works for both
  else if(hasDouble){
    res->type = DOUBLE_TYPE; 
     res-> d = result; 
  }
  else{
    res->type = INT_TYPE; 
    int temp = (int) result;
    res-> i = temp; 
  }
  return res; 
}
//very similar to primplus but some extra checks
Value *primitiveMinus(Value *args){

  Value* res = makeNull();
  int counter = 0; 
  bool hasDouble = false; 
  double result = 0; 

  while(!isNull(args)){
    counter++; 

    if(car(args)->type != INT_TYPE && car(args)->type != DOUBLE_TYPE){
      evaluationError();
    }
    //just number in a - sequence is positive
    else if (counter == 1){
      if(car(args)->type == INT_TYPE){
        result = car(args) -> i;
      }
      else if(car(args)->type == DOUBLE_TYPE){
        result = car(args) -> d; 
      }
    }
    else if(car(args)->type == INT_TYPE){
      result = result - car(args) -> i; 
    }
    else if(car(args)->type == DOUBLE_TYPE){
      result = result - car(args) -> d; 
      hasDouble = true; 
    }
    args = cdr(args); 
  }
//- throws error instead of 0
  if(counter == 0){
    evaluationError();
    // res->type = INT_TYPE; 
    // int temp = (int) result;
    // res->i = 0; 
  }
  else if(hasDouble){
    res->type = DOUBLE_TYPE; 
    if(counter == 1){
      res-> d = result * -1; 
    }
    else{
      res-> d = result; 
    }
  }
  else{
    res->type = INT_TYPE; 
    int temp = (int) result;
    if(counter == 1){
      res-> i = temp * -1; 
    }
    else{
      res-> i = temp; 
    }
  }
  return res; 
}
// < sign evaluation.
Value *primitiveLess(Value *args){
  // numArgsChecker(args,2);
  int counter = 0; 
  Value *res = makeNull(); 
  res -> type = BOOL_TYPE; 
  res -> s = talloc(3);
  
 Value *temp = args;
  while(!isNull(temp)){
    counter++; 
    if(car(args)->type != INT_TYPE && car(args)->type != DOUBLE_TYPE){
      evaluationError();
    }
    
    temp = cdr(temp);
  }
  //copy in values since it breaks otherwise for some reason
  char *true_ = "#t";
  char *false_ = "#f";
    
    Value *next = car(cdr(args));
    if(car(args) -> i > next -> i){
      strcpy(res -> s, false_);
    }
    else if(car(args)-> i < next -> i){
      strcpy(res -> s, true_);
    }
    else{
      strcpy(res -> s, false_);
    }
  return res; 
}

Value *primitiveGreat(Value *args){
  int counter = 0; 

  Value *res = makeNull(); 
  res -> type = BOOL_TYPE; 
  res -> s = talloc(3);
  Value *temp = args; 
  while(!isNull(temp)){
    counter++; 
    if(car(temp)->type != INT_TYPE && car(temp)->type != DOUBLE_TYPE){
      evaluationError();
    }
    temp = cdr(temp);
  }

  char *true_ = "#t";
  char *false_ = "#f";
  
    Value *next = car(cdr(args));
    if(car(args) -> i > next-> i){
      strcpy(res -> s, true_);
    }
    else if(car(args)-> i < next-> i){
      strcpy(res -> s, false_);
    }
    else{
      strcpy(res -> s, false_);
    }
  
  return res; 
}
// = check operation
Value *primitiveEqual(Value *args){
  
  numArgsChecker(args,2);
  int counter = 0; 
  Value *res = makeNull(); 
  res -> type = BOOL_TYPE; 
  res -> s = talloc(3);

 Value* next = car(cdr(args));

  char *true_ = "#t";
  char *false_ = "#f";

  Value *temp = args;
  while(!isNull(temp)){
    if(car(args)->type != INT_TYPE && car(args)->type != DOUBLE_TYPE){
      evaluationError();
    }
    temp = cdr(temp);
  }
  
  if(car(args) -> type == DOUBLE_TYPE){
    int temp = (int) next -> d;
    next -> type = INT_TYPE; 
    next -> type = temp; 
  }
  if (next -> type == DOUBLE_TYPE){
    int temp = (int) next ->d;
    next -> type = INT_TYPE; 
    next -> type = temp; 
  } 
  if(car(args) -> i == next-> i){
    strcpy(res -> s, true_);
  }
  else{
    strcpy(res -> s, false_);
  }
  return res; 
}
//division operation
Value *primitiveDivide(Value *args){
  double div = 0; 
  bool doublePresent = false;
  numArgsChecker(args,2);
  Value *next = car(cdr(args));

  //first and second values are the values in the expression
  Value* res = makeNull(); 
  Value *first = makeNull();
  Value *second = makeNull();
  
  //convert to double for all operations
  if(car(args) -> type == INT_TYPE){
    first -> type = INT_TYPE;
    first -> d = (car(args) -> i)  * 1.0;
  }
  else if(car(args) -> type == DOUBLE_TYPE){
    first -> type = DOUBLE_TYPE;
    first -> d = car(args) -> d;
    doublePresent = true;
  }
  else{
    //not int or double type.
    evaluationError();
  }

  if(next -> type == INT_TYPE){
    second -> type = INT_TYPE;
    second -> d = (next -> i) * 1.0;
  }
  else if(next -> type == DOUBLE_TYPE){
    second -> type = DOUBLE_TYPE;
    second -> d = next -> d;
    doublePresent = true;
  }
  else{
    //not int or double type.
    evaluationError();
  }

  double opResult = (first -> d) / (second -> d);
  if(doublePresent){
    res -> type = DOUBLE_TYPE;
    res -> d = opResult;
  }
  //if result is not a whole number, res = double.
  else if((int)(first -> d) % (int)(second -> d) == 0){
    res -> type = INT_TYPE;
    res -> i = (int)opResult; 
  }
  else{
    res -> type = DOUBLE_TYPE;
    res -> d = opResult;
  }

    return res; 
  //if number does not divide evenly, return double
  //if number does return evenly, return int
}
//simple multiplication operation
Value *primitiveMultiply(Value *args){

  Value* res = makeNull();
  int counter = 0; 
  bool hasDouble = false; 
  double result = 1; 

  while(!isNull(args)){
    // printf("in primitive add func while loop \n");
    counter++; 

    if(car(args)->type != INT_TYPE && car(args)->type != DOUBLE_TYPE){
      evaluationError();
    }

    else if(car(args)->type == INT_TYPE){
      result = car(args) -> i * result; 
    }
    else if(car(args)->type == DOUBLE_TYPE){
      result = car(args) -> d * result; 
      hasDouble = true; 
    }
    args = cdr(args); 
  }
  if(hasDouble){
    res->type = DOUBLE_TYPE; 
     res-> d = result; 
  }
  else{
    res->type = INT_TYPE; 
    int temp = (int) result;
    res-> i = temp; 
  }
  return res; 
} 
//since modulo can only take in ints, this is quite simple
Value *primitiveModulo(Value *args){

  int resNum = 0; 
  Value *res = makeNull(); 
  res -> type = INT_TYPE;

  Value *first = car(args);
  Value *second = car(cdr(args));
  
  resNum = first -> i % second -> i; 
  res -> i = resNum; 

  return res; 
}

//check is something is null 
Value  *primitiveNull(Value *args){

  numArgsChecker(args,1);
  
  Value *res = makeNull(); 
  res -> type = BOOL_TYPE; 
  res -> s = talloc(3);

  char *true_ = "#t";
  char *false_ = "#f";
  

  if (isNull(car(args))){
    strcpy(res -> s, true_);
  }
  else{
    strcpy(res -> s, false_);
  }
  
  return res; 
}

Value *primitiveCar(Value *args){
  numArgsChecker(args, 1); 
  
  if(car(args)->type != CONS_TYPE){
    evaluationError(); 
  }
  return car(car(args)); 
}

Value *primitiveCdr(Value *args){
  numArgsChecker(args, 1);
  
  if(car(args)->type != CONS_TYPE){
    evaluationError(); 
  }

  if(length(car(args)) == 1){
    return makeNull();
  }

  Value *temp= car(cdr(car(args))); 

  if(temp ->type == DOT_TYPE){
    //printf("IN DOT CASE\n");
    return car(cdr(cdr(car(args))));
  }

  return cdr(car(args)); 
}

//add dot when you cons an item with non-list
//if second paramter is not a cons
//DOT_TYPE
//where do we do this? in print or the primitiveCons function? 

Value *primitiveCons(Value *args){

  // printf("in cons function\n");
  numArgsChecker(args, 2); 
  Value* divider = makeNull(); 
  divider -> type = DOT_TYPE; 
  Value *combine = makeNull(); 

  if(car(cdr(args)) -> type == NULL_TYPE){
    combine = cons(car(args), car(cdr(args)));
  }

  else if(car(cdr(args)) -> type != CONS_TYPE){
      combine = cons(car(cdr(args)), combine);
      combine = cons(divider, combine);
      combine = cons(car(args) , combine); 
  }
  else{
    // printf("in else case inside cons function\n");
    Value *temp = cons(car(args), car(cdr(args)));
    combine = temp;

  }
  
  return combine; 
}

//takes in: primitive function, args, frame
Value *evalPrimitive( Value *primPointer, Value *args, Frame *frame){
  //printf("in eval primitive func \n");
  Value *evalArgs = makeNull();
  
  while(!isNull(args)){
    // printf("evalprimitive: in while loop \n");
    Value *temp = eval(car(args), frame);
    evalArgs = cons(temp , evalArgs); 
    args = cdr(args); 
  }

  return (primPointer -> pf)(reverse(evalArgs)); 
}


//creates global frame that has no parent. This is the starting point of the interpreter class.
void interpret(Value *tree){
  //create global frame
  Frame *globalFrame = (Frame *)talloc(sizeof(Frame));
  globalFrame ->parent = NULL; 
  globalFrame ->bindings = makeNull();
  
  bind("+", primitiveAdd, globalFrame); 
  bind("-", primitiveMinus, globalFrame); 
  bind("<", primitiveLess, globalFrame);
  bind(">", primitiveGreat, globalFrame);
  bind("=", primitiveEqual, globalFrame);
  bind("/", primitiveDivide, globalFrame);
  bind("*", primitiveMultiply, globalFrame);
  bind("null?", primitiveNull, globalFrame);
  bind("car", primitiveCar, globalFrame);
  bind("cdr", primitiveCdr, globalFrame);
  bind("cons", primitiveCons, globalFrame);
  bind("modulo", primitiveModulo, globalFrame);
  
  while(!isNull(tree)){
      Value *temp = eval(car(tree), globalFrame); 
      //maybe check for void type 
      if (temp->type != VOID_TYPE) {
        if(temp->type == CONS_TYPE){
          // printf("cons type detected for printTree\n");
          printTree(temp);
        }
        else{
          // printf("interpreter before print eval tree\n");
          print(temp);
          // printf("interpreter after print eval tree\n");
        }
      }
      //printf("")
      tree = cdr(tree);
  }
}

//check if the value is in the binding list of that frame 
//if there is a match return value cell of that value
//since the frame is the stack, check the parent of the frame. 
//if value is not in any of the binding list of the frames, return error. 
Value *lookUpSymbol(Value *tree, Frame *frame){

  Value *cur = frame -> bindings;

  //if you reach top of the list and nothing is found error.
  if (frame == NULL){
    evaluationError();
  }
  
  while(!isNull(cur)){
      
    if (!strcmp(tree ->s, car(car(cur))->s)){
      //printf("symbol exists\n"); 
      //printf("symbol is: %s\n", tree ->s);
      return car(cdr(car(cur)));
    }
    cur = cdr(cur);
  }

  if(isNull(cur) && frame -> parent == NULL){
    evaluationError();
  }

  //carr recursivly to go up frame tree.
  return lookUpSymbol(tree, frame -> parent);
}


//checks params and returns result of an if statement
Value *evalIf(Value *args, Frame *frame){
  // printf("in eval If function\n");
  numArgsChecker(args, 3);

  Value *temp = eval(car(args), frame);
  
  if(!strcmp(temp->s, "#t")){
    // printf("in true case\n");
    return eval(car(cdr(args)), frame); 
    }
  else{
    // printf("in false case\n");
    return eval(car(cdr(cdr(args))),frame);
  }
}

//evaluates let case.
Value *evalLet(Value *args, Frame *frame){
  
  //sets up new frames in the let expression.
    Frame *newFrame = (Frame *)talloc(sizeof(Frame));
    newFrame -> parent = frame; 
    newFrame -> bindings = makeNull(); 

    //the things you want to bind.
    Value *exp = car(args);

    //edge case checks
    if(isNull(exp) && isNull(cdr(args))){
      evaluationError();
    }
    if(exp -> type != CONS_TYPE && exp -> type != NULL_TYPE){
      evaluationError();
    }
    
    //sets all the bindings according to exp
    while(!isNull(exp)){
        // printf("in while loop \n");
        Value *thisExp = car(exp);
        numArgsChecker(thisExp, 2);
        Value *symbol = car(thisExp);
        
        if(thisExp -> type != NULL_TYPE ){

            if(symbol -> type != SYMBOL_TYPE){
              evaluationError(); 
            }
            //checks for duplicates in cur frame.
            Value *temp = newFrame -> bindings;
            while(!isNull(temp)){
              if(!strcmp(symbol -> s, car(temp) -> c.car -> s)){
                //printf("dup found error thrown\n");
                evaluationError();
              }
              temp = cdr(temp);
            }
          
          Value *newBinding = cons(eval(car(cdr(thisExp)), frame), makeNull());
          newBinding = cons(symbol, newBinding); 
          newFrame -> bindings = cons(newBinding, newFrame -> bindings);
          exp = cdr(exp);
      }
    }
    //evaluates the body
    Value *body = cdr(args);
    if(body -> type == NULL_TYPE){
      //printf("printf body type null\n");
      evaluationError();
    }
    //indexes through body so only the last thing is evaluated
    while(!isNull(cdr(body))){
      Value *temp = eval(car(body), newFrame); 
      body = cdr(body);
    }

    return eval(car(body), newFrame);
}
//takes symbol and args, binds body, and adds the new binding to cur frame.
Value *evalDefine(Value *args, Frame *frame){
  Value *thisExp = args; 
  numArgsChecker(thisExp, 2);
  
  if(car(args) -> type != SYMBOL_TYPE){
    evaluationError();
  }
  Value *body = cons((eval(car(cdr(args)), frame)), makeNull());
  

  Value *currBinding = cons(car(args), body);
  
  frame -> bindings = cons(currBinding, frame -> bindings);
  Value *temp = (Value *)talloc(sizeof(Value));
  temp -> type = VOID_TYPE;

  return temp;
}

//if a lambda is seen, a new value CLOSURE type is created and added to cur frame.
Value *createClosure(Value *args, Frame *frame){
  Value *thisExp = args; 
  numArgsChecker(thisExp, 2);

  Value *temp = car(args);

  //check if Formal params are symbols
  while(!isNull(temp)){
    if (car(temp)-> type != SYMBOL_TYPE){
      evaluationError();
    }
    temp = cdr(temp);
  }

  temp = car(args);
  Value *tempList = makeNull();

  while(!isNull(temp)){
    Value *tempPointer = tempList;
    while(!isNull(tempPointer)){
      if(!strcmp(car(temp) -> s, car(tempPointer) -> s)){
        //printf("dup found\n");
        evaluationError();
      }
      tempPointer = cdr(tempPointer);
    }
    tempList = cons(car(temp), tempList);
    temp = cdr(temp);
  }

  
  Value *currClosure = (Value *)talloc(sizeof(Value));
  currClosure -> type = CLOSURE_TYPE;
  currClosure ->cl.paramNames = car(thisExp);
  currClosure ->cl.functionCode = car(cdr(thisExp));
  currClosure ->cl.frame = frame;

  
  return currClosure;
}
//takes in a closure and args. puts these two together so the function in the closure is run with the args applied to it.
Value *apply(Value *function, Value *args, Frame *frame){

  Value *closure = function;
  Frame *newFrame = (Frame *)talloc(sizeof(Frame));
  newFrame -> parent = closure ->cl.frame; 
  newFrame -> bindings = makeNull(); 
  Value *formalParams = closure ->cl.paramNames; 
  Value *actualParams = args; 

  Value *tempValue = args;
  int counter = 0;

  while (!isNull(tempValue)){
    counter++;
    tempValue = cdr(tempValue);
  }

  while(!isNull(actualParams)){
    Value *body = cons(eval(car(actualParams), frame), makeNull());
    body = cons(car(formalParams), body);
    
    newFrame -> bindings = cons(body, newFrame -> bindings);

    actualParams = cdr(actualParams);
    formalParams = cdr(formalParams);
  }
  
   Value *functionBody = closure ->cl.functionCode;
   
  return eval(functionBody, newFrame);
   //bind formal and actual parameters
}


// Implement and and or. Note that and and or are special forms; make sure you understand why. You should be able to create cases with side-effect generating code (like set!) where the special form nature of and and or matters. Each of and and or should work properly as long as they have at least 1 argument.
// and and or both have specified behavior if you supply no arguments to them, or if you supply arguments that aren't Booleans. We will not test any of those cases.

Value *evalAnd(Value *args, Frame *frame){

  Value *cur = args;
  Value *evaluatedExp;

  while(!isNull(cur)){
    evaluatedExp = eval(car(cur), frame);
    if (evaluatedExp -> type == BOOL_TYPE && !strcmp(evaluatedExp->s, "#f")){
      return evaluatedExp;
    }
    cur = cdr(cur);
  }
  return evaluatedExp;

}

Value *evalOr(Value *args, Frame *frame){


  Value *cur = args;
  Value *evaluatedExp;

  while(!isNull(cur)){
    evaluatedExp = eval(car(cur), frame);
    if (evaluatedExp -> type == BOOL_TYPE && !strcmp(evaluatedExp->s, "#t")){
      return evaluatedExp;
    }
    cur = cdr(cur);
  }
  return evaluatedExp;
}

//begin should evaluate each of its arguments and return the result of the last argument. Read the specification in Dybvig. If there's nothing to return (i.e. zero arguments to begin), return VOID_TYPE.
Value *evalBegin(Value *args, Frame *frame){
  Value *temp = args; 
  Value *res = makeNull();
  int counter = 0; 
  while(!isNull(temp)){ 
    counter++;
    res = eval(car(temp), frame); 
    temp = cdr(temp);
  }

  if(counter == 0){
    res -> type = VOID_TYPE; 
    return res; 
  }
  return res; 
}

//eval conditional case, like a nested if loop
Value *evalCond(Value *args, Frame *frame){
  Value *temp = args; 
  Value *res = makeNull();
  
  //can have mutiple cond expressions
  while(!isNull(temp)){

     Value *oneCond = car(temp);
     //else case reached, execute else
    if(!strcmp(car(oneCond) ->s, "else")){
       
      return eval(car(cdr(oneCond)), frame); 
    }

    Value *evaluatedArgs = eval(car(oneCond), frame); 

    if(!strcmp(evaluatedArgs->s, "#t")){
      return eval(car(cdr(oneCond)), frame); 
    }
    
    temp = cdr(temp);
  }

 res -> type = VOID_TYPE; 
  return res; 
}

//similar struct to lookupsymbol, looks up the frames and tries to edit a bindings value.
Value *evalSet(Value *args, Frame *frame, Value *exp){

  Value *thisExp = args; 
  numArgsChecker(thisExp, 2);
  Value *cur = frame -> bindings;
  Value *expression = car(cdr(thisExp));
  Value *evaluatedExp = exp; 

  if(isNull(evaluatedExp)){
    evaluatedExp = eval(expression, frame); 
  }

  if(car(args) -> type != SYMBOL_TYPE){
    evaluationError();
  }
  if (frame == NULL){
    evaluationError();
  }
  if(isNull(cur) && frame -> parent == NULL){
    evaluationError();
  }
  while(!isNull(cur)){
    if (!strcmp( car(thisExp) ->s, car(car(cur))->s)){
      cdr(car(cur)) ->c.car = evaluatedExp; 
      Value *res = makeNull();
      res -> type = VOID_TYPE;
      return res;
      
    }
    cur = cdr(cur);
  }
  //recursive step to go to parent frame
  return evalSet(thisExp, frame ->parent, evaluatedExp);
}

//every binding has a new frame so when eval left to right, right args have access to left args. Does not allow recusion.
Value *evalLetStar(Value *args, Frame *frame){
    //sets up new frames in the let expression.
    
    //the things you want to bind.
    Value *exp = car(args);
    
    //edge case checks
    if(isNull(exp) && isNull(cdr(args))){
      evaluationError();
    }
    if(exp -> type != CONS_TYPE && exp -> type != NULL_TYPE){
      evaluationError();
    }
    Frame *curFrame = frame;

    while(!isNull(exp)){

        Value *thisExp = car(exp);
        numArgsChecker(thisExp, 2);
        Value *symbol = car(thisExp);
        
        if(thisExp -> type != NULL_TYPE ){

            if(symbol -> type != SYMBOL_TYPE){
              evaluationError(); 
            }
            Frame *tempFrame = (Frame *)talloc(sizeof(Frame));
            tempFrame -> parent = curFrame; 
            tempFrame -> bindings = makeNull(); 

          Value *newBinding = cons(eval(car(cdr(thisExp)), curFrame), makeNull());
          newBinding = cons(symbol, newBinding); 
          tempFrame -> bindings = cons(newBinding, tempFrame -> bindings);
          exp = cdr(exp);
          curFrame = tempFrame;
      }
    }

    //evaluates the body
    Value *body = cdr(args);
    if(body -> type == NULL_TYPE){
      evaluationError();
    }
    //indexes through body so only the last thing is evaluated
    while(!isNull(cdr(body))){
      Value *temp = eval(car(body), curFrame); 
      body = cdr(body);
    }
    return eval(car(body), curFrame);
}

// Create a new frame env’ with parent env.
// Evaluate each of e1, …, en in environment env’.
// After all of thesex evaluations are complete, add bindings to bind each xi to the evaluated result of ei (from step 2) in environment env’.
// Evaluate body1, …, bodyn sequentially in env’, returning the result of evaluating bodyn.
Value *evalLetRec(Value *args, Frame *frame){
  //printf("in let rec\n");
  Value *rawBinding = car(args); 
  Value *evaluatedArgsList = makeNull();
  Value *symbolList = makeNull();

  Frame *newFrame = (Frame *)talloc(sizeof(Frame));
  newFrame -> parent = frame; 
  newFrame -> bindings = makeNull(); 
  while(!isNull(rawBinding)){
    Value *thisExp = car(rawBinding);
    numArgsChecker(thisExp, 2);
    Value* symbol = car(thisExp);
      if(thisExp -> type != NULL_TYPE ){

            if(symbol -> type != SYMBOL_TYPE){
              evaluationError(); 
            }
        Value *curValue = car(cdr(thisExp));
        Value *evaluatedValue = eval(curValue, newFrame); 
      //printf("check 1\n");

      evaluatedArgsList = cons(evaluatedValue, evaluatedArgsList);
      rawBinding = cdr(rawBinding);
      symbolList = cons(symbol, symbolList);
      }
  }
  // printf("after raw bindings while loop\n");
  while(!isNull(evaluatedArgsList)){
    // printf("binding loop\n");
    Value *currBinding = makeNull();
    currBinding = cons(car(evaluatedArgsList), currBinding); 
    
    currBinding = cons(car(symbolList), currBinding);

    newFrame -> bindings = cons(currBinding, newFrame -> bindings);

    evaluatedArgsList = cdr(evaluatedArgsList);
    symbolList = cdr(symbolList); 
  }

  Value *body = cdr(args);
 if(body -> type == NULL_TYPE){
  
      // printf("body is null\n");
      evaluationError();
    }
    //indexes through body so only the last thing is evaluated
    while(!isNull(cdr(body))){
      // printf("in while loooop\n");
      Value *temp = eval(car(body), newFrame); 
      body = cdr(body);
    }
    // printf("outside while loop\n");
    //printf("checker\n");
    return eval(car(body), newFrame);
}

//evaluates expressions by type, this calls all other functions in this program
Value *eval(Value *tree, Frame *frame) {
  // printf("in eval case\n");
   switch (tree->type)  {
     case INT_TYPE: {
        return tree; 
        break;
     }
    case DOUBLE_TYPE: {
         return tree; 
         break;
     }
    case BOOL_TYPE: {
        return tree; 
        break;
     }  
    case STR_TYPE: {
         return tree; 
         break;
     }
    case SYMBOL_TYPE: {
        return lookUpSymbol(tree, frame);
     }  
    case VOID_TYPE:{
      return tree;
    }
    case CLOSURE_TYPE:{
      
      return tree;
    }
    case CONS_TYPE: {
        Value *first = car(tree);
        Value *args = cdr(tree);
         
        if (!strcmp(first->s,"if")) {
          return evalIf(args,frame);
        }
        //if the car of the cons type is an "let" expression, evaluation the case accordingly. 
        
        else if(!strcmp(first->s, "let")){
            // printf("let case\n");
            return evalLet(args,frame);
        }
        //if the car of the cons type is a "quote" expression, return the cdr of the cons without evaluating it.
        else if(!strcmp(first->s, "quote")){

            numArgsChecker(args, 1); 
  
            return car(args);
        }

        else if(!strcmp(first->s, "define")){
            return evalDefine(args,frame); 
        }
        else if (!strcmp(first->s, "lambda")){ 
            return createClosure(args,frame);
        }
        
        else if(!strcmp(first->s, "and")){
          //  printf("K: in and case\n");
          return evalAnd(args, frame);
        }
        else if(!strcmp(first->s, "or")){
          //  printf("K: in or case\n");
          return evalOr(args,frame);
        }
        else if (!strcmp(first->s, "begin")){
          //printf("K: in begin case\n");
          return evalBegin(args,frame);
        }
        else if(!strcmp(first->s, "cond")){
          // printf("K: in cond case\n");
          return evalCond(args,frame);
        }
        else if(!strcmp(first->s, "set!")){
         // printf("K: in set! case\n");
          return evalSet(args, frame, makeNull());
        }
        else if(!strcmp(first->s, "let*")){
         // printf("K: in let* case\n");
          return evalLetStar(args, frame);
        }
        else if(!strcmp(first->s, "letrec")){
        //  printf("K: in letrec case\n");
          return evalLetRec(args, frame);
        }

        // if the expression is not a recognized special form, return error
        else {
          Value *evalFirst = eval(first,frame);
          if (evalFirst->type == CLOSURE_TYPE){
            return apply(evalFirst, args, frame);
          }
          else{
            // printf("evalPrimitive Case\n");
            return evalPrimitive(evalFirst, args, frame);
          }
        }
     }
    case NULL_TYPE:
        return tree;
    default:
    //printf("default case reached\n");
        evaluationError();
        return tree;
        break;
    } 
}
