/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 13:46:22 by aokhapki          #+#    #+#             */
/*   Updated: 2026/03/03 13:55:24 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map>
#include <string>

class BitcoinExchange
{
	private:
		std::map<std::string, double> m_rates;
	public:
		BitcoinExchange();
		BitcoinExchange(const BitcoinExchange& src);
		BitcoinExchange& operator=(const BitcoinExchange& rhs);
		~BitcoinExchange();

		// void loadDB(const std::string& dbPath);
		
}

#endif