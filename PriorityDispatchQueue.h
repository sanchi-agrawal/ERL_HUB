#ifndef PRIORITYDISPATCHQUEUE_H
#define PRIORITYDISPATCHQUEUE_H
#include <queue>
#include <vector>
#include <functional>

template <typename T>
class PriorityDispatchQueue {
    std::priority_queue<T,std::vector<T>,std::function<bool(const T&,const T&)>> queue;
public:
    explicit PriorityDispatchQueue(std::function<bool(const T&,const T&)> comparator = std::less<T>()) : queue(comparator) {}
    void push(const T& item){queue.push(item);}
    bool empty() const{return queue.empty();}
    T pop(){T t=queue.top();queue.pop();return t;}
    std::size_t size() const{return queue.size();}
};
#endif
