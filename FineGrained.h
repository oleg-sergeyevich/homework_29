#pragma once

#include <mutex>
#include <shared_mutex>
#include <condition_variable>

class Node
{
public:
    Node(int data) : value(data), next(nullptr), node_mutex(new std::mutex) {}
    int value = 0;
	Node* next = nullptr;
	std::mutex* node_mutex;
};


class FineGrainedQueue
{
public:
    FineGrainedQueue() : head(nullptr), queue_mutex(new std::mutex) {}

    ~FineGrainedQueue() = default;

    void push_back(const int value);

    void push_front(const int value);

    void pop(const int value);

    auto size();

    void remove(const int value);

    void insertIntoMiddle(const int value, const int pos);

    void show();

private:
	Node* head = nullptr;
	std::mutex* queue_mutex;
    std::shared_mutex mutex;
    std::condition_variable_any data_cond;
};