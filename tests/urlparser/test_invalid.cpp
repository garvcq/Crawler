#include <gtest/gtest.h>
#include "parser/URLParser.h"
#include "parser/ParsedURL.h"

TEST(URLParserTest, InvalidScheme)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse("1http://google.com").valid);
    EXPECT_FALSE(parser.parse("+http://google.com").valid);
    EXPECT_FALSE(parser.parse("-http://google.com").valid);
    EXPECT_FALSE(parser.parse(".http://google.com").valid);
    EXPECT_FALSE(parser.parse("ftp://google.com").valid);
}
TEST(URLParserTest, MissingSlashes)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse("https:/google.com").valid);
    EXPECT_FALSE(parser.parse("https:google.com").valid);
    EXPECT_FALSE(parser.parse("https//google.com").valid);
}
TEST(URLParserTest, MissingHost)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse("https://").valid);
    EXPECT_FALSE(parser.parse("http://").valid);
}
TEST(URLParserTest, InvalidPortRange)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse("https://google.com:0").valid);
    EXPECT_FALSE(parser.parse("https://google.com:65536").valid);
    EXPECT_FALSE(parser.parse("https://google.com:999999").valid);
}
TEST(URLParserTest, InvalidPortCharacters)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse("https://google.com:80abc").valid);
    EXPECT_FALSE(parser.parse("https://google.com:abc").valid);
    EXPECT_FALSE(parser.parse("https://google.com:12-3").valid);
}   
TEST(URLParserTest, EmptyCredentials)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse("https://@google.com").valid);
}
TEST(URLParserTest, MultipleCredentials)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse("https://user@admin@google.com").valid);
}
TEST(URLParserTest, InvalidIPv6)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse("https://[2001:db8::1").valid);
}
TEST(URLParserTest, EmptyURL)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse("").valid);
}
TEST(URLParserTest, Garbage)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse("hello").valid);
    EXPECT_FALSE(parser.parse("////").valid);
    EXPECT_FALSE(parser.parse("::::::::").valid);
}
TEST(URLParserTest, Spaces)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse(" ").valid);
    EXPECT_FALSE(parser.parse("https:// google.com").valid);
}

TEST(URLParserTest, InvalidWhitespaceInHost)
{
    URLParser parser;

    EXPECT_FALSE(parser.parse("https:// google.com").valid);
    EXPECT_FALSE(parser.parse("https://goo gle.com").valid);
    EXPECT_FALSE(parser.parse("https://google.com ").valid);
    EXPECT_FALSE(parser.parse("https://\tgoogle.com").valid);
}