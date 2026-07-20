#include <iostream>
#include "config/ConfigLoader.h"
#include "indexer/Indexer.h"

int main()
{
    std::cout << "Starting Indexer CLI...\n";
    ConfigLoader config;
    if (!config.load("config/indexer.conf"))
    {
        std::cerr << "[Warning] Failed to load config/indexer.conf, using default settings.\n";
    }
    Indexer indexer(config);
    indexer.run();
    return 0;
}
