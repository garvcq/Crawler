#include <gtest/gtest.h>
#include "common/PatternMatcher.h"

class PatternMatcherTest : public ::testing::Test
{
protected:
    PatternMatcher matcher;
};

//
// find()
//

TEST_F(PatternMatcherTest, FindBeginning)
{
    EXPECT_EQ(matcher.find("abcdef", "abc"), 0);
}

TEST_F(PatternMatcherTest, FindMiddle)
{
    EXPECT_EQ(matcher.find("abcdef", "cd"), 2);
}

TEST_F(PatternMatcherTest, FindEnd)
{
    EXPECT_EQ(matcher.find("abcdef", "def"), 3);
}

TEST_F(PatternMatcherTest, PatternNotFound)
{
    EXPECT_EQ(matcher.find("abcdef", "xyz"), -1);
}

TEST_F(PatternMatcherTest, PatternEqualsText)
{
    EXPECT_EQ(matcher.find("abcdef", "abcdef"), 0);
}

TEST_F(PatternMatcherTest, EmptyPattern)
{
    EXPECT_EQ(matcher.find("abcdef", ""), 0);
}

TEST_F(PatternMatcherTest, EmptyText)
{
    EXPECT_EQ(matcher.find("", "abc"), -1);
}

TEST_F(PatternMatcherTest, BothEmpty)
{
    EXPECT_EQ(matcher.find("", ""), 0);
}

TEST_F(PatternMatcherTest, PatternLongerThanText)
{
    EXPECT_EQ(matcher.find("abc", "abcdef"), -1);
}

TEST_F(PatternMatcherTest, SingleCharacter)
{
    EXPECT_EQ(matcher.find("abcdef", "e"), 4);
}

TEST_F(PatternMatcherTest, RepeatedCharacters)
{
    EXPECT_EQ(matcher.find("aaaaaa", "aaa"), 0);
}

TEST_F(PatternMatcherTest, OverlappingPattern)
{
    EXPECT_EQ(matcher.find("abababa", "ababa"), 0);
}

TEST_F(PatternMatcherTest, ClassicKMPExample)
{
    EXPECT_EQ(matcher.find("abxabcabcaby", "abcaby"), 6);
}

TEST_F(PatternMatcherTest, LongPrefixSuffix)
{
    EXPECT_EQ(matcher.find("AAACAAAAAC", "AAAA"), 4);
}

TEST_F(PatternMatcherTest, CaseSensitive)
{
    EXPECT_EQ(matcher.find("Google", "google"), -1);
}

TEST_F(PatternMatcherTest, Spaces)
{
    EXPECT_EQ(matcher.find("hello world", "world"), 6);
}

TEST_F(PatternMatcherTest, Symbols)
{
    EXPECT_EQ(matcher.find("abc@#$xyz", "@#$"), 3);
}

//
// contains()
//

TEST_F(PatternMatcherTest, ContainsTrue)
{
    EXPECT_TRUE(matcher.contains("abcdef", "cde"));
}

TEST_F(PatternMatcherTest, ContainsFalse)
{
    EXPECT_FALSE(matcher.contains("abcdef", "xyz"));
}

TEST_F(PatternMatcherTest, ContainsEmptyPattern)
{
    EXPECT_TRUE(matcher.contains("abcdef", ""));
}

TEST_F(PatternMatcherTest, ContainsWholeText)
{
    EXPECT_TRUE(matcher.contains("pattern", "pattern"));
}

//
// findall()
//

TEST_F(PatternMatcherTest, FindAllMultiple)
{
    DynamicArray<int> result =
        matcher.findall("abcabcabc", "abc");

    ASSERT_EQ(result.size(), 3);

    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 3);
    EXPECT_EQ(result[2], 6);
}

TEST_F(PatternMatcherTest, FindAllOverlapping)
{
    DynamicArray<int> result =
        matcher.findall("aaaaa", "aaa");

    ASSERT_EQ(result.size(), 3);

    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 1);
    EXPECT_EQ(result[2], 2);
}

TEST_F(PatternMatcherTest, FindAllSingleOccurrence)
{
    DynamicArray<int> result =
        matcher.findall("abcdef", "cd");

    ASSERT_EQ(result.size(), 1);

    EXPECT_EQ(result[0], 2);
}

TEST_F(PatternMatcherTest, FindAllNone)
{
    DynamicArray<int> result =
        matcher.findall("abcdef", "xyz");

    EXPECT_TRUE(result.isEmpty());
}

TEST_F(PatternMatcherTest, FindAllWholeText)
{
    DynamicArray<int> result =
        matcher.findall("abcdef", "abcdef");

    ASSERT_EQ(result.size(), 1);

    EXPECT_EQ(result[0], 0);
}

TEST_F(PatternMatcherTest, FindAllPatternLongerThanText)
{
    DynamicArray<int> result =
        matcher.findall("abc", "abcdef");

    EXPECT_TRUE(result.isEmpty());
}

TEST_F(PatternMatcherTest, FindAllEmptyText)
{
    DynamicArray<int> result =
        matcher.findall("", "abc");

    EXPECT_TRUE(result.isEmpty());
}

TEST_F(PatternMatcherTest, FindAllEmptyPattern)
{
    DynamicArray<int> result =
        matcher.findall("abcdef", "");

    EXPECT_TRUE(result.isEmpty());
}