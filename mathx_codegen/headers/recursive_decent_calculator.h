#pragma once

#include <stack>
#include <map>
#include "../headers/simple_lexer.h"

enum NODE_TYPE
{
    NT_VOID,       // Not any intended types  
    NT_ADD,        // num + num
    NT_SUB,        // num - num
    NT_MUL,        // *
    NT_DIV,        // /
    NT_EQ,         // ==
    NT_NE,         // !=
    NT_LT,         // <
    NT_LE,         // <=
    NT_ASSIGN,     // =
    NT_LOGAND,     // &&
    NT_LOGOR,      // ||
    NT_RETURN,     // "return"
    NT_IF,         // "if"
    NT_WHILE,      // "while"
    NT_FOR,        // "for"
    NT_DO,         // "do"
    NT_SWITCH,     // "switch"
    NT_CASE,       // "case"
    NT_BLOCK,      // { ... }
    NT_BREAK,      // "break"
    NT_CONTINUE,   // "continue"
    NT_FUNCALL,    // Function call
    NT_VAR,        // Variable
    NT_NUM,        // Integer
    NT_CONDITION,  // condition
    NT_EXPR,       // Expr
    NT_PRINT,      // print sys call  
    NT_NULL,       // Empty statement
};

struct Node
{
    Node* lhs;
    Node* rhs;
    NODE_TYPE type;

    Node* next;

    Node* condition;
    Node* then;

    long value;
    std::string variable_name;

    bool is_leaf = false;

    int temp_var_index = 0;

    std::string str;

    std::string to_string()
    {
        if (is_leaf)
        {
            if (type == NT_VAR)
                return variable_name;
            else if (type == NT_NUM)
                return std::to_string(value);
        }
        else
        {
            str.append("t");
            str.append(std::to_string(temp_var_index));
            //str = "t"+temp_var_index;
            return str;
        }
    }
};

struct stmts
{
    std::string next;
    std::string code;
    std::string begin;
};

struct condition
{
    std::string true_lbl;
    std::string false_lbl;
    std::string code;
};

struct expr {
    std::string addr;
    std::string code;
    double value;
};

class SimpleParser {
	SimpleLexer* lexer;

	Node* factor();
	Node* term();
	Node* term_();
	Node* exprssion();
	Node* exprssion_();

	Node* relation();

	void program();
	void decls();
	void decl();
	void ids();

	Node* block();

	Node* statements(Node* current);
	Node* statement();


	void getNextToken();

	Token* next_token = nullptr;

	std::stack<Token*> backtrack_stack;

	//store the identifier and current value of that identifier
	std::map<std::string, double> symbol_table;

	std::string out_id_name;

	bool eol = false;

	Node* head;

public:
	SimpleParser(SimpleLexer* _lexer) : lexer(_lexer)
	{}

	void parse();

	Node* get_head();
};