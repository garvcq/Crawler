#include "indexer/Indexer.h"
#include <iostream>
#include <thread>
#include <chrono>

Indexer::Indexer(const ConfigLoader& config)
{
    // Retrieve settings
    std::string host = config.getString("mysql_host", "127.0.0.1");
    std::string user = config.getString("mysql_user", "root");
    std::string password = config.getString("mysql_password", "8305");
    std::string dbname = config.getString("mysql_database", "crawler");
    int port = config.getInt("mysql_port", 3306);

    batchThreshold_ = config.getInt("batch_threshold", 30);
    pollIntervalSeconds_ = config.getInt("poll_interval_seconds", 5);
    adaptivePolling_ = config.getBool("adaptive_polling", true);
    stopwordFile_ = config.getString("stopword_file", "config/stopwords.txt");

    std::cout << "[Indexer] Connecting to MySQL database at " << host << ":" << port << "...\n";
    if (!storage_.connect(host, user, password, dbname, port))
    {
        std::cerr << "[Indexer] Database connection failed!\n";
        std::exit(1);
    }
}

void Indexer::run()
{
    std::cout << "[Indexer] Creating index tables...\n";
    if (!storage_.createIndexerTables())
    {
        std::cerr << "[Indexer] Failed to set up database indexer tables!\n";
        return;
    }

    std::cout << "[Indexer] Loading stopwords from " << stopwordFile_ << "...\n";
    if (!stopwordFilter_.loadStopwords(stopwordFile_))
    {
        std::cout << "[Warning] Stopwords file could not be loaded, proceeding without stopwords.\n";
    }

    std::cout << "[Indexer] Indexing Cycle started.\n";

    int currentSleep = pollIntervalSeconds_;
    int ticksSinceNewPages = 0;
    int lastUnindexedCount = 0;

    while (true)
    {
        int count = storage_.countUnindexed();
        if (count >= batchThreshold_)
        {
            std::cout << "[Indexer] Found " << count << " unindexed pages. Processing batch...\n";
            ticksSinceNewPages = 0;
            currentSleep = pollIntervalSeconds_;
            DynamicArray<RawPage> pages = storage_.getAllUnindexed();
            for (int i = 0; i < pages.size(); ++i)
            {
                indexPage(pages[i]);
            }
            std::cout << "[Indexer] Batch index completed successfully.\n";
        }
        else
        {
            // Adaptive polling sleep scaling
            if (count > 0 && count == lastUnindexedCount)
            {
                ticksSinceNewPages++;
            }
            else
            {
                ticksSinceNewPages = 0;
            }

            if (adaptivePolling_ && ticksSinceNewPages > 4)
            {
                currentSleep = std::min(currentSleep * 2, pollIntervalSeconds_ * 8);
                ticksSinceNewPages = 0;
            }

            std::cout << "[Indexer] Polling... " << count << " unindexed pages (threshold: " << batchThreshold_ << "). Sleep: " << currentSleep << "s\n";
            std::this_thread::sleep_for(std::chrono::seconds(currentSleep));
        }
        lastUnindexedCount = count;
    }
}

void Indexer::indexPage(const RawPage& page)
{
    std::cout << "[Indexer] Processing page id " << page.id << ": " << page.url << "\n";
    // Extract raw text and title
    std::string text = extractor_.stripTags(page.html);
    std::string title = extractor_.extractTitle(page.html);
    // Tokenize
    DynamicArray<std::string> tokens = tokenizer_.tokenize(text);
    // Count non-stopwords frequencies locally
    HashMap<std::string, int> termCounts;
    for (int i = 0; i < tokens.size(); ++i)
    {
        std::string token = tokens[i];
        if (!stopwordFilter_.isStopword(token))
        {
            int count = 0;
            if (termCounts.get(token, count))
            {
                termCounts.insert(token, count + 1);
            }
            else
            {
                termCounts.insert(token, 1);
            }
        }
    }

    // Insert results into the database in a transaction
    mysql_query(storage_.getConnection(), "START TRANSACTION");

    DynamicArray<std::string> uniqueWords = termCounts.getKeys();
    for (int i = 0; i < uniqueWords.size(); ++i)
    {
        std::string word = uniqueWords[i];
        int count = 0;
        termCounts.get(word, count);
        int wordId = storage_.getOrInsertWord(word);
        if (wordId != -1)   
        {
            storage_.insertWordOccurrence(wordId, page.id, count);
        }
    }
    // Mark page as indexed with title and clean text content
    storage_.markPageAsIndexed(page.id, title, text);
    mysql_query(storage_.getConnection(), "COMMIT");
}
