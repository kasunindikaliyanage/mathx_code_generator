#pragma once

#include <string>
#include <iostream>
#include <cctype>
#include <fstream>

// TODO arrange the constant in more suitable or logical order
// TODO introduce list of unique pointers which stores the reusable tokens and return those when match found.

#define PLUS		 1
#define MINUS		 2
#define MUL			 3
#define DIV			 4
#define NUM			 5
#define O_PARAM		 6
#define E_PARAM		 7
#define POW			 8
#define OTHER		 99
#define EMPTY		 100

#define ID			 50

#define START		 20
#define END			 21
#define AUTO		 22
#define OUT			 23
#define SEMI_COLON	 25
#define EQUAL		 26
#define OUT			 27
#define COMMA		 28
#define O_BRACES	 29
#define E_BRACES	 30	

#define EE			 35
#define LE			 36
#define GE			 37
#define GREATER		 38
#define LESS		 39

#define MIN_RELATION_OPS EE
#define MAX_RELATION_OPS LESS

#define IF			 60
#define ELSE		 61
#define WHILE		 62       

struct Token
{
	int token_type = -1;
	std::string value = "";

	Token(int _token_type, std::string _value) : token_type(_token_type), value(_value)
	{}
};


class SimpleLexer
{
	int line_count = 0;
	char next_char =' ';

	std::ifstream input_stream;

	bool compare_string(std::string str1, std::string str2)
	{
		if (str1.compare(str2) == 0)
			return true;

		return false;
	}

	char peek_next()
	{
		char str = input_stream.peek();
		return str;
	}

	void read_next()
	{
		input_stream.get(next_char);
		if (input_stream.eof())
			isEOF = true;
	}

public:
	bool isEOF = false;

	SimpleLexer(std::string file_name)
	{
		isEOF = false;
		input_stream.open(file_name);
	}

	Token* getNextToken()
	{
		read_next();

		while (!isEOF)
		{
			if (next_char == ' ' || next_char == '\t')
			{
				read_next();
				continue;
			}
			else if (next_char == '\n')
			{
				line_count += 1;
				read_next();
			}
			else
			{
				break;
			}
		}

		if (std::isdigit(next_char))
		{
			int v = 0;
			char chr;

			do {
				v = 10 * v + std::atoi(&next_char);
				chr = peek_next();
				if (std::isdigit(chr))
				{
					read_next();
					continue;
				}
				break;
			} while (true);

			if (chr != '.')
				return new Token(NUM, std::to_string(v));
			
			read_next();
			float r = v;
			float d = 10;

			do {
				chr = peek_next();
				if (!std::isdigit(chr))
					break;

				read_next();
				r = r + std::atoi(&next_char) / d;
				d = d * 10;
			} while (true);

			return new Token(NUM, std::to_string(r));
		}
		else if (std::isalpha(next_char))
		{
			std::string* temp = new std::string();
			char chr;
			do {
				temp->append(1, next_char);
				chr = peek_next();
				if (std::isalnum(chr))
				{
					read_next();
					continue;
				}
				break;
			} while (true);

			if (compare_string(*temp, "out"))
			{
				return new Token(OUT, *temp);
			}
			else if (compare_string(*temp, "auto"))
			{
				return new Token(AUTO, *temp);
			}
			else if (compare_string(*temp, "START"))
			{
				return new Token(START, *temp);
			}
			else if (compare_string(*temp, "END"))
			{
				return new Token(END, *temp);
			}
			else if (compare_string(*temp, "if"))
			{
				return new Token(IF, *temp);
			}
			else if (compare_string(*temp, "else"))
			{
				return new Token(ELSE, *temp);
			}
			else if (compare_string(*temp, "while"))
			{
				return new Token(WHILE, *temp);
			}
			else
			{
				return new Token(ID, *temp);
			}
		}
		else if (next_char == '+')
		{
			return new Token(PLUS, std::string(1, next_char));
		}
		else if (next_char == '-')
		{
			return new Token(MINUS, std::string(1, next_char));
		}
		else if (next_char == '*')
		{
			return new Token(MUL, std::string(1, next_char));
		}
		else if (next_char == '/')
		{
			return new Token(DIV, std::string(1, next_char));
		}
		else if (next_char == '(')
		{
			return new Token(O_PARAM, std::string(1, next_char));
		}
		else if (next_char == ')')
		{
			return new Token(E_PARAM, std::string(1, next_char));
		}
		else if (next_char == '^')
		{
			return new Token(POW, std::string(1, next_char));
		}
		else if (next_char == ';')
		{
			return new Token(SEMI_COLON, std::string(1, next_char));
		}
		else if (next_char == '=')
		{
			char chr = peek_next();
			
			if (chr == '=')
			{
				read_next();
				return new Token(EE, std::string(1, next_char));
			}
			else
				return new Token(EQUAL, std::string(1, next_char));
		}
		else if (next_char == ',')
		{
			return new Token(COMMA, std::string(1, next_char));
		}
		else if (next_char == '{')
		{
			return new Token(O_BRACES, std::string(1, next_char));
		}
		else if (next_char == '}')
		{
			return new Token(E_BRACES, std::string(1, next_char));
		}
		else if (next_char == '>')
		{
			char chr = peek_next();
			
			if ( chr == '=' )
			{
				read_next();
				return new Token(GE, std::string(1, next_char));
			}
			else
				return new Token(GREATER, std::string(1, next_char));
		}
		else if (next_char == '<')
		{
			char chr = peek_next();

			if (chr == '=')
			{
				read_next();
				return new Token(LE, std::string(1, next_char));
			}
			else
				return new Token(LESS, std::string(1, next_char));
		}
		else if (next_char == '\0' || next_char == '\n')
		{
			return new Token(EMPTY, std::string("Empty string"));
		}
		else
		{
			return new Token(OTHER, std::string(1, next_char));
		}
	}
};