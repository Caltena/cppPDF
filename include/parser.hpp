#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <regex>

class SimpleUrlParser
{
public:
    std::string scheme;
    std::string host;
    int port = -1;
    std::string path;
    std::string query;
    std::string fragment;
    std::map<std::string, std::string> query_params;

    bool parse(const std::string &url);

private:
    void parse_query_params();
    void parse_authority(const std::string &authority);
    std::string url_decode(const std::string &str);
};
#endif