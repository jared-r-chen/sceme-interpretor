#include "value.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "talloc.h"

// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car(Value *list){
  return list -> c.car; 
}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list){
    return list -> c.cdr; 
}


Value *makeNull(){
  Value *value = (Value *)talloc(sizeof(Value));
  value->type = NULL_TYPE;
  return value;
 
}

// Create a new CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr){
    Value *value = (Value *)talloc(sizeof(Value));  
    value-> type = CONS_TYPE;
    value-> c.car = newCar;
    value-> c.cdr = newCdr;
    return value;
}

bool isNull(Value *value){
  assert(value != NULL);
  if (value->type == NULL_TYPE){
    return true;
  }
  return false;
}

int length(Value *value){
  int counter = 0;
  while(!isNull(value)){
    counter++;
    value = value->c.cdr;
  }
  return counter;
}

// Return a new list that is the reverse of the one that is passed in. No stored
// data within the linked list should be duplicated; rather, a new linked list
// of CONS_TYPE nodes should be created, that point to items in the original
// list.

Value *reverse(Value *list){
  Value *tail = makeNull();
  Value *prev = tail;

  if (length(list) == 0){
    return tail;
  }
  while(!(isNull(list))){
    Value *value = (Value *)talloc(sizeof(Value));
    *value = *list;
    value -> c.car = list -> c.car;
    value -> c.cdr = prev;
    prev = value;
    list = list -> c.cdr;    
  }
  return prev;
}


// Display the contents of the linked list to the screen in some kind of
// readable format


void display(Value *list){
  while(!isNull(list)){
    switch (list->c.car->type) {
        case INT_TYPE:
          printf("%i ", list->c.car->i);
            break;
        case DOUBLE_TYPE:
          printf("%f ", list->c.car->d);
            break;
        case STR_TYPE:
            printf("%s ", list->c.car->s);
            break;
        default:
            break;
    }
    list = list ->c.cdr;       
  }
  printf("\n");
}
