/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 22:33:44 by aokhapki          #+#    #+#             */
/*   Updated: 2026/04/07 22:57:22 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"
#include <cctype>
#include <sstream>
#include <stdexcept>

RPN::RPN() {}

RPN::RPN(const RPN& src) : m_stack(src.m_stack) {}

RPN& RPN::operator=(const RPN& rhs)
{
	if(this != &rhs)
	{
		m_stack = rhs.m_stack;
	}
	return *this;
}

RPN::~RPN() {}

bool RPN::isOperator(const std::string& token)
{
	return token.size() == 1 && (token[0] =='+' || token[0] == '-' || token[0] == '*' || token[0] == '/');
}

//TODO: Handle empty strings, negative numbers and multi-digit numbers in isDigit
bool RPN::isDigit(const std::string& token)
{
	if (token.empty())
		return false;

	// Поддерживаем обычные числа вроде "7", "42" и знаковые числа вроде "-3".
	// Это делает парсер полезнее и позволяет корректно обрабатывать более широкий
	// набор входных выражений, не путая знак числа с бинарным оператором.
	size_t index = 0;
	if (token[0] == '+' || token[0] == '-')
	{
		if (token.size() == 1)
			return false;
		index = 1;
	}

	for (; index < token.size(); ++index)
	{
		if (!std::isdigit(static_cast<unsigned char>(token[index])))
			return false;
	}
	return true;
}

void RPN::evaluateOperator(const std::string& op)
{
	// Для любого бинарного оператора в RPN нужны как минимум два числа в стеке.
	// Если элементов меньше, выражение некорректно и продолжать вычисление нельзя.
	if (m_stack.size() < 2)
		throw std::runtime_error("Error");
	// Снимаем правый операнд первым, потому что в стеке он лежит сверху.
	// Пример: "3 2 -" -> верхушка стека это 2 (rhs), ниже 3 (lhs).
	long long rhs = m_stack.top();
	m_stack.pop();

	// Затем снимаем левый операнд.
	long long lhs = m_stack.top();
	m_stack.pop();

	// Переменная для результата выбранной операции.
	long long result = 0;

	// Выбираем действие по оператору.
	if (op == "+")
		result = lhs + rhs;
	else if (op == "-")
		result = lhs - rhs;
	else if (op == "*")
		result = lhs * rhs;
	else if (op == "/")
	{
		// Деление на ноль недопустимо: сразу бросаем исключение.
		if (rhs == 0)
			throw std::runtime_error("Error");

		// Целочисленное деление согласно типу long long.
		result = lhs / rhs;
	}
	else
		// Защита от неизвестных операторов.
		throw std::runtime_error("Error");

	// Возвращаем промежуточный результат в стек,
	// чтобы его можно было использовать в следующих шагах выражения.
	m_stack.push(result);
}

//TODO: Handle division by zero and other edge cases in evaluateOperator
void RPN::calculate(const std::string& expression)
{
	// Сбрасываем стек перед новым вычислением, чтобы результат предыдущего вызова
	// не влиял на текущее выражение.
	while (!m_stack.empty())
		m_stack.pop();

	// Разбираем строку по пробелам: в RPN каждый токен уже отделён от других.
	std::istringstream input(expression);
	std::string token;

	// input >> token читает следующий токен из строки; цикл идёт, пока чтение успешно.
	while (input >> token)
	{
		if (isDigit(token))
		{
			// Число просто кладём в стек как операнд для будущих операций.
			// std::stoll(token) берёт строку с цифрами, превращает её в long long,
			// а push() помещает это число в стек, чтобы потом его можно было
			// использовать как один из операндов для '+' , '-' , '*' или '/'.
			m_stack.push(std::stoll(token));
		}
		else if (isOperator(token))
		{
			// Оператор снимает два верхних операнда, вычисляет результат и
			// возвращает его обратно в стек.
			evaluateOperator(token);
		}
		else
		{
			// Любой другой токен считается ошибкой ввода.
			throw std::runtime_error("Error");
		}
	}

	// После корректного выражения в стеке должен остаться ровно один результат.
	// Если элементов больше или меньше, значит выражение было неполным либо
	// содержало лишние операнды/операторы.
	if (m_stack.size() != 1)
		throw std::runtime_error("Error");
}
