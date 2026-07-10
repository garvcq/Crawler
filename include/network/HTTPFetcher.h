#pragma once
#include <string>
#include "network/Page.h"
class HTTPFetcher
{
    public:
        Page fetch(const std::string& url);
};