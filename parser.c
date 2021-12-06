#include "value.h"
#include "linkedlist.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "talloc.h"

Value *parse(Value *tokens){
    //initialize variables
    Value *stack = makeNull();
    Value *S_expressions_list= makeNull();
    int parenMatch = 0; 

    while(!isNull(tokens)){
        if(tokens ->c.car ->type != CLOSE_TYPE){
            //add to counter if OPEN_TYPE
            if(tokens ->c.car ->type  == OPEN_TYPE){
                parenMatch++;
            }
            Value *temp; 
            temp = tokens->c.car; 
            stack = cons(temp,stack);
           //create temp value with size of Value

        }
        if(tokens ->c.car ->type == CLOSE_TYPE){
            //subtract from counter; 
            parenMatch--; 
            //initialize subtree;
            Value *subtree = makeNull();
            //if null type, exit program. 
            if(stack ->type== NULL_TYPE){
                printf("Syntax error"); 
                texit(1);
            }
            while(stack ->c.car -> type != OPEN_TYPE){

                if(stack ->type== NULL_TYPE){
                    printf("Syntax error"); 
                    texit(1);
                }

            //create temp value
                Value *temp = stack; 
                //pop top value from stack; 
                stack = stack ->c.cdr; 
                //point cdr of temp to subtree 
                temp ->c.cdr = subtree;
                //set subtree to temp
                subtree = temp; 
            }
                //index stack to pop OPEN_TYPE; 
                stack = stack ->c.cdr;
                //add subtree to top of stack; 
                stack = cons(subtree, stack);
                
        }

        //if # of OPEN_TYPEs and CLOSE_TYPEs match, move stack into S_expressions_list. 
        if(parenMatch == 0){
            Value *temp = stack; 
            stack = stack->c.cdr; 
            temp ->c.cdr = S_expressions_list; 
            S_expressions_list = temp; 
        }
        //index tokens
        tokens = tokens -> c.cdr;
        
    }
    ////if there are more OPEN_TYPEs than CLOSE_TYPES, return error. 
    if(parenMatch > 0){
        printf("Syntax error: Too many open parentheses!");
        texit(1);
    }
    //if there are more CLOSE_TYPEs than OPEN_TYPES, return error. 
    else if (parenMatch < 0){
        printf("Syntax error: Too many closed parentheses!");
        texit(1);
    }
    return reverse(S_expressions_list); 

}

//helper function for printTree that recursively calls itself if the car of the value is type cons. 
// void printTreeHelper(Value *value){
//     //print OPEN_TYPE for every CONS_TYPE to indicate start of subtree

void displayHelper(Value *val) {
  switch (val->type) {
      case INT_TYPE:
        printf("%i ", val->i);
        break;
      case DOUBLE_TYPE:
        printf("%f ", val->d);
        break;
      case STR_TYPE:
        printf("%s ", val->s);
        break;
      case BOOL_TYPE:
        printf("%s ", val->s);
        break;
      case SYMBOL_TYPE:
        printf("%s ", val->s);
        break;
      case DOT_TYPE:
        printf(". ");
        break;
      default:
        break; 
    }
}

void printTree(Value *tree) {
  if(isNull(tree->c.car)) {
    printf("()");
    return;
  }
  printf("(");
  Value *current = tree;
  Value *val;
  while (current->type != NULL_TYPE) {
    val = current->c.car;
    if (val->type != CONS_TYPE) {
      //simple case, just print the leaf
      displayHelper(val);
    } else {
      //need to recurse, cons type
      printTree(val);
    }
  current = current->c.cdr;
  }
  printf(") ");
}

