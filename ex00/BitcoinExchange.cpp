/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 13:55:29 by aokhapki          #+#    #+#             */
/*   Updated: 2026/03/09 16:28:46 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"
#include <fstream>

BitcoinExchange::BitcoinExchange() : m_rates(){}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& src) : m_rates(src.m_rates){}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& rhs)
{
	if(this != &rhs)
		m_rates = rhs.m_rates;
	return *this;
}

BitcoinExchange::~BitcoinExchange(){}

//Removes spaces and tabs from the beginning and end of a string
static std::string trim(const std::string& s)
{
	std::size_t start = 0;
	while (start <s.size() && (s[start] == ' ' || s[start] =='\t'))
		++start;
	std::size_t end = s.size();
	while(end > start && (s[end-1] == ' ' || s[end-1] == '\t'))
		--end;
	return s.substr(start, end - start);
}

static bool isDigit(char c)
{
	return(c >= '0' && c <= '9');
}

static bool isValidDate(int year, int month, int day)
{
	if(year < 2009 || month < 1 || month > 12 || day < 1)
		return false;
	static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int maxDay = daysInMonth[month -1];
	if(month == 2 && ((year %4 ==0 && year %100 !=0) || (year %400 ==0)))
		maxDay =29;
	return day <= maxDay;
}

static bool isValidDateFormat(const std::string& date)
{
    if (date.size() != 10)
        return false;

    if (date[4] != '-' || date[7] != '-')
        return false;

    for (int i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7)
            continue;

        if (!isDigit(date[i]))
            return false;
    }

    return true;
}

double BitcoinExchange::getRateForDateOrClosestLower(const std::string& date) const
{
	std::map<std::string, double>::const_iterator it = m_rates.lower_bound(date);
	if(it != m_rates.end() && it->first == date)
		return it->second;
	if(it == m_rates.begin())
		throw std::runtime_error("No rate available for date " + date);
	--it;
	return it->second;
}

void BitcoinExchange::loadDB(const std::string& dbPath)
{
	std::ifstream dbFile(dbPath.c_str());
	if(!dbFile.is_open())
		throw std::runtime_error("Could not open database file: " + dbPath);
	std::string line;
	std::getline(dbFile, line);
	while(std::getline(dbFile, line))
	{
		std::size_t commaPos = line.find(',');
		if(commaPos == std::string::npos)
			continue;
		std::string date = trim(line.substr(0, commaPos));
		std::string rateStr = trim(line.substr(commaPos + 1));
		if(!isValidDateFormat(date))
			continue;
		double rate = std::atof(rateStr.c_str());
		m_rates[date] = rate;
	}
}