#pragma once
#include <string_view>
#include <string>
#include "DynamicArray.h"
#include "common/PatternMatcher.h"

class HTMLParser{
    private:
    PatternMatcher matcher;
    std::string extractattributes(std::string_view html,size_t pos,std::string_view attribute)const;
    public:
    DynamicArray<std::string> extractlinks(std::string_view html)const;
};