#include <gtest/gtest.h>
#include "crawler/Frontier.h"

TEST(FrontierTest,EmptyInitially)
{
    Frontier frontier;
    EXPECT_TRUE(frontier.empty());
    EXPECT_EQ(frontier.size(),0);
}
