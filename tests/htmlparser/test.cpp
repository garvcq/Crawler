#include <gtest/gtest.h>
#include "parser/HTMLParser.h"

TEST(HTMLParserTest, SingleLink)
{
    HTMLParser parser;

    DynamicArray<std::string> links =
        parser.extractlinks(R"(<a href="about.html">)");

    ASSERT_EQ(links.size(), 1);

    EXPECT_EQ(links[0], "about.html");
}

TEST(HTMLParserTest, MultipleLinks)
{
    HTMLParser parser;

    DynamicArray<std::string> links =
        parser.extractlinks(R"(
            <a href="a.html">
            <a href="b.html">
            <a href="c.html">
        )");

    ASSERT_EQ(links.size(), 3);

    EXPECT_EQ(links[0], "a.html");
    EXPECT_EQ(links[1], "b.html");
    EXPECT_EQ(links[2], "c.html");
}

TEST(HTMLParserTest, SingleQuotes)
{
    HTMLParser parser;

    DynamicArray<std::string> links =
        parser.extractlinks(R"(<a href='page.html'>)");

    ASSERT_EQ(links.size(), 1);

    EXPECT_EQ(links[0], "page.html");
}

TEST(HTMLParserTest, SpacesAroundEqual)
{
    HTMLParser parser;

    DynamicArray<std::string> links =
        parser.extractlinks(R"(<a href = "/docs">)");

    ASSERT_EQ(links.size(), 1);

    EXPECT_EQ(links[0], "/docs");
}

TEST(HTMLParserTest, SpacesAfterEqual)
{
    HTMLParser parser;

    DynamicArray<std::string> links =
        parser.extractlinks(R"(<a href= "/docs">)");

    ASSERT_EQ(links.size(), 1);

    EXPECT_EQ(links[0], "/docs");
}

TEST(HTMLParserTest, RelativeLink)
{
    HTMLParser parser;

    DynamicArray<std::string> links =
        parser.extractlinks(R"(<a href="../images/logo.png">)");

    ASSERT_EQ(links.size(), 1);

    EXPECT_EQ(links[0], "../images/logo.png");
}

TEST(HTMLParserTest, IgnoreDataHref)
{
    HTMLParser parser;

    DynamicArray<std::string> links =
        parser.extractlinks(R"(<div data-href="/fake"></div>)");

    EXPECT_TRUE(links.isEmpty());
}

TEST(HTMLParserTest, MissingClosingQuote)
{
    HTMLParser parser;

    DynamicArray<std::string> links =
        parser.extractlinks(R"(<a href="/about>)");

    EXPECT_TRUE(links.isEmpty());
}

TEST(HTMLParserTest, EmptyHref)
{
    HTMLParser parser;

    DynamicArray<std::string> links =
        parser.extractlinks(R"(<a href="">)");

    EXPECT_TRUE(links.isEmpty());
}

TEST(HTMLParserTest, NoHrefAttribute)
{
    HTMLParser parser;

    DynamicArray<std::string> links =
        parser.extractlinks(R"(
            <html>
                <body>
                    <p>Hello World</p>
                    <img src="logo.png">
                </body>
            </html>
        )");

    EXPECT_TRUE(links.isEmpty());
}