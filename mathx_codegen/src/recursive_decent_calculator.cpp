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


static void new_label( stmts* statements, bool isBegin= false )
{
	y++;

	if(!isBegin)
		statements->next = "L" + std::to_string(y);
	else
		statements->begin = "L" + std::to_string(y);
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


void SimpleParser::program()
{
	getNextToken();
	if (next_token->token_type == START)
	{
		decls();

		stmts* temp = new stmts();
		//new_label(temp);
		//std::cout << temp->next << ":" <<std::endl;
		
		new_label(temp);
		statements(temp);
		//std::cout << temp->next << ":" << std::endl;
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


void SimpleParser::statements( stmts* inherited )
{
	getNextToken();
	backtrack_stack.push(next_token);

	//here the condition check should be END && EOF
	if (next_token->token_type != END && !lexer->isEOF && next_token->token_type != E_BRACES)
	{
		std::cout << inherited->next << ":\n";
		new_label( inherited );
		statement( inherited );
		statements( inherited );
	}
	else
	{
		return;
	}
}


void SimpleParser::statement( stmts* inherited )
{
	getNextToken();

	expr* temp_expr = new expr();

	if (next_token->token_type == ID)
	{
		if (symbol_table.find(next_token->value) != symbol_table.end())
		{
			std::string temp = next_token->value;
			getNextToken();

			if (next_token->token_type == EQUAL)
			{
				temp_expr = exprssion();
				getNextToken();

				if (next_token->token_type != SEMI_COLON)
				{
					std::cout << "Error : expect ; after expression \n";
				}

				temp_expr->code += temp + " = " + temp_expr->addr+ "\n";
				std::cout << temp_expr->code << std::endl;

				auto it = symbol_table.find(temp);
				it->second = temp_expr->value;
			}
		}
		else
		{
			std::cout << "Error : found use of undeclared variable : " << next_token->value << " \n";
		}
	}
	else if (next_token->token_type == OUT)
	{
		getNextToken();
		if (next_token->token_type == O_PARAM)
		{
			expr* result = exprssion();

			getNextToken();
			if (next_token->token_type != E_PARAM)
			{
				std::cout << "Error : expected ) at the end of OUT expression \n";
			}

			std::cout << "DEBUG : \"" << out_id_name << "\" : " << result->value << " \n";
		}
		else
		{
			std::cout << "Error : expected ( after keyword OUT \n";
		}
	}
	else if (next_token->token_type == IF)
	{
		stmts* temp = new stmts();
		new_label(temp);

		getNextToken();
		if (next_token->token_type != O_PARAM)
		{
			std::cout << "Expected ( for IF statement \n";
		}

		condition* cond_temp = new condition();
		cond_temp->true_lbl = temp->next;
		cond_temp->false_lbl = inherited->next;

		relation(cond_temp);

		getNextToken();
		if (next_token->token_type != E_PARAM)
		{
			std::cout << "Expected ) for IF statement \n";
		}

		std::cout << cond_temp->code;
		std::cout << cond_temp->true_lbl << ":\n";

		block(inherited);

	} // end IF statement
	else if (next_token->token_type == WHILE)
	{
		getNextToken();
		if (next_token->token_type == O_PARAM)
		{
			expr* result1 = exprssion();

			stmts* temp = new stmts();
			new_label(temp, true);
			new_label(temp);
;

			std::cout << temp->begin << ":\n";
			std::cout << result1->code << "\n";

			//temp->code += temp->begin + 
			//temp->code += result1->code;

			getNextToken();
			if (is_relation_operator(next_token))
			{
				std::string temp_ops = next_token->value;
				// store the token type, as below call to expr will change the tocken
				int relation_op = next_token->token_type;

				// right side of the relation expression.
				expr* result2 = exprssion();
				std::cout << result2->code;
				//temp->code += result2->code;

				getNextToken();

				// end paranthasis of IF condition when both E1 and E2 are present
				if (next_token->token_type == E_PARAM)
				{
					temp->code += "if " + result1->addr + " " + temp_ops + " " + result2->addr + " goto "+ temp->next +"\n";
					temp->code += "goto " + inherited->next + " \n";
					
					std::cout << temp->code ;
					
					block(temp);
					std::cout << "goto " << temp->begin << "\n";
					std::cout << inherited->next << ":\n";
				}
			}
			else if (next_token->token_type == E_PARAM) // end paranthasis of IF condition if only one E is present
			{
				//std::cout << " iffalse " << result1->addr << " > " << "0  goto " << inherited->next << "\n";
				//block(temp);

				//std::cout << inherited->next << ":\n";
			}
			else
			{
				std::cout << "Error : expected relation operator in the WHILE statement \n";
			}
		}
		else
		{
			std::cout << "Error : expected ( after keyword WHILE \n";
		}
	} // end WHILE statement
	else
	{
		backtrack_stack.push(next_token);
		temp_expr = exprssion();
		getNextToken();
		
		if (next_token->token_type != SEMI_COLON)
		{
			std::cout << "Error : expect ; after expression \n";
		}

		std::cout << temp_expr->code << std::endl;
	}
}


void SimpleParser::relation(condition* inherited)
{
	expr* lhs_expr = exprssion();

	getNextToken();
	std::string temp_ops = next_token->value;

	if ( is_relation_operator( next_token ))		// handle E < E, E > E, E >= E, E <= E
	{
		expr* rhs_expr = exprssion();
		
		inherited->code = lhs_expr->code + rhs_expr->code
						+ "if " + lhs_expr->addr + " " + temp_ops + " " + rhs_expr->addr + " goto " + inherited->true_lbl + "\n"
						+ "goto " + inherited->false_lbl + "\n";
	}
	else if (next_token->token_type == E_PARAM)		// handle only E
	{
		// end paranthasis will be handled from statement level.
		// so push the current token to the backtrack stack.
		backtrack_stack.push(next_token);
		inherited->code = lhs_expr->code
			+ "if " + lhs_expr->addr + " " + temp_ops + " 0 " + " goto " + inherited->true_lbl + "\n"
			+ "goto " + inherited->false_lbl + "\n";
	}
	else
	{
		std::cout << "Error : expected relation operator in the relational expression \n";
	}
}

// evaluation of condition in the if statement is give as inherited attribute for this.
void SimpleParser::block( stmts* inherited )
{
	// check the begining of the if block by open { curly braces
	
	getNextToken();
	if (next_token->token_type != O_BRACES)
	{
		std::cout << "Error : expect { at the begining of the IF block \n";
	}

	//std::cout << inherited->next << ":\n";

	statements(inherited);

	// check the end of the if block by closing } curly braces
	getNextToken();
	if (next_token->token_type != E_BRACES)
	{
		std::cout << "Error : expect } at the end of the IF block \n";
	}
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
			temp->code = f_return->code + inherited->code + temp->addr + " = " + inherited->addr + " * " + f_return->addr + "\n";
			temp->value = inherited->value * f_return->value;

			return term_( temp );
		}
		if (next_token->token_type == DIV)
		{
			expr* f_return = factor(inherited);
			new_temp(temp);
			temp->code = f_return->code + inherited->code + temp->addr + " = " + inherited->addr + " / " + f_return->addr + "\n";
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
	//expr* return_val = exprssion();
	program();

	//std::cout << "Generated Code : "<< "\n" << return_val->code << std::endl;
	//std::cout << "Calculated value : " << return_val->value << std::endl;
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