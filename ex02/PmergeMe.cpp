/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 23:33:07 by aokhapki          #+#    #+#             */
/*   Updated: 2026/04/15 19:32:36 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"
#include <string>
#include <climits>
#include <iostream>
#include <iomanip>
#include <algorithm>   // lower_bound, min
#include <chrono>      // time 1 ms = 1000 us / 1 us = 1000 ns
#include <cstdlib>     // strtol - string to int
#include <cerrno>      // errno and ERANGE in strtol
#include <utility>     // pair / make_pair
#include <stdexcept>   // invalid_argument / overflow_error

/*  STL containers release memory automatically in their destructors (RAII).
	When PmergeMe finishes, the memory is cleaned up automatically.
	duration_cast converts the time difference to microseconds.
	count() returns the numeric value stored in a std::chrono::duration.
*/

PmergeMe::PmergeMe(){}
PmergeMe::PmergeMe(const PmergeMe& src)
{
	this->m_vector = src.m_vector;
	this->m_deque = src.m_deque;
}
PmergeMe& PmergeMe::operator=(const PmergeMe& rhs)
{
	if(this != &rhs)
	{
		this->m_vector = rhs.m_vector;
		this->m_deque = rhs.m_deque;
	}
	return *this;
}

PmergeMe::~PmergeMe(){}

static bool isDigitsOnly(const std::string& str)
{
	if(str.empty())
		return false;
	for(std::size_t i =0; i <str.size(); ++i)
	{
		if(str[i] < '0' || str[i] > '9')
			return false;
	}
	return true;
}
/*
errno = 0;  // Clear the global error variable.
char *endptr;  // Pointer to the rest of the string.
str.c_str() to convert from C++ to C format for strtol (C function).
std::strtol(str.c_str(), &endptr, 10); - strtol converts a str to a long val, 10 = in decimal only (from 0 to 9)
Overflow - INT_MAX || negative || ERANGE - too large.
if (*endptr != '\0') // "123abc" is not valid, endptr points to 'a', not '\0'
*/
static long parseNumber(const std::string& str)
{
	errno = 0;
	char *endptr;  
    
    long num = std::strtol(str.c_str(), &endptr, 10);
    if (errno == ERANGE || num > INT_MAX || num <= 0)
        throw std::overflow_error("Error: Invalid argument");
    if (*endptr != '\0')
        throw std::invalid_argument("Error: Invalid argument");
    return num;
}
// Print container values with a label.
template <typename Container>
static void printSequence(const std::string& label, const Container& c)
{
	std::cout << label;
	for(typename Container::const_iterator it = c.begin(); it != c.end(); ++it)
		std::cout << *it << ' ';
	std::cout << std::endl;
}

// Non-generic helper: sort pair elements by their second value.
static void sortPairsBySecond(std::vector<std::pair<int, int> >& pairs)
{
	for (std::size_t i = 1; i < pairs.size(); ++i)
	{
		std::pair<int, int> current = pairs[i];
		std::size_t j = i;

		while (j > 0 && pairs[j - 1].second > current.second)
		{
			pairs[j] = pairs[j - 1];
			--j;
		}
		pairs[j] = current;
	}
}

/*
Jacobsthal: 1, 3, 2, 5, 4, 10, 9, 8, 7, 6. How this order is built -  J(n) = J(n-1) + 2 × J(n-2):
| # | Jacobsthal range  | Add indexes in reverse order for each group 
| 1 | start 			| 1 		     
| 2 | from 1 to 3 		| 3, 2 		     
| 3 | from 3 to 5 		| 5, 4 		     
| 4 | from 5 to 11 		| 10, 9, 8, 7, 6 
*/
static std::vector<std::size_t> buildJacobsthalInsertionOrder(std::size_t pairCount)
{	
	std::vector<std::size_t> order;

	if (pairCount == 0)
		return order;
	order.push_back(1);
	if (pairCount == 1)
		return order;
	std::size_t previousJacobsthal = 1;
	std::size_t currentJacobsthal = 3;
	while (previousJacobsthal < pairCount) // Build groups until all pairs are processed
	{
		// Prevent going beyond the number of pairs.
		std::size_t upperBound = std::min(currentJacobsthal, pairCount);
		// Add pair indexes in reverse order for this group.
		for (std::size_t i = upperBound; i > previousJacobsthal; --i)
			order.push_back(i);
		std::size_t nextJacobsthal = currentJacobsthal + 2 * previousJacobsthal;// Move to the next Jacobsthal value
		previousJacobsthal = currentJacobsthal;
		currentJacobsthal = nextJacobsthal;
	}
	return order;
}
/*
mainChain already has all larger values in sorted order.
Insert smaller values in Jacobsthal order.
mainChain is always sorted, so use lower_bound to find the correct insert position.
-----------------------------------------
№	index	pair	insert 	  	mainChain
1	  1		(5,9)	     5		[5, 9, 11, 12, 14, 15, 16, 17, 18, 19, 20]
2	  3		(6,12)       6		[5, 6, 9, 11, 12, 14, 15, 16, 17, 18, 19, 20]
...  ...     ...        ...      ......		
10	  6		(8,16)       8		[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20]
-----------------------------------------
*/
static void putSmallerValuesInJacobsthalOrder(std::vector<int>& mainChain,
	const std::vector<std::pair<int, int> >& pairs)
{
	const std::vector<std::size_t> order = buildJacobsthalInsertionOrder(pairs.size());
	for (std::size_t i = 0; i < order.size(); ++i)
	{
		std::size_t pairIndex = order[i] - 1; // Convert 1-based pair number to 0-based index.
		std::vector<int>::iterator pos = std::lower_bound(
			mainChain.begin(), mainChain.end(), pairs[pairIndex].first);
		mainChain.insert(pos, pairs[pairIndex].first);
	}
}
// Same insertion logic for deque.
static void putSmallerValuesInJacobsthalOrder(std::deque<int>& mainChain,
	const std::vector<std::pair<int, int> >& pairs)
{
	const std::vector<std::size_t> order = buildJacobsthalInsertionOrder(pairs.size());
	for (std::size_t i = 0; i < order.size(); ++i)
	{
		std::size_t pairIndex = order[i] - 1;
		std::deque<int>::iterator pos = std::lower_bound(
			mainChain.begin(), mainChain.end(), pairs[pairIndex].first);
		mainChain.insert(pos, pairs[pairIndex].first);
	}
}

/* Ford-Johnson flow:
   Example:  ./PmergeMe 18 3 7 14 2 19 9 5 11 1 16 8 4 20 6 12 17 10 15 13 77
1) split input and order pairs (small, large), unpaired - 77
2) sort pairs by large value, after sorting by pair.second: 1:(5,9)  2:(1,11)  3:(6,12)  4:(7,14)  ...  10:(4,20)
3) build chain from large values mainChain.push_back(pairs[i].second) = [9, 11, 12, 14, 15, 16, 17, 18, 19, 20]
4) insert small values by Jacobsthal order
5) insert unpaired value (if any): 77 -> mainChain = [1, 2, ..., 20, 77]
6) final output: 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 77 ✓
 */
void PmergeMe::sortVectorFordJohnson()
{
	std::vector< std::pair<int, int> > pairs;
	pairs.reserve(m_vector.size() / 2);
	// Keep last value if input size is odd.
	bool hasUnpaired = (m_vector.size() % 2 != 0);
	int unpaired = 0;
	if (hasUnpaired)
		unpaired = m_vector.back();
	// Read input two values at a time [i] & [i+1] and store (smaller,larger) pair
	for (std::size_t i = 0; i + 1 < m_vector.size(); i += 2)
	{
		int left = m_vector[i];
		int right = m_vector[i + 1];
		if (left <= right)
			pairs.push_back(std::make_pair(left, right));
		else
			pairs.push_back(std::make_pair(right, left));
	}
	sortPairsBySecond(pairs);
	std::vector<int> mainChain;
	mainChain.reserve(pairs.size()); 
	for (std::size_t i = 0; i < pairs.size(); ++i) 
	{
		mainChain.push_back(pairs[i].second);
	}
	putSmallerValuesInJacobsthalOrder(mainChain, pairs);
	if (hasUnpaired)
	{
		std::vector<int>::iterator pos = std::lower_bound(
			mainChain.begin(), mainChain.end(), unpaired);
		mainChain.insert(pos, unpaired);
	}
	m_vector = mainChain;
}
// Same steps as vector version, but with deque chain.
void PmergeMe::sortDequeFordJohnson()
{
	std::vector< std::pair<int, int> > pairs;
	pairs.reserve(m_deque.size() / 2);
	bool hasUnpaired = (m_deque.size() % 2 != 0);
	int unpaired = 0;
	if (hasUnpaired)
		unpaired = m_deque.back();
	for (std::size_t i = 0; i + 1 < m_deque.size(); i += 2)
	{
		int left = m_deque[i];
		int right = m_deque[i + 1];
		if (left <= right)
			pairs.push_back(std::make_pair(left, right));
		else
			pairs.push_back(std::make_pair(right, left));
	}
 
	sortPairsBySecond(pairs);

	std::deque<int> mainChain;
	for (std::size_t i = 0; i < pairs.size(); ++i)
	{
		mainChain.push_back(pairs[i].second);
	}
	putSmallerValuesInJacobsthalOrder(mainChain, pairs);

	if (hasUnpaired)
	{
		mainChain.insert(std::lower_bound(mainChain.begin(), mainChain.end(), unpaired), unpaired);
	}
	m_deque = mainChain;
}

/*
1) check the number of arguments (at least 2 are needed: program + 1 number).
	   .clear(); // Clear the containers before a new run.
2) for each arg (starting from index 1 because 0 is the program name).
	- argv[i] comes as a C string (char*), so convert it to C++ std::string.
	- validate input,
	- fill vector and deque,
3) print original before sorting,
4) sort both,
5) print after sorting 
6) print count of elements and the processing time.
*/
void PmergeMe::run(int ac, char **av)
{
	if(ac < 2)
		throw std::invalid_argument("Error: usage is ./PmergeMe [positive int] ...") ;
	m_vector.clear();
	m_deque.clear();

	m_vector.reserve(static_cast<std::size_t>(ac - 1));
	for (int i = 1; i < ac; ++i)
	{
		std::string arg(av[i]); 
		if(!isDigitsOnly(arg))
			throw std::invalid_argument("Error: Invalid argument");
		long num = parseNumber(arg);
		m_vector.push_back(static_cast<int>(num));
		m_deque.push_back(static_cast<int>(num));
	}

	// Print input sequence.
	printSequence<std::vector<int>>("Before: ", m_vector);
	
	// Measure time for vector sorting.
	std::chrono::high_resolution_clock::time_point startVec = std::chrono::high_resolution_clock::now();
	sortVectorFordJohnson();
	std::chrono::high_resolution_clock::time_point endVec = std::chrono::high_resolution_clock::now();
	double vectorTimeUs = std::chrono::duration<double, std::micro>(endVec - startVec).count();
	
	// Measure time for deque sorting.
	std::chrono::high_resolution_clock::time_point startDeq = std::chrono::high_resolution_clock::now();
	sortDequeFordJohnson();
	std::chrono::high_resolution_clock::time_point endDeq = std::chrono::high_resolution_clock::now();
	double dequeTimeUs = std::chrono::duration<double, std::micro>(endDeq - startDeq).count();
	
	// Print sorted sequence.
	printSequence(std::string("After : "), m_vector);
	
	// Print timings with fixed precision.
	std::cout << std::fixed << std::setprecision(5);
	std::cout << "Time to process a range of " << m_vector.size()
		<< " elements with std::vector : " << vectorTimeUs << " us" << std::endl;
	std::cout << "Time to process a range of " << m_deque.size()
		<< " elements with std::deque  : " << dequeTimeUs << " us" << std::endl;
}
