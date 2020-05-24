#include "../headers/recursive_decent_calculator.h"

#include <cmath>

// TODO urgently add understandable comments : consider using CFG for corresponding function in the comment as well
// TODO this is the branch check.

static int x = 0;


void SimpleParser::getNextToken()
{
	if (!backtrack_stack.empty())
	{
		next_token = backtrack_stack.top();
		backtrack_stack.pop();
	}
	else if( !lexer->isEOF)
		next_token = lexer->getNextToken();
}


static bool is_relation_operator(Token* current_token)
{
	if (current_token->token_type >= MIN_RELATION_OPS && current_token->token_type <= MAX_RELATION_OPS)
		return true;

	else return false;
}

// Generate new temporary label ti
static void new_temp( expr* expression )
{
	x++;
	expression->addr = "t"+ std::to_string(x);
}


static bool evaluate_relation_operator(int l_value, int r_value, int ops)
{
	if ( ops == EE )			// == operator
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


expr* SimpleParser::exprssion()
{
	expr* temp = new expr();
	expr* t_return = term(temp);
	return exprssion_(t_return);
}


expr* SimpleParser::exprssion_( expr* inherited )
{
	if (!eol)
	{
		expr* temp = new expr();
		getNextToken();

		if (next_token->token_type == PLUS)
		{
			expr* t_return = term(temp);
			new_temp(temp);
			temp->code = inherited->code + t_return->code + temp->addr + " = " + inherited->addr + " + " + t_return->addr + "\n";
			temp->value = inherited->value + t_return->value;

			return exprssion_(temp);
		}
		else if (next_token->token_type == MINUS)
		{
			expr* t_return = term(temp);
			new_temp(temp);
			temp->code = inherited->code + t_return->code + temp->addr + " = " + inherited->addr + " - " + t_return->addr + "\n";
			temp->value = inherited->value - t_return->value;

			return exprssion_(temp);
		}
		else if (next_token->token_type == EMPTY)
		{
			return inherited;
			eol = true;
		}
		else
		{
			backtrack_stack.push(next_token);
			return inherited;
		}
	}
	return inherited;
}


expr* SimpleParser::term( expr* inherited )
{
	expr* f_return = factor(inherited);
	expr* t_dash_return = term_(f_return);
	return t_dash_return;
}


expr* SimpleParser::term_( expr* inherited )
{
	if (!eol)
	{
		expr* temp = new expr();
		getNextToken();

		if (next_token->token_type == MUL)
		{
			expr* f_return = factor(inherited);
			new_temp(temp);
			temp->code = f_return->code + inherited->code + temp->addr + " = " + f_return->addr + " * " + inherited->addr + "\n";
			temp->value = inherited->value * f_return->value;

			return term_( temp );
		}
		if (next_token->token_type == DIV)
		{
			expr* f_return = factor(inherited);
			new_temp(temp);
			temp->code = f_return->code + inherited->code + temp->addr + " = " + f_return->addr + " / " + inherited->addr + "\n";
			temp->value = inherited->value / f_return->value;

			return term_(temp);
		}
		else if (next_token->token_type == EMPTY)
		{
			eol = true;
			return inherited;
		}
		else
		{
			backtrack_stack.push(next_token);
			return inherited;
		}
	}
	return inherited;
}

/*
expr SimpleParser::power(double inherited, bool condition_status )
{
	double p_return = factor(inherited);
	double t_dash_return = power_(p_return);
	return t_dash_return;
}


expr SimpleParser::power_(double inherited, bool condition_status )
{
	if (!eol)
	{
		getNextToken();

		if (next_token->token_type == POW)
		{
			expr p_return = factor(inherited);
			double f_dash_inherited = std::pow(inherited, p_return.value);

			return power_(f_dash_inherited);
		}
		else if (next_token->token_type == EMPTY)
		{
			eol = true;
			return inherited;
		}
		else
		{
			backtrack_stack.push(next_token);
			return inherited;
		}
	}
	return inherited;
}
*/

expr* SimpleParser::factor( expr* inherited )
{
	if (!eol)
	{
		expr* temp = new expr();
		getNextToken();

		if (next_token->token_type == NUM)
		{
			temp->addr = next_token->value;
			temp->value = atof(next_token->value.c_str());
			temp->code = "";

			return temp;
		}
		else if (next_token->token_type == O_PARAM)
		{
			expr* result = exprssion();
			getNextToken();
			if (!next_token->token_type == E_PARAM)
			{
				std::cout << "found an error : could not find the end paranthasis \n";
			}
			return result;
		}
		else if (next_token->token_type == ID)
		{
			auto it = symbol_table.find(next_token->value);

			if (it != symbol_table.end())
			{
				out_id_name = it->first;
				temp->addr = it->first;
				temp->value = it->second;
				temp->code = "";
				return temp;
			}
			else
			{
				std::cout << "Error : found use of undeclared variable" << next_token->value << " \n";
			}
		}
		else if (next_token->token_type == END || next_token->token_type == SEMI_COLON)
		{
			backtrack_stack.push(next_token);
			return inherited;
		}
		else if (next_token->token_type == EMPTY)
		{
			eol = true;
			return inherited;
		}
		else
		{
			std::cout << "found an error string : " << next_token->value << "\n";
			return inherited;
		}
	}
}


void SimpleParser::parse()
{
	expr* return_val = exprssion();
	//program();

	std::cout << "Generated Code : "<< "\n" << return_val->code << std::endl;
	std::cout << "Calculated value : " << return_val->value << std::endl;
}