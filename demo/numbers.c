#include <stdio.h>
#include "fsm.h"

int main(int argc, char *argv[])
{ 
  int num = 0;
  int sign = 1;
  char *in="";
  char *cur;
  
  if (argc>1) in = argv[1];

  cur = in;
  fsm { // Parse C Integers
    fprintf(stderr,"Never printed!!\n");
    fsmstate(START) {
      if (*cur == '+') {            cur++; fsmgoto(decimal); }
      if (*cur == '-') { sign = -1; cur++; fsmgoto(decimal); }
      if (*cur == '0') { 
            if (cur[1] == 'x' || cur[1] == 'X') {cur += 2; fsmgoto(hex); }
            else { cur ++; fsmgoto(octal); }
      }
      if ('1' <= *cur && *cur <= '9') fsmgoto(decimal);
      if (*cur == 'Z') fsmexit;
      if (*cur == 'z') break;
      cur++;
      if (*cur) fsmgoto(START);
    }

    fprintf(stderr,"neither this!!\n");

    fsmstate(octal) {
      fprintf(stderr,"octal: %c\n",*cur);
      if ('0' <= *cur && *cur <= '7') {
        num = num * 8 + (*cur - '0');
        cur++;
        fsmgoto(octal); 
      }
    }

    fsmstate(hex) {
      char hexdigit;
      hexdigit = '\0';
      fprintf(stderr,"hex: %c\n",*cur);
      if ('0' <= *cur && *cur <= '9') hexdigit = '0';
      else if ('A' <= *cur && *cur <= 'F') hexdigit = 'A' - 10;
      else if ('a' <= *cur && *cur <= 'f') hexdigit = 'a' - 10;
      if (hexdigit) {
        num = num * 16 + (*cur - hexdigit);
        cur++;
        fsmgoto(hex);
      }
    }

    fsmstate(decimal) {
      fprintf(stderr,"decimal: %c\n",*cur);
      if ('0' <= *cur && *cur <= '9') {
        num = num * 10 + (*cur - '0');
        cur++;
        fsmgoto(decimal); 
      }
    }

  }

  num = sign * num;

  fprintf(stderr,"'%s' -> %d\n",in,num);
}
