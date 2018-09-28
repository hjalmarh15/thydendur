// SC-T-603-THYD Fall 2018. Project part I.
#ifndef LEXER_FLEXER_H
#define LEXER_FLEXER_H

#include <FlexLexer.h>
#include "lexer.h"

class FLexer : public Lexer {
public:

    FLexer( std::istream& is )
            : Lexer( is ), lexer_( &is ){
    }

    virtual void get_next( Token& token ) {
        int token_no = lexer_.yylex();
        token.type = (token_no == 0) ? Tokentype::EOI
                                     : static_cast<Tokentype>(token_no);
        token.lexeme = lexer_.YYText();
        token.line = lexer_.lineno();
    }

    virtual std::string get_name() const {
        return "flex";
    }

    virtual ~FLexer() {
    }

private:
    yyFlexLexer lexer_;
};

#endif //LEXER_FLEXER_H