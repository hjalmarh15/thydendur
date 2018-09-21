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
        default:
            if( isdigit(c_))
            {
                while(is_.good() && isdigit(c_)) {
                    token.type = Tokentype::IntValue;
                    token.lexeme.push_back(c_);
                    c_ = is_.get();
                }
            }
            else{
                token.type = Tokentype::ErrUnknown;
                token.lexeme.push_back(c_);
                is_.get(c_);
            }
            break;
    }
}

std::string HLexer::get_name() const {
    return "handmade";
}

HLexer::~HLexer()
{
}
