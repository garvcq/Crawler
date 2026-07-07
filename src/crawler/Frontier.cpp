#include "crawler/Frontier.h"


 void Frontier::enqueue(const URLDepth& urlDepth)
{
    queue.enqueue(urlDepth);
}

 URLDepth Frontier::dequeue()
{
    return queue.dequeue();
}

 URLDepth& Frontier::front()
{
    return queue.front();
}

 const URLDepth& Frontier::front() const
{
    return queue.front();
}

 bool Frontier::empty() const
{
    return queue.empty();
}

 int Frontier::size() const
{
    return queue.size();
}

 void Frontier::clear()
{
    queue.clear();
}
