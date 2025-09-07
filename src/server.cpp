#include "server.hpp"


    //  curl http://localhost:8080/users

   void RestServer::handle_get(http_request request) {
        auto path = request.relative_uri().path();
        SimpleUrlParser parser;
        std::string url = "https://api.example.com:8081/users/search?name=John+Doe&age=25&active=true#results";
            
            if (parser.parse(url)) {
                std::cout << "Original URL: " << url << std::endl;
                std::cout << "Scheme: " << parser.scheme << std::endl;
                std::cout << "Host: " << parser.host << std::endl;
                std::cout << "Port: " << parser.port << std::endl;
                std::cout << "Path: " << parser.path << std::endl;
                std::cout << "Query: " << parser.query << std::endl;
                std::cout << "Fragment: " << parser.fragment << std::endl;
                
                std::cout << "Query Parameters:" << std::endl;
                for (const auto& param : parser.query_params) {
                    std::cout << "  " << param.first << " = " << param.second << std::endl;
                }
            }
        
        if (path == "/users") {
            // Alle Benutzer zurückgeben
            json::value response = json::value::object();
            json::value user_array = json::value::array();

          
            
            int i = 0;
            for (const auto& user : users) {
                json::value user_obj = json::value::object();
                user_obj["id"] = json::value::number(user.first);
                user_obj["name"] = json::value::string(user.second);
                user_array[i++] = user_obj;
            }
            
            response["users"] = user_array;
            request.reply(status_codes::OK, response);
        }
        else if (path.find("/users/") == 0) {
            // Einzelnen Benutzer nach ID zurückgeben
            std::string id_str = path.substr(7); // "/users/" entfernen
            try {
                int id = std::stoi(id_str);
                auto it = users.find(id);
                
                if (it != users.end()) {
                    json::value response = json::value::object();
                    response["id"] = json::value::number(it->first);
                    response["name"] = json::value::string(it->second);
                    request.reply(status_codes::OK, response);
                } else {
                    json::value error = json::value::object();
                    error["error"] = json::value::string("User not found");
                    request.reply(status_codes::NotFound, error);
                }
            } catch (const std::exception& e) {
                json::value error = json::value::object();
                error["error"] = json::value::string("Invalid user ID");
                request.reply(status_codes::BadRequest, error);
            }
        }
        else {
            request.reply(status_codes::NotFound, "Endpoint not found");
        }
    }

    /**
     * @brief Handles HTTP POST requests for the server.
     *
     * This method processes incoming POST requests, specifically for the "/users" endpoint.
     * It expects a JSON body containing a "name" field to create a new user.
     * If the "name" field is present, a new user is added to the internal users map,
     * and a JSON response with the user's ID, name, and a success message is returned
     * with HTTP status code 201 (Created).
     * If the "name" field is missing, it responds with HTTP status code 400 (Bad Request)
     * and an error message.
     * If the JSON is invalid, it also responds with HTTP status code 400 and an error message.
     * For any other endpoint, it responds with HTTP status code 404 (Not Found).
     *
     * @param request The incoming HTTP request to be handled.
     */
    void RestServer::handle_post(http_request request) {
        auto path = request.relative_uri().path();
        
        if (path == "/users") {
            request.extract_json().then([=](pplx::task<json::value> task) {
                try {
                    auto body = task.get();
                    
                    if (body.has_field("name")) {
                        std::string name = body["name"].as_string();
                        users[next_id] = name;
                        
                        json::value response = json::value::object();
                        response["id"] = json::value::number(next_id);
                        response["name"] = json::value::string(name);
                        response["message"] = json::value::string("User created successfully");
                        
                        next_id++;
                        request.reply(status_codes::Created, response);
                    } else {
                        json::value error = json::value::object();
                        error["error"] = json::value::string("Name field required");
                        request.reply(status_codes::BadRequest, error);
                    }
                } catch (const std::exception& e) {
                    json::value error = json::value::object();
                    error["error"] = json::value::string("Invalid JSON");
                    request.reply(status_codes::BadRequest, error);
                }
            });
        } else {
            request.reply(status_codes::NotFound, "Endpoint not found");
        }
    }

    /**
     * @brief Handles HTTP PUT requests for updating user information.
     *
     * This method processes PUT requests to the "/users/{id}" endpoint.
     * It extracts the user ID from the request path, validates it, and checks if the user exists.
     * If the user exists, it expects a JSON body containing a "name" field to update the user's name.
     * Responds with appropriate status codes and messages:
     * - 200 OK with updated user info if successful.
     * - 400 Bad Request if the user ID is invalid, JSON is malformed, or "name" field is missing.
     * - 404 Not Found if the user does not exist or the endpoint is incorrect.
     *
     * @param request The incoming HTTP request object.
     */
    void RestServer::handle_put(http_request request) {
        auto path = request.relative_uri().path();
        
        if (path.find("/users/") == 0) {
            std::string id_str = path.substr(7);
            try {
                int id = std::stoi(id_str);
                auto it = users.find(id);
                
                if (it != users.end()) {
                    request.extract_json().then([=](pplx::task<json::value> task) {
                        try {
                            auto body = task.get();
                            
                            if (body.has_field("name")) {
                                std::string name = body["name"].as_string();
                                users[id] = name;
                                
                                json::value response = json::value::object();
                                response["id"] = json::value::number(id);
                                response["name"] = json::value::string(name);
                                response["message"] = json::value::string("User updated successfully");
                                
                                request.reply(status_codes::OK, response);
                            } else {
                                json::value error = json::value::object();
                                error["error"] = json::value::string("Name field required");
                                request.reply(status_codes::BadRequest, error);
                            }
                        } catch (const std::exception& e) {
                            json::value error = json::value::object();
                            error["error"] = json::value::string("Invalid JSON");
                            request.reply(status_codes::BadRequest, error);
                        }
                    });
                } else {
                    json::value error = json::value::object();
                    error["error"] = json::value::string("User not found");
                    request.reply(status_codes::NotFound, error);
                }
            } catch (const std::exception& e) {
                json::value error = json::value::object();
                error["error"] = json::value::string("Invalid user ID");
                request.reply(status_codes::BadRequest, error);
            }
        } else {
            request.reply(status_codes::NotFound, "Endpoint not found");
        }
    }

    /**
     * @brief Handles HTTP DELETE requests for user resources.
     *
     * This method processes DELETE requests targeting the "/users/{id}" endpoint.
     * It extracts the user ID from the request URI, attempts to find and remove the user
     * from the internal users map, and sends an appropriate HTTP response:
     * - If the user is found and deleted, replies with status 200 (OK) and a success message.
     * - If the user is not found, replies with status 404 (Not Found) and an error message.
     * - If the user ID is invalid, replies with status 400 (Bad Request) and an error message.
     * - For other endpoints, replies with status 404 (Not Found).
     *
     * @param request The incoming HTTP request to be handled.
     */
    void RestServer::handle_delete(http_request request) {
        auto path = request.relative_uri().path();
        
        if (path.find("/users/") == 0) {
            std::string id_str = path.substr(7);
            try {
                int id = std::stoi(id_str);
                auto it = users.find(id);
                
                if (it != users.end()) {
                    users.erase(it);
                    
                    json::value response = json::value::object();
                    response["message"] = json::value::string("User deleted successfully");
                    
                    request.reply(status_codes::OK, response);
                } else {
                    json::value error = json::value::object();
                    error["error"] = json::value::string("User not found");
                    request.reply(status_codes::NotFound, error);
                }
            } catch (const std::exception& e) {
                json::value error = json::value::object();
                error["error"] = json::value::string("Invalid user ID");
                request.reply(status_codes::BadRequest, error);
            }
        } else {
            request.reply(status_codes::NotFound, "Endpoint not found");
        }
    }

