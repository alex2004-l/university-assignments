#include "utils.hpp"

string parse_cookies(string &response) {
    const string field_name = "Set-Cookie: ";
    string cookie;

    if (response.find(field_name) == 0) {
        response.erase(0, field_name.size());
    }

    size_t pos = 0;
    if ((pos = response.find("; ")) != std::string::npos) {
        cookie = response.substr(0, pos);
    }

    return cookie;
}

bool is_integer(const string& str) {
    if (str.empty()) {
        return false;
    }
    return all_of(str.begin(), str.end(), ::isdigit);
}