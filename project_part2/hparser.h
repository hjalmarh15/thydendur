
#ifndef DECAFPARSER_HPARSER_H
#define DECAFPARSER_HPARSER_H

#include <list>
#include "parser.h"

#define OUTPUT_TT(tt) case decaf::token_type::tt: os << #tt; break;

inline std::ostream& operator<<(std::ostream& os, decaf::token_type type) {
    switch ( type ) {
        OUTPUT_TT(Identifier)
        OUTPUT_TT(IntValue)
        OUTPUT_TT(RealValue)
        OUTPUT_TT(BoolValue)
        OUTPUT_TT(OpRelEQ)
        OUTPUT_TT(OpRelNEQ)
        OUTPUT_TT(OpRelLT)
        OUTPUT_TT(OpRelLTE)
        OUTPUT_TT(OpRelGT)
        OUTPUT_TT(OpRelGTE)
        OUTPUT_TT(OpArtInc)
        OUTPUT_TT(OpArtDec)
        OUTPUT_TT(OpArtPlus)
        OUTPUT_TT(OpArtMinus)
        OUTPUT_TT(OpArtMult)
        OUTPUT_TT(OpArtDiv)
        OUTPUT_TT(OpArtModulus)
        OUTPUT_TT(OpLogAnd)
        OUTPUT_TT(OpLogOr)
        OUTPUT_TT(OpLogNot)
        OUTPUT_TT(OpAssign)
        OUTPUT_TT(kwClass)
        OUTPUT_TT(kwStatic)
        OUTPUT_TT(kwVoid)
        OUTPUT_TT(kwIf)
        OUTPUT_TT(kwElse)
        OUTPUT_TT(kwFor)
        OUTPUT_TT(kwReturn)
        OUTPUT_TT(kwBreak)
        OUTPUT_TT(kwContinue)
        OUTPUT_TT(kwInt)
        OUTPUT_TT(kwReal)
        OUTPUT_TT(kwBool)
        OUTPUT_TT(ptLBrace)
        OUTPUT_TT(ptRBrace)
        OUTPUT_TT(ptLBracket)
        OUTPUT_TT(ptRBracket)
        OUTPUT_TT(ptLParen)
        OUTPUT_TT(ptRParen)
        OUTPUT_TT(ptSemicolon)
        OUTPUT_TT(ptComma)
        OUTPUT_TT(EOI)
        OUTPUT_TT(ErrUnknown)
    }
    return os;
}

class HParser : public Parser {
private:

    struct Token {
        yy::parser_decaf::token_type type;   // Type of the token.
        std::string lexeme;                  // Matched lexeme.
        int line;                            // Line number in file where token is.
        int col;                             // Column number in file where token is.
    };

    Token token_;

    static void get_next( Token& token )
    {
        yy::parser_decaf::symbol_type st( yylex() );
        token.type = st.token();
        if (token.type == yy::parser_decaf::token_type::Identifier ||
            token.type == yy::parser_decaf::token_type::IntValue ||
            token.type == yy::parser_decaf::token_type::RealValue ||
            token.type == yy::parser_decaf::token_type::BoolValue ||
            token.type == yy::parser_decaf::token_type::ErrUnknown ) {
            token.lexeme = st.value.as<std::string>();
        }
        else {
            token.lexeme.clear();
        }
        token.line = st.location.begin.line;
        token.col = st.location.begin.column;
    }

    void error( decaf::token_type type_expected )
    {
        std::cout << "Syntax error (line "
                  << token_.line << ", col " << token_.col << "): expected token "
                  << type_expected << ", but got token "
                  << token_.type <<  " (" << token_.lexeme << ")." << std::endl;
        exit(-1);
    }

    void match( decaf::token_type type )
    {
        if ( token_.type == type ) {
            get_next( token_ );
        }
        else { error( type ); }
    }

public:

    HParser( FILE* file, bool debug_lexer, bool debug_parser )
            : Parser(file, debug_lexer, debug_parser)
    {
        extern FILE* yyin;
        extern bool yy_flex_debug;
        yyin = file_;
        yy_flex_debug = debug_lexer_;

        get_next( token_ );
    }

    virtual int parse() override;

    virtual std::string get_name() const override { return "Handmade"; }

private:

    // Add your private functions and variables here below ...
    ProgramNode* program();
    std::list<VariableDeclarationNode*>* variable_declarations();
    std::list<VariableNode*>* variable_list();
    VariableNode* variable( std::string id = "" );
    ValueType type();
    std::list<MethodNode*>* method_declarations();
    MethodNode* method_declaration();
    std::list<ParameterNode*> *parameters();
    std::list<StmNode*> *statement_list();
    void parameter_list(std::list<ParameterNode*>* list_param);
    StmNode* statement();
    StmNode* id_start_stm();
    StmNode* op_incr_decr();
    ExprNode*  optional_expression();
    BlockStmNode* statement_block();
    BlockStmNode* optional_else();
    std::list<ExprNode*> *expr_list();
    std::list<ExprNode*> *more_expr(std::list<ExprNode*>* lst);
    ExprNode* expression();
    ExprNode* expression_pr(ExprNode* lhs_expr);
    ExprNode* expression_and();
    ExprNode* expression_and_pr(ExprNode* lhs_expr);
    ExprNode* expression_eq();
    ExprNode* expression_eq_pr(ExprNode* lhs_expr);
    ExprNode* expression_rel();
    ExprNode* expression_rel_pr(ExprNode* lhs_expr);
    ExprNode* expression_add();
    ExprNode* expression__add_pr(ExprNode* lhs_expr);
    ExprNode* expression_mult();
    ExprNode* expression_mult_pr(ExprNode* lhs_expr);
    ExprNode* expression_unary();
    ExprNode* factor();
    ValueType method_return_type();

};
#endif //DECAFPARSER_HPARSER_H
