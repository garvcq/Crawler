#pragma once
#include "Queue.h"
#include "crawler/URLDepth.h"

class Frontier{
    private:
        Queue<URLDepth> queue;
    public:
        void enqueue(const URLDepth& urldepth);
        URLDepth dequeue();
        URLDepth& front();
        const URLDepth& front()const;
        bool empty()const;
        int size()const;
        void clear();
};