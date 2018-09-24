// SC-T-603-THYD Fall 2018. Project part I.
#include "hlexer.h"

using namespace std;

HLexer::HLexer( std::istream& is  )
    : Lexer( is ), line_no_(1)
{
    is_.get( c_ );
}

//TODO Ekki gleyma að prófa deilingu í flexer!!!

void HLexer::get_next( Token& token )
{
    token.lexeme.clear();

    consumeWhitespace();


    //check for comments.
    while(c_ == '/')
    {
        char c = c_;
        is_.get(c_);
        if(c_ == '*')
        {
            is_.get(c_);
            while ( is_.good())
            {
                if( c_ == '\n' )
                {
                    ++line_no_;
                }
                if( c_  == '*')
                {
                    is_.get(c_);
                    if(c_ == '/')
                    {
                        is_.get(c_);
                        break;
                    }
                }
                is_.get(c_);
            }
            if ( !is_.good() )
            {
                token.type = Tokentype::EOI;
                return;
            }
        }
        else if(c_ == '/')
        {
            while ( is_.good())
            {
                if( c_ == '\n' )
                {
                    break;
                }
                is_.get(c_);
            }
            if ( !is_.good() ) {
                token.type = Tokentype::EOI;
            }
        }
        else
        {
            token.type = Tokentype::OpArtDiv;
            token.lexeme.push_back(c);

            return;
        }
        consumeWhitespace();
    }

    token.line = line_no_;

    if ( !is_.good() ) {
        token.type = Tokentype::EOI;
        return;
    }

    //switch case for different single character tokens.
    //Some of them need to check the next token, i.e. != and &&
    switch ( c_ ) {
        case '{': {
            token.type = Tokentype::ptLBrace;
            token.lexeme.push_back(c_);
            is_.get(c_);
            break;
        }
        case '}': {
            token.type = Tokentype::ptRBrace;
            token.lexeme.push_back(c_);
            is_.get(c_);
            break;
        }
        case '(':
            token.type = Tokentype::ptLParen;
            token.lexeme.push_back(c_);
            is_.get(c_);
            break;
        case ')':
            token.type = Tokentype::ptRParen;
            token.lexeme.push_back(c_);
            is_.get(c_);
            break;
        case '[':
            token.type = Tokentype::ptLBracket;
            token.lexeme.push_back(c_);
            is_.get(c_);
            break;
        case ']':
            token.type = Tokentype::ptRBracket;
            token.lexeme.push_back(c_);
            is_.get(c_);
            break;
        case ';':
            token.type = Tokentype::ptSemicolon;
            token.lexeme.push_back(c_);
            is_.get(c_);
            break;
        case ',':
            token.type = Tokentype::ptComma;
            token.lexeme.push_back(c_);
            is_.get(c_);
            break;
        case '=': {
            token.lexeme.push_back(c_);
            is_.get(c_);
            if (c_ == '=') {
                token.type = Tokentype::OpRelEQ;
                token.lexeme.push_back(c_);
                is_.get(c_);
            } else {
                token.type = Tokentype::OpAssign;
            }
            break;
        }
        case '!': {
            token.lexeme.push_back(c_);
            is_.get(c_);
            if (c_ == '=') {
                token.type = Tokentype::OpRelNEQ;
                token.lexeme.push_back(c_);
                is_.get(c_);
            } else {
                token.type = Tokentype::OpLogNot;
            };
            break;
        }
        case '<': {
            token.lexeme.push_back(c_);
            is_.get(c_);
            if(c_ == '=') {
                token.type = Tokentype::OpRelLTE;
                token.lexeme.push_back(c_);
                is_.get(c_);
            }
            else {
                token.type = Tokentype::OpRelLT;
            };
            break;
        }
        case '>': {
            token.lexeme.push_back(c_);
            is_.get(c_);
            if(c_ == '=') {
                token.type = Tokentype::OpRelGTE;
                token.lexeme.push_back(c_);
                is_.get(c_);
            }
            else {
                token.type = Tokentype::OpRelGT;
            }
            break;
        }
        case '+': {
            token.lexeme.push_back(c_);
            is_.get(c_);
            if(c_ == '+') {
                token.type = Tokentype::OpArtInc;
                token.lexeme.push_back(c_);
                is_.get(c_);
            }
            else {
                token.type = Tokentype::OpArtPlus;
            }
            break;
        }
        case '-': {
            token.lexeme.push_back(c_);
            is_.get(c_);
            if(c_ == '-') {
                token.type = Tokentype::OpArtDec;
                token.lexeme.push_back(c_);
                is_.get(c_);
            }
            else {
                token.type = Tokentype::OpArtMinus;
            }
            break;
        }
        case '*':
            token.type = Tokentype::OpArtMult;
            token.lexeme.push_back(c_);
            is_.get(c_);
            break;
        case '%':
            token.type = Tokentype::OpArtModulus;
            token.lexeme.push_back(c_);
            is_.get(c_);
            break;
        case '|':
            token.lexeme.push_back(c_);
            is_.get(c_);
            if(c_ == '|') {
                token.type = Tokentype::OpLogOr;
                token.lexeme.push_back(c_);
                is_.get(c_);
            }
            else {
                token.type = Tokentype::ErrUnknown;
            }
            break;
        case '&':
            token.lexeme.push_back(c_);
            is_.get(c_);
            if(c_ == '&') {
                token.type = Tokentype::OpLogAnd;
                token.lexeme.push_back(c_);
                is_.get(c_);
            }
            else {
                token.type = Tokentype::ErrUnknown;
            }
            break;
        default:
            //if we get a number we see if it's an int or a real value
            if( isdigit(c_))
            {
                constructNumber(token);
                break;
            }
            //if we get a letter or _ we construct the following string
            else if(isalpha(c_) || c_ == '_') {
                char c = c_;
                constructString(c,token );
                break;
            }
            else {
                token.type = Tokentype::ErrUnknown;
                token.lexeme.push_back(c_);
                is_.get(c_);
                break;
            }
    }
}

std::string HLexer::get_name() const {
    return "handmade";
}

void HLexer::consumeWhitespace(){
    while ( is_.good() && isspace(c_) ) {
        if ( c_ == '\n' ) { ++line_no_; }
        is_.get(c_);
    }
}


/*
 * This function constructs a string and then compares it to known keywords
 * If no keyword is found, the string is an identifier.
 */
void HLexer::constructString(char c, Token& token) {
    token.lexeme.push_back(c);
    is_.get(c_);
    while(isalpha(c_) || isdigit(c_) || c_ == '_') {
        token.lexeme.push_back(c_);
        is_.get(c_);
    }

    if(token.lexeme == "class")
        token.type = Tokentype::kwClass;
    else if(token.lexeme == "static")
        token.type = Tokentype::kwStatic;
    else if(token.lexeme == "void")
        token.type = Tokentype::kwVoid;
    else if(token.lexeme == "if")
        token.type = Tokentype::kwIf;
    else if(token.lexeme == "else")
        token.type = Tokentype::kwElse;
    else if(token.lexeme == "for")
        token.type = Tokentype::kwFor;
    else if(token.lexeme == "return")
        token.type = Tokentype::kwReturn;
    else if(token.lexeme == "break")
        token.type = Tokentype::kwBreak;
    else if(token.lexeme == "continue"s)
        token.type = Tokentype::kwContinue;
    else if(token.lexeme == "int")
        token.type = Tokentype::kwInt;
    else if(token.lexeme == "real")
        token.type = Tokentype::kwReal;
    else if(token.lexeme == "bool")
        token.type = Tokentype::kwBool;
    else if(token.lexeme == "true" || token.lexeme == "false")
        token.type = Tokentype::BoolValue;
    else
        token.type = Tokentype::Identifier;
}


/*
 * This function constructs a number. If the . is found, we construct a fraction
 */
void HLexer::constructNumber(Token& token) {
    token.type = Tokentype::IntValue;
    while(is_.good() && isdigit(c_)) {
        token.lexeme.push_back(c_);
        is_.get(c_);
        if(c_ == '.'){
            token.type = Tokentype::RealValue;
            constructFraction(token);

        }
    }

}


/*
 * Constructs a fraction if . is found in a number. Can have optional exponent
 */
void HLexer::constructFraction(Token& token) {
    token.lexeme.push_back(c_);
    is_.get(c_);
    //Already have the '.' in lexeme, now loop until not digit anymore
    while(is_.good() && isdigit(c_)) {
        token.lexeme.push_back(c_);
        is_.get(c_);
    }
    //Check exponent part
    if(c_ == 'E' || c_ == 'e'){
        token.lexeme.push_back(c_);
        is_.get(c_);
        while(is_.good() && isdigit(c_)) {
            token.lexeme.push_back(c_);
            is_.get(c_);
        }
    }
}

HLexer::~HLexer()
{
}
