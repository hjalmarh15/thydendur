/* SC-T-603-THYD Fall 2018. Project part I. */
%option c++
%option noyywrap
%option yylineno
%{
#include "token.h"
%}
ws [ \t\r\n]
digit [0-9]
float [0-9]+\.[0-9]+([eE][+-]?[0-9]+)?

%%  IntValue, RealValue, BoolValue,

{ws}+             /* do nothing */
\/\/.*            /* do nothing */
\/\*(.|\n)*\*\/   /* do nothing */
{digit}+        { return Tokentype::IntValue;}
{float}         { return Tokentype::RealValue;}
"true"          { return Tokentype::BoolValue;}
"false"         { return Tokentype::BoolValue;}

"{"             { return Tokentype::ptLBrace; }
"}"             { return Tokentype::ptRBrace; }
"("             { return Tokentype::ptLParen; }
")"             { return Tokentype::ptRParen; }
"\["             { return Tokentype::ptLBracket; }
"\]"             { return Tokentype::ptRBracket; }
";"             { return Tokentype::ptSemicolon; }
","             { return Tokentype::ptComma; }

"=="            { return Tokentype::OpRelEQ; }
"!="            { return Tokentype::OpRelNEQ; }
"<"             { return Tokentype::OpRelLT; }
"<="            { return Tokentype::OpRelLTE; }
">"             { return Tokentype::OpRelGT; }
">="            { return Tokentype::OpRelGTE;}

"++"            { return Tokentype::OpArtInc; }
"--"            { return Tokentype::OpArtDec; }
"+"             { return Tokentype::OpArtPlus; }
"-"             { return Tokentype::OpArtMinus; }
"*"             { return Tokentype::OpArtMult; }
"/"             { return Tokentype::OpArtDiv; }
"%"             { return Tokentype::OpArtModulus; }

"&&"            { return Tokentype::OpLogAnd; }
"||"            { return Tokentype::OpLogOr; }
"!"             { return Tokentype::OpLogNot; }

"="             { return Tokentype::OpAssign; }

"class"         { return Tokentype::kwClass; }
"static"        { return Tokentype::kwStatic; }
"void"          { return Tokentype::kwVoid; }
"if"            { return Tokentype::kwIf; }
"else"          { return Tokentype::kwElse; }
"for"           { return Tokentype::kwFor; }
"return"        { return Tokentype::kwReturn; }
"break"         { return Tokentype::kwBreak; }
"continue"      { return Tokentype::kwContinue; }
"int"           { return Tokentype::kwInt; }
"real"          { return Tokentype::kwReal; }
"bool"          { return Tokentype::kwBool; }

[a-zA-Z_][a-zA-Z0-9_]*       { return Tokentype::Identifier; }
.               { return Tokentype::ErrUnknown; }

<<EOF>>           {  return Tokentype::EOI; }

%%
