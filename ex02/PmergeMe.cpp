/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 23:33:07 by aokhapki          #+#    #+#             */
/*   Updated: 2026/04/13 21:33:55 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"
#include <string>
#include <climits>
#include <iostream>
#include <algorithm>   // std::sort 
#include <chrono>      // time 1 ms = 1000 us / 1 us = 1000 ns
#include <cstdlib>     // std::strtol string to int
#include <cerrno>      // errno и ERANGE in strtol
#include <stdexcept>   // std::invalid_argument / std::overflow_error
// m_vector и m_deque — поля класса, не new-указатели
// у STL контейнеров память освобождается автоматически в деструкторе (RAII)
// при завершении PmergeMe память сама очистится

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

static bool isValidPositivNumber(const std::string& str)
{
	if(str.empty())
		return false;
	for(std::size_t i =0; i <str.size(); ++i)
	{
		// Если символ < '0' ИЛИ > '9' → это не цифра
		if(str[i] < '0' || str[i] > '9')
			return false;
	}
	return true;
}

static long parseNumber(const std::string& str)
{
    errno = 0;  // Очищаем глобальную переменную ошибок
    char *endptr;  // Указатель на остаток строки (если она не полностью число)
    
    // strtol: преобразует строку в long число
    // 10 = десятичная система
    long num = std::strtol(str.c_str(), &endptr, 10);
    
    // Переполнение - INT_MAX/ negative / ERANGE = too large)
    if (errno == ERANGE || num > INT_MAX || num < 0)
        throw std::overflow_error("Number out of range or negative");
    
    // Остались ещё символы? (например "123abc")
    if (*endptr != '\0')
        throw std::invalid_argument("Invalid number format");
    
    return num;
}
// унифицированный хелпер для печати последовательности до сортировки.
// как для vector, так и для deque, чтобы избежать дублирования кода.
// it != c.end(); or < for vector, deque, but bestpractice template !=, because < impossible for list, set, map...
//it — итератор, он указывает на текущий элемент контейнера.
// *it — разыменование итератора, то есть сам элемент, на который он указывает.
// std::cout << — отправить этот элемент в стандартный вывод.
// ' ' — добавить после числа один пробел, чтобы элементы печатались не слитно, а через пробел.
template <typename Container>
static void printSequence(const std::string& label, const Container& c)
{
	std::cout << label;
	for(typename Container::const_iterator it = c.begin(); it != c.end(); ++it)
		std::cout << *it << ' ';
	std::cout << std::endl;
}
//duration_cast переводит разницу времени в микросекунды.
template <typename Container>
static long long sortAndMeasureUs(Container& c)
{
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	std::sort(c.begin(), c.end());
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); // count() - берёт числовое значение из объекта std::chrono::duration.
}

/*
    1. Проверить количество аргументов (нужно минимум 2: программа + 1 число)
	   .clear();// Очищаем контейнеры перед новым запуском.
    2. Цикл по каждому аргументу (начиная с индекса 1, т.к. 0 - это имя программы)
    3. Для каждого аргумента:
       - Проверить, что это только цифры
       - Преобразовать в число
       - Добавить в оба контейнера (vector и deque)
    4. Вывести исходный массив
    5. Запустить сортировку и измерить время
    6. Вывести результат
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
		// argv[i] приходит как C-строка(char*), переводим её в std::string.
		std::string arg(av[i]);
		// должна состоять из цифр, исключаем варианты: пробелы, знаки, буквы и тд
		if(!isValidPositivNumber(arg))
			throw std::invalid_argument("Error: Invalid argument");
		// Преобразуем строку в число и проверяем переполнение.
		long num = parseNumber(arg);

		// Кладём одно и то же значение в оба контейнера, чтобы сравнить их сортировку.
		m_vector.push_back(static_cast<int>(num));
		m_deque.push_back(static_cast<int>(num));
	}

	// Выводим исходную последовательность до сортировки.
	// Печатаем именно vector, потому что его содержимое полностью совпадает с deque.
	// Идём по контейнеру итератором и печатаем каждый элемент.
	printSequence<std::vector<int>>("Before: ", m_vector);
	// Сортируем и меряем время через один helper для обоих контейнеров.
	long long vectorTimeUs = sortAndMeasureUs(m_vector);
	long long dequeTimeUs = sortAndMeasureUs(m_deque);
	// Выводим отсортированную последовательность, только vector, тк deque заполнен теми же значениями.
	printSequence(std::string("After: "), m_vector);
	// Печатаем время обработки vector. 
	std::cout << "Time to process a range of " << m_vector.size()
		<< " elements with std::vector : " << vectorTimeUs << " us" << std::endl;
	// Печатаем аналогичное время для deque, чтобы можно было сравнить структуры.
	std::cout << "Time to process a range of " << m_deque.size()
		<< " elements with std::deque  : " << dequeTimeUs << " us" << std::endl;
}




