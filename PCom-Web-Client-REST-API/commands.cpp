#include "commands.hpp"

#define REGISTER_PATH "/api/v1/tema/auth/register"
#define LOGIN_PATH "/api/v1/tema/auth/login"
#define ENTER_LIBRARY_PATH "/api/v1/tema/library/access"
#define GET_BOOKS_PATH "/api/v1/tema/library/books"
#define BOOK_PATH "/api/v1/tema/library/books/"
#define ADD_BOOK_PATH "/api/v1/tema/library/books"
#define LOGOUT_PATH "/api/v1/tema/auth/logout"

void invalid_command(void) {
    cout << "ERROR : INVALID COMMAND!Please select one of the following :\n";
    cout << "\t*register\n\t*login\n\t*enter_library\n";
    cout << "\t*get_books\n\t*get_book\n\t*add_book\n";
    cout << "\t*delete_book\n\t*logout\n\t*exit\n";
}

char *get_login_register_response(const char *path, string &cookies, string &token) {
    string username, password;

    /* Se cere un username de la input cat timp username-ul curent e invalid */
    while(1) {
        cout << "username=";
        getline(cin, username);

        if (username.find(' ') == std::string::npos)
            break;
        cout << "ERROR : Invalid username. Try again with another username.\n";
    }       

    /* Se cere o parola de la input cat timp username-ul curent e invalid */
    while (1) {
        cout << "password=";
        getline(cin, password);

        if (password.find(' ') == std::string::npos)
            break;

        cout << "ERROR : Invalid password. Try again with another password.\n";
    }        

    /* Se construieste mesajul sub format json */
    nlohmann::json json_data = {
        {"username", username},
        {"password", password}
    };

    char *response = send_server_request(path, POST, cookies, token, json_data.dump().c_str());

    return response;
}

void register_req(string &cookies, string &token) {
    /* Se verifica ca sa nu fie deja un utilizator logat */
    if (!cookies.empty()) {
        cout << "ERROR : Already logged in with another account." <<
        "Please log out before registering a new account" << endl;
        return;
    }

    /* Se trimite cererea catre server si se primeste raspunsul */
    char *response = get_login_register_response(REGISTER_PATH, cookies, token);

    /* Se parseaza payload-ul raspunsului venit de la server */
    char *json_response = basic_extract_json_response(response);
    if (json_response) {
        json r_err = json::parse(json_response);
        cout << "ERROR : " << r_err.value("error", "") << endl;
    } else {
        cout << "User successfully registered." << endl;
    }

    free(response);
}

void login_req(string &cookies, string &token) {
    /* Se verifica sa nu existe deja un utilizator logat */
    if (!cookies.empty()) {
        cout << "ERROR : Already logged in." << 
        "Please log out before logging in with another account." << endl;
        return;
    }

    /* Se trimite cererea catre server si se primeste raspunsul */
    char *response = get_login_register_response(LOGIN_PATH, cookies, token);

    /* Se parseaza payload-ul raspunsului venit de la server */
    char *json_response = basic_extract_json_response(response);
    if (json_response) {
        /* Eroare */
        json r_err = json::parse(json_response);
        cout << "ERROR : " << r_err.value("error", "") << endl;
    } else {
        cout << "Login successful." << endl;

        /* Parsare cookies sesiune curenta */
        char* cookies_hdr = strstr(response, "Set-Cookie:");
        string str = string(cookies_hdr);
        cookies = parse_cookies(str);
    }
    free(response);
}

void enter_library_req(string &cookies, string &token) {
    /* Se deschide conexiunea cu serverul, se trimite mesajul si se primeste raspunsul */
    char *response = send_server_request(ENTER_LIBRARY_PATH, GET, cookies, token, NULL);

    /* Se parseaza raspunsul venit de la server */
    char *json_response = basic_extract_json_response(response);
    json r_enter_lib = json::parse(json_response);

    if (r_enter_lib.contains("error")) {
        /* Eroare */
        cout << "ERROR : " << r_enter_lib.value("error", "") << endl;
    } else {
        /* Se primeste tokenul de acces si se salveaza */
        r_enter_lib.at("token").get_to(token);
        cout << "User successfully accesed the library." << endl;
    }

    free(response);
}

void get_books_req(string &cookies, string &token) {
    /* Se deschide conexiunea cu serverul, se trimite mesajul si se primeste raspunsul */
    char *response = send_server_request(GET_BOOKS_PATH, GET, cookies, token, NULL);

    /* Se parseaza raspunsul venit de la server */
    if (strstr(response, "[") != NULL) {
        /* Se primeste o lista cu cartile */
        char *json_resp = strstr(response, "[");
        json r_g = json::parse(json_resp);
        cout << r_g.dump(3) << endl;
    } else {
        /* Eroare */
        char *json_response = basic_extract_json_response(response);
        json r_get = json::parse(json_response);
        cout << "ERROR :" << r_get.value("error", "") << endl;
    }

    free(response);
}

void get_book_req(string &cookies, string &token) {
    /* Se citeste id-ul cartii */
    string id;
    cout << "id=";
    getline(cin, id);

    string path = BOOK_PATH + id;

    /* Se deschide conexiunea cu serverul, se trimite mesajul si se primeste raspunsul */
    char *response = send_server_request(path.c_str(), GET, cookies, token, NULL);

    /* Se parseaza raspunsul venit de la server */
    char *json_response = basic_extract_json_response(response);
    json r_get_book = json::parse(json_response);

    if (r_get_book.contains("error")) {
        /* Eroare */
        cout << "ERROR : " << r_get_book.value("error", "") << endl;
    } else {
        /* Se primesc informatii valide */
        cout << r_get_book.dump(3) << endl;
    }

    free(response);
}

void add_book_req(string &cookies, string &token) {
    /* Se citesc de la tastaura informatiile referitoare la carte */
    string title, author, genre, publisher, page_count;

    cout << "title=";
    getline(cin, title);

    cout << "author=";
    getline(cin, author);

    cout << "genre=";
    getline(cin, genre);

    cout << "publisher=";
    getline(cin, publisher);

    /* Se citeste numarul de pagini pana se obtine o valoare valida */
    while (1) {
        cout << "page_count=";
        getline(cin, page_count);

        if (is_integer(page_count)) {
            break;
        } else cout << "ERROR : Invalid number. Try again." << endl;
    }

    /* Se construieste payload-ul sub format json */
    json json_data = {
        {"title", title},
        {"author", author},
        {"genre", genre},
        {"page_count", atoi(page_count.c_str())},
        {"publisher", publisher}
    };

    /* Se deschide conexiunea cu serverul, se trimite mesajul si se primeste raspunsul */
    char *response = send_server_request(ADD_BOOK_PATH, POST, cookies, token, json_data.dump().c_str());

    /* Se parseaza raspunsul venit de la server */
    char *json_response = basic_extract_json_response(response);
    if (json_response) {
        json r_err = json::parse(json_response);
        cout << "ERROR : " << r_err.value("error", "") << endl;
    } else {
        cout << "New book added successfully." << endl;
    }

    free(response);
}

void delete_book_req(string &cookies, string &token) {
    /* Se citeste id-ul cartii ce urmeaza a fi stearsa */
    string id;
    cout << "id=";
    getline(cin, id);

    /* Inline function de adaugat */
    string path = BOOK_PATH + id;

    /* Se deschide conexiunea cu serverul, se trimite mesajul si se primeste raspunsul */
    char *response = send_server_request(path.c_str(), DELETE, cookies, token, NULL);

    /* Se parseaza raspunsul venit de la server */
    char *json_response = basic_extract_json_response(response);
    if (json_response) {
        /* Eroare */
        json r_err = json::parse(json_response);
        cout << "ERROR : " << r_err.value("error", "") << endl;
    } else {
        /* Confirmare ca stergerea cartii s-a produc cu succes */
        cout << "Book with id " << id << " deleted successfully." << endl;
    }

    free(response);
}

void logout_req(string &cookies, string &token) {
    /* Se deschide conexiunea cu serverul, se trimite mesajul si se primeste raspunsul */
    char *response = send_server_request(LOGIN_PATH, GET, cookies, token, NULL);

    /* Se parseaza raspunsul venit de la server */
    char *json_response = basic_extract_json_response(response);
    if (json_response) {
        /* Eroare */
        json r_err = json::parse(json_response);
        cout << "ERROR : " << r_err.value("error", "") << endl;
    } else {
        /* Succes */
        cout << "User logged out successfully" << endl;
        /* Se sterg cookies si token-ul pentru sesiunea curenta */
        cookies.clear();
        token.clear();
    }

    free(response);
}
