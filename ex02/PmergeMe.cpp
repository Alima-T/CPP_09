/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 23:33:07 by aokhapki          #+#    #+#             */
/*   Updated: 2026/04/09 18:02:08 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"
#include <string>
#include <climits>
#include <stdexcept>

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
    
    // ПРОВЕРКА 1: Переполнение? (число больше INT_MAX)
    // ПРОВЕРКА 2: Отрицательное число?
    // ПРОВЕРКА 3: strtol не смог корректно преобразовать (ERANGE = ошибка)
    if (errno == ERANGE || num > INT_MAX || num < 0)
        throw std::overflow_error("Number out of range or negative");
    
    // ПРОВЕРКА 4: Остались ещё символы? (например "123abc")
    if (*endptr != '\0')
        throw std::invalid_argument("Invalid number format");
    
    return num;
}


void PmergeMe::run(int ac, char **av)
{
    // ШАГИ:
    // 1. Проверить количество аргументов (нужно минимум 2: программа + 1 число)
    // 2. Цикл по каждому аргументу (начиная с индекса 1, т.к. 0 - это имя программы)
    // 3. Для каждого аргумента:
    //    - Проверить, что это только цифры
    //    - Преобразовать в число
    //    - Добавить в оба контейнера (vector и deque)
    // 4. Вывести исходный массив
    // 5. Запустить сортировку и измерить время
    // 6. Вывести результат
}
