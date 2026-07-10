#include <gtest/gtest.h>
#include "network/PageFetcher.h"

TEST(PageFetcherTest, FetchExampleDotCom)
{
    PageFetcher fetcher;
    Page page =fetcher.fetch("https://example.com");
    EXPECT_EQ(page.statusCode, 200);
    EXPECT_FALSE(page.html.empty());
    EXPECT_NE(page.html.find("Example Domain"),std::string::npos);
}

TEST(PageFetcherTest, FetchWikipedia)
{
    PageFetcher fetcher;
    Page page =fetcher.fetch("https://www.wikipedia.org");
    EXPECT_EQ(page.statusCode, 200);
    EXPECT_FALSE(page.html.empty());
}

TEST(PageFetcherTest, FetchGitHub)
{
    PageFetcher fetcher;
    Page page =fetcher.fetch("https://github.com");
    EXPECT_EQ(page.statusCode, 200);
    EXPECT_FALSE(page.html.empty());
}

TEST(PageFetcherTest, FetchSSIPMT)
{
    PageFetcher fetcher;
    Page page =fetcher.fetch("https://ssipmt.edu.in");
    EXPECT_EQ(page.statusCode, 200);
    EXPECT_FALSE(page.html.empty());
}

TEST(PageFetcherTest, FetchCodeQuotient)
{
    PageFetcher fetcher;
    Page page =fetcher.fetch("https://codequotient.com");
    EXPECT_EQ(page.statusCode, 200);
    EXPECT_FALSE(page.html.empty());
    EXPECT_NE(page.html.find("<html"),std::string::npos);
}