#include <fstream>
#include <sstream>
#include <gtest/gtest.h>
#include "parser/HTMLParser.h"
#include <filesystem>
#include <iostream>

TEST(HTMLParserTest, ParseRealHTMLPage)
{
    std::ifstream file("test/codequotient.com.html");

    ASSERT_TRUE(file.is_open());

    std::stringstream buffer;
    buffer << file.rdbuf();

    HTMLParser parser;

    DynamicArray<std::string> links =
        parser.extractlinks(buffer.str());

    for(int i=0;i<links.size();i++)
    {
        std::cout<<links[i]<<std::endl;
    }
    std::cout<<links.size();
    
}