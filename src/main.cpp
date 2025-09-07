#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>
#include <vector>
#include <map>
#include <stdexcept> 

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

#include "server.hpp"
#include "podofotest.hpp"


int main() {


    ifstream file("data.json");
    if (!file.is_open()) {
        cerr << "Failed to open file." << endl;
        return 1;
    }

    // Read the file into a string
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    // Parse the string into a JSON value
    json::value jsonData = json::value::parse(buffer.str());

    // Access JSON fields (example)
    if (jsonData.has_field(U("version"))) {
        cout << "version: " << jsonData[U("version")].as_string() << endl;
        /*
        You can access nested fields using chained indexing: jsonData[U("user")][U("email")].as_string();
        */
    }

     fopodo fo ;
     fo.test();

    try {
        RestServer server("http://localhost:8081");
        
        std::cout << "Starting REST server on http://localhost:8080" << std::endl;
        std::cout << "Available endpoints:" << std::endl;
        std::cout << "  GET    /users      - Get all users" << std::endl;
        std::cout << "  GET    /users/{id} - Get user by ID" << std::endl;
        std::cout << "  POST   /users      - Create new user" << std::endl;
        std::cout << "  PUT    /users/{id} - Update user" << std::endl;
        std::cout << "  DELETE /users/{id} - Delete user" << std::endl;
        std::cout << std::endl;
        
        server.start().wait();
        
        std::cout << "Server started successfully!" << std::endl;
        std::cout << "Press Enter to stop the server..." << std::endl;
        std::cin.get();
        
        server.stop().wait();
        std::cout << "Server stopped." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}