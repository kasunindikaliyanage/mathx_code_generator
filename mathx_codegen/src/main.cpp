#include <iostream>

#include "../headers/simple_lexer.h"
#include "../headers/recursive_decent_calculator.h"

int main()
{
	//SimpleLexer my_lexer("C:\\Projects\\mathx\\mathx\\tests\\recursive_decent_calculator.txt");
	//SimpleLexer my_lexer("C:\\Projects\\mathx\\mathx\\tests\\if_with_relation_operation_check.txt");
	SimpleLexer my_lexer("C:\\Projects\\mathx_code_genarator\\mathx_codegen\\tests\\plus_mul_code_gen.txt");
	SimpleParser parser(&my_lexer);

	parser.parse();

	return 0;
}