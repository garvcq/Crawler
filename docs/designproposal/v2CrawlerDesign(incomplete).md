//HAD A FEW DESIGN DECISIONS WHICH LED TO MAJOR CHANGES IN THE CRAWLER SO I CONTINUED IN A NEW V3 version instead
# Crawler Design -
This project implements a web crawler that starts from a seed URL and visits web pages using Breadth first search BFS. The crawler downloads web pages using libcurl stores the downloaded content in a SQLite database avoids duplicate crawling and extracts new URLs for further crawling. The collected pages are stored in a format that can be directly used by the Indexer.
Its designed in a way that makes it work with many modern websites that load their content through APIs and JavaScript.

## Design Decisions - 
* During the design phase several changes were made to improve the crawler architecture and make it easier to extend in future projects -
    * A separate Queue class is used instead of directly using SinglyList so that only queue operations are exposed to the Frontier.
    * URLs and their crawling depth are stored together using the URLDepth structure so they always move together inside the Frontier.
    * A separate SeenStore is maintained to prevent duplicate URLs from entering the Frontier before downloading.
    * SQLite is used instead of text files for permanent storage because it provides faster lookup easier management and can be directly reused by the Indexer 
    * The crawler stores the original downloaded HTML or JSON without modifying it. This allows the Indexer to process the raw content later
    * HTML parsing JSON parsing URL normalization and downloading are implemented as separate components instead of placing all the logic inside the Crawler. This keeps each component responsible for only one task.
    * Pattern matching is implemented using the Knuth Morris Pratt KMP algorithm instead of repeated naive string searching. This allows efficient searching of patterns while extracting URLs and API endpoints.
    * API endpoint discovery is separated into an EndpointExtractor component which scans downloaded JavaScript files for common API calls such as fetch axios and GraphQL requests. This allows the crawler to work with many modern websites that load their content through APIs.
* The crawler is divided into multiple independent components where each component performs a single responsibility. This keeps the design modular makes the implementation easier to maintain and allows the same components to be reused in future projects such as the Indexer.

Architecture Diagram -
//an architecture diagram showing the components and their interactions

Flow Diagram -
//a flow diagram showing the flow of the crawler from seed URL to downloaded pages and extracted URLs

## Components - 

### 1. Queue -
* Reusable FIFO (First in First out) data structure used by the frontier . It stores URLs in the order they are discovered and always returns the oldest present URL first . This helps the frontier in the crawler to follow BFS traversal.

* Design -  The queue is implemented using a singly linked list made by me in the previous project since a normal queue only needs insertion at rear and removal from front a singly linked list could do both of them in constant time so i have created a seperated queue class instead of directly using the linked list class in the frontier so that only queue operations are exposed to the frontier and the rest of the linked list functions are not available to the frontier.

* Public APIs / Structure  - 
```cpp
template<typename T>
class Queue{
    private:
        SinglyList<T> queue;//data structure to store the elements of the queue
    public:
        Queue();//creates an empty queue
        void push(const T& value);//adds an element to the rear end
        T pop();//remove eand returns front element 
        T& front();//returns the front element without removing it 
        const T& front() const;//read only access to front element
        bool empty() const;//checks whether the queue is empty
        int size() const;//returns the no of elements
        void clear();//removes all the elements from the queue
};
```
* Internal Representation - //to be added

* Time complexity - 
    * Push - O(1)
    * Pop - O(1)
    * Front - O(1)
    * Empty - O(1)
    * Size - O(1)
    * Clear - O(n)

* The project only requires a queue for the Frontier but does not specify how is should be implemented so instead of using singly list directly i made a seperate queue class which hides list specific functions and only exposes the required queue operations to the frontier and also queue could be later used in other projects if required

* DynamicArray was not used because it requires shifting of elements on removal and resizing overhead and circular DynamicArray needed a lot modification to the exisitin DA Class and DLL was not used because we did not need to remove elements from the end and it had more memory overhead  also so SLL gives us O(1) insertion from rear and removal from front which is needed and is ideal for this.

### 2. URL Depth Structure -
* This structure stores a URL and its depth every entry added to the frontier is stored as a URL depth object so that the crawler always knows which page to visit and its depth from the seed URL 

* This helps the crawler to avoid visiting pages beyond the specified depth .

* Design -  The URL and Depth are stored together in a single structure instead of maintaining two seperate data structures this keeps both values together in sync (if one gets added both move together and get ordered together ) and avoids unnecessary lookups while processing the frontier + cleaner API design instead of passing multiple collections to the frontier and having to maintain them separately 

* Structure - 
```cpp
class URLDepth{
    string url;//stores the url of the page
    int depth;//stores the depth of the page from the seed url
};
```
* Internal Representation - //to be added

### 3. Frontier -
* The frontier stores all the urls waiting to be crawler whenever a new valid url is discovered it is added to the frontier the crawler always processes the url from the front of the frontier which allows the pages to be visited in BFS order.The frontier is directly used by the Crawler to manage pending pages.

* Design - The frontier is implemented using the queue class Queue<URLDepth> . A queue follows the FIFO order which matches the BFS crawling strategy each queue entry stores both the url and it depth using the URLDepth structure.

* Public APIs / Structure - 
```cpp
class Frontier{
    private:
        Queue<URLDepth> frontier;//stores URLs waiting to be crawled
    public:
        Frontier();//creates an empty frontier
        void push(const URLDepth& url);//adds a new url to the frontier
        URLDepth pop();//removes and returns the next url
        URLDepth& front();//returns the next url without removing it
        bool empty() const;//checks whether the frontier is empty
        int size() const;//returns the no of urls in the frontier
        void clear();//removes all the urls from the frontier
};
```
* Internal Representation - //to be added

* Time complexity - //same as queue
    * Push - O(1)
    * Pop - O(1)
    * Front - O(1)
    * Empty - O(1)
    * Size - O(1)
    * Clear - O(n)

* A custom Queue<URLDepth> is used because it helps with the BFS traversing while hiding the underlying linked list implementation. This also keeps the frontier independent of the actual data structure used by the queue.

### 4. SeenStore -
* The seenstore keeps track of all the urls that have already been crawled or are waiting to be crawled in the frontier this helps the crawler to avoid visiting the same page multiple times.The SeenStore is directly used by the crawler before adding any new URL to the frontier 

* Design -  
    * The seenstore is implemented using the custom HashMap<string,URLState> which stores the url as the key while the value stores its current crawling state. Hashmap provides fast insertion and lookup of urls which makes duplicate checking faster & efficient even when crawling a large number of pages.

    * The seenstore is kept seperate from the SQLite storage because duplicate checking is required before a page is downlaoded and stored , if duplicate detection depended only on the database the crawler could still enqueue the same URL multiple times before it is stored. ()

* Public APIs / Structure  - 
```cpp
enum URLState{
    DISCOVERED,//url added to the frontier or already crawled
    CRAWLED,//url is already crawled and stored
    FAILED//url failed to download or process
};

class SeenStore{
    private:
        HashMap<string,URLState>seen;//stores the urls that have been seen and their state
        public:
        SeenStore();//creates an empty seenstore
        bool contains(const string& url) const;//checks whether the url has been seen
        void add(const string& url);//adds a new url to the seenstore
        URLState getState(const string& url) const;//returns the crawling state of a url
        void updateState(const string& url,URLState state);//updates the crawling state of a url
        void rebuld(SQLiteStorage& db);//rebuilds the seenstore from the database after a restart
        void clear();//removes all the urls from the seenstore
        int size() const;//returns the no of urls in the seenstore
};
```
* Internal Representation - //to be added

* Time complexity - 
    * Contains - Best - O(1),Average - O(1),Worst - O(n) 
    * Add - Best - O(1),Average - O(1),Worst - O(n) 
    * UpdateState - Best - O(1),Average - O(1),Worst - O(n) 
    * getState - Best - O(1),Average - O(1),Worst - O(n) 
    * Clear - O(n)
    * Size - O(1) 
    * Note - O(n) in worst case is due to collisions in the hashmap

* The seenstore is maintained separately from the SQLite storage because a URL can be discovered multiple times before it is downloaded. Since a page is stored in the database only after a successful download  checking only the database would not detect duplicate URLs that are already waiting in the Frontier. The seenstore records a URL as soon as it is discovered, ensuring that every URL is added to the Frontier only once.

* Maintaining the current crawling state instead of only a boolean also amkes the design easier to extend by distinguishing between discovered crawled and failed pages.

* During a restart the seenstore can be rebuit from the database allowing the crawler to resume from previous crawls without revisiting already crawled pages.

* An enum is used because a URL can only be in one crawling state at a time. It provides a simple way to represent a fixed set of possible states while using less memory than a struct a struct would be required if additional information such as retry count, status code, or last crawl time needed to be stored for each URL.


### 5. PatternMatcher -
* its a reusable utility component reponsible for efficiently searching patterns inside strings. It is used by the HTML Parser and JSON Parser and the EndpointExtractor to locate URLs HTML attributes and API related patterns while parsing downloaded content

* Design - The patternmatcher is implemented using the KMP Knuth Morris Pratt string matching algorithm instead of repeated string searching since multiple components need to search for different patterns the searching logic is seperated into its own class this avoids code duplication and allows all parsers to reuse the same implementation

* Public APIs / Structure  - 
```cpp
class PatternMatcher{
    public:
        PatternMatcher();//creates a new pattern matcher
        bool contains(const string& text,const string& pattern);//checks whether the pattern is present in the text
        int find(const string& text,const string& pattern);//returns the index of the first occurrence of the pattern in the text or -1 if not found
        DynamicArray<int> findAll(const string& text,const string& pattern);//returns the indexes of all occurrences of the pattern in the text
        private:
        DynamicArray<int> buildLPS(const string& pattern);//builds the longest prefix suffix array for the pattern used in KMP algorithm
};
```
* Time Complexity - 
    * contains() - O(n+m) where n is the length of the text and m is the length of the pattern
    * find() - O(n+m) where n is the length of the text and m is the length of the pattern
    * findAll() - O(n+m+k) where n is the length of the text and m is the length of the pattern and k is the number of occurrences found

* The crawler performs pattern matching in multiple components while extracting HTML links JavaScript API endpoints and URLs from json responses. Implementing the searching logic seperately keeps the parsers simple and avoids maintaining multiple string searching implementations. The kmp algorithm provides linear time searching making it suitable for processing large HTML and JSON.

### 6. Downloader -

* it is responsible for fetching web pages from the internet using a given URL. it sends HTTP requests using libcurl receives the server response and returns all the information required by the crawler it is directly used by crawler before parsing and storing a page.

* Design - 
    * The downloader is implemented as a separate component instead of placing the networking logic inside ethe crawler . This keeps the crawling algorithm independent from the networking implementation and allows the downloading logic to be modified without affecting the crawler.

    * Instead of returning only the downloaded content the Downloader returns a response object containing the response body HTTP status final URL after redirect and the content type. This allowes the crawler to decide ehow the downloaded content shouldbe processed based on the content type and status code.

* Public APIs / Structure  - 
```cpp
struct Response{
    bool success;//indicates whether the download was successful
    string body;//stores the downloaded content
    int statuscode;//stores the HTTP status code of the response
    string finalurl;//stores the final URL after any redirects
    string contenttype;//stores the content type of the response - text/html application/json etc
};

class Downloader{
    public:
        Downloader();//creates a new downloader
        Response download(const string& url);//downloads the content of the given URL and returns a response object
}
```
* Internal Representation - //to be added

* Time Complexity - The downloader performs a single HTTP request the running time mainly depends on the network latency server response time and the size of the downloaded content . 

* keeping the downloading logic separate from the crawler follows 

### 5. PageStorage -

* The pagestorage is used for permanently storing all succesfully downloaded pages it stores the HTML of each page in a seperate text file and maintains the required metadata for fast lookup and retrieval it also provides the interface that will be used by the indexer to access the stored pages and their metadata

* Design -  A file based storage system is used instead of a database each downloaded page is stored in a seperate text file while an index.txt file stores the metadata of all pages during startup the metadata is loaded into memory to provide fast lookups without reading every page file. 

* Public APIs / Structure  - 
```cpp
//Structure to store a page
class Page{
    int id;//stores the unique id of the page
    string url;//stores the url of the page
    int depth;//stores the depth of the page from the seed url
    string html;//stores the html of the page
};

//Structure to store the metadata of a page used for fast lookups in Hashmap
struct Metadata{
    int id;//stores the unique id of the page
    int depth;//stores the depth of the page from the seed url
};

//Class to store all the pages and their metadata
class PageStorage{
    private:
        HashMap<string,Metadata> index;//stores the metadata of all pages
        string storagepath;//path to the storage directory
        int totalpages;//total no of pages stored
        void loadIndex();//rebuild hashmap from index.txt to continue crawling after a restart
    public:
        PageStorage();//creates a new pagestorage
        void storePage(const Page& page);//stores a new page in the storage
        Page getPage(const string& url) const;//retrieves a page from the storage
        bool hasPage(const string& url) const;//checks whether a page is present in the storage
        string getURLbyID(int id) const;//returns the url of a page by its id
        int pageCount() const;//returns the total no of pages stored
}
```
* Storage Format - 
```cpp
Every page is stored in a seperate text file - 

pages/
    1.txt
    2.txt
    3.txt
    ...
    Each page file stores - 
    ID: 1
    URL: http://example.com
    Depth: 0
    HTML: <html>...</html>
    
    The metadata of all pages is maintained in a single index.txt.
    like - 

    1|0|https://example.com
    2|1|https://example.com/about
    3|1|https://example.com/contact
    During startup only index.txt is read to rebuild the hashmap for fast lookups.
```

* Internal Representation - //to be added

* Time complexity - 
    * storepage() - Best - O(1),Average - O(1),Worst - O(n) (in case of hash collisions)
    * getpage() - Best - O(1),Average - O(1),Worst - O(n) (in case of hash collisions)
    * haspage() - Best - O(1),Average - O(1),Worst - O(n) (in case of hash collisions) 
    * getURLbyID() - Best - O(1),Average - O(1),Worst - O(n) (in case of hash collisions)
    * pagecount() - O(1)

* The project requires pages to be stored permanently and acceseed later by the indexer so a file based storage system is used to keep it simple and easier to debug and does not require any additional database setup keeping the metadata in both the pagefiles and index.txt makes every page file self contained while allowing the hashmap to be rebuilt from index.txt makes every page file self contained while allowing the hashmap to be rebuilt quickly without reading and parsing every stored page

### 6. Crawler -
*  The crawler controls the complete crawling process it starts fromt seed URL downloads pages extract links from them and normalizes URLs and coordinates all other components . It does not store data directly and instead uses the Frontier , SeenStore and PageStorage to store and retrieve data.

* Design - The crawler is implemented as a class that handles all taks itself . Responsibilities like duplicate detection page storage and frontier management are delegated to seperate components . This keeps the implementation of the crawler simple and makes each component easier to maintain and test

* Public APIs / Structure  - 
```cpp
class Crawler{
    private:
        Frontier frontier;//stores the urls waiting to be crawled
        SeenStore seen;//stores the urls that have been seen
        PageStorage pagestorage;//stores the downloaded pages and their metadata
        int maxdepth;//maximum depth to crawl
    public:
        Crawler(int maxdepth);//creates a new crawler with the specified max depth
        void crawl(const string& seedurl);//starts crawling from the seed url
    private:
        void processpage(const string& url,int depth);//downloads a page and processes it
};
```
* Internal Representation - //to be added

* Time complexity - The overall complexity depends upon the no of pages crawled and the no of links extracted from each page . The crawler performs constant time operations for Frontier , SeenStore and PageStorage on average while the total crawling time is affected by network .

* The crawler only coordinates the crawling process and assigns individual tasks to the responsible components . This keeps the design modular and follows the single responsibility principle . If any component changes in the future such as the storage implementation the crawler logic wont be affected by it 

### 7. HTML Parser - 
* used to extract useful information from the downloaded HTML page int his project it is mainly used to extract URLs from HTML docs The same parser can be extended in the next project to extract page text , titles and other HTML elements for indexing

* Design - A seperate HTML Parser class is created instead of immplementing the parsing logic inside the crawler direclty because it keeps the crawler simple and seperates HTML processing from crawling and allows the samer parser to be reused in the next projects without making changes to Crawler.

* Structure - 
```cpp
Class HTMLParser{
    public:
        HTMLParser();//creates a new HTML parser
        DynamicArray<string> extractLinks(const string& html,const string& baseurl);//extracts all the links from the html and returns them as a dynamic array of strings
    private:
        bool isValidURL(const string& url);//checks whether the url is valid and supported
    //methods like extracttext() and extractTitle() can be added in the next project to extract visible text and title from the html
};
```

* Working - The parser would scan the entire HTML doc and searches for anchor tags containing href attributes and every extracted URL is validated and returned to the crawler . Indexer can also use the same parser for extracting visible text and other HTML elements from the downloaded pages in the next project

* Time Complexity - O(n) where n is the length of the HTML doc since every character needs to be scanned to find all the links

* This project only requires link extraction but seperating the parser from the crawler keeps the design simple and modular also the same parser can later be reused by the indexer to extract text and other page information without duplicating the parsing logic in the indexer.

### 8. URL Normalizer  -
* it converts different forms of the same URL into a single standard format before they are processed by the crawler this helps to avoid duplicate crawling caused by different representations of the same page such as http://example.com and http://example.com/ or http://example.com#section

* Design - seperate URL normalizer class is used instead of placing the normalization logic inside the crawler this keeps URL processing independent of the crawling logic and allows the updating normalization rules in the future without affecting the crawler logic

* Public APIs / Structure - 
```cpp
class URLNormalizer{
    public:
        URLNormalizer();//creates a new URL normalizer
        string normalize(const string& url)//converts the URL into a standard format and returns it
        bool isvalid(const string& url);//checks whether the URL is valid
};
```
* Cases handled - 
    * Lowercase scheme and host
    * Trailing slash removal
    * URL fragments #section removal
    * Empty query removal
    * Convert relative URLs to absolute URLs
    * Duplicate slashes removal
    * mailto: and tel: and js links are ignored
    * empty and unsupported URLs are ignored

* Time Complexity - O(n) where n is the length of the URL since every character needs to be scanned to normalize the URL

* Seperating URL normalization from the crawler makes the implementation easier to maintain and extend

## Overall Flow - 
//an entire flow diagram

## Section 4 - Failure Handling
* The crawler handles common failures without stopping crawling invalid pages are skipped and the crawler continues with the remaining URLs in the frontier

* Failures -
    * Invalid URL - If a URL is invalid or unsupported it is skipped and not added to the frontier
    * Network Errors - If a page fails to download due to network errors it is skipped and the crawler continues with the remaining URLs in the frontier
    * Duplicate URLs - If a URL has already been seen it is skipped and not added to the frontier
    * Malformed HTML - If a page contains malformed HTML the parser skips the invalid parts and extracts the valid links from the rest of the page
    * Empty Page - If a page is empty it is skipped and the crawler continues with the remaining URLs in the frontier
    * Maximum Depth Reached - If a page is at the maximum depth it is not added to the frontier and the crawler continues with the remaining URLs in the frontier

## Section 5 - Future Compatibility


The Page storage is designed to be compatible with the Indexer in the next project without changing the implementation of this one every downloaded page is stored with its URL depth and HTML while index.txt maintains the metadata required for fast lookup

The following public methods will be used by the indexer - 

string getURLbyID(int id) const; - returns the URL of a page by its ID
string getPage(const string& url) const; - returns the HTML of a page by its URL

The indexer can iterate over all stored pages by calling pagecount() and getURLbyID() the returned URL is then used with getpage() to retrieve corresponding HTML and build the indexer

The HTML Parser is also designed for reuse in Indexer right now it only extracts links but in indexer it could be used to extract text and other HTML elements from the pages like titles and headings without duplicating the parsing logic in the indexer .