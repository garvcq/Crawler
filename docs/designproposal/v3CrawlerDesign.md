# Crawler Design - Version 3
The objective of this project is to implement a web crawler capable of collecting web pages from the internet for use by a search engine . Starting from a seed URL the crawler visits pages , extracts hyperlinks , discovers new pages and stores the rendered HTML in a SQLite database .

- Crawl web pages using BFS(Queue)
- Avoid duplicate crawling
- Supports modern JS websites that load their content through APIs and JavaScript.
- Store rendered HTML in a SQLite database for use by the indexer.
- Problems in the older versions - 
    * Older versions of the crawler design had a problem were they were only downloading the initial HTML returned by the server , this crawler renders pages using the Chrome DevTools Protocol(CDP). JS is executed before the page is processed.

## Architecture -
