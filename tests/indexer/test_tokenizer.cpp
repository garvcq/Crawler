#include <gtest/gtest.h>
#include "indexer/Tokenizer.h"
#include "DynamicArray.h"

TEST(TokenizerTest, BasicTokenization)
{
    Tokenizer tokenizer;
    DynamicArray<std::string> tokens = tokenizer.tokenize("hello world");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0], "hello");
    EXPECT_EQ(tokens[1], "world");
}

TEST(TokenizerTest, CaseFoldingAndPunctuation)
{
    Tokenizer tokenizer;
    DynamicArray<std::string> tokens = tokenizer.tokenize("Hello, World! C++ Indexer.");
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0], "hello");
    EXPECT_EQ(tokens[1], "world");
    EXPECT_EQ(tokens[2], "c");
    EXPECT_EQ(tokens[3], "indexer");
}

TEST(TokenizerTest, AlphanumericAndEmpty)
{
    Tokenizer tokenizer;
    DynamicArray<std::string> tokens = tokenizer.tokenize("Agent007 2026 test");
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0], "agent007");
    EXPECT_EQ(tokens[1], "2026");
    EXPECT_EQ(tokens[2], "test");
}

TEST(TokenizerTest, EmptyInput)
{
    Tokenizer tokenizer;
    DynamicArray<std::string> tokens = tokenizer.tokenize("   !!! ??? \t\n ");
    EXPECT_EQ(tokens.size(), 0);
}
