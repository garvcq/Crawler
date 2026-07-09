#pragma once
#include <string>
//because stringview are read only
struct NormalizedURL
{
    std::string scheme;
    std::string credentials;
    std::string host;
    std::string port;
    std::string path;
    std::string query;
    bool hasport = false;
};