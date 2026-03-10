/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aokhapki <aokhapki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 12:04:29 by aokhapki          #+#    #+#             */
/*   Updated: 2026/03/10 12:25:11 by aokhapki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

#include <exception>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Error: could not open file." << std::endl;
        return 1;
    }

    try
    {
        BitcoinExchange ex;
        ex.loadDB("data.csv");
        ex.processInput(argv[1]);
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cout << "Error: unknown exception." << std::endl;
        return 1;
    }

    return 0;
}