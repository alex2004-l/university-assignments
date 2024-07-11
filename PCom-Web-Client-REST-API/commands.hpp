#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "request.hpp"
#include "utils.hpp"
#include "helpers.hpp"
#include "nlohmann/json.hpp"

#include <iostream>

using namespace std;
using json = nlohmann::json;

/* Prompt afisat dupa o comanda invalida */
void invalid_command(void);

/* Functie comuna pentru construirea si transmiterea cererilor POST
 *catre server in cazul comenzilor login/register */
char *get_login_register_response(const char *path, string &cookies, string &token);

/* Se trimite care server o cerere de inregistrare */
void register_req(string &cookies, string &token);

/* Se trimite catre server o cerere de logare */
void login_req(string &cookies, string &token);

/* Se trimite catre server o cerere de accesare a bibliotecii */
void enter_library_req(string &cookies, string &token);

/* Se trimite o cerere catre server pentru a se accesa lista cu carti */
void get_books_req(string &cookies, string &token);

/* Se trimite o cerere catre server pentru a se accesa o carte specifica */
void get_book_req(string &cookies, string &token);

/* Se trimite o cerere catre server pentru a se adauga o carte noua */
void add_book_req(string &cookies, string &token);

/* Se trimite o cerere catre server pentru a sterge o carte */
void delete_book_req(string &cookies, string &token);

/* Se trimite o cerere catre server pentru a deloga utilizatorul curent */
void logout_req(string &cookies, string &token);

#endif