/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 13:55:29 by aokhapki          #+#    #+#             */
/*   Updated: 2026/03/03 14:02:07 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange() : m_rates(){}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& src) : m_rates(src.m_rates){}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& rhs)
{
	if(this != &rhs)
		m_rates = rhs.m_rates;
	return *this;
}

BitcoinExchange::~BitcoinExchange(){}
