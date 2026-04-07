/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 23:33:07 by aokhapki          #+#    #+#             */
/*   Updated: 2026/04/07 23:37:47 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

PmergeMe::PmergeMe(){}
PmergeMe::PmergeMe(const PmergeMe& src)
{
	this->m_vector = src.m_vector;
	this->m_queue = src.m_queue;
}
PmergeMe& PmergeMe::operator=(const PmergeMe& rhs)
{
	if(this != &rhs)
	{
		this->m_vector = rhs.m_vector;
		this->m_queue = rhs.m_queue;
	}
	return *this;
}

PmergeMe::~PmergeMe(){}
