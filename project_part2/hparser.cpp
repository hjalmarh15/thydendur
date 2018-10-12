
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
       //rror( decaf::token_type::kwVoid );
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
    match( decaf::token_type::ptSemicolon );
    return list_mdn;
}

MethodNode* HParser::method_declaration() {
        ValueType type = this->type();
        string id = token_.lexeme;
        return new MethodNode(type, id, nullptr , variable_declarations(), nullptr );
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