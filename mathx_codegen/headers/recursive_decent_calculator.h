#pragma once

// This is the most simple parser which calculate mathamatical expressions
// But it is getting grow day by day.

// TODO 1. Change the type of the variables to double and handle calculations as doubles for more precision.	- DONE
// TODO 2. Add usfull functions add, sub, pow, root, log etc.													- 
// TODO 3. Addition of multi-dimention arrays, printing of multi-dimension arrays.								-
// TODO 4. Add while loops.																						- 
// TODO 5. Addition of vec and mat types and matrix related calculations.										-
// TODO 6. Make parser can evaluate and report all errors, with error recovery mechanism.						-
// TODO 7. Parallel implementation of matrix calculations.														-
// TODO 8. Add &&, || operations.
// TODO 9. Division by zero exception.
// TODO 10.Add types (short, int, float, double)
// TODO 11.Add type string. 

#include <stack>
#include <map>
#include "../headers/simple_lexer.h"

struct stmts
{
	std::string next;
	std::string code;
};


struct expr{
	std::string addr;
	std::string code;
	double value;
};

class SimpleParser {
	SimpleLexer* lexer;

	expr* factor( expr* );
	//expr power_( expr );
	expr* term( expr* );
	expr* term_( expr* );
	expr* exprssion();
	expr* exprssion_( expr* );
	//expr power( expr );

	void program();
	void decls();
	void decl();
	void ids();

	void block( stmts* );

	void statements( stmts* );
	void statement( stmts* );


	void getNextToken();

	Token* next_token = nullptr;

	std::stack<Token*> backtrack_stack;

	//store the identifier and current value of that identifier
	std::map<std::string, double> symbol_table;

	std::string out_id_name;

	bool eol = false;

public:

	SimpleParser(SimpleLexer* _lexer) : lexer(_lexer)
	{}

	void parse();
};