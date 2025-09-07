#ifndef SERVER_HPP
#define SERVER_HPP


#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>
#include <vector>
#include <map>
#include "parser.hpp"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

class RestServer
{
private:
    http_listener listener;
    std::map<int, std::string> users;
    int next_id;

public:
    RestServer(const std::string &url)
        : listener(http::uri_builder(url).to_uri()), next_id(1)
    {

        // Route-Handler registrieren
        listener.support(methods::GET, [this](http_request request)
                         { handle_get(request); });

        listener.support(methods::POST, [this](http_request request)
                         { handle_post(request); });

        listener.support(methods::PUT, [this](http_request request)
                         { handle_put(request); });

        listener.support(methods::DEL, [this](http_request request)
                         { handle_delete(request); });

        // Beispieldaten hinzufügen
        users[1] = "Max Mustermann";
        users[2] = "Anna Schmidt";
        next_id = 3;
    }

    pplx::task<void> start()
    {
        return listener.open();
    }

    pplx::task<void> stop()
    {
        return listener.close();
    }

private:
    void handle_get(http_request request);
    void handle_post(http_request request);
    void handle_put(http_request request);
    void handle_delete(http_request request);
};

#endif