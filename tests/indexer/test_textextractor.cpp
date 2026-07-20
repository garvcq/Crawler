#include <gtest/gtest.h>
#include "indexer/TextExtractor.h"

TEST(TextExtractorTest, StripTagsBasic)
{
    TextExtractor extractor;
    std::string clean = extractor.stripTags("<div>hello <b>world</b></div>");
    EXPECT_EQ(clean, "hello world");
}

TEST(TextExtractorTest, StripScriptAndStyle)
{
    TextExtractor extractor;
    std::string clean = extractor.stripTags("<html><script>var x = 10;</script><style>body {color: red;}</style><body>content</body></html>");
    EXPECT_EQ(clean, "content");
}

TEST(TextExtractorTest, ExtractTitle)
{
    TextExtractor extractor;
    EXPECT_EQ(extractor.extractTitle("<html><head><title>My Title</title></head></html>"), "My Title");
    EXPECT_EQ(extractor.extractTitle("<html><head><TITLE>Mixed Case</TITLE></head></html>"), "Mixed Case");
    EXPECT_EQ(extractor.extractTitle("<html><body>No Title</body></html>"), "");
}

TEST(TextExtractorTest, DecodeEntities)
{
    TextExtractor extractor;
    // Decode quotes, ampersand, copy,hellip, etc.
    EXPECT_EQ(extractor.decodeEntities("hello &amp; world"), "hello & world");
    EXPECT_EQ(extractor.decodeEntities("user&#039;s page"), "user's page");
    EXPECT_EQ(extractor.decodeEntities("user&#39;s page"), "user's page");
    EXPECT_EQ(extractor.decodeEntities("&ldquo;Hello&rdquo;"), "\"Hello\"");
    EXPECT_EQ(extractor.decodeEntities("&copy; 2026"), "(c) 2026");
}

TEST(TextExtractorTest, IgnoreJsonPayload)
{
    TextExtractor extractor;
    // JSON object
    EXPECT_EQ(extractor.stripTags("   { \"id\": 10, \"name\": \"item\" }"), "");
    // JSON array
    EXPECT_EQ(extractor.stripTags("[\"hello\", \"world\"]"), "");
    // Normal HTML starting with space is OK
    EXPECT_EQ(extractor.stripTags("  <p>hello</p>"), "hello");
}
