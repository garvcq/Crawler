#include <gtest/gtest.h>
#include "parser/URLParser.h"
#include "parser/URLNormalizer.h"
#include "parser/NormalizedURL.h"

TEST(URLNormalizerTest, LowercaseScheme)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL parsed = parser.parse("HTTPS://google.com");

    NormalizedURL url = normalizer.normalize(parsed);

    EXPECT_EQ(normalizer.tostring(url),
              "https://google.com/");
}

TEST(URLNormalizerTest, LowercaseHost)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL parsed = parser.parse("https://Google.COM");

    NormalizedURL url = normalizer.normalize(parsed);

    EXPECT_EQ(normalizer.tostring(url),
              "https://google.com/");
}

TEST(URLNormalizerTest, RemoveHTTPDefaultPort)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL parsed = parser.parse("http://google.com:80");

    NormalizedURL url = normalizer.normalize(parsed);

    EXPECT_EQ(normalizer.tostring(url),
              "http://google.com/");
}

TEST(URLNormalizerTest, RemoveHTTPSDefaultPort)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL parsed = parser.parse("https://google.com:443");

    NormalizedURL url = normalizer.normalize(parsed);

    EXPECT_EQ(normalizer.tostring(url),
              "https://google.com/");
}

TEST(URLNormalizerTest, KeepNonDefaultPort)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL parsed = parser.parse("https://google.com:8443");

    NormalizedURL url = normalizer.normalize(parsed);

    EXPECT_EQ(normalizer.tostring(url),
              "https://google.com:8443/");
}

TEST(URLNormalizerTest, RemoveTrailingDot)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL parsed = parser.parse("https://google.com.");

    NormalizedURL url = normalizer.normalize(parsed);

    EXPECT_EQ(normalizer.tostring(url),
              "https://google.com/");
}

TEST(URLNormalizerTest, EmptyPathBecomesSlash)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL parsed = parser.parse("https://google.com");

    NormalizedURL url = normalizer.normalize(parsed);

    EXPECT_EQ(url.path, "/");
}

TEST(URLNormalizerTest, RemoveDuplicateSlashes)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL parsed =
        parser.parse("https://google.com//a///b");

    NormalizedURL url = normalizer.normalize(parsed);

    EXPECT_EQ(url.path, "/a/b");
}

TEST(URLNormalizerTest, RemoveCurrentDirectory)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL parsed =
        parser.parse("https://google.com/a/./b");

    NormalizedURL url = normalizer.normalize(parsed);

    EXPECT_EQ(url.path, "/a/b");
}

TEST(URLNormalizerTest, ResolveParentDirectory)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL parsed =
        parser.parse("https://google.com/a/b/../c");

    NormalizedURL url = normalizer.normalize(parsed);

    EXPECT_EQ(url.path, "/a/c");
}

TEST(URLNormalizerTest, MultipleParentDirectories)
{
    URLParser parser;
    URLNormalizer normalizer;

    ParsedURL parsed =
        parser.parse("https://google.com/a/b/c/../../d");

    NormalizedURL url = normalizer.normalize(parsed);

    EXPECT_EQ(url.path, "/a/d");
}
