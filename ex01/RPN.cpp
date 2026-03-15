/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 22:33:44 by aokhapki          #+#    #+#             */
/*   Updated: 2026/03/16 00:46:40 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

RPN::RPN() {}

RPN::RPN(const RPN& src) : m_stack(src.m_stack) {}

RPN& RPN::operator=(const RPN& rhs)
{
	if(this != &rhs)
	{
		m_stack = rhs.m_stack;
	}
	return *this;
}

RPN::~RPN() {}

bool RPN::isOperator(const std::string& token)
{
	return token.size() == 1 && (token[0] =='+' || token[0] == '-' || token[0] == '*' || token[0] == '/');
}

//TODO: Handle empty strings, negative numbers and multi-digit numbers in isDigit
bool RPN::isDigit(const std::string& token)
{
	return token.length() == 1 && token[0] >= '0' && token[0] <= '9';
}

void RPN::evaluateOperator(const std::string& op)
{
	//TODO: Implement the logic to pop the top two operands from the stack, perform the operation based on the operator, and push the result back onto the stack
}

//TODO: Handle division by zero and other edge cases in evaluateOperator
void RPN::calculate(const std::string& expression)
{
	//TODO: Implement the logic to parse the expression, handle whitespace, and evaluate the RPN expression using the stack
}
