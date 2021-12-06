#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include <ctype.h>

//tests if something a symbol excluding + and -.
bool isSymbol(char charRead){
    if(charRead == '!' || charRead == '$' || charRead =='%' || charRead =='&' || charRead =='*' || charRead =='/' || charRead ==':' || charRead =='<' || charRead =='=' || charRead =='>' || charRead =='?' || charRead =='~' || charRead =='_' || charRead =='^'){
        return true;
    }
    else if(isalpha(charRead)){
        return true;
    }
    else{
        return false; 
    }
}
//main tokenization method
Value *tokenize() {
    char charRead;
    Value *list = makeNull();
    charRead = (char)fgetc(stdin);

    while (charRead != EOF) {
        if (charRead == ';') {
          while (charRead != '\n'){
            charRead = (char)fgetc(stdin);
          }   
          //open case
        }else if(charRead == '('){
          Value *value = (Value *)talloc(sizeof(Value)); 
          value -> type = OPEN_TYPE;
          char *text = "(";
          value -> s = talloc(sizeof(char)*(strlen(text) + 1));
          strcpy(value->s,text);
          list = cons(value,list);
          //close case
        }else if(charRead == ')'){
          Value *value = (Value *)talloc(sizeof(Value)); 
          value -> type = CLOSE_TYPE;
          char *text = ")";
          value -> s = talloc(sizeof(char)*(strlen(text) + 1));
          strcpy(value->s,text);
          list = cons(value,list);
        }
        //boolean case
        else if (charRead == '#'){
          charRead = (char)fgetc(stdin);
          if(charRead == 't'){
            Value *value = (Value *)talloc(sizeof(Value)); 
            value -> type = BOOL_TYPE;
            char *text = "#t";
            value -> s = talloc(sizeof(char)*(strlen(text) + 1));
            strcpy(value->s,text);
            list = cons(value,list);

          }else if (charRead == 'f'){
            Value *value = (Value *)talloc(sizeof(Value)); 
            value -> type = BOOL_TYPE;
            char *text = "#f";
            value -> s = talloc(sizeof(char)*(strlen(text) + 1));
            strcpy(value->s,text);
            list = cons(value,list);
          }
          else{
            printf("Syntax error: untokenizeable");
            texit(1);
          }
        }
        //string indicator
        else if(charRead == '"'){   
          //max 301 char per string      
          char *str = talloc(301);
          int index = 0;
          str[index] = charRead;
          charRead = (char)fgetc(stdin);
          //strcpy(str, "");
          while (charRead != '"'){
            //indexing and making string
            str[++index] = charRead;
             charRead = (char)fgetc(stdin);
          }
          //null terminator and quote
          str[++index] = '\"';
          str[++index] = '\0';
          Value *value = (Value *)talloc(sizeof(Value)); 
          value -> type = STR_TYPE;
          value -> s = talloc(sizeof(char)*(strlen(str) + 1));
          strcpy(value->s,str);
          list = cons(value,list);
        }
        //case where it could be integer, double, or symbol
        else if (isdigit(charRead) || charRead == '+' || charRead == '-') {
          bool isDouble = false; 
          bool hasSymbol = false; 
          double tempDouble; 
          int tempInt;
          char *endptr;

          char *tempString = talloc(301);
          int index = 0;
          tempString[index] = charRead;
          //test if first char is a symbol or double
          if(charRead == '.'){
            isDouble = true;
          }
          if (charRead == '+' || charRead == '-'){
            hasSymbol = true;
          }
          charRead = (char)fgetc(stdin);
          //test if the rest of the string is symbol or double
            while(charRead != ' '&& charRead != '\n' && charRead != ')'){
                if(charRead == '.'){
                    isDouble = true; 
                }
                else if(isSymbol(charRead)){
                    hasSymbol = true;
                }
                else if(isdigit(charRead)){
                  hasSymbol = false;
                }
                tempString[++index] = charRead;
                charRead = (char)fgetc(stdin);
            }
            tempString[++index] = '\0';
            if (isDouble && hasSymbol){
              printf("DOUBLE & SYMBOL Syntax error: untokenizeable");
              texit(1);
          }
          //case where string is double
            if(isDouble){
                tempDouble = strtod(tempString, &endptr);
                Value *value = (Value *)talloc(sizeof(Value)); 
                value -> type = DOUBLE_TYPE;
                value -> d = tempDouble;
                list = cons(value,list);
            }
            //case where string is integer
            else if (!hasSymbol){
                tempInt = strtol(tempString, &endptr, 10);
                Value *value = (Value *)talloc(sizeof(Value)); 
                value -> type = INT_TYPE;
                value -> i = tempInt; 
                list = cons(value,list);
            }
            //case where string is symbol
            else if(hasSymbol){
              Value *value = (Value *)talloc(sizeof(Value)); 
              value -> type = SYMBOL_TYPE;
              char *text = tempString;
              value -> s = talloc(sizeof(char)*(strlen(text) + 1));
              strcpy(value->s,text);
              list = cons(value,list);
            }
            //case where string is just close bracket
            if(charRead == ')'){
              Value *value = (Value *)talloc(sizeof(Value)); 
              value -> type = CLOSE_TYPE;
              char *text = ")";
              value -> s = talloc(sizeof(char)*(strlen(text) + 1));
              strcpy(value->s,text);
              list = cons(value,list);
          }
        }
        //if a string is purely a symbol
        else if (isSymbol(charRead)){            
            char *str = talloc(301);
            int index = 0;
            str[index] = charRead;
            charRead = (char)fgetc(stdin);
            while (charRead != ' ' && charRead != ')' && charRead != '\n'){
                str[++index] = charRead;
                charRead = (char)fgetc(stdin);
            }
            str[++index] = '\0';


          Value *value = (Value *)talloc(sizeof(Value)); 
          value -> type = SYMBOL_TYPE;
          char *text = str;
          value -> s = talloc(sizeof(char)*(strlen(text) + 1));
          strcpy(value->s,text);
          list = cons(value,list);

          if(charRead == ')'){
            Value *value = (Value *)talloc(sizeof(Value)); 
            value -> type = CLOSE_TYPE;
            char *text = ")";
            value -> s = talloc(sizeof(char)*(strlen(text) + 1));
            strcpy(value->s,text);
            list = cons(value,list);
          }
            
        }
            
        else if (charRead != ' ' && charRead != '\n') {
            printf("Syntax error: untokenizeable\n");
            texit(1);
        }
        charRead = (char)fgetc(stdin);
    }
  

    Value *revList = reverse(list);
    return revList;
}
//Print each case according to what type is is in the linked list.
void displayTokens(Value *list){
    while(!isNull(list)){
    switch (list->c.car->type) {
       case OPEN_TYPE:
          printf("(:open\n");
            break;
         case CLOSE_TYPE:
          printf("):close\n");
            break;
        case BOOL_TYPE:
          printf("%s:boolean\n", list->c.car->s);
            break;
        case INT_TYPE:
          printf("%i:integer\n", list->c.car->i);
            break;
        case DOUBLE_TYPE:
          printf("%f:double\n", list->c.car->d);
            break;
        case STR_TYPE:
            printf("%s:string\n", list->c.car->s);
            break;
        case SYMBOL_TYPE:
            printf("%s:symbol\n", list->c.car->s);
            break;
        default:
            break;
    }
    list = list ->c.cdr;       
  }
  printf("\n");

}

