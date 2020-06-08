#pragma once

#include <iostream>

#include "recursive_decent_calculator.h"

const char* CHR_ADD = "+";
const char* CHR_SUB = "-";
const char* CHR_MUL = "*";
const char* CHR_DIV = "/";
const char* CHR_ASSIGN = "=";


class code_generator {

	int label_index = 1;
	int temp_var_index = 0;

	void print_binary_ops(Node* lhs, Node* rhs, std::string ops)
	{
		std::cout << "	t" << temp_var_index << " = " << lhs->to_string() << ops << " " << rhs->to_string() << "\n";
	}

	bool is_binary_ops(NODE_TYPE type)
	{
		if (type == NODE_TYPE::NT_ADD
			|| type == NODE_TYPE::NT_SUB
			|| type == NODE_TYPE::NT_MUL
			|| type == NODE_TYPE::NT_DIV
			|| type == NODE_TYPE::NT_ASSIGN)
		{
			return true;
		}

		return false;
	}

public:

	void generate(Node* current_node, NODE_TYPE parent_type)
	{
		if (current_node == nullptr || current_node->type == NT_VOID)
			return;

		generate(current_node->lhs, current_node->type);
		generate(current_node->rhs, current_node->type);

		if (current_node->type == NT_ADD)
		{
			current_node->temp_var_index = ++temp_var_index;
			print_binary_ops(current_node->lhs, current_node->rhs, CHR_ADD);
		}
		else if (current_node->type == NT_SUB)
		{
			current_node->temp_var_index = ++temp_var_index;
			print_binary_ops(current_node->lhs, current_node->rhs, CHR_SUB);
		}
		else if (current_node->type == NT_MUL)
		{
			current_node->temp_var_index = ++temp_var_index;
			print_binary_ops(current_node->lhs, current_node->rhs, CHR_MUL);
		}
		else if (current_node->type == NT_DIV)
		{
			current_node->temp_var_index = ++temp_var_index;
			print_binary_ops(current_node->lhs, current_node->rhs, CHR_DIV);
		}
		else if (current_node->type == NT_ASSIGN)
		{
			std::cout << "	" << current_node->variable_name << " = " << current_node->rhs->to_string() << "\n";
		}

		// TODO if the parent node is a binary operator then we can print the leaf value if self
		else if (!is_binary_ops(parent_type))
		{
			//
		}

		if (current_node->next != nullptr)
		{
			std::cout << "L" << ++label_index;
			generate(current_node->next, current_node->type);
		}
	}
};