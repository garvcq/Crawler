#include "indexer/IndexerStorage.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

int IndexerStorage::countUnindexed()
{
    if (connection_ == nullptr) return 0;

    const char* query = 
        "SELECT COUNT(*) FROM pages p "
        "LEFT JOIN indexed_pages i ON p.id = i.page_id "
        "WHERE p.status_code = 200 AND i.page_id IS NULL";

    if (mysql_query(connection_, query) != 0)
    {
        std::cout<<"[IndexerStorage] Error counting unindexed pages: "<< mysql_error(connection_) << "\n";
        return 0;
    }
    MYSQL_RES* result = mysql_store_result(connection_);
    if (result == nullptr) return 0;
    int count = 0;
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0])
    {
        count = std::atoi(row[0]);
    }
    mysql_free_result(result);
    return count;
}

DynamicArray<RawPage> IndexerStorage::getAllUnindexed()
{
    DynamicArray<RawPage> pages;
    if (connection_ == nullptr) return pages;
    const char* query = 
        "SELECT p.id, p.url, p.html FROM pages p "
        "LEFT JOIN indexed_pages i ON p.id = i.page_id "
        "WHERE p.status_code = 200 AND i.page_id IS NULL";

    if (mysql_query(connection_, query) != 0)
    {
        std::cout << "[IndexerStorage] Error loading unindexed pages: "<< mysql_error(connection_) << "\n";
        return pages;
    }
    MYSQL_RES* result = mysql_store_result(connection_);
    if (result == nullptr) return pages;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        if (row[0] && row[1] && row[2])
        {
            RawPage page;
            page.id = std::atoi(row[0]);
            page.url = row[1];
            page.html = row[2];
            pages.append(page);
        }
    }
    mysql_free_result(result);
    return pages;
}

bool IndexerStorage::createIndexerTables()
{
    if (connection_ == nullptr) return false;

    // 1. dictionary table
    const char* wordsTableQuery = 
        "CREATE TABLE IF NOT EXISTS words ("
        "  id INT AUTO_INCREMENT PRIMARY KEY,"
        "  word VARCHAR(255) UNIQUE"
        ") ENGINE=InnoDB";

    if (mysql_query(connection_, wordsTableQuery) != 0)
    {
        std::cout << "[IndexerStorage] Failed to create words catalog: "<< mysql_error(connection_) << "\n";
        return false;
    }

    // 2. occurrences table
    const char* occurrencesTableQuery = 
        "CREATE TABLE IF NOT EXISTS word_occurrences ("
        "  word_id INT,"
        "  page_id BIGINT,"
        "  frequency INT,"
        "  PRIMARY KEY(word_id, page_id)"
        ") ENGINE=InnoDB";

    if (mysql_query(connection_, occurrencesTableQuery) != 0)
    {
        std::cout << "[IndexerStorage] Failed to create occurrences table: "<< mysql_error(connection_) << "\n";
        return false;
    }

    // 3. progress tracking table
    const char* progressTableQuery = 
        "CREATE TABLE IF NOT EXISTS indexed_pages ("
        "  page_id BIGINT PRIMARY KEY,"
        "  title TEXT,"
        "  content LONGTEXT"
        ") ENGINE=InnoDB";

    if (mysql_query(connection_, progressTableQuery) != 0)
    {
        std::cout << "[IndexerStorage] Failed to create indexed_pages table: "<< mysql_error(connection_) << "\n";
        return false;
    }
    return true;
}

int IndexerStorage::getOrInsertWord(const std::string& word)
{
    if (connection_ == nullptr) return -1;
    // Try inserting first 
    const char* insertQuery = "INSERT IGNORE INTO words (word) VALUES (?)";
    MYSQL_STMT* insertStmt = mysql_stmt_init(connection_);
    if (insertStmt)
    {
        if (mysql_stmt_prepare(insertStmt, insertQuery, std::strlen(insertQuery)) == 0)
        {
            MYSQL_BIND bind[1]{};
            unsigned long len = word.length();
            bind[0].buffer_type = MYSQL_TYPE_STRING;
            bind[0].buffer = (void*)word.c_str();
            bind[0].length = &len;
            mysql_stmt_bind_param(insertStmt, bind);
            mysql_stmt_execute(insertStmt);
        }
        mysql_stmt_close(insertStmt);
    }

    // Retrieve the ID
    const char* selectQuery = "SELECT id FROM words WHERE word = ?";
    MYSQL_STMT* selectStmt = mysql_stmt_init(connection_);
    if (!selectStmt) return -1;
    int wordId = -1;
    if (mysql_stmt_prepare(selectStmt, selectQuery, std::strlen(selectQuery)) == 0)
    {
        MYSQL_BIND bind[1]{};
        unsigned long len = word.length();
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (void*)word.c_str();
        bind[0].length = &len;
        if (mysql_stmt_bind_param(selectStmt, bind) == 0)
        {
            MYSQL_BIND resultBind[1]{};
            resultBind[0].buffer_type = MYSQL_TYPE_LONG;
            resultBind[0].buffer = &wordId;
            if (mysql_stmt_bind_result(selectStmt, resultBind) == 0)
            {
                if (mysql_stmt_execute(selectStmt) == 0)
                {
                    if (mysql_stmt_fetch(selectStmt) != 0)
                    {
                        wordId = -1;
                    }
                }
            }
        }
    }
    mysql_stmt_close(selectStmt);
    return wordId;
}

bool IndexerStorage::insertWordOccurrence(int wordId, int pageId, int frequency)
{
    if (connection_ == nullptr) return false;
    const char* query = 
        "INSERT INTO word_occurrences (word_id, page_id, frequency) "
        "VALUES (?, ?, ?) ON DUPLICATE KEY UPDATE frequency = VALUES(frequency)";

    MYSQL_STMT* stmt = mysql_stmt_init(connection_);
    if (!stmt) return false;
    if (mysql_stmt_prepare(stmt, query, std::strlen(query)) != 0)
    {
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[3]{};
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &wordId;

    bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    long long pId = pageId;
    bind[1].buffer = &pId;

    bind[2].buffer_type = MYSQL_TYPE_LONG;
    bind[2].buffer = &frequency;

    if (mysql_stmt_bind_param(stmt, bind) != 0)
    {
        mysql_stmt_close(stmt);
        return false;
    }
    bool success = (mysql_stmt_execute(stmt) == 0);
    mysql_stmt_close(stmt);
    return success;
}

bool IndexerStorage::markPageAsIndexed(int pageId, const std::string& title, const std::string& content)
{
    if (connection_ == nullptr) return false;

    const char* query = 
        "INSERT INTO indexed_pages (page_id, title, content) VALUES (?, ?, ?) "
        "ON DUPLICATE KEY UPDATE title = VALUES(title), content = VALUES(content)";
    MYSQL_STMT* stmt = mysql_stmt_init(connection_);
    if (!stmt) return false;

    if (mysql_stmt_prepare(stmt, query, std::strlen(query)) != 0)
    {
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[3]{};
    
    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    long long pId = pageId;
    bind[0].buffer = &pId;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (void*)title.c_str();
    unsigned long titleLen = title.length();
    bind[1].length = &titleLen;

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (void*)content.c_str();
    unsigned long contentLen = content.length();
    bind[2].length = &contentLen;

    if (mysql_stmt_bind_param(stmt, bind) != 0)
    {
        mysql_stmt_close(stmt);
        return false;
    }

    bool success = (mysql_stmt_execute(stmt) == 0);
    mysql_stmt_close(stmt);
    return success;
}
