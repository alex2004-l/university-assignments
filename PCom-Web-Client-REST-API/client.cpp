#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unordered_map>

#include "helpers.hpp"
#include "request.hpp"
#include "nlohmann/json.hpp"
#include "utils.hpp"
#include "commands.hpp"

using namespace std;

enum COMMANDS {
    REGISTER,
    LOGIN,
    ENTER_LIBRARY,
    GET_BOOKS,
    GET_BOOK,
    ADD_BOOK,
    DELETE_BOOK,
    LOGOUT,
    EXIT };


/* Initializare map folosit pentru match-ul comenzilor */
void initialize_map(unordered_map<string, COMMANDS> &map) {
    map.insert({"register", REGISTER});
    map.insert({"login", LOGIN});
    map.insert({"enter_library", ENTER_LIBRARY});
    map.insert({"get_books", GET_BOOKS});
    map.insert({"get_book", GET_BOOK});
    map.insert({"add_book", ADD_BOOK});
    map.insert({"delete_book", DELETE_BOOK});
    map.insert({"logout", LOGOUT});
    map.insert({"exit", EXIT});
}

int main(int argc, char **argv) {

    unordered_map<string, COMMANDS> commands;
    string cookies;
    string token;
    initialize_map(commands);

    bool exit_flag = false;
    string command;
    
    /* Se citeste comanda de la stdin si se verifica daca e valida*/
    while(!exit_flag) {
        getline(cin, command);
        try{
            switch(commands.at(command)) {
                case REGISTER:
                    register_req(cookies, token);
                    break;
                case LOGIN:
                    login_req(cookies, token);
                    break;
                case ENTER_LIBRARY:
                    enter_library_req(cookies, token);
                    break;
                case GET_BOOKS:
                    get_books_req(cookies, token);
                    break;
                case GET_BOOK:
                    get_book_req(cookies, token);
                    break;
                case ADD_BOOK:
                    add_book_req(cookies, token);
                    break;
                case DELETE_BOOK:
                    delete_book_req(cookies, token);
                    break;
                case LOGOUT:
                    logout_req(cookies, token);
                    break;
                case EXIT:
                    exit_flag = true;
                    break;
        
            }
        } catch (const out_of_range& e) {
            invalid_command();
        }
    }
    return 0;
}
