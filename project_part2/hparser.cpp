
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
    else {
       error(decaf::token_type::kwInt);
    }
    return valuetype;
}

ValueType HParser::method_return_type() {
    if( token_.type == decaf::token_type::kwVoid) {
        match(decaf::token_type::kwVoid);
        return ValueType ::VoidVal;
    }

    return type();
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
    auto meth_dec = method_declaration();
    list_mdn->push_back(meth_dec);

    while ( token_.type == decaf::token_type::kwStatic) {
        meth_dec = method_declaration();
        list_mdn->push_back(meth_dec);
    }
    return list_mdn;

}

MethodNode* HParser::method_declaration() {
        match(decaf::token_type::kwStatic);
        ValueType return_type = method_return_type();
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

    auto stm_list = new list<StmNode*>();

    while(token_.type == decaf::token_type::kwIf ||
          token_.type == decaf::token_type::kwFor ||
          token_.type == decaf::token_type::kwReturn ||
          token_.type == decaf::token_type::kwBreak ||
          token_.type == decaf::token_type::kwContinue ||
          token_.type == decaf::token_type::ptLBrace ||
          token_.type == decaf::token_type::Identifier)
    {
        //match(token_.type);
        stm_list->push_back(statement());
    }
    return stm_list;
}

StmNode* HParser::statement(){
    StmNode* stm_node;
    if(token_.type == decaf::token_type::kwIf){
        match(decaf::token_type::kwIf);
        match(decaf::token_type::ptLParen);
        auto expr = expression();
        match(decaf::token_type::ptRParen);
        auto stm_block = statement_block();
        auto else_block = optional_else();
        stm_node = new IfStmNode(expr, stm_block, else_block);

    }
    else if(token_.type == decaf::token_type::kwFor){

        match(decaf::token_type::kwFor);
        match(decaf::token_type::ptLParen);
        auto var1 = variable();
        match(decaf::token_type::OpAssign);
        auto expr1 = expression();
        AssignStmNode* assign = new AssignStmNode(var1, expr1);
        match(decaf::token_type::ptSemicolon);
        auto expr2 = expression();
        match(decaf::token_type::ptSemicolon);
                auto var = variable();
        IncrDecrStmNode* incr_decr;
        if (token_.type == decaf::token_type::OpArtInc) {
            match(decaf::token_type::OpArtInc);
            incr_decr = new IncrStmNode(var);
        }
        else if(token_.type == decaf::token_type::OpArtDec) {
            match(decaf::token_type::OpArtDec);
            incr_decr = new DecrStmNode(var);
        }
        match(decaf::token_type::ptRParen);
        auto stm_block = statement_block();
        stm_node = new ForStmNode(assign, expr2, incr_decr, stm_block);

    }
    else if(token_.type == decaf::token_type::kwReturn){
        match(decaf::token_type::kwReturn);
        auto return_stm_node = optional_expression();
        match(decaf::token_type::ptSemicolon);
        stm_node = new ReturnStmNode(return_stm_node);
    }
    else if(token_.type == decaf::token_type::kwBreak){
        match(decaf::token_type::kwBreak);
        match(decaf::token_type::ptSemicolon);
        stm_node = new BreakStmNode();
    }
    else if(token_.type == decaf::token_type::kwContinue){
        match(decaf::token_type::kwContinue);
        match(decaf::token_type::ptSemicolon);
        stm_node = new ContinueStmNode();
    }
    else if(token_.type == decaf::token_type::ptLBrace){
        stm_node = statement_block();
    }
    else{
        stm_node = id_start_stm();
    }

    return stm_node;
}

StmNode* HParser::id_start_stm(){
    StmNode* stm_nd;
    string id = token_.lexeme;
    match( decaf::token_type::Identifier );

    if(token_.type == decaf::token_type::ptLParen){
        match(decaf::token_type::ptLParen);
        stm_nd = new MethodCallExprStmNode(id, expr_list());
        match(decaf::token_type::ptRParen);
    }
    else{
        auto var = variable(id);
        if (token_.type == decaf::token_type::OpArtInc) {
            match(decaf::token_type::OpArtInc);
            stm_nd = new IncrStmNode(var);
        }
        else if (token_.type == decaf::token_type::OpArtDec) {
            match(decaf::token_type::OpArtDec);
            stm_nd = new DecrStmNode(var);
        }
        else{
            match(decaf::token_type::OpAssign);
            stm_nd = new AssignStmNode(var, expression());

        }
    }
    match(decaf::token_type::ptSemicolon);
    return stm_nd;
}

StmNode* HParser::op_incr_decr() {
    auto var = variable();
    if (token_.type == decaf::token_type::OpArtInc) {
        match(decaf::token_type::OpArtInc);
        return new IncrStmNode(var);
    }
    else if (token_.type == decaf::token_type::OpArtDec) {
        match(decaf::token_type::OpArtDec);
        return new DecrStmNode(var);
    }
}
ExprNode*  HParser::optional_expression(){
    if(token_.type != decaf::token_type::ptSemicolon)
        return expression();
    else
        return nullptr;
}

BlockStmNode* HParser::statement_block(){
    match(decaf::token_type::ptLBrace);
    auto stm_block = new BlockStmNode(statement_list());
    match(decaf::token_type::ptRBrace);
        return stm_block;
}

BlockStmNode* HParser::optional_else() {
    if(token_.type != decaf::token_type::kwElse)
        return nullptr;
    match(decaf::token_type::kwElse);
    return statement_block();
}

std::list<ExprNode*> *HParser::expr_list(){
    auto lst = new list<ExprNode*>();
    lst->push_back(expression());
    return more_expr(lst);;
}

std::list<ExprNode*> *HParser::more_expr(std::list<ExprNode*>* lst){
    while(token_.type == decaf::token_type::ptComma) {
        match(decaf::token_type::ptComma);
        lst->push_back(expression());
    }
    return lst;
}

ExprNode* HParser::expression(){
    auto lhs_expr = expression_and();
    return expression_pr(lhs_expr);
}

ExprNode* HParser::expression_pr(ExprNode* lhs_expr){
    if(token_.type == decaf::token_type::OpLogOr){
        match(decaf::token_type::OpLogOr);
        auto rhs_expr = expression_and();
        return expression_pr(new OrExprNode(lhs_expr, rhs_expr));
    }
    return lhs_expr;
}
ExprNode* HParser::expression_and(){
    auto lhs_expr = expression_eq();
    return expression_and_pr(lhs_expr);
}
ExprNode* HParser::expression_and_pr(ExprNode* lhs_expr){
    if(token_.type == decaf::token_type::OpLogAnd){
        match(decaf::token_type::OpLogAnd);
        auto rhs_expr = expression_eq();
        return expression_and_pr(new AndExprNode(lhs_expr, rhs_expr));
    }
    return lhs_expr;
}
ExprNode* HParser::expression_eq(){
    auto lhs_expr = expression_rel();
    return expression_eq_pr(lhs_expr);
}
ExprNode* HParser::expression_eq_pr(ExprNode* lhs_expr){
    if(token_.type == decaf::token_type::OpRelEQ){
        match(decaf::token_type::OpRelEQ);
        auto rhs_expr = expression_rel();
        return expression_eq_pr(new EqExprNode(lhs_expr, rhs_expr));
    }
    else if(token_.type == decaf::token_type::OpRelNEQ){
        match(decaf::token_type::OpRelNEQ);
        auto rhs_expr = expression_rel();
        return expression_eq_pr(new NeqExprNode(lhs_expr, rhs_expr));
    }
    return lhs_expr;
}
ExprNode* HParser::expression_rel(){
    auto lhs_expr = expression_add();
    return expression_rel_pr(lhs_expr);
}
ExprNode* HParser::expression_rel_pr(ExprNode* lhs_expr){
    if(token_.type == decaf::token_type::OpRelGT){
        match(decaf::token_type::OpRelGT);
        auto rhs_expr = expression_add();
        return expression_rel_pr(new GtExprNode(lhs_expr, rhs_expr));
    }
    else if(token_.type == decaf::token_type::OpRelGTE){
        match(decaf::token_type::OpRelGTE);
        auto rhs_expr = expression_add();
        return expression_rel_pr(new GteExprNode(lhs_expr, rhs_expr));
    }
    else if(token_.type == decaf::token_type::OpRelLT){
        match(decaf::token_type::OpRelLT);
        auto rhs_expr = expression_add();
        return expression_rel_pr(new LtExprNode(lhs_expr, rhs_expr));
    }
    else if(token_.type == decaf::token_type::OpRelLTE){
        match(decaf::token_type::OpRelLTE);
        auto rhs_expr = expression_add();
        return expression_rel_pr(new LteExprNode(lhs_expr, rhs_expr));
    }
    return lhs_expr;
}
ExprNode* HParser::expression_add(){
    auto lhs_expr = expression_mult();
    return expression__add_pr(lhs_expr);
}
ExprNode* HParser::expression__add_pr(ExprNode* lhs_expr){
    if(token_.type == decaf::token_type::OpArtPlus){
        match(decaf::token_type::OpArtPlus);
        auto rhs_expr = expression_mult();
        return expression__add_pr(new PlusExprNode(lhs_expr, rhs_expr));
    }
    else if(token_.type == decaf::token_type::OpArtMinus){
        match(decaf::token_type::OpArtMinus);
        auto rhs_expr = expression_mult();
        return expression__add_pr(new MinusExprNode(lhs_expr, rhs_expr));
    }
    return lhs_expr;
}
ExprNode* HParser::expression_mult(){
    auto lhs_expr = expression_unary();
    return expression_mult_pr(lhs_expr);
}
ExprNode* HParser::expression_mult_pr(ExprNode* lhs_expr){
    if(token_.type == decaf::token_type::OpArtMult){
        match(decaf::token_type::OpArtMult);
        auto rhs_expr = expression_unary();
        return expression_mult_pr(new MultiplyExprNode(lhs_expr, rhs_expr));
    }
    else if(token_.type == decaf::token_type::OpArtDiv){
        match(decaf::token_type::OpArtDiv);
        auto rhs_expr = expression_unary();
        return expression_mult_pr(new DivideExprNode(lhs_expr, rhs_expr));
    }
    else if(token_.type == decaf::token_type::OpArtModulus){
        match(decaf::token_type::OpArtModulus);
        auto rhs_expr = expression_unary();
        return expression_mult_pr(new ModulusExprNode(lhs_expr, rhs_expr));
    }
    return lhs_expr;
}
ExprNode* HParser::expression_unary(){
    if(token_.type == decaf::token_type::OpArtPlus){
        match(decaf::token_type::OpArtPlus);
        auto rhs_expr = expression_unary();
        return new PlusExprNode(rhs_expr);
    }
    else if(token_.type == decaf::token_type::OpArtMinus){
        match(decaf::token_type::OpArtMinus);
        auto rhs_expr = expression_unary();
        return new MinusExprNode(rhs_expr);
    }
    else if(token_.type == decaf::token_type::OpLogNot){
        match(decaf::token_type::OpLogNot);
        auto rhs_expr = expression_unary();
        return new NotExprNode(rhs_expr);
    }


    return factor();
}


ExprNode* HParser::factor(){
    if(token_.type == decaf::token_type::BoolValue) {
        auto val = token_.lexeme;
        match(token_.type);
        return new ValueExprNode(val);
    }
    else if(token_.type == decaf::token_type::RealValue) {
        auto val = token_.lexeme;
        match(token_.type);
        return new ValueExprNode(val);
    }
    else if(token_.type == decaf::token_type::IntValue){
        auto val = token_.lexeme;
        match(token_.type);
        return new ValueExprNode(val);
    }
    else if(token_.type == decaf::token_type::ptLParen){
        match(decaf::token_type::ptLParen);
        auto expr = expression();
        match(decaf::token_type::ptRParen);
        return expr;
    }
    else{
        ExprNode* expr_nd;
        string id = token_.lexeme;
        match( decaf::token_type::Identifier );
                if(token_.type == decaf::token_type::ptLParen){
            match(decaf::token_type::ptLParen);
            expr_nd = new MethodCallExprStmNode(id, expr_list());
            match(decaf::token_type::ptRParen);
        }
        else{
            expr_nd = variable(id);
        }
        return expr_nd;
    }
}
