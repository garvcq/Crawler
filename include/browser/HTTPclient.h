#pragma once

#include <string>

class HTTPClient
{
public:
    std::string get(const std::wstring& host,int port,const std::wstring& path);
};