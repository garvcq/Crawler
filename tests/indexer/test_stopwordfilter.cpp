#include <gtest/gtest.h>
#include "indexer/StopwordFilter.h"
#include <fstream>

TEST(StopwordFilterTest, LoadAndVerify)
{
    // Create a temporary stopwords file
    const std::string tempStopwordFile = "temp_stopwords.txt";
    std::ofstream outFile(tempStopwordFile);
    outFile << "the\n";
    outFile << " and \n"; // Leading/trailing whitespace
    outFile << "a\n";
    outFile.close();

    StopwordFilter filter;
    ASSERT_TRUE(filter.loadStopwords(tempStopwordFile));

    // Test stopwords
    EXPECT_TRUE(filter.isStopword("the"));
    EXPECT_TRUE(filter.isStopword("and"));
    EXPECT_TRUE(filter.isStopword("a"));

    // Test non-stopwords
    EXPECT_FALSE(filter.isStopword("hello"));
    EXPECT_FALSE(filter.isStopword("google"));

    // Clean up
    std::remove(tempStopwordFile.c_str());
}

TEST(StopwordFilterTest, MissingFileReturnsFalse)
{
    StopwordFilter filter;
    EXPECT_FALSE(filter.loadStopwords("non_existent_stopwords_file_12345.txt"));
}
