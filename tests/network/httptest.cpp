#include <gtest/gtest.h>

#include "network/HTTPFetcher.h"

TEST(HTTPFetcherTest, FetchExampleDotCom)
{
    HTTPFetcher fetcher;
    Page page =fetcher.fetch("https://example.com");
    EXPECT_EQ(page.statusCode, 200);
    EXPECT_FALSE(page.html.empty());
    EXPECT_NE(page.html.find("Example Domain"),std::string::npos);
    EXPECT_FALSE(page.rendered);
}

TEST(HTTPFetcherTest, FetchWikipedia)
{
    HTTPFetcher fetcher;
    Page page =fetcher.fetch("https://www.wikipedia.org");
    EXPECT_EQ(page.statusCode, 200);
    EXPECT_FALSE(page.html.empty());
    EXPECT_NE(page.html.find("<html"),std::string::npos);
}

TEST(HTTPFetcherTest, InvalidURL)
{
    HTTPFetcher fetcher;
    Page page =fetcher.fetch("https://this-domain-does-not-exist-123456789.com");
    EXPECT_TRUE(page.html.empty());
    EXPECT_NE(page.statusCode, 200);
}

TEST(HTTPFetcherTest, Redirect)
{
    HTTPFetcher fetcher;
    Page page =fetcher.fetch("http://github.com");
    EXPECT_EQ(page.statusCode, 200);
    EXPECT_FALSE(page.html.empty());
}

TEST(HTTPFetcherTest, FetchCodeQuotient)
{
    HTTPFetcher fetcher;
    Page page =fetcher.fetch("https://codequotient.com");
    EXPECT_EQ(page.statusCode, 200);
    EXPECT_FALSE(page.html.empty());
}