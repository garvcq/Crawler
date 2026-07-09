#include <gtest/gtest.h>
#include "common/PatternMatcher.h"

TEST(PatternMatcherTest, FindAtBeginning)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abcdef", "abc"), 0);
}

TEST(PatternMatcherTest, FindInMiddle)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abcdef", "cd"), 2);
}

TEST(PatternMatcherTest, FindAtEnd)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abcdef", "def"), 3);
}

TEST(PatternMatcherTest, PatternNotFound)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abcdef", "xyz"), -1);
}

TEST(PatternMatcherTest, EmptyText)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("", "abc"), -1);
}

TEST(PatternMatcherTest, EmptyPattern)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abcdef", ""), 0);
}

TEST(PatternMatcherTest, BothEmpty)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("", ""), 0);
}

TEST(PatternMatcherTest, PatternLongerThanText)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abc", "abcdef"), -1);
}

TEST(PatternMatcherTest, SingleCharacterFound)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abcdef", "e"), 4);
}

TEST(PatternMatcherTest, SingleCharacterNotFound)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abcdef", "x"), -1);
}

TEST(PatternMatcherTest, RepeatedCharacters)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("aaaaaa", "aaa"), 0);
}

TEST(PatternMatcherTest, OverlappingPattern)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abababa", "ababa"), 0);
}

TEST(PatternMatcherTest, KMPFallbackCase)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abababc", "ababc"), 2);
}

TEST(PatternMatcherTest, ClassicKMPExample)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abxabcabcaby", "abcaby"), 6);
}

TEST(PatternMatcherTest, MultipleOccurrencesReturnsFirst)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abcabcabc", "abc"), 0);
}

TEST(PatternMatcherTest, WholeTextMatchesPattern)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("pattern", "pattern"), 0);
}

TEST(PatternMatcherTest, CaseSensitiveSearch)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("Google", "google"), -1);
}

TEST(PatternMatcherTest, SearchWithSpaces)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("hello world", "world"), 6);
}

TEST(PatternMatcherTest, SearchWithSymbols)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("abc@#$xyz", "@#$"), 3);
}

TEST(PatternMatcherTest, LongPrefixSuffixPattern)
{
    PatternMatcher kmp;

    EXPECT_EQ(kmp.find("AAACAAAAAC", "AAAA"), 4);
}

//contains

TEST(PatternMatcherTest, ContainsTrue)
{
    PatternMatcher kmp;

    EXPECT_TRUE(kmp.contains("abcdef", "cde"));
}

TEST(PatternMatcherTest, ContainsFalse)
{
    PatternMatcher kmp;

    EXPECT_FALSE(kmp.contains("abcdef", "xyz"));
}

TEST(PatternMatcherTest, ContainsEmptyPattern)
{
    PatternMatcher kmp;

    EXPECT_TRUE(kmp.contains("abcdef", ""));
}

