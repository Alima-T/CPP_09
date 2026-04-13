/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 23:24:02 by aokhapki          #+#    #+#             */
/*   Updated: 2026/04/13 23:52:30 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMERGEME_HPP
#define PMERGEME_HPP
#include<vector>
#include<deque>

class PmergeMe
{
private:
	std::vector<int>	m_vector;
	std::deque<int>		m_deque;
	
public:
	PmergeMe();
	PmergeMe(const PmergeMe& src);
	PmergeMe& operator=(const PmergeMe& rhs);
	~PmergeMe();
	void run(int ac, char **av);
	long long sortVectorFordJohnson();
	long long sortDequeFordJohnson();
};



#endif