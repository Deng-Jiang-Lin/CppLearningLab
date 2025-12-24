#include <iostream>
/*
 *类的析构顺序（上->下）和构造顺序（下-上）完全相反
 *在C++中，基类的析构函数需要定义为虚函数，以确保在通过基类指针或引用删除派生类对象时，
 *能够正确地调用派生类的析构函数，否则派生类的析构函数不会被调用，这部分资源也就并无法被释放。
 *将基类的析构函数定义为虚函数后，C++运行时会自动识别指向的对象的实际类型，并确保调用正确的派生类析构函数。
 *当派生类析构函数执行完毕后，基类的析构函数也会自动调用，以确保对象完全销毁。
 */

class Base {
public:
  Base() { std::cout << "Base constructor" << std::endl; }
  ~Base() { std::cout << "Base destructor" << std::endl; }
};

class Base1 {
public:
  Base1() { std::cout << "Base1 constructor" << std::endl; }
  ~Base1() { std::cout << "Base1 destructor" << std::endl; }
};

class Base2 {
public:
  Base2() { std::cout << "Base2 constructor" << std::endl; }
  ~Base2() { std::cout << "Base2 destructor" << std::endl; }
};

class Base3 {
public:
  Base3() { std::cout << "Base3 constructor" << std::endl; }
  ~Base3() { std::cout << "Base3 destructor" << std::endl; }
};

class MyClass : public virtual Base3, public Base1, public virtual Base2 {
public:
  MyClass() : num1(1), num2(2) {
    std::cout << "MyClass constructor" << std::endl;
  }
  ~MyClass() { std::cout << "MyClass destructor" << std::endl; }

private:
  int num1;
  int num2;
  // 这个是为了看成员变量的初始化顺序
  Base base;
};

int main() {
  MyClass obj;
  return 0;
}