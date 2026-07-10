#include <gtest/gtest.h>

#include "crawler/SeenStore.h"

TEST(SeenStoreTest, InitiallyEmpty)
{
    SeenStore store;
    EXPECT_EQ(store.size(), 0);
}

TEST(SeenStoreTest, AddURL)
{
    SeenStore store;
    EXPECT_TRUE(store.tryAdd("https://example.com",0));
    EXPECT_EQ(store.size(), 1);
}

TEST(SeenStoreTest, DuplicateURL)
{
    SeenStore store;

    EXPECT_TRUE(store.tryAdd("https://example.com",0));
    EXPECT_FALSE(store.tryAdd("https://example.com",1));
    EXPECT_EQ(store.size(), 1);
}

TEST(SeenStoreTest, Contains)
{
    SeenStore store;
    store.tryAdd("https://example.com",0);
    EXPECT_TRUE(store.contains("https://example.com"));
    EXPECT_FALSE(store.contains("https://google.com"));
}

TEST(SeenStoreTest, GetExisting)
{
    SeenStore store;
    store.tryAdd("https://example.com",3);
    URLInfo info;
    EXPECT_TRUE(store.get("https://example.com",info));
    EXPECT_EQ(info.depth,3);
    EXPECT_EQ(info.state,URLState::Queued);
}

TEST(SeenStoreTest, GetMissing)
{
    SeenStore store;
    URLInfo info;
    EXPECT_FALSE(store.get("https://example.com",info));
}

TEST(SeenStoreTest, UpdateCompleted)
{
    SeenStore store;
    store.tryAdd("https://example.com",2);
    EXPECT_TRUE(store.updateState("https://example.com",URLState::Completed));
    URLInfo info;
    EXPECT_TRUE(store.get("https://example.com",info));
    EXPECT_EQ(info.state,URLState::Completed);
    EXPECT_NE(info.crawlTime,0);
}

TEST(SeenStoreTest, UpdateMissing)
{
    SeenStore store;
    EXPECT_FALSE(store.updateState("https://example.com",URLState::Completed));
}