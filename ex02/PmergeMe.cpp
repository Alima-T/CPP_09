/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 23:33:07 by aokhapki          #+#    #+#             */
/*   Updated: 2026/04/15 14:10:53 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"
#include <string>
#include <climits>
#include <iostream>
#include <algorithm>   // std::lower_bound, std::min
#include <chrono>      // time 1 ms = 1000 us / 1 us = 1000 ns
#include <cstdlib>     // std::strtol string to int
#include <cerrno>      // errno and ERANGE in strtol
#include <utility>     // std::pair / std::make_pair
#include <stdexcept>   // std::invalid_argument / std::overflow_error
// m_vector and m_deque are class fields, not raw new pointers.
// STL containers release memory automatically in their destructors (RAII).
// When PmergeMe finishes, the memory is cleaned up automatically.
// duration_cast converts the time difference to microseconds.
// count() returns the numeric value stored in a std::chrono::duration.

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
Overflow - INT_MAX / negative / ERANGE = too large.
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
// Unified helper for both vector and deque for printing a sequence before sorting.
// it != c.end(); or < for vector/deque, but best practice is != because < is not available for list, set, map...
// it - iterator pointing to the current el in the container. *it dereferences & returns the element itself.
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
1st pair = 1, 2d pair = 2 ...
After pairIndex = order[i] - 1, to make them C++-index.1 -> 0, 2 -> 1, 3 -> 2 ...
order stores 1 pair index for inserting smaller values into mainChain.
If no pairs (only 1 digit), return empty order, it is handled separately and is not stored here.
The first pair always comes first.
We use it to insert smaller values in an order, to reduces the number of comparisons during binary search.
Jacobsthal numbers:
J(0) = 0
J(1) = 1
J(n) = J(n - 1) + 2 * J(n - 2)
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
		// Add pair indexes in reverse order from high to low inside this group.
		for (std::size_t i = upperBound; i > previousJacobsthal; --i)
			order.push_back(i);
		// Move to the next Jacobsthal value using the formula J(n) = J(n - 1) + 2 * J(n - 2).
		std::size_t nextJacobsthal = currentJacobsthal + 2 * previousJacobsthal;
		previousJacobsthal = currentJacobsthal;
		currentJacobsthal = nextJacobsthal;
	}
	return order;
}
/*
mainChain already has all larger values in sorted order.
Insert smaller values in Jacobsthal order.
Use lower_bound so mainChain stays sorted.
*/
static void putSmallerValuesInJacobsthalOrder(std::vector<int>& mainChain,
	const std::vector<std::pair<int, int> >& pairs)
{
	const std::vector<std::size_t> order = buildJacobsthalInsertionOrder(pairs.size());
	for (std::size_t i = 0; i < order.size(); ++i)
	{
		std::size_t pairIndex = order[i] - 1; // order is 1-based, pairs is 0-based, so adjust index.
		std::vector<int>::iterator pos = std::lower_bound(
			mainChain.begin(), mainChain.end(), pairs[pairIndex].first);
		mainChain.insert(pos, pairs[pairIndex].first);
	}
}
// Same logic for deque.
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

/*
run: orchestration (preparation and execution) is the conductor, while sortVectorFordJohnson() and sortDequeFordJohnson() are the performers.
(For information: choreography is decentralized. The participants (dancers) react to each other according to the rules.)
sortVectorFordJohnson: the entire Ford-Johnson logic for vector.
sortDequeFordJohnson: the same logic for deque.
Vector: method PmergeMe::sortVectorFordJohnson.
The logic is as follows:
Step A: build pairs.
Take numbers two at a time and store them as (smaller, larger).
Example: (7,3) becomes (3,7).

Step B: sort pairs by second through fordJohnsonSort (without std::sort).
The pairs are sorted with the same merge-insert approach,
but the comparison is done on the larger element (second).
After that, the larger values are in ascending order.

Step C: build mainChain.
Take only second from each pair and place it into mainChain.
This gives you the already sorted backbone.

Step D: insert smaller values with lower_bound.
For each first value:
lower_bound finds the insertion position in mainChain,
insert puts the number there,
so mainChain stays sorted after every insertion.

Step E: if there is an unpaired value.
Do the same:
lower_bound
insert
and finally assign m_vector = mainChain.
sortVectorFordJohnson and sortDequeFordJohnson do the same algorithm.
Steps:
1) Make pairs as (smaller, larger).
2) Sort pairs by larger value.
3) Build mainChain from larger values.
4) Insert smaller values with Jacobsthal order and lower_bound.
5) If one value is left (odd count), insert it with lower_bound.
*/
// The same logic is applied to deque in sortDequeFordJohnson, but with deque-specific types and methods.

long long PmergeMe::sortVectorFordJohnson()
{
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	// Step 2.1 Ford-Johnson: split the input sequence into pairs (smaller, larger).
	std::vector< std::pair<int, int> > pairs;
	pairs.reserve(m_vector.size() / 2);
	// If count is odd, keep last value for final insert.
	bool hasUnpaired = (m_vector.size() % 2 != 0);
	int unpaired = 0;
	if (hasUnpaired)
		unpaired = m_vector.back();
	// Read input two values at a time [i] & [i+1] and store the smaller value first and the larger one second
	for (std::size_t i = 0; i + 1 < m_vector.size(); i += 2)
	{
		int left = m_vector[i];
		int right = m_vector[i + 1];
		if (left <= right)
			pairs.push_back(std::make_pair(left, right));
		else
			pairs.push_back(std::make_pair(right, left));
	}
	sortPairsBySecond(pairs); //second = larger
	std::vector<int> mainChain;
	mainChain.reserve(pairs.size()); 
	//push larger elements only (already sorted)
	for (std::size_t i = 0; i < pairs.size(); ++i) 
	{
		mainChain.push_back(pairs[i].second);
	}
	putSmallerValuesInJacobsthalOrder(mainChain, pairs); // insert the smaller vals in Jacobsthal order.
	if (hasUnpaired) // insert unpaired value (if any).
	{
		std::vector<int>::iterator pos = std::lower_bound(
			mainChain.begin(), mainChain.end(), unpaired);
		mainChain.insert(pos, unpaired);
	}
	m_vector = mainChain;	// sorted result
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

long long PmergeMe::sortDequeFordJohnson()
{
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	// 1. Initialize the pair container.
	// 2. Allocate memory for the pairs.
	// 3. Check whether there is an unpaired element; if yes, store it.
	// 4. Walk through deque two elements at a time, build pairs (smaller, larger), and store them in pairs.
	// 5. Sort pairs by the larger element (second) using fordJohnsonSort.
	// 6. Create mainChain as a deque<int>, fill it with only the larger elements from the pairs (they are already sorted).
	// 7. Loop over pairs and, for each smaller element:
	// 	- find the insertion position in mainChain with std::lower_bound
	// 	- insert the smaller value there while keeping mainChain sorted
	// 8. If there is an unpaired element, insert it into mainChain in the same way.
	// 9. Assign mainChain back to m_deque.
	// 10. Measure execution time and return it in microseconds.
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
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds> (end - start).count();
}

/*
1. Check the number of arguments (at least 2 are needed: program + 1 number).
	   .clear(); // Clear the containers before a new run.
2. For each arg (starting from index 1 because 0 is the program name).
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
	printSequence<std::vector<int>>("Before: ", m_vector);
	long long vectorTimeUs = sortVectorFordJohnson();
	long long dequeTimeUs = sortDequeFordJohnson();
	printSequence(std::string("After: "), m_vector);
	std::cout << "Time to process a range of " << m_vector.size()
		<< " elements with std::vector : " << vectorTimeUs << " us" << std::endl;
	std::cout << "Time to process a range of " << m_deque.size()
		<< " elements with std::deque  : " << dequeTimeUs << " us" << std::endl;
}




