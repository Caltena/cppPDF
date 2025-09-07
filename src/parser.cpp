#include "parser.hpp"


    bool SimpleUrlParser::parse(const std::string& url) {
        // Regex für URL-Parsing
        std::regex url_regex(
            R"(^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)"
        );
        
        std::smatch matches;
        if (!std::regex_match(url, matches, url_regex)) {
            return false;
        }
        
        scheme = matches[2].str();
        std::string authority = matches[4].str();
        path = matches[5].str();
        query = matches[7].str();
        fragment = matches[9].str();
        
        // Host und Port aus Authority extrahieren
        parse_authority(authority);
        
        // Query-Parameter parsen
        parse_query_params();
        
        return true;
    }

    void SimpleUrlParser::parse_authority(const std::string& authority) {
        if (authority.empty()) return;
        
        size_t colon_pos = authority.find(':');
        if (colon_pos != std::string::npos) {
            host = authority.substr(0, colon_pos);
            try {
                port = std::stoi(authority.substr(colon_pos + 1));
            } catch (...) {
                port = -1;
            }
        } else {
            host = authority;
        }
    }
    
    void SimpleUrlParser::parse_query_params() {
        if (query.empty()) return;
        
        std::stringstream ss(query);
        std::string param;
        
        while (std::getline(ss, param, '&')) {
            size_t eq_pos = param.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = param.substr(0, eq_pos);
                std::string value = param.substr(eq_pos + 1);
                
                // URL-Dekodierung (vereinfacht)
                key = url_decode(key);
                value = url_decode(value);
                
                query_params[key] = value;
            }
        }
    }
    
    std::string SimpleUrlParser::url_decode(const std::string& str) {
        std::string result;
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '%' && i + 2 < str.length()) {
                int hex_value;
                std::stringstream ss;
                ss << std::hex << str.substr(i + 1, 2);
                ss >> hex_value;
                result += static_cast<char>(hex_value);
                i += 2;
            } else if (str[i] == '+') {
                result += ' ';
            } else {
                result += str[i];
            }
        }
        return result;
    }
