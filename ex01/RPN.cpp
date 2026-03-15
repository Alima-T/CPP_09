/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 22:33:44 by aokhapki          #+#    #+#             */
/*   Updated: 2026/03/15 22:51:43 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

RPN::RPN(){}

RPN::RPN(const RPN& src) : m_stack(src.m_stack) {}

RPN& RPN::operator=(const RPN& rhs)
{
	if(this != &rhs)
	{
		m_stack = rhs.m_stack;
	}
	return *this;
}