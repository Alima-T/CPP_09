/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 22:33:47 by aokhapki          #+#    #+#             */
/*   Updated: 2026/03/16 00:43:32 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPN_HPP
#define RPN_HPP

#include <string>
#include <stack>

class RPN
{
private:
	std::stack<long long> m_stack;
	
	static bool isOperator(const std::string& token);
	static bool isDigit(const std::string& token);
	//TODO: whitespace handling and error handling for invalid tokens
	void evaluateOperator(const std::string& op);
	
	
public:
	RPN();
	RPN(const RPN& src);
	RPN& operator=(const RPN& rhs);
	~RPN();
	
	void calculate(const std::string& expression);
	long long getResult() const;
};

#endif