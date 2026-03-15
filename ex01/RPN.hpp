/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 22:33:47 by aokhapki          #+#    #+#             */
/*   Updated: 2026/03/15 22:50:06 by aokhapki         ###   ########.fr       */
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
public:
	RPN();
	RPN(const RPN& src);
	RPN& operator=(const RPN& rhs);
	~RPN();
};

#endif