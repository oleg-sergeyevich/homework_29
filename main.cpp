#include <vector>
#include <thread>
#include <cstdlib>
#include <iostream>

#include "FineGrained.h"

int main()
{
    FineGrainedQueue queue;
    std::vector<std::thread> threads;
    srand(0);
    int random = 0;

    for (size_t i = 0; i < 10; i++)
    {
        random = rand() % 100;
        threads.emplace_back( [=, &queue]() {queue.push_back(random); } );
    }
    for (auto& t : threads)
    {
        if (t.joinable())
            t.join();
    }
    queue.show();

    threads.emplace_back([=, &queue]() {queue.insertIntoMiddle(100, 9); });
    threads.emplace_back([=, &queue]() {queue.push_back(111); });
    threads.emplace_back([=, &queue]() {queue.insertIntoMiddle(200, 15); });
    threads.emplace_back([=, &queue]() {queue.push_back(222); });
    threads.emplace_back([=, &queue]() {queue.insertIntoMiddle(300, 5); });
    for (auto& t : threads)
    {
        if (t.joinable())
            t.join();
    }
    std::cout << std::endl;
    queue.show();

    return 0;
}