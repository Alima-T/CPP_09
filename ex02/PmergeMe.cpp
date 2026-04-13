/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 23:33:07 by aokhapki          #+#    #+#             */
/*   Updated: 2026/04/13 20:03:39 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"
#include <string>      // std::string для работы с аргументами
#include <climits>     // INT_MAX для проверки диапазона int
#include <iostream>    // std::cout / std::endl для вывода
#include <algorithm>   // std::sort для сортировки контейнеров
#include <chrono>      // std::chrono для замера времени
#include <cstdlib>     // std::strtol для преобразования строки в число
#include <cerrno>      // errno и ERANGE для контроля ошибок strtol
#include <stdexcept>   // std::invalid_argument / std::overflow_error

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
    
    // Переполнение? (число больше INT_MAX)
    // Отрицательное число?
    // strtol не смог корректно преобразовать (ERANGE = ошибка)
    if (errno == ERANGE || num > INT_MAX || num < 0)
        throw std::overflow_error("Number out of range or negative");
    
    // ПРОВЕРКА 4: Остались ещё символы? (например "123abc")
    if (*endptr != '\0')
        throw std::invalid_argument("Invalid number format");
    
    return num;
}

/*
    1. Проверить количество аргументов (нужно минимум 2: программа + 1 число)
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
	// Проверяем, что после имени программы передан хотя бы один аргумент.
	// Если аргументов нет, то сортировать нечего и работа программы бессмысленна.
	if(ac < 2)
		throw std::invalid_argument("Error: usage is ./PmergeMe [positive int] ...") ;

	// Очищаем контейнеры на случай повторного вызова run().
	// Это защищает от ситуации, когда старые данные случайно смешались бы с новыми.
	m_vector.clear();
	m_deque.clear();

	// Заранее выделяем память под vector.
	// Это уменьшает число перераспределений памяти при добавлении элементов.
	m_vector.reserve(static_cast<std::size_t>(ac - 1));
	for (int i = 1; i < ac; ++i)
	{
		// Каждый argv[i] приходит как C-строка, поэтому сначала переводим её в std::string.
		// Так удобнее выполнять проверки и преобразования.
		std::string arg(av[i]);

		// Проверяем только внешний формат строки: она должна состоять из цифр.
		// На этом этапе отбрасываем пробелы, знаки, буквы и любые смешанные значения.
		if(!isValidPositivNumber(arg))
			throw std::invalid_argument("Error: Invalid argument");

		// Преобразуем строку в число и дополнительно проверяем переполнение.
		// Это защищает от слишком больших значений и некорректного разбора строки.
		long num = parseNumber(arg);

		// Кладём одно и то же значение в оба контейнера.
		// Это нужно, чтобы потом сравнить их сортировку на одинаковых данных.
		m_vector.push_back(static_cast<int>(num));
		m_deque.push_back(static_cast<int>(num));
	}

	// Выводим исходную последовательность до сортировки.
	// Печатаем именно vector, потому что его содержимое полностью совпадает с deque.
	std::cout << "Before: ";
	// Идём по контейнеру итератором и печатаем каждый элемент.
	// Используем const_iterator, потому что элементы здесь только читаются.
	for (std::vector<int>::const_iterator it = m_vector.begin(); it != m_vector.end(); ++it)
		std::cout << *it << ' '; 
	//it — итератор, он указывает на текущий элемент контейнера.
	// *it — разыменование итератора, то есть сам элемент, на который он указывает.
	// std::cout << — отправить этот элемент в стандартный вывод.
	// ' ' — добавить после числа один пробел, чтобы элементы печатались не слитно, а через пробел.
	// Завершаем строку, чтобы следующий вывод начался с новой линии.
	std::cout << std::endl;

	// Фиксируем момент времени перед сортировкой vector.
	// high_resolution_clock используем, чтобы получить максимально точный замер.
	std::chrono::high_resolution_clock::time_point vStart = std::chrono::high_resolution_clock::now();
	// Сортируем vector стандартным алгоритмом.
	// Пока это базовая реализация, чтобы проверить логику парсинга и вывода.
	std::sort(m_vector.begin(), m_vector.end());
	// Фиксируем момент времени сразу после сортировки vector.
	std::chrono::high_resolution_clock::time_point vEnd = std::chrono::high_resolution_clock::now();

	// Повторяем тот же замер для deque, чтобы сравнение было честным.
	std::chrono::high_resolution_clock::time_point dStart = std::chrono::high_resolution_clock::now();
	// Сортируем deque тем же алгоритмом.
	std::sort(m_deque.begin(), m_deque.end());
	// Сохраняем время окончания сортировки deque.
	std::chrono::high_resolution_clock::time_point dEnd = std::chrono::high_resolution_clock::now();

	// Выводим отсортированную последовательность.
	// Показываем только vector, потому что deque был заполнен теми же значениями.
	std::cout << "After:  ";
	// Снова печатаем все элементы по порядку, но уже после сортировки.
	for (std::vector<int>::const_iterator it = m_vector.begin(); it != m_vector.end(); ++it)
		std::cout << *it << ' ';
	// Перевод строки нужен для аккуратного вывода времени ниже.
	std::cout << std::endl;

	// Печатаем время обработки vector.
	// duration_cast переводит разницу времени в микросекунды.
	std::cout << "Time to process a range of " << m_vector.size()
		<< " elements with std::vector : "
		<< std::chrono::duration_cast<std::chrono::microseconds>(vEnd - vStart).count()
		<< " us" << std::endl;
	// Печатаем аналогичное время для deque, чтобы можно было сравнить структуры.
	std::cout << "Time to process a range of " << m_deque.size()
		<< " elements with std::deque  : "
		<< std::chrono::duration_cast<std::chrono::microseconds>(dEnd - dStart).count()
		<< " us" << std::endl;

}


