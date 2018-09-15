// SC-T-603-THYD Fall 2018. Project part I.
#ifndef LEXER_LEXER_H
#define LEXER_LEXER_H

#include<iostream>
#include<string>
#include "token.h"

class Lexer {
public:
    // Constructor, input stream to read and a symbol table are provided.
    Lexer( std::istream& is )
        : is_(is) {}

    // Get the next token from the input stream. Return token EOI on end of input.
    // This method could potentially throw IO-related exceptions.
    virtual void get_next( Token& token ) = 0;

    // Return a name given to the lexical analyzer (e.g., "flex" or "handmade").
    virtual std::string get_name() const = 0;

    // Destructor.
    virtual ~Lexer() {};

protected:
    std::istream& is_;
};

#endif //LEXER_LEXER_H
