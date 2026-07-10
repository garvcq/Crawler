#pragma once
#include <string>

struct Page
{
    std::string url;
    std::string html;
    int statusCode = 0;
    bool rendered = false;
};