#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <iostream>
#include <cmath>
#include <QList>
#include <QHash>
#include <QString>

using namespace std;

// Custom Min-Heap Priority Queue implemented from scratch (No STL <queue> or std::priority_queue)
template <typename T, typename PriorityType = double>
struct PriorityItem {
    T value;
    PriorityType priority;

    bool operator<(const PriorityItem& other) const {
        return priority < other.priority;
    }
    bool operator>(const PriorityItem& other) const {
        return priority > other.priority;
    }
};

template <typename T, typename PriorityType = double>
class MinHeap {
private:
    QList<PriorityItem<T, PriorityType>> heap;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index].priority < heap[parent].priority) {
                PriorityItem<T, PriorityType> temp = heap[index];
                heap[index] = heap[parent];
                heap[parent] = temp;
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int index) {
        int size = heap.size();
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int smallest = index;

            if (left < size && heap[left].priority < heap[smallest].priority) {
                smallest = left;
            }
            if (right < size && heap[right].priority < heap[smallest].priority) {
                smallest = right;
            }
            if (smallest != index) {
                PriorityItem<T, PriorityType> temp = heap[index];
                heap[index] = heap[smallest];
                heap[smallest] = temp;
                index = smallest;
            } else {
                break;
            }
        }
    }

public:
    MinHeap() = default;

    void push(const T& value, PriorityType priority) {
        PriorityItem<T, PriorityType> item;
        item.value = value;
        item.priority = priority;
        heap.append(item);
        heapifyUp(heap.size() - 1);
    }

    PriorityItem<T, PriorityType> top() const {
        if (!heap.isEmpty()) {
            return heap.first();
        }
        PriorityItem<T, PriorityType> emptyItem;
        return emptyItem;
    }

    void pop() {
        if (heap.isEmpty()) return;
        heap[0] = heap.last();
        heap.removeLast();
        if (!heap.isEmpty()) {
            heapifyDown(0);
        }
    }

    bool empty() const {
        return heap.isEmpty();
    }

    int size() const {
        return heap.size();
    }

    void clear() {
        heap.clear();
    }
};

#endif // DATASTRUCTURES_H
