#ifndef DECAFPARSER_AST_H
#define DECAFPARSER_AST_H

#include <iostream>
#include <string>
#include <list>
#include <stack>
#include <algorithm>
#include "symbol_table.h"
#include "tac.h"


/////////////////////////////////////////////////////////////////////////////////

struct Data {

    Data( SymbolTable& st )
            : sym_table(st), variable_no(0), label_no(0), expr_return_type(ValueType::VoidVal) {}

    SymbolTable& sym_table;         // Reference to the symbol-table; do not delete.

    int variable_no;                // Counters for increasing the numbers of
    int label_no;                   // variables and labels.

    std::string method_name;        // Method name, needed for return.
    std::stack<int> for_label_no;   // The label_no of enclosing for loops,
    // needed for continue/break statements.

    std::string expr_return_var;    // Variable used to store expr value.
    ValueType   expr_return_type;   // Type of expression evaluated.

    // You may add/remove data members to this structure as you see fit.

};

inline void warning_msg( std::string message )
{
    std::cout << "WARNING: " << message << std::endl;
}

inline void error_msg( std::string message )
{
    std::cout << "ERROR: " << message << std::endl;
    throw;
}

inline void add_to_symbol_table( SymbolTable& st, EntryType entry_type, std::string scope,
                                 std::string name, ValueType value_type, std::string signature )
{
    SymbolTable::Entry *entry = st.lookup( scope, name );
    if ( entry == nullptr ) {
        SymbolTable::Entry entry;
        entry.name = name;
        entry.scope = scope;
        entry.entry_type = entry_type;
        entry.value_type = value_type;
        entry.signature = signature;
        st.add( entry );
    }
    else {
        error_msg("Identifier '" + name + "' already declared in scope.");
    }
}

/////////////////////////////////////////////////////////////////////////////////


class Node {
public:
    virtual const std::string str( ) const = 0;
    virtual void icg( Data& data, TAC& tac ) const = 0;
    virtual ~Node() = default;
};

inline std::string tostr( const Node* node ) {
    return (node == nullptr ? "(null)" : node->str() );
}


/////////////////////////////////////////////////////////////////////////////////

class ExprNode : public Node { };


class ValueExprNode : public ExprNode
{
public:
    explicit ValueExprNode( const std::string value ) : value_(value) { }

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        data.expr_return_var = value_;
        // Bools can be treated as integers and vice verse (so simply treat BoolVal as IntVal).
        data.expr_return_type =
                (std::all_of(value_.begin(), value_.end(), ::isdigit) ? ValueType::IntVal : ValueType::RealVal);
    }

    virtual const std::string str( ) const override {
        return std::string("(VAL ") + value_ + ')';
    }

protected:
    std::string value_;
};


class AndExprNode : public ExprNode
{
public:
    AndExprNode( ExprNode *lhs, ExprNode *rhs ) : lhs_(lhs), rhs_(rhs) {}

    virtual void icg( Data& data, TAC& tac ) const override {
        // To do ...
    }

    virtual const std::string str( ) const override {
        return std::string("(&& ") + tostr(lhs_) + ' ' + tostr(rhs_) + ')';
    }

protected:
    ExprNode *lhs_, *rhs_;
};


class OrExprNode : public ExprNode
{
public:
    OrExprNode( ExprNode *lhs, ExprNode *rhs ) : lhs_(lhs), rhs_(rhs) {}

    virtual void icg( Data& data, TAC& tac ) const override {
        // To do ...
    }

    virtual const std::string str( ) const override {
        return std::string("(|| ") + tostr(lhs_) + ' ' + tostr(rhs_) + ')';
    }

protected:
    ExprNode *lhs_, *rhs_;
};


class NotExprNode : public ExprNode
{
public:
    NotExprNode( ExprNode *rhs ) : rhs_(rhs) {}

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        std::string var = tac.tmp_variable_name( data.variable_no++ );
        std::string lab_not_true = tac.label_name( "not_true", data.label_no );
        std::string lab_not_end = tac.label_name( "not_end", data.label_no );
        data.label_no++;

        tac.append( TAC::InstrType::VAR, var );
        rhs_->icg( data, tac );
        ValueType type_rhs = data.expr_return_type;
        tac.append( TAC::InstrType::NE, data.expr_return_var, "0", lab_not_true );
        tac.append( TAC::InstrType::ASSIGN, "1", var );
        tac.append( TAC::InstrType::GOTO, lab_not_end );
        tac.label_next_instr( lab_not_true );
        tac.append( TAC::InstrType::ASSIGN, "0", var );
        tac.label_next_instr( lab_not_end );

        data.expr_return_var = var;
        data.expr_return_type = ValueType::IntVal;

        if ( type_rhs == ValueType::RealVal ) {
            warning_msg("Type mismatch in logical ! operation (operand is not an integer/bool value)." );
        }
    }

    virtual const std::string str( ) const override {
        return std::string("(! ") + tostr(rhs_) + ')';
    }

protected:
    ExprNode *rhs_;
};


class RelationalExprNode : public ExprNode {
public:

    RelationalExprNode( TAC::InstrType instr_type, ExprNode *lhs, ExprNode *rhs )
            : instr_type_(instr_type), lhs_(lhs), rhs_(rhs ) {}

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        lhs_->icg( data, tac );
        std::string var_lhs = data.expr_return_var;
        ValueType type_lhs = data.expr_return_type;

        rhs_->icg( data, tac );
        std::string var_rhs = data.expr_return_var;
        ValueType type_rhs = data.expr_return_type;

        std::string lab_rel_true = tac.label_name("rel_true", data.label_no);
        std::string lab_rel_end = tac.label_name("rel_end", data.label_no);
        data.label_no++;
        std::string var = tac.tmp_variable_name(data.variable_no++);

        tac.append( TAC::InstrType::VAR, var );
        tac.append( instr_type_, var_lhs, var_rhs, lab_rel_true );
        tac.append( TAC::InstrType::ASSIGN, "0", var );
        tac.append( TAC::InstrType::GOTO, lab_rel_end );
        tac.label_next_instr( lab_rel_true );
        tac.append( TAC::InstrType::ASSIGN, "1", var );
        tac.label_next_instr( lab_rel_end );

        data.expr_return_var = var;
        data.expr_return_type = ValueType::IntVal;

        if ( (type_lhs == ValueType::RealVal && type_rhs != ValueType::RealVal) ||
             (type_lhs != ValueType::RealVal && type_rhs == ValueType::RealVal) ) {
            warning_msg("Type mismatch in operation " + tac.IName[instr_type_] + ".");
        }
    }

protected:
    TAC::InstrType instr_type_;
    ExprNode *lhs_, *rhs_;
};

class EqExprNode : public RelationalExprNode
{
public:
    EqExprNode( ExprNode *lhs, ExprNode *rhs )
            : RelationalExprNode( TAC::InstrType::EQ, lhs, rhs ) {}

    virtual const std::string str( ) const override {
        return std::string("(== ") + tostr(lhs_) + ' ' + tostr(rhs_) + ')';
    }
};


class NeqExprNode : public RelationalExprNode
{
public:
    NeqExprNode( ExprNode *lhs, ExprNode *rhs )
            : RelationalExprNode( TAC::InstrType::NE, lhs, rhs ) {}

    virtual const std::string str( ) const override {
        return std::string("(!= ") + tostr(lhs_) + ' ' + tostr(rhs_) + ')';
    }
};


class LtExprNode : public RelationalExprNode
{
public:
    LtExprNode( ExprNode *lhs, ExprNode *rhs )
            : RelationalExprNode( TAC::InstrType::LT, lhs, rhs ) {}

    virtual const std::string str( ) const override {
        return std::string("(< ") + tostr(lhs_) + ' ' + tostr(rhs_) + ')';
    }
};


class LteExprNode : public RelationalExprNode
{
public:
    LteExprNode( ExprNode *lhs, ExprNode *rhs )
            : RelationalExprNode( TAC::InstrType::LE, lhs, rhs ) {}

    virtual const std::string str( ) const override {
        return std::string("(<= ") + tostr(lhs_) + ' ' + tostr(rhs_) + ')';
    }
};


class GtExprNode : public RelationalExprNode
{
public:
    GtExprNode( ExprNode *lhs, ExprNode *rhs )
            : RelationalExprNode( TAC::InstrType::GT, lhs, rhs ) {}

    virtual const std::string str( ) const override {
        return std::string("(> ") + tostr(lhs_) + ' ' + tostr(rhs_) + ')';
    }
};


class GteExprNode : public RelationalExprNode
{
public:
    GteExprNode( ExprNode *lhs, ExprNode *rhs )
            : RelationalExprNode( TAC::InstrType::GE, lhs, rhs ) {}

    virtual const std::string str( ) const override {
        return std::string("(>= ") + tostr(lhs_) + ' ' + tostr(rhs_) + ')';
    }
};


class ArithmeticExprNode: public ExprNode {
public:
    ArithmeticExprNode( TAC::InstrType instr_type, ExprNode *lhs, ExprNode *rhs )
            : instr_type_(instr_type), lhs_(lhs), rhs_(rhs ) {}

    virtual void icg( Data& data, TAC& tac ) const override
    {
        // To do ...
        data.expr_return_var = "?";
    }

protected:
    TAC::InstrType instr_type_;
    ExprNode *lhs_, *rhs_;
};


class MultiplyExprNode : public ArithmeticExprNode
{
public:
    MultiplyExprNode( ExprNode *lhs, ExprNode *rhs )
            : ArithmeticExprNode( TAC::InstrType::MULT, lhs, rhs ) {}

    virtual const std::string str( ) const override {
        return std::string("(* ") + tostr(lhs_) + ' ' + tostr(rhs_) + ')';
    }
};

class DivideExprNode : public ArithmeticExprNode
{
public:
    DivideExprNode( ExprNode *lhs, ExprNode *rhs )
            : ArithmeticExprNode( TAC::InstrType::DIVIDE, lhs, rhs ) {}

    virtual const std::string str( ) const override {
        return std::string("(/ ") + tostr(lhs_) + ' ' + tostr(rhs_) + ')';
    }
};


class ModulusExprNode : public ArithmeticExprNode
{
public:
    ModulusExprNode( ExprNode *lhs, ExprNode *rhs )
            : ArithmeticExprNode( TAC::InstrType::MOD, lhs, rhs ) {}

    virtual const std::string str( ) const override {
        return std::string("(% ") + tostr(lhs_) + ' ' + tostr(rhs_) + ')';
    }
};


class PlusExprNode : public ArithmeticExprNode
{
public:
    PlusExprNode( ExprNode *rhs )
            : ArithmeticExprNode( TAC::InstrType::ADD, nullptr, rhs ) {}

    PlusExprNode( ExprNode *lhs, ExprNode *rhs )
            : ArithmeticExprNode( TAC::InstrType::ADD, lhs, rhs ) {}

    virtual const std::string str( ) const override {
        std::string s = std::string("(+ ");
        if ( lhs_ != nullptr ) {
            s += tostr(lhs_) + ' ';
        }
        s += tostr(rhs_) + ')';
        return s;
    }

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        if ( lhs_ == nullptr ) {
            rhs_->icg( data, tac );
        }
        else {
            ArithmeticExprNode::icg( data, tac );
        }
    }
};


class MinusExprNode : public ArithmeticExprNode
{
public:
    MinusExprNode( ExprNode *rhs )
            : ArithmeticExprNode( TAC::InstrType::SUB, nullptr, rhs ) {}
    MinusExprNode( ExprNode *lhs, ExprNode *rhs )
            : ArithmeticExprNode( TAC::InstrType::SUB, lhs, rhs ) {}

    virtual const std::string str( ) const override {
        std::string s = std::string("(- ");
        if ( lhs_ != nullptr ) {
            s += tostr(lhs_) + ' ';
        }
        s += tostr(rhs_) + ')';
        return s;
    }

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        if ( lhs_ == nullptr ) {
            rhs_->icg( data, tac );
            std::string var = tac.tmp_variable_name(data.variable_no++);
            tac.append( TAC::InstrType::VAR, var );
            tac.append( TAC::InstrType::UMINUS, data.expr_return_var, var );
            data.expr_return_var = var;
        }
        else {
            ArithmeticExprNode::icg( data, tac );
        }
    }
};


class VariableNode : public ExprNode
{
public:

    explicit VariableNode( const std::string& id, int dim=0 ) : id_(id), dim_(dim) {
        if ( dim != 0 ) {
            error_msg( "Sorry, arrays are not supported in Part3 of the assignment!");
        }
    }

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        data.expr_return_var = id_;
        data.expr_return_type = ValueType::VoidVal;

        SymbolTable::Entry* entry = data.sym_table.lookup( data.method_name, id_ );
        if ( entry == nullptr && !data.method_name.empty() ) { // also look in global scope.
            entry = data.sym_table.lookup( "", id_ );
        }
        if ( entry == nullptr ) {
            error_msg("Undeclared identifier '" + id_ + "'.");
        }
        else {
            data.expr_return_type = entry->value_type;
        }

    }

    virtual const std::string str( ) const override {
        std::string str = std::string("(VAR ") + id_ ;
        if ( dim_ > 0 ) {
            str += '[' + std::to_string(dim_) + ']';
        }
        str += ')';
        return str;
    }

    std::string get_id() const { return id_; }


protected:
    std::string id_;
    int dim_;  // dimension > 0 means an array.
};

typedef VariableNode VariableExprNode;


/////////////////////////////////////////////////////////////////////////////////

class VariableDeclarationNode : public Node
{
public:
    VariableDeclarationNode( ValueType type, std::list<VariableExprNode*>* vars )
            : type_(type), vars_(vars) {}

    virtual const std::string str( ) const override {
        std::string s = std::string("(DECLARE ") + tostr(type_);
        for ( auto node : *vars_ ) {
            s +=  ' ' + tostr(node);
        }
        s += ')';
        return s;
    }

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        for ( auto e : *vars_ ) {
            add_to_symbol_table( data.sym_table, EntryType::Variable, data.method_name, e->get_id(), type_, "" );
            tac.append( TAC::InstrType::VAR, e->get_id() );
        }
    }

    ValueType get_type( ) const { return type_; }

    const std::list<VariableExprNode*>* get_vars() const { return vars_; }

protected:
    ValueType type_;
    const std::list<VariableExprNode*>* vars_;
};


class ParameterNode : public Node {
public:
    ParameterNode( ValueType type, VariableExprNode* var )
            : type_(type), var_(var) {}

    virtual const std::string str( ) const override {
        return std::string("(PARAM ") + tostr(type_) + tostr(var_) + ")";
    }

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        add_to_symbol_table( data.sym_table, EntryType::Variable, data.method_name, var_->get_id(), type_, "" );
        tac.append( TAC::InstrType::FPARAM, var_->get_id() );
    }

    ValueType get_type( ) const { return type_; }

    std::string get_id() const { return var_->get_id(); }

protected:
    ValueType type_;
    VariableExprNode *var_;
};

/////////////////////////////////////////////////////////////////////////////////

class StmNode : public Node { };

class MethodCallExprStmNode: public ExprNode, public StmNode
{
public:

    MethodCallExprStmNode( std::string id, std::list<ExprNode*>* expr_list )
            : id_(id), expr_list_(expr_list) {
    }

    virtual void icg( Data& data, TAC& tac ) const override {
        // To do ...
    }

    virtual const std::string str( ) const override {
        std::string s( "(CALL " + id_ );
        for ( auto e : *expr_list_ ) {
            s += " " + tostr(e);
        }
        s += ')';
        return s;
    }

protected:
    std::string id_;
    std::list<ExprNode*> *expr_list_;
};


class AssignStmNode: public StmNode
{
public:

    AssignStmNode( VariableExprNode *lvar, ExprNode *expr )
            : lvar_(lvar), expr_(expr) {
    }

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        lvar_->icg( data, tac );
        std::string var = data.expr_return_var;
        ValueType var_type = data.expr_return_type;

        expr_->icg( data, tac );
        std::string exp_var = data.expr_return_var;
        ValueType exp_type = data.expr_return_type;

        tac.append( TAC::InstrType::ASSIGN, exp_var, var );

        if ( (var_type == ValueType::RealVal && exp_type != ValueType::RealVal) ||
             (var_type != ValueType::RealVal && exp_type == ValueType::RealVal) ) {
            warning_msg("Type mismatch in assigning to variable '" + var + "'.");
        }
    }

    virtual const std::string str( ) const override {
        return std::string("(= ") + tostr(lvar_) + ' ' + tostr(expr_) + ')';
    }

protected:
    VariableExprNode *lvar_;
    ExprNode *expr_;
};


class IncrDecrStmNode : public StmNode {};


class IncrStmNode: public IncrDecrStmNode
{
public:

    IncrStmNode( VariableExprNode *var ) : var_(var) {}

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        var_->icg( data, tac );
        if ( data.expr_return_type == ValueType::RealVal ) {
            tac.append( TAC::InstrType::ADD, data.expr_return_var, "1.0", data.expr_return_var );
        }
        else {
            tac.append( TAC::InstrType::ADD, data.expr_return_var, "1", data.expr_return_var );
        }
    }

    virtual const std::string str( ) const override {
        return std::string("(++ ") + tostr(var_) + ')';
    }

protected:
    VariableExprNode *var_;
};


class DecrStmNode: public IncrDecrStmNode
{
public:

    DecrStmNode( VariableExprNode *var ) : var_(var) {}

    virtual void icg( Data& data, TAC& tac ) const override {
        // To do ...
    }

    virtual const std::string str( ) const override {
        return std::string("(-- ") + tostr(var_) + ')';
    }

protected:
    VariableExprNode *var_;
};


class ReturnStmNode: public StmNode
{
public:

    ReturnStmNode( )
            : expr_(nullptr) {
    }

    ReturnStmNode( ExprNode *expr )
            : expr_(expr) {
    }

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        if ( expr_ != nullptr ) {
            expr_->icg( data, tac );
            tac.append( TAC::InstrType::ASSIGN, data.expr_return_var, data.method_name );
        }
        tac.append( TAC::InstrType::RETURN );

        SymbolTable::Entry *entry = data.sym_table.lookup("", data.method_name);
        if ( entry != nullptr ) {
            if ( (expr_ != nullptr && entry->value_type == ValueType::VoidVal) ||
                 (expr_ == nullptr && entry->value_type != ValueType::VoidVal) ) {
                error_msg( "Return statement in '" + data.method_name + "' does not match return value.");
            }
            if (  expr_ != nullptr &&
                 ((data.expr_return_type == ValueType::RealVal && entry->value_type != ValueType::RealVal) ||
                  (data.expr_return_type != ValueType::RealVal && entry->value_type == ValueType::RealVal)) ) {
                warning_msg( "Returned value in '" + data.method_name + "' does not match return type.");
            }
        }
    }

    virtual const std::string str( ) const override {
        std::string s("(RET ");
        if ( expr_ != nullptr ) {
            s += tostr(expr_);
        }
        s += ')';
        return s;
    }

protected:
    ExprNode *expr_;
};


class BreakStmNode: public StmNode
{
public:

    BreakStmNode(  ) { }

    virtual void icg( Data& data, TAC& tac ) const override {
        // To do ...
    }

    virtual const std::string str( ) const override {
        return std::string( "(BREAK)" );
    }
};


class ContinueStmNode: public StmNode
{
public:

    ContinueStmNode(  ) { }

    virtual void icg( Data& data, TAC& tac ) const override {
        // To do ...
    }

    virtual const std::string str( ) const override {
        return std::string( "(CONTINUE)" );
    }
};


class BlockStmNode: public StmNode
{
public:

    BlockStmNode( std::list<StmNode*>* stms ) : stms_(stms) {}

    virtual void icg( Data& data, TAC& tac ) const override {
        // To do ...
    }

    virtual const std::string str( ) const override {
        std::string s( "(BLOCK" );
        for ( auto stm : *stms_ ) {
            s += " " + tostr(stm);
        }
        s += ")";
        return s;
    };

protected:
    std::list<StmNode*>* stms_;
};


class IfStmNode: public StmNode
{
public:

    IfStmNode( ExprNode* expr, BlockStmNode* stm_if, BlockStmNode* stm_else )
            : expr_(expr), stm_if_(stm_if), stm_else_(stm_else) {}

    virtual void icg( Data& data, TAC& tac ) const override {
        // To do ...
    }

    virtual const std::string str( ) const override {
        std::string s( "(IF " );
        s += tostr(expr_) + tostr(stm_if_);
        if ( stm_else_ != nullptr ) {
            s += tostr(stm_else_);
        }
        s += ')';
        return s;
    };

protected:
    ExprNode *expr_;
    BlockStmNode *stm_if_, *stm_else_;
};


class ForStmNode: public StmNode
{
public:

    ForStmNode( AssignStmNode* assign, ExprNode* expr, IncrDecrStmNode* inc_dec,
                BlockStmNode* stms_ )
            : assign_(assign), expr_(expr), inc_dec_(inc_dec), stms_(stms_) {}

    virtual void icg( Data& data, TAC& tac ) const override {
        // To do ...
    }

    virtual const std::string str( ) const override {
        return "(FOR " + tostr(assign_) + tostr(expr_) + tostr(inc_dec_) + tostr(stms_) + ')';
    };

protected:
    AssignStmNode *assign_;
    ExprNode *expr_;
    IncrDecrStmNode *inc_dec_;
    BlockStmNode *stms_;
};

/////////////////////////////////////////////////////////////////////////////////

class MethodNode : public Node {
public:
    MethodNode( ValueType return_type,
                std::string id,
                std::list<ParameterNode*> *params,
                std::list<VariableDeclarationNode*> *vars_decl,
                std::list<StmNode*>* stms )
            : return_type_(return_type), id_(id), params_(params), vars_decl_(vars_decl), stms_(stms) {}

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        std::string signature;
        data.method_name = id_;
        tac.label_next_instr( data.method_name );
        for ( auto pd : *params_ ) {
            pd->icg( data, tac );
            if ( !signature.empty() ) {
                signature += "::";
            }
            signature += tostr( pd->get_type() );
        }
        add_to_symbol_table( data.sym_table, EntryType::Method, "", id_, return_type_, signature );
        // Prevent someone from using a variable with the same name as the enclosing method.
        add_to_symbol_table( data.sym_table, EntryType::Method, id_, id_, return_type_, signature );

        for ( auto vd : *vars_decl_ ) {
            vd->icg( data, tac );
        }
        for ( auto stm : *stms_ ) {
            stm->icg( data, tac );
        }
        if ( tac.last_instr_type() != TAC::InstrType::RETURN ) {
            tac.append( TAC::InstrType::RETURN );
        }
        data.method_name = "";
    }

    virtual const std::string str( ) const override {
        std::string s = "(METHOD " + tostr(return_type_) + ' ' + id_ + ' ';
        for (auto p : *params_) {
            s += tostr( p );
        }
        for (auto vds : *vars_decl_) {
            s += tostr( vds );
        }
        for (auto stm : *stms_) {
            s += tostr( stm );
        }
        s += ')';
        return s;
    }

protected:
    ValueType return_type_;
    std::string id_;
    std::list<ParameterNode*> *params_;
    std::list<VariableDeclarationNode*> *vars_decl_;
    std::list<StmNode*> *stms_;
};


class ProgramNode : public Node {
public:
    ProgramNode( std::string id,
                std::list<VariableDeclarationNode*> *var_decls,
                std::list<MethodNode*> *method_decls )
            : id_(id), var_decls_(var_decls), method_decls_(method_decls) {}

    virtual void icg( Data& data, TAC& tac ) const override {
        // Provided.
        for ( auto vd : *var_decls_ ) {
            vd->icg( data, tac );
        }
        tac.append( TAC::InstrType::GOTO, "main" );
        for ( auto md : *method_decls_ ) {
            md->icg( data, tac );
        }

        SymbolTable::Entry* entry = data.sym_table.lookup( "", "main" );
        if ( entry == nullptr ) {
            error_msg( "Main method is missing." );
        }
    }

    virtual const std::string str( ) const override {
        std::string s = "(CLASS " + id_;
        if ( var_decls_ != nullptr ) {
            for (auto v : *var_decls_) {
                s += " " + tostr( v );
            }
        }
        if ( method_decls_ != nullptr ) {
            for (auto m : *method_decls_) {
                s += " " + tostr( m );
            }
        }
        s += ')';
        return s;
    }

protected:
    std::string id_;
    std::list<VariableDeclarationNode*> *var_decls_;
    std::list<MethodNode*> *method_decls_;
};

#endif //DECAFPARSER_AST_H
