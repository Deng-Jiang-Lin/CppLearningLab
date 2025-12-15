
#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
/*
智能指针通过RAII(Resource Acquisition
IsInitialization)自动管理内存——在其生命周期结束时自动释放资源。 背景： 1、传统
new / delete 容易出现问题： 内存泄漏（没 delete）、 重复delete（野指针）、
    异常提前退出导致资源未释放、
    delete 后继续使用悬空指针

*/

int main() {
  // 1、独占指针：unique_ptr，独占所有权，离开作用域，自动释放
  {
    std::unique_ptr<int> ptr1(new int(100));
    auto ptr2 = std::make_unique<int>(); // 推荐使用make_unique的创建
    //打印指针指向的地址
    std::cout << "ptr2 Address ptr points to: " << ptr2.get()
              << std::endl; // 0x55ca054572d0
    std::cout << "ptr1 Address ptr points to: " << ptr1.get()
              << std::endl; // 0x55ca054572b0

    std::cout << *ptr2 << std::endl; // 0 默认值
    *ptr2 = 2;
    std::cout << *ptr2 << std::endl; // 2 修改后的值
    ptr1 = std::move(ptr2);          //把ptr2交给ptr1,ptr2->nullptr
    std::cout << "ptr1 Address ptr points to: " << ptr1.get()
              << std::endl; // 0x55ca054572d0 和ptr原始指向一样
  }
  // 2、引用计数智能指针：shared_ptr，当 use_count()
  // 变成0时，释放对象，如果没有指向对象，则其被释放，
  // 只要引用计数变为0，无论是否在作用域内，内存立即被释放！
  auto ptr3 = std::make_shared<int>(100);
  auto ptr4 = ptr3;
  std::cout << "use_count 引用数量:" << ptr3.use_count() << std::endl; // 输出 2
  ptr4.reset();
  ptr3.reset();
  std::cout << "use_count 引用数量:" << ptr3.use_count() << std::endl; // 输出 0
  std::cout << !ptr3 << std::endl; // 输出1,释放

  // weak_ptr 弱引用智能指针，解决shared_ptr循环引用问题
  struct Node {
    int value;
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev; // 使用weak_ptr避免循环引用
    Node(int val) : value(val), next(nullptr), prev() {}
  };
  {
    auto node1 = std::make_shared<Node>(1);
    auto node2 = std::make_shared<Node>(2);
    node1->next = node2;
    node2->prev = node1; // 使用weak_ptr避免循环引用

    std::cout << "Node2's previous value: " << node2->prev.lock()->value
              << std::endl; // 输出 1
  } // node1 和 node2 离开作用域，内存自动释放
  return 0;
}