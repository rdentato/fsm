//.  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
//.  SPDX-PackageHomePage: https://repo.dentato.com/fsm
//.  SPDX-License-Identifier: MIT

/* THIS BLOCK WILL DISAPPEAR!!
**
*/

#include <stdio.h>

#include "fsm.h"

void uncomment(FILE *in, FILE *out)
{
  fsm {
    int c; // Can't initialize here!

    fsmstate(START) {
      fsmgoto(code);
    }

    fsmstate(code) {
      if ((c = fgetc(in)) == EOF) fsmexit; 
      if (c == '/') fsmgoto(slash);
      fsmgoto(code_char);
    }

    fsmstate(code_char) {
      fputc(c, out); 
      
      if (c == '"')  fsmgoto(string);
      if (c == '\'') fsmgoto(literal);
      fsmgoto(code); 
    }

    fsmstate(slash) {
      if ((c = fgetc(in)) == EOF) fsmexit; 
      if (c == '/') fsmgoto(line_comment);
      if (c == '*') fsmgoto(block_comment);
      fputc('/',out); 
      fsmgoto(code_char);
    }
     
    fsmstate(string) {
      if ((c = fgetc(in)) == EOF) fsmexit; 
      if ((c == '\\')) fsmgoto(escaped);
      fputc(c,out);
      if (c == '"') fsmgoto(code);
      fsmgoto(string);
    }
   
    fsmstate(literal) {
      if ((c = fgetc(in)) == EOF) fsmexit; 
      if ((c == '\\')) fsmgoto(escaped);
      fputc(c,out);
      if (c == '\'') fsmgoto(code);
      fsmgoto(literal);
    }

    fsmstate(escaped) {
      int quote;
      quote = c;
      fputc(quote, out);
      if ((c = fgetc(in)) == EOF) fsmexit; 
      fputc(c, out);
      if (quote == '"') fsmgoto(string);
      fsmgoto(literal);
    }

    fsmstate(line_comment) {
      if ((c = fgetc(in)) == EOF) fsmexit; 
      if (c == '\n') fsmgoto(code);
      fsmgoto(line_comment);
    }

    fsmstate(block_comment) {
      if ((c = fgetc(in)) == EOF) fsmexit; 
      if (c == '*') fsmgoto(star);
      fsmgoto(block_comment);
    }

    fsmstate(star) {
      if ((c = fgetc(in)) == EOF) fsmexit; 
      if (c == '/') fsmgoto(code);
      fsmgoto(block_comment);
    }
  }
}

int main(int argc, char *argv[])
{
 #if 0 
  char *test="// Not a comment!";
  char t='\"'; // not a string
  /* DELETE ME */
 #endif

  uncomment(stdin,stdout);
}
