
#include "hparser.h"

using namespace std;

int HParser::parse()
{
    set_AST( program() );
    return 0;
}

ProgramNode*
HParser::program() {
    match( decaf::token_type::kwClass );
    string name = token_.lexeme;
    match( decaf::token_type::Identifier );
    match( decaf::token_type::ptLBrace );
    auto list_vdn = variable_declarations();
    auto list_mdn = method_declarations();
    match( decaf::token_type::ptRBrace );
    match( decaf::token_type::EOI );
    return new ProgramNode(name, list_vdn, list_mdn);
}


list<VariableDeclarationNode*>*
HParser::variable_declarations()
{
    auto list_vdn = new list<VariableDeclarationNode*>();
    while ( token_.type == decaf::token_type::kwInt ||
            token_.type == decaf::token_type::kwReal ||
            token_.type == decaf::token_type::kwBool ) {
        ValueType type = this->type();
        auto list_v = variable_list();
        list_vdn->push_back( new VariableDeclarationNode( type, list_v ) );
    }
    return list_vdn;
}


ValueType HParser::type()
{
    ValueType valuetype = ValueType::VoidVal;
    if ( token_.type == decaf::token_type::kwInt ) {
        match( decaf::token_type::kwInt );
        valuetype = ValueType::IntVal;
    }
    else if ( token_.type == decaf::token_type::kwReal ) {
        match( decaf::token_type::kwReal );
        valuetype = ValueType::RealVal;
    }
    else if ( token_.type == decaf::token_type::kwBool ) {
        match( decaf::token_type::kwBool );
        valuetype = ValueType::BoolVal;
    }
    else if( token_.type == decaf::token_type::kwVoid) {
        match(decaf::token_type::kwVoid);
        valuetype = ValueType ::VoidVal;
    }
    else {
       error(decaf::token_type::kwInt);
    }
    return valuetype;
}


list<VariableNode*>*
HParser::variable_list()
{
    auto list_v = new list<VariableNode*>();
    list_v->push_back( variable() );
    while ( token_.type == decaf::token_type::ptComma ) {
        match( decaf::token_type::ptComma );
        list_v->push_back( variable() );
    }
    match( decaf::token_type::ptSemicolon );
    return list_v;
}


VariableNode*
HParser::variable( string id )
{
    if ( id.empty() ) {
        id = token_.lexeme;
        match( decaf::token_type::Identifier );
    }
    int dim = 0;
    if ( token_.type == decaf::token_type::ptLBracket ) {
        match( decaf::token_type::ptLBracket );
        if ( token_.type == decaf::token_type::IntValue) {
            dim = stoi(token_.lexeme);
        }
        match( decaf::token_type::IntValue );
        match( decaf::token_type::ptRBracket );
    }
    return new VariableNode( id, dim );
}



list<MethodNode*>* HParser::method_declarations() {
    auto list_mdn = new list<MethodNode*>();
    list_mdn->push_back(method_declaration());
    while ( token_.type == decaf::token_type::ptComma ) {
        match( decaf::token_type::ptComma );
        list_mdn->push_back( method_declaration() );
    }
    return list_mdn;

}

MethodNode* HParser::method_declaration() {

        match(decaf::token_type::kwStatic);
        ValueType return_type = this->type();
        std::string id = token_.lexeme;
        match(decaf::token_type::Identifier);
        match(decaf::token_type::ptLParen);
        auto list_param = parameters();
        match(decaf::token_type::ptRParen);
        match(decaf::token_type::ptLBrace);
        auto list_vdn = variable_declarations();
        auto list_stm = statement_list();
        match(decaf::token_type::ptRBrace);
        return new MethodNode(return_type, id, list_param, list_vdn, list_stm );
}

list<ParameterNode*> *HParser::parameters() {
    auto list_param = new list<ParameterNode*>();
    parameter_list(list_param);

    while( token_.type == decaf::token_type::ptComma ) {
            match(decaf::token_type::ptComma);
            parameter_list(list_param);
    }
    return list_param;
}

void HParser::parameter_list(list<ParameterNode*>* list_param) {
    while(token_.type == decaf::token_type::kwInt ||
          token_.type == decaf::token_type::kwReal ||
          token_.type == decaf::token_type::kwBool) {
        ValueType type = this->type();
        VariableNode* var = variable();
        list_param->push_back(new ParameterNode(type, var));
    }
}
list<StmNode*> *HParser::statement_list() {
    return new list<StmNode*>();
}
/*
method_declaration::= static  method_return_type id( parameters){variable_declarationsstatement_list}
protected:
ValueType return_type_;
std::string id_;
synstd::list<ParameterNode*> *params_;
std::list<VariableDeclarationNode*> *vars_decl_;
std::list<StmNode*> *stms_;
};
 */