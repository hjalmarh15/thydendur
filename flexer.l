/* SC-T-603-THYD Fall 2018. Project part I. */
%option c++
%option noyywrap
%option yylineno
%{
#include "token.h"
%}
ws [ \t\r\n]
%%
{ws}+             /* do nothing */
"{"             { return Tokentype::ptLBrace; }
"}"             { return Tokentype::ptRBrace; }
"class"         { return Tokentype::kwClass; }
"Program"       { return Tokentype::
.               { return Tokentype::ErrUnknown; }
%%
 // No need to add any code here.
