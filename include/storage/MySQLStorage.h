#pragma once

#include <mysql/mysql.h>
#include <string>
#include "DynamicArray.h"
#include "network/Page.h"
#include "crawler/URLState.h"

class MySQLStorage
{
public:
    MySQLStorage();
    ~MySQLStorage();
    bool connect(const std::string& host,const std::string& user,const std::string& password,const std::string& database,unsigned int port = 3306);
    void disconnect();
    bool createTables();
    bool savePage(const Page& page);
    bool insertURL(const std::string& url,int depth,URLState state);
    bool updateState(const std::string& url,URLState state);
    bool isConnected() const;
    bool urlExists(const std::string& url);
    bool loadURLs(DynamicArray<std::string>& urls, DynamicArray<int>& depths, DynamicArray<URLState>& states);
    bool clearStorage();

    // Direct access to MYSQL connection for helper engines
    MYSQL* getConnection() const { return connection_; }
protected:
    MYSQL* connection_;
};