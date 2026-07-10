#pragma once

#include <string_view>

class QuickFilter{
    public:
        static bool shouldparse(std::string_view url);
    private:
    static bool iswhitespace(char c);
};