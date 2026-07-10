#include <gtest/gtest.h>

#include "filter/LinkFilter.h"
#include <fstream>

class LinkFilterTest : public ::testing::Test
{
protected:

    std::string domainFile = "blockeddomains_test.txt";
    std::string extensionFile = "blockedextensions_test.txt";

    void SetUp() override
    {
        std::ofstream domains(domainFile);

        domains << "fonts.googleapis.com\n";
        domains << "cdnjs.cloudflare.com\n";
        domains.close();

        std::ofstream extensions(extensionFile);

        extensions << "css\n";
        extensions << "js\n";
        extensions << "png\n";
        extensions << "jpg\n";
        extensions << "pdf\n";
        extensions.close();
    }

    void TearDown() override
    {
        std::remove(domainFile.c_str());
        std::remove(extensionFile.c_str());
    }

    NormalizedURL makeURL(
        const std::string& scheme,
        const std::string& host,
        const std::string& path)
    {
        NormalizedURL url;

        url.scheme = scheme;
        url.host = host;
        url.path = path;

        return url;
    }
};

TEST_F(LinkFilterTest, AllowNormalPage)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "example.com",
        "/about");

    EXPECT_TRUE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, RejectMailTo)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "mailto",
        "",
        "");

    EXPECT_FALSE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, RejectJavascript)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "javascript",
        "",
        "");

    EXPECT_FALSE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, RejectFTP)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "ftp",
        "example.com",
        "/");

    EXPECT_FALSE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, RejectBlockedDomain)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "fonts.googleapis.com",
        "/css");

    EXPECT_FALSE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, AllowUnknownDomain)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "github.com",
        "/");

    EXPECT_TRUE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, RejectCSS)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "example.com",
        "/style.css");

    EXPECT_FALSE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, RejectJS)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "example.com",
        "/app.js");

    EXPECT_FALSE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, RejectPNG)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "example.com",
        "/images/logo.png");

    EXPECT_FALSE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, RejectPDF)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "example.com",
        "/docs/file.pdf");

    EXPECT_FALSE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, AllowHTML)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "example.com",
        "/index.html");

    EXPECT_TRUE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, AllowNoExtension)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "example.com",
        "/products");

    EXPECT_TRUE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, IgnoreDotsInFolderName)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "example.com",
        "/v1.2/products");

    EXPECT_TRUE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, RejectCSSWithQuery)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "example.com",
        "/style.css?v=2");

    EXPECT_FALSE(filter.shouldvisit(url));
}

TEST_F(LinkFilterTest, RejectPNGWithQuery)
{
    LinkFilter filter(domainFile, extensionFile);

    auto url = makeURL(
        "https",
        "example.com",
        "/images/logo.png?width=200");

    EXPECT_FALSE(filter.shouldvisit(url));
}