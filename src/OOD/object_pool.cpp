#include <iostream>
#include <stdexcept>

template<typename T>
class Queue {
public:
    struct Node {
        // 数据 + 指针
        T data;
        Node *next;

        explicit Node(const T &d) : data(d), next(nullptr) {
        }

        // 静态对象池指针
        static Node *freeList;
        static constexpr int POOL_ITEM_SIZE = 10000;

        // 重载 new：从对象池获取节点
        void *operator new(const size_t size) {
            if (freeList == nullptr) {
                // 分配一大块内存
                Node *block = reinterpret_cast<Node *>(new char[size * POOL_ITEM_SIZE]);

                // 串成自由链表
                for (int i = 0; i < POOL_ITEM_SIZE - 1; ++i)
                    block[i].next = &block[i + 1];
                block[POOL_ITEM_SIZE - 1].next = nullptr;

                freeList = block;
            }

            // 从池头取一个节点
            Node *node = freeList;
            freeList = freeList->next;
            return node;
        }

        // 重载 delete：归还节点到对象池
        void operator delete(void *ptr) {
            Node *node = static_cast<Node *>(ptr);
            node->next = freeList;
            freeList = node;
        }
    };

    // 构造：创建哨兵节点
    Queue() {
        head = tail = new Node(T());
    }

    // 析构：删除所有节点
    ~Queue() {
        while (!empty()) dequeue();
        delete head;
    }

    // 入队：尾插
    void enqueue(const T &data) {
        Node *node = new Node(data);
        tail->next = node;
        tail = node;
    }

    // 出队：头删
    void dequeue() {
        if (empty()) throw std::out_of_range("Queue is empty");

        Node *first = head->next;
        head->next = first->next;

        if (first == tail)
            tail = head;

        delete first;
    }

    // 返回队头数据
    [[nodiscard]] T front() const {
        if (empty()) throw std::out_of_range("Queue is empty");
        return head->next->data;
    }

    // 判空
    [[nodiscard]] bool empty() const {
        return head == tail;
    }

private:
    Node *head; // 哨兵节点
    Node *tail; // 最后一个真实节点
};

template<typename T>
typename Queue<T>::Node *Queue<T>::Node::freeList = nullptr;

int main() {
    Queue<int> q;
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);

    while (!q.empty()) {
        std::cout << q.front() << " ";
        q.dequeue();
    }
}
