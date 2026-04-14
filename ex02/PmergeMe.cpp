/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 23:33:07 by aokhapki          #+#    #+#             */
/*   Updated: 2026/04/14 14:23:37 by aokhapki         ###   ########.fr       */
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
// duration_cast переводит разницу времени в микросекунды.
// count() - берёт числовое значение из объекта std::chrono::duration.

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

/*
run: orchestration (подготовка и вызов) = центр “дирижер”, а sortVectorFordJohnson() и sortDequeFordJohnson() = “исполнители”.
(для инфо - Choreography Децентрализованное. Участники (танцоры)сами реагируют на события друг друга по правилам.)
sortVectorFordJohnson: вся логика Ford-Johnson для vector
sortDequeFordJohnson: та же логика для deque
Вектор: метод PmergeMe::sortVectorFordJohnson
Здесь логика по шагам такая:
Шаг A: формируешь пары
Берёшь числа по 2, внутри пары раскладываешь как (smaller, larger).
Пример: (7,3) превращается в (3,7).

Шаг B: std::sort для pairs
Вот эта часть:
std::sort(pairs.begin(), pairs.end(), ... a.second < b.second ...)
Она сортирует пары по larger (то есть по second).
После этого larger идут по возрастанию.

Шаг C: строишь mainChain
Ты берёшь только second из каждой пары и кладёшь в mainChain.
Получается уже отсортированная “основа”.

Шаг D: вставляешь smaller через lower_bound
Для каждого first:
lower_bound находит место вставки в mainChain
insert вставляет туда число
Поэтому mainChain всегда остаётся отсортированным после каждой вставки.

Шаг E: если есть unpaired
Точно так же:
lower_bound
insert
И в конце присваиваешь m_vector = mainChain.

*/

long long PmergeMe::sortVectorFordJohnson()
{
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	// Шаг 2.1 Ford-Johnson (только подготовка):
	// разбиваем входную последовательность на пары (smaller, larger).
	// ВАЖНО: на этом шаге мы лишь готовим структуру данных,
	// финальный алгоритм дальше добавим поэтапно.
	std::vector< std::pair<int, int> > pairs;
	pairs.reserve(m_vector.size() / 2);

	// Если элементов нечётное количество, последний элемент временно откладываем.
	// Позже (на отдельном шаге) вставим его в уже собранную main chain.
	bool hasUnpaired = (m_vector.size() % 2 != 0);
	int unpaired = 0;
	if (hasUnpaired)
		unpaired = m_vector.back();

	// Идём по 2 элемента: [i] и [i + 1].
	// Внутри пары храним сначала меньший, потом больший,
	// чтобы дальше оперировать структурой (smaller, larger).
	for (std::size_t i = 0; i + 1 < m_vector.size(); i += 2)
	{
		int left = m_vector[i];
		int right = m_vector[i + 1];
		if (left <= right)
			pairs.push_back(std::make_pair(left, right));
		else
			pairs.push_back(std::make_pair(right, left));
	}

	// Шаг 2.2: сортируем пары по larger элементу (second).
	// Comparator: первая пара меньше, если её larger < larger второй пары.
	std::sort(pairs.begin(), pairs.end(), 
		[](const std::pair<int, int>& a, const std::pair<int, int>& b) {
			return a.second < b.second;
		});

	// Шаг 2.3: построить main chain только из больших элементов (уже отсортированы).
	std::vector<int> mainChain;
	mainChain.reserve(pairs.size());
	for (std::size_t i = 0; i < pairs.size(); ++i) {
		mainChain.push_back(pairs[i].second);
	}

	// Шаг 2.4: вставить smaller элементы бинарным поиском в main chain.
	// lower_bound находит первую позицию, где элемент >= smaller.
	// Вставляем в эту позицию, чтобы maintain sorted order.
	for (std::size_t i = 0; i < pairs.size(); ++i) {
		int smaller = pairs[i].first;
		std::vector<int>::iterator pos = std::lower_bound(
			mainChain.begin(), mainChain.end(), smaller);
		mainChain.insert(pos, smaller);
	}

	// Шаг 2.5: если был непарный элемент, вставить его в mainChain.
	if (hasUnpaired) {
		std::vector<int>::iterator pos = std::lower_bound(
			mainChain.begin(), mainChain.end(), unpaired);
		mainChain.insert(pos, unpaired);
	}

	// Результат Ford-Johnson: отсортированная последовательность в mainChain.
	m_vector = mainChain;
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

long long PmergeMe::sortDequeFordJohnson()
{
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	// 1. инициировать переменную пара
	// 2. выделить памать для пар 
	// 3. проверить есть ли непарный элемент, если да, сохранить его
	// 4. цикл по deque по 2 элемента, формируя пары (smaller, larger) и сохраняя их в pairs
	// 5. отсортировать pairs по larger элементу (second) с помощью std::sort и компаратора
	// 6. создать mainChain с vector<int>, выделить память и заполнить его только larger элементами из пар (они уже отсортированы)
	// 7. цикл по pairs, для каждого smaller элемента:
	// 	- найти позицию вставки в mainChain с помощью std::lower_bound
	// 	- вставить smaller в эту позицию, сохраняя сортировку mainChain
	// 8. если был непарный элемент, вставить его в mainChain аналогично smaller
	// 9. присвоить mainChain обратно в m_deque
	// 10. измерить время выполнения и вернуть его в микросекундах
	
	std::deque< std::pair<int, int> > pairs;
	// no need to reserve memory for deque, because it will grow dynamically as we push_back pairs.
	bool hasUnpaired = (m_deque.size() % 2 != 0);
	int unpaired = 0;
	if(hasUnpaired)
		unpaired = m_deque.back(); // сохраняем последний элемент, если он непарный, для дальнейшей вставки в mainChain
	for(std::size_t i = 0; i + 1 < m_deque.size(); i += 2)
	{
		int left = m_deque[i];
		int right = m_deque[i + 1];
		if(left <= right)
			pairs.push_back(std::make_pair(left, right));
		else
			pairs.push_back(std::make_pair(right, left));
	}
	std::sort(pairs.begin(),pairs.end(),
		[](const std::pair<int, int>& a, const std::pair<int, int>& b)
		{
			return a.second < b.second; // сортируем по larger элементу (second) в каждой паре
		});
	std::vector<int> mainChain;
	mainChain.reserve(pairs.size()); // выделяем память для mainChain, который вначале будет содержать только larger элементы из пар, потом мы довставляем smaller элементы
	for(std::size_t i = 0; i < pairs.size(); ++i)
	{
		mainChain.push_back(pairs[i].second); // заполняем mainChain только larger элементами из пар, они уже отсортированы по возрастанию
	}
//lower_bound находит место вставки в отсортированном диапазоне.
// Ищет первую позицию, где элемент не меньше заданного значения (>= smaller).
// Возвращает итератор на эту позицию.
	for(std::size_t i = 0; i < pairs.size(); ++i)
	{
		int smaller = pairs[i].first; // smaller (у нас 1-й элемент из пары smaller)
		std::vector<int>::iterator pos = std::lower_bound(mainChain.begin(), mainChain.end(), smaller); // находим позицию для вставки smaller в mainChain, чтобы сохранить сортировку
		mainChain.insert(pos, smaller); // вставляем smaller в найденную позицию, mainChain остаётся отсортированным

	}
	if(hasUnpaired)
	{
		mainChain.insert(std::lower_bound(mainChain.begin(), mainChain.end(), unpaired), unpaired); // if there is an unpaired element, insert it into mainChain the same way as smaller
	}
	m_deque.assign(mainChain.begin(), mainChain.end()); // копируем отсортированный mainChain обратно в m_deque
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds> (end - start).count();
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
	long long vectorTimeUs = sortVectorFordJohnson();
	long long dequeTimeUs = sortDequeFordJohnson();
	// Выводим отсортированную последовательность, только vector, тк deque заполнен теми же значениями.
	printSequence(std::string("After: "), m_vector);
	// Печатаем время обработки vector. 
	std::cout << "Time to process a range of " << m_vector.size()
		<< " elements with std::vector : " << vectorTimeUs << " us" << std::endl;
	// Печатаем аналогичное время для deque, чтобы можно было сравнить структуры.
	std::cout << "Time to process a range of " << m_deque.size()
		<< " elements with std::deque  : " << dequeTimeUs << " us" << std::endl;
}




