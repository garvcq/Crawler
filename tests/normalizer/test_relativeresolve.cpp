#include <gtest/gtest.h>
#include "parser/URLParser.h"
#include "parser/URLNormalizer.h"
#include "parser/NormalizedURL.h"

TEST(URLResolveRelativeTest, RelativeFile)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL base =
        parser.parse("https://example.com/docs/tutorial/page.html");

    NormalizedURL url =
        normalizer.resolverelative("about.html", base);

    EXPECT_EQ(normalizer.tostring(url),
              "https://example.com/docs/tutorial/about.html");
}

TEST(URLResolveRelativeTest, CurrentDirectory)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL base =
        parser.parse("https://example.com/docs/tutorial/page.html");

    NormalizedURL url =
        normalizer.resolverelative("./contact.html", base);

    EXPECT_EQ(normalizer.tostring(url),
              "https://example.com/docs/tutorial/contact.html");
}

TEST(URLResolveRelativeTest, ParentDirectory)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL base =
        parser.parse("https://example.com/docs/tutorial/page.html");

    NormalizedURL url =
        normalizer.resolverelative("../image.png", base);

    EXPECT_EQ(normalizer.tostring(url),
              "https://example.com/docs/image.png");
}

TEST(URLResolveRelativeTest, MultipleParentDirectories)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL base =
        parser.parse("https://example.com/docs/tutorial/page.html");

    NormalizedURL url =
        normalizer.resolverelative("../../index.html", base);

    EXPECT_EQ(normalizer.tostring(url),
              "https://example.com/index.html");
}

TEST(URLResolveRelativeTest, RootRelative)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL base =
        parser.parse("https://example.com/docs/tutorial/page.html");

    NormalizedURL url =
        normalizer.resolverelative("/about", base);

    EXPECT_EQ(normalizer.tostring(url),
              "https://example.com/about");
}

TEST(URLResolveRelativeTest, RootOnly)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL base =
        parser.parse("https://example.com/docs/tutorial/page.html");

    NormalizedURL url =
        normalizer.resolverelative("/", base);

    EXPECT_EQ(normalizer.tostring(url),
              "https://example.com/");
}

TEST(URLResolveRelativeTest, QueryOnly)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL base =
        parser.parse("https://example.com/docs/tutorial/page.html");

    NormalizedURL url =
        normalizer.resolverelative("?page=2", base);

    EXPECT_EQ(normalizer.tostring(url),
              "https://example.com/docs/tutorial/page.html?page=2");
}

TEST(URLResolveRelativeTest, FragmentOnly)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL base =
        parser.parse("https://example.com/docs/tutorial/page.html");

    NormalizedURL url =
        normalizer.resolverelative("#section", base);

    EXPECT_EQ(normalizer.tostring(url),
              "https://example.com/docs/tutorial/page.html");
}

TEST(URLResolveRelativeTest, ProtocolRelative)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL base =
        parser.parse("https://example.com/index.html");

    NormalizedURL url =
        normalizer.resolverelative("//cdn.example.com/app.js", base);

    EXPECT_EQ(normalizer.tostring(url),
              "https://cdn.example.com/app.js");
}