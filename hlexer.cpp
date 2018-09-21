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

    while ( is_.good() && isspace(c_) ) {
        if ( c_ == '\n' ) { ++line_no_; }
        is_.get(c_);
    }

    if(c_ == '/')
    {
        char c = c_;
        is_.get(c_);
        if(c_ == '*')
        {
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
                    ++line_no_;
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
        }
        return;
    }

    token.line = line_no_;

    if ( !is_.good() ) {
        token.type = Tokentype::EOI;
        return;
    }

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
            char c = c_;
            is_.get(c_);

            if (c_ == '=') {
                token.type = Tokentype::OpRelEQ;
                token.lexeme.push_back(c);
                token.lexeme.push_back(c_);
                is_.get(c_);
            } else {
                token.type = Tokentype::OpAssign;
                token.lexeme.push_back(c);
            }
            break;
        }
        case '!': {
            char c = (char) c_;
            is_.get(c_);
            if (c_ == '=') {
                token.type = Tokentype::OpRelNEQ;
                token.lexeme.push_back(c);
                token.lexeme.push_back(c_);
                is_.get(c_);
            } else {
                token.type = Tokentype::OpLogNot;
                token.lexeme.push_back(c);
            };
            break;
        }
        case '<': {
            char c = (char) c_;
            is_.get(c_);
            if(c_ == '=') {
                token.type = Tokentype::OpRelLTE;
                token.lexeme.push_back(c);
                token.lexeme.push_back(c_);
                is_.get(c_);
            }
            else {
                token.type = Tokentype::OpRelLT;
                token.lexeme.push_back(c);
            };
            break;
        }
        case '>': {
            char c = (char) c_;
            is_.get(c_);
            if(c_ == '=') {
                token.type = Tokentype::OpRelGTE;
                token.lexeme.push_back(c);
                token.lexeme.push_back(c_);
                is_.get(c_);
            }
            else {
                token.type = Tokentype::OpRelGT;
                token.lexeme.push_back(c);
            }
            break;
        }
        case '+': {
            char c = c_;
            is_.get(c_);
            if(c_ == '+') {
                token.type = Tokentype::OpArtInc;
                token.lexeme.push_back(c);
                token.lexeme.push_back(c_);
                is_.get(c_);
            }
            else {
                token.type = Tokentype::OpArtPlus;
                token.lexeme.push_back(c);
            }
            break;
        }
        case '-': {
            char c = c_;
            is_.get(c_);
            if(c_ == '-') {
                token.type = Tokentype::OpArtDec;
                token.lexeme.push_back(c);
                token.lexeme.push_back(c_);
                is_.get(c_);
            }
            else {
                token.type = Tokentype::OpArtMinus;
                token.lexeme.push_back(c);
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
        default:
            if( isdigit(c_))
            {
                while(is_.good() && isdigit(c_)) {
                    token.type = Tokentype::IntValue;
                    token.lexeme.push_back(c_);
                    c_ = is_.get();
                }
            }
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
    else if(token.lexeme == "continue")
        token.type = Tokentype::kwContinue;
    else if(token.lexeme == "int")
        token.type = Tokentype::kwInt;
    else if(token.lexeme == "real")
        token.type = Tokentype::kwReal;
    else if(token.lexeme == "bool")
        token.type = Tokentype::kwBool;
    else
        token.type = Tokentype::Identifier;
}
HLexer::~HLexer()
{
}
