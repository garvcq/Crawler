#include <gtest/gtest.h>
#include "filter/QuickFilter.h"

TEST(QuickFilterTest, Empty)
{
    EXPECT_FALSE(QuickFilter::shouldparse(""));
}

TEST(QuickFilterTest, Spaces)
{
    EXPECT_FALSE(QuickFilter::shouldparse("     "));
}

TEST(QuickFilterTest, Fragment)
{
    EXPECT_FALSE(QuickFilter::shouldparse("#"));
}

TEST(QuickFilterTest, FragmentSection)
{
    EXPECT_FALSE(QuickFilter::shouldparse("#about"));
}

TEST(QuickFilterTest, Relative)
{
    EXPECT_TRUE(QuickFilter::shouldparse("../page"));
}

TEST(QuickFilterTest, Absolute)
{
    EXPECT_TRUE(QuickFilter::shouldparse("https://google.com"));
}

TEST(QuickFilterTest, MailTo)
{
    EXPECT_TRUE(QuickFilter::shouldparse("mailto:test@test.com"));
}