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
"("             { return Tokentype::ptLParen; }
")"             { return Tokentype::ptRParen; }
"["             { return Tokentype::ptLBracket; }
"]"             { return Tokentype::ptRBracket; }
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

[a-zA-z]+       { return Tokentype::Identifier; }
.               { return Tokentype::ErrUnknown; }
%%
