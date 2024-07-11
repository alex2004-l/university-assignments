#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <algorithm>
using namespace std;

/* Returneaza cookie-urile de sesiune din header-ul primit de la server */
string parse_cookies(string &response);

/* Functie care verifica daca un string este un numar natural */
bool is_integer(const string& str);

#endif