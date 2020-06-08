#include "../headers/recursive_decent_calculator.h"

#include <cmath>

// TODO urgently add understandable comments : consider using CFG for corresponding function in the comment as well
// TODO this is the branch check.

//rename these accordingly
static int x = 0;
static int y = 0;


void SimpleParser::getNextToken()
{
	if (!backtrack_stack.empty())
	{
		next_token = backtrack_stack.top();
		backtrack_stack.pop();
	}
	else if (!lexer->isEOF)
		next_token = lexer->getNextToken();
}


static bool is_relation_operator(Token* current_token)
{
	if (current_token->token_type >= MIN_RELATION_OPS && current_token->token_type <= MAX_RELATION_OPS)
		return true;

	else return false;
}

// Generate new temporary label ti
static void new_temp(expr* expression)
{
	x++;
	expression->addr = "t" + std::to_string(x);
}


static void new_label(stmts* statements, bool isBegin = false)
{
	y++;

	if (!isBegin)
		statements->next = "L" + std::to_string(y);
	else
		statements->begin = "L" + std::to_string(y);
}


static bool evaluate_relation_operator(int l_value, int r_value, int ops)
{
	if (ops == EE)			// == operator
	{
		if (l_value == r_value)
			return true;
	}
	else if (ops == LE)			// <= opertor
	{
		if (l_value <= r_value)
			return true;

	}
	else if (ops == GE)			// >= operator
	{
		if (l_value >= r_value)
			return true;
	}
	else if (ops == GREATER)	// > operator
	{
		if (l_value > r_value)
			return true;
	}
	else						// < operator
	{
		if (l_value < r_value)
			return true;
	}

	return false;
}


void SimpleParser::program()
{
	getNextToken();
	if (next_token->token_type == START)
	{
		decls();
		head = new Node();
		statements(head);
	}
	else
	{
		std::cout << "Program should begin with START keyword \n";
		return;
	}

	getNextToken();
	if (next_token->token_type == END)
	{
		std::cout << "Parse completed successfully \n";
	}
	else
	{
		std::cout << "Program should terminate with END keyword \n";
		return;
	}
}


void SimpleParser::decls()
{
	getNextToken();
	backtrack_stack.push(next_token);

	if (next_token->token_type != AUTO)
	{
		return;
	}

	decl();
	decls();
}


void SimpleParser::decl()
{
	getNextToken();
	if (next_token->token_type == AUTO)
	{
		getNextToken();
		if (next_token->token_type == ID)
		{
			backtrack_stack.push(next_token);
			ids();

			getNextToken();
			if (next_token->token_type != SEMI_COLON)
			{
				backtrack_stack.push(next_token);
				std::cout << "Expected ; at the end of the declaration \n";
			}
		}
		else
		{
			std::cout << "Expected identifier after AUTO in declarations \n";
		}
	}
}


void SimpleParser::ids()
{
	if (next_token->token_type == ID)
	{
		getNextToken();
		symbol_table.insert(std::pair<std::string, double>(next_token->value, 0.0));

		getNextToken();
		if (next_token->token_type == COMMA)
		{
			getNextToken();
			backtrack_stack.push(next_token);
			ids();
		}

		backtrack_stack.push(next_token);
	}
}


Node* SimpleParser::statements(Node* current)
{
	getNextToken();
	backtrack_stack.push(next_token);

	//here the condition check should be END && EOF
	if (next_token->token_type != END && !lexer->isEOF && next_token->token_type != E_BRACES)
	{
		current->next = statement();
		statements(current->next);
	}
	else
	{
		return nullptr;
	}
}


Node* SimpleParser::statement()
{
	getNextToken();

	Node* this_node = new Node();

	if (next_token->token_type == ID)
	{
		Token* id_token = next_token;
		if (symbol_table.find(next_token->value) != symbol_table.end())
		{
			std::string temp = next_token->value;
			getNextToken();

			if (next_token->token_type == EQUAL)
			{
				this_node->variable_name = temp;
				this_node->rhs = exprssion();
				this_node->type = NT_ASSIGN;
				getNextToken();

				if (next_token->token_type != SEMI_COLON)
				{
					std::cout << "Error : expect ; after expression \n";
					backtrack_stack.push(next_token);
				}

				return this_node;
			}
			else
			{
				backtrack_stack.push(next_token);
				backtrack_stack.push(id_token);

				this_node = exprssion();
				getNextToken();

				if (next_token->token_type != SEMI_COLON)
				{
					std::cout << "Error : expect ; after expression \n";
				}

				return this_node;
			}
		}
		else
		{
			std::cout << "Error : found use of undeclared variable : " << next_token->value << " \n";
			return nullptr;
		}
	}
	else if (next_token->token_type == OUT)
	{
		getNextToken();
		if (next_token->token_type == O_PARAM)
		{
			this_node->lhs = exprssion();
			this_node->type = NT_PRINT;

			getNextToken();
			if (next_token->token_type != E_PARAM)
			{
				std::cout << "Error : expected ) at the end of OUT expression \n";
			}

			getNextToken();
			if (next_token->token_type != SEMI_COLON)
			{
				std::cout << "Error : expect ; after expression \n";
				backtrack_stack.push(next_token);
			}

			return this_node;
		}
		else
		{
			std::cout << "Error : expected ( after keyword OUT \n";
			return nullptr;
		}
	}
	else if (next_token->token_type == IF)
	{
		getNextToken();
		if (next_token->token_type != O_PARAM)
		{
			std::cout << "Expected ( for IF statement \n";
			backtrack_stack.push(next_token);
			return nullptr;
		}

		this_node->condition = relation();

		getNextToken();
		if (next_token->token_type != E_PARAM)
		{
			std::cout << "Expected ) for IF statement \n";
			backtrack_stack.push(next_token);
		}

		this_node->then = block();
		this_node->type = NT_IF;
		return this_node;

	} // end IF statement
	else if (next_token->token_type == WHILE)
	{
		getNextToken();
		if (next_token->token_type != O_PARAM)
		{
			std::cout << "Expected ( for WHILE statement \n";
			backtrack_stack.push(next_token);
			return nullptr;
		}

		this_node->condition = relation();

		getNextToken();
		if (next_token->token_type != E_PARAM)
		{
			std::cout << "Expected ) for WHILE statement \n";
			backtrack_stack.push(next_token);
		}

		this_node->then = block();
		this_node->type = NT_WHILE;
		return this_node;
	} // end WHILE statement
	else
	{
		backtrack_stack.push(next_token);
		return nullptr;
	}
}


Node* SimpleParser::relation()
{
	Node* this_node = new Node();
	this_node->lhs = exprssion();
	this_node->type = NT_CONDITION;

	getNextToken();
	std::string temp_ops = next_token->value;

	if (is_relation_operator(next_token))		// handle E < E, E > E, E >= E, E <= E
	{
		this_node->rhs = exprssion();
		return this_node;
	}
	else
	{
		std::cout << "Error : expected relation operator in the relational expression \n";
	}
}

// evaluation of condition in the if statement is give as inherited attribute for this.
Node* SimpleParser::block()
{
	// check the begining of the if block by open { curly braces

	Node* this_node = new Node();

	getNextToken();
	if (next_token->token_type != O_BRACES)
	{
		backtrack_stack.push(next_token);
		std::cout << "Error : expect { at the begining of the block \n";
		return this_node;
	}

	//std::cout << inherited->next << ":\n";

	statements(this_node);

	// check the end of the if block by closing } curly braces
	getNextToken();
	if (next_token->token_type != E_BRACES)
	{
		backtrack_stack.push(next_token);
		std::cout << "Error : expect } at the end of the block \n";
	}

	return this_node;
}


Node* SimpleParser::exprssion()
{
	Node* expr_node = new Node();
	Node* term_node = term();

	getNextToken();

	if (next_token->token_type == PLUS)
	{
		expr_node->type = NT_ADD;
	}
	else if (next_token->token_type == MINUS)
	{
		expr_node->type = NT_SUB;
	}

	backtrack_stack.push(next_token);

	Node* expr_node_ = exprssion_();

	if (expr_node_)
	{
		expr_node->lhs = term_node;
		expr_node->rhs = expr_node_;
		//expr_node->type = NT_EXPR;
	}
	else
	{
		expr_node = term_node;
	}

	return expr_node;
}


Node* SimpleParser::exprssion_()
{
	if (!eol)
	{
		Node* this_node = new Node();
		getNextToken();

		if (next_token->token_type == PLUS)
		{
			Node* term_node = term();
			this_node->lhs = term_node;
			Node* expr_node = exprssion_();

			if (expr_node == nullptr)
				return term_node;

			this_node->lhs = term_node;
			this_node->rhs = expr_node;
			this_node->type = NT_ADD;
			return this_node;
		}
		else if (next_token->token_type == MINUS)
		{
			Node* term_node = term();
			this_node->lhs = term_node;
			Node* expr_node = exprssion_();

			if (expr_node == nullptr)
				return term_node;

			this_node->lhs = term_node;
			this_node->rhs = expr_node;
			this_node->type = NT_SUB;
			return this_node;
		}
		else if (next_token->token_type == EMPTY)
		{
			eol = true;
			return nullptr;
		}
		else
		{
			backtrack_stack.push(next_token);
			return nullptr;
		}
	}
	return nullptr;
}


Node* SimpleParser::term()
{
	Node* term_node = new Node();
	Node* factor_node = factor();

	getNextToken();

	if (next_token->token_type == MUL)
	{
		term_node->type = NT_MUL;
	}
	else if (next_token->token_type == DIV)
	{
		term_node->type = NT_DIV;
	}

	backtrack_stack.push(next_token);

	Node* term_node_ = term_();

	if (term_node_)
	{
		term_node->lhs = factor_node;
		term_node->rhs = term_node_;
	}
	else
	{
		term_node = factor_node;
	}

	return term_node;
}


Node* SimpleParser::term_()
{
	if (!eol)
	{
		Node* this_node = new Node();
		getNextToken();

		if (next_token->token_type == MUL)
		{
			Node* f_return = factor();
			Node* term_node = term_();

			if (term_node == nullptr)
				return f_return;

			this_node->lhs = f_return;
			this_node->rhs = term_node;
			this_node->type = NT_MUL;
			return this_node;
		}
		if (next_token->token_type == DIV)
		{
			Node* f_return = factor();
			Node* term_node = term_();

			if (term_node == nullptr)
				return f_return;

			this_node->lhs = f_return;
			this_node->rhs = term_node;
			this_node->type = NT_DIV;
			return this_node;
		}
		else if (next_token->token_type == EMPTY)
		{
			eol = true;
			return nullptr;
		}
		else
		{
			backtrack_stack.push(next_token);
			return nullptr;
		}
	}
	return nullptr;
}


Node* SimpleParser::factor()
{
	if (!eol)
	{
		Node* this_node = new Node();
		getNextToken();

		if (next_token->token_type == NUM)
		{
			this_node->type = NODE_TYPE::NT_NUM;
			this_node->value = atof(next_token->value.c_str());
			this_node->is_leaf = true;
			return this_node;
		}
		else if (next_token->token_type == O_PARAM)
		{
			this_node = exprssion();
			getNextToken();
			if (!next_token->token_type == E_PARAM)
			{
				std::cout << "found an error : could not find the end paranthasis \n";
			}
			return this_node;
		}
		else if (next_token->token_type == ID)
		{
			auto it = symbol_table.find(next_token->value);

			if (it != symbol_table.end())
			{
				this_node->type = NODE_TYPE::NT_VAR;
				this_node->value = it->second;
				this_node->variable_name = it->first;
				this_node->is_leaf = true;
				return this_node;
			}
			else
			{
				std::cout << "Error : found use of undeclared variable" << next_token->value << " \n";
				backtrack_stack.push(next_token);
			}
		}
		else if (next_token->token_type == END || next_token->token_type == SEMI_COLON)
		{
			backtrack_stack.push(next_token);
			return nullptr;
		}
		else if (next_token->token_type == EMPTY)
		{
			eol = true;
			return nullptr;
		}
		else
		{
			std::cout << "found an error string : " << next_token->value << "\n";
			return nullptr;
		}
	}
}


void SimpleParser::parse()
{
	program();
}

Node* SimpleParser::get_head()
{
	return head;
}

