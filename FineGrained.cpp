#include <iostream>

#include "FineGrained.h"

void FineGrainedQueue::push_back(const int value)
{
    // создаем новый узел
    Node* node = new Node(value);
    std::unique_lock<std::shared_mutex> guard(mutex);

    // если список пуст, возвращаем узел
    if (head == nullptr)
    {
        head = node;
        // пробуждаем один ожидающий поток c вызовом pop(), если таковые имеются
        data_cond.notify_one();
        return;
    }

    // в цикле ищем последний элемент списка
    Node* last = head;
    while (last->next != nullptr)
    {
        last = last->next;
    }

    // Обновляем указатель next последнего узла на указатель на новый узел
    last->next = node;
    // пробуждаем один ожидающий поток c вызовом pop(), если таковые имеются
    data_cond.notify_one();
}

void FineGrainedQueue::push_front(const int value)
{
    Node* node = new Node(value);
    std::unique_lock<std::shared_mutex> guard(mutex);
    node->next = head;
    head = node;
    // пробуждаем один ожидающий поток c вызовом pop(), если таковые имеются
    data_cond.notify_one();
}

void FineGrainedQueue::pop(const int value)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    // дожидаемся, пока в очередь добавят элемент
    data_cond.wait(lock, [this] { return head != nullptr; });

    Node* temp = head;
    Node* prev = nullptr;
    
    // крайний случай удаления начала списка
    if (temp && temp->value == value)
    {
        head = temp->next;
        delete temp;
        return;
    }

    // идем по списку, пока не найдем узел со значением данных, равных ключу
    while (temp && temp->value != value)
    {
        prev = temp;
        temp = temp->next;
    }

    // если узел не найден, возвращаем
    if (!temp)
        return;
    // меняем указатель следующего узла для предыдущего узла на узел,
    // следующий за удаляемым узлом, и удаляем узел с данными
    prev->next = temp->next;
    delete temp;
}

auto FineGrainedQueue::size()
{
    std::shared_lock<std::shared_mutex>(mutex);
    auto result = 0;
    Node* last = head;

    while (last->next != nullptr)
    {
        ++result;
    }
    return result;
}

void FineGrainedQueue::remove(const int value)
{
    Node* prev = nullptr;
    Node* cur = nullptr;
    Node* old_prev = nullptr;

    queue_mutex->lock();
    // здесь должна быть обработка случая пустого списка

    prev = this->head;
    cur = this->head->next;

    prev->node_mutex->lock();
    // здесь должна быть обработка случая удаления первого элемента
    queue_mutex->unlock();

    if (cur) // проверили и только потом залочили
        cur->node_mutex->lock();
    while (cur)
    {
        if (cur->value == value)
        {
            prev->next = cur->next;
            prev->node_mutex->unlock();
            cur->node_mutex->unlock();
            delete cur;
            return;
        }
        old_prev = prev;
        prev = cur;
        cur = cur->next;
        old_prev->node_mutex->unlock();
        if (cur) // проверили и только потом залочили
            cur->node_mutex->lock();
    }
    prev->node_mutex->unlock();
}

void FineGrainedQueue::insertIntoMiddle(const int value, const int pos)
{
    // создаем новый узел 
    Node* newNode = new Node(value);

    //if (head == nullptr)
    //{
    //    // если список пуст, новый узел и будет началом списка
    //    head = newNode;
    //    return;
    //}

    //if (pos == 0)
    //{
    //    // крайний случай - вставка  в начало списка
    //    newNode->next = head;
    //    head = newNode;
    //    return;
    //}

    int currPos = 0;
    Node* next_ = nullptr;
    Node* current = nullptr;

    queue_mutex->lock();
    current = this->head;
    current->node_mutex->lock();
    queue_mutex->unlock();

    // в цикле идем по списку, пока список не кончится, или пока не дойдем до позиции
    while (currPos < pos - 1 && nullptr != current->next)
    {
        next_ = current->next;
        next_->node_mutex->lock();
        current->node_mutex->unlock();
        current = next_;
        currPos++;
    }
    // меняем указатель -> следующий узел на указатель -> новый узел
    next_ = current->next;
    current->next = newNode;
    // связываем список обратно
    newNode->next = next_;
    current->node_mutex->unlock();
}

void FineGrainedQueue::show()
{
    std::unique_lock<std::shared_mutex> guard(mutex);
    if (head == nullptr)
    {
        return;
    }
    Node* current = head;
    while (nullptr != current)
    {
        std::cout << current->value << ", ";
        current = current->next;
    }
}