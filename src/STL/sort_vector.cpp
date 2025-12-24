#include <algorithm>
#include <array>
#include <cstdlib>
#include <deque>
#include <forward_list>
#include <initializer_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <vector>

//
// Created by 11041 on 2025/12/11.
//
int main() {
  /*顺序容器*/
  /*1、动态数组
  数据结构：
    template <typename T, typename Alloc = std::allocator<T>>
    class vector {
    private:
        T* _start;        // 指向数组起始位置（已使用内存的首地址）
        T* _finish;       // 指向数组末尾（已使用内存的尾地址的下一位）
        T* _end_of_storage; // 指向分配的内存末尾（总容量的尾地址的下一位）
        Alloc _alloc;     // 内存分配器（默认std::allocator）
    };
    扩容步骤：
    分配新的更大内存（容量 = 原容量 × 扩容因子（1.5）；将原数组元素移动 /
    拷贝到新内存（C++11 后优先移动，避免拷贝开销）； 释放原内存； 更新
    _start/_finish/_end_of_storage 指向新内存
*/
  std::vector<int> v = {1, 2, 3, 4, 5, 6};
  v.push_back(1); //尾插  _finish构造新元素，然后_finish++
  v.emplace_back(2); //尾插（原地构造） _finish原地构造新元素，然后_finish++
  v.pop_back(); //尾删  _finish--，然后析构_finish指向的元素
  v.insert(v.begin(), 0); //插入元素  在指定位置构造新元素，然后将后续元素后移
  v.erase(v.begin()); //删除元素  将指定位置后续元素前移，然后析构最后一个元素
  v.clear(); //清空容器  析构所有元素，然后_finish = _start
  std::sort(v.begin(), v.end()); //排序
  for (const auto &e : v) {
    std::cout << e << " ";
  }
  // vector扩容，resize和reserve的区别
  v.reserve(10); //预留空间，改变容量不改变大小
  v.resize(8);   //改变大小，可能改变容量
  std::cout << "size: " << v.size() << ", capacity: " << v.capacity() << "\n";

  /* 2、双端队列
  一块连续的指针数组（map） + 多块固定大小的连续内存缓冲区（buffer）
        ┌───────┐
map --> │  ptr  │ ──▶ [ buffer0 ]
        ├───────┤
        │  ptr  │ ──▶ [ buffer1 ]
        ├───────┤
        │  ptr  │ ──▶ [ buffer2 ]
        ├───────┤
        │  ptr  │ ──▶ [ buffer3 ]
        └───────┘
    迭代器：
    struct deque_iterator {
    T* cur;        // 当前元素
    T* first;      // 当前 buffer 起始
    T* last;       // 当前 buffer 末尾
    T** node;      // 指向 map 中的某个指针
    };
    */
  std::deque<int> deque = {1, 3, 4, 5, 6, 7};
  deque.push_back(8);  //尾插 _finish构造新元素，然后_finish++
  deque.push_front(0); //头插 _start--，然后在_start构造新元素
  deque.pop_back();    //尾删 _finish--，然后析构_finish指向的元素
  deque.pop_front();   //头删 _start++，然后析构_start指向的元素
  deque.insert(deque.begin() + 1,
               2); //插入元素 在指定位置构造新元素，然后将后续元素后移
  deque.erase(deque.begin() + 1);        //删除元素
  deque.clear();                         //清空容器
  std::sort(deque.begin(), deque.end()); //排序
  for (const auto &e : deque) {
    std::cout << e << " ";
  }
  /* 3、双向循环链表
    数据结构：
        template <typename T, typename Alloc = std::allocator<T>>
        class list {
        private:
            struct Node {
                T data;         // 节点数据
                Node* prev;     // 指向前驱节点
                Node* next;     // 指向后继节点
            };
            Node* head;         // 指向链表头节点
            Node* tail;         // 指向链表尾节点
            size_t size;        // 链表大小
            Alloc _alloc;       // 内存分配器（默认std::allocator）
        };
        迭代器：
        struct list_iterator {
            Node* node; // 指向当前节点
        };
  */
  std::list<int> lst = {1, 2, 34, 5, 6, 7};
  lst.push_back(8);  //尾插 在尾节点后插入新节点
  lst.push_front(0); //头插 在头节点前插入新节点
  lst.pop_back();    //尾删 删除尾节点
  lst.pop_front();   //头删 删除头节点
  lst.insert(++lst.begin(),
             2);            //插入元素 在指定位置前插入新节点
  lst.erase(++lst.begin()); //删除元素 删除指定位置节点
  lst.clear();              //清空容器 删除所有节点
  lst.sort();               //排序
  for (const auto &e : lst) {
    std::cout << e << " ";
  }

  /* 4、单向链表
    数据结构：
        template <typename T, typename Alloc = std::allocator<T>>
        class forward_list {
        private:
            struct Node {
                T data;         // 节点数据
                Node* next;     // 指向后继节点
            };
            Node* head;         // 指向链表头节点
            size_t size;        // 链表大小
            Alloc _alloc;       // 内存分配器（默认std::allocator）
        };
        迭代器：
        struct forward_list_iterator {
            Node* node; // 指向当前节点
        };
  */
  std::forward_list<int> flist = {1, 2, 3, 4, 5};
  flist.push_front(0); //头插 在头节点前插入新节点
  flist.pop_front();   //头删 删除头节点
  flist.insert_after(flist.begin(),
                     1); //插入元素 在指定位置后插入新节点
  flist.erase_after(flist.begin()); //删除元素 删除指定位置后节点
  flist.clear();                    //清空容器 删除所有节点
  flist.sort();                     //排序
  for (const auto &e : flist) {
    std::cout << e << " ";
  }

  /* 4、定长数组
    数据结构：
        template <typename T, std::size_t N>
        class array {
        private:
            T elems[N]; // 固定大小的数组
        };
  */
  std::array<int, 5> arr = {1, 2, 3, 4, 5};
  std::sort(arr.begin(), arr.end()); //排序
  for (const auto &e : arr) {
    std::cout << e << " ";
  }
  /*关联式容器（有序）*/
  /*    1、set
    底层数据结构：红黑树（self-balancing binary search tree）
    特点：自动排序，set不允许重复元素，multiset允许重复元素
    操作复杂度：插入、删除、查找 O(log n)
        struct rb_tree_node {
        rb_tree_node* parent;
        rb_tree_node* left;
        rb_tree_node* right;
        Color color;   // 红 / 黑
        Key value;
};
  */
  std::set<int> s = {5, 3, 4, 1, 2};
  s.insert(3); //插入元素（重复元素无效）
  s.erase(4);  //删除元素
  auto it = s.find(3);
  if (it != s.end()) {
    std::cout << "found";
  }

  /*2、multiset
    底层数据结构：红黑树（self-balancing binary search tree）
    特点：自动排序，允许重复元素
    操作复杂度：插入、删除、查找 O(log n)
  */
  std::multiset<int> ms = {5, 3, 4, 1, 2, 2, 3};
  /*3、map
    底层数据结构：红黑树（self-balancing binary search tree）
    特点：自动排序，key唯一，value可重复，multimap允许key重复
    操作复杂度：插入、删除、查找 O(log n)
  */
  std::map<int, std::string> m = {
      {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}, {5, "five"}};

  /*4、multimap
  底层数据结构：红黑树（self-balancing binary search tree）
  特点：自动排序，key可重复，value可重复
  操作复杂度：插入、删除、查找 O(log n)
  */
  std::multimap<int, std::string> mm = {
      {1, "one"}, {2, "two"}, {2, "deux"}, {3, "three"}, {4, "four"}};

  /*无序关联式容器*/
  /*1、unordered_set
  底层数据结构：哈希表（hash table）
  特点：无序，set不允许重复元素，unordered_multiset允许重复元素
  操作复杂度：插入、删除、查找 O(1) 平均，O(n) 最坏
  unordered_set
│
├── bucket_count
├── size
├── hash_function
├── key_equal
│
└── buckets  ----->  [ 0 ] -> node -> node
                     [ 1 ] -> nullptr
                     [ 2 ] -> node
                     [ 3 ] -> node -> node -> node
                     [ 4 ] -> nullptr
                     [ ... ]

  */
  std::unordered_set<int> us = {5, 3, 4, 1, 2, 2, 3};

  /* 2、unordered_multiset
    底层数据结构：哈希表（hash table）
    特点：无序，允许重复元素
    操作复杂度：插入、删除、查找 O(1) 平均，O(n) 最坏
    */
  std::unordered_multiset<int> ums = {5, 3, 4, 1, 2, 2, 3};

  return 0;
}
