#include <iostream>
using namespace std;

/*
 * ------------------------------------
 *  基类：包含一个虚函数 和 一个非虚函数
 * ------------------------------------
 */
class Base {
public:
    // 虚函数：允许子类 override，实现动态绑定（运行时多态）
    virtual void virtualFunc() {
        cout << "[Base] virtualFunc()" << endl;
    }

    // 非虚函数：不会被动态绑定，只会静态绑定
    void normalFunc() {
        cout << "[Base] normalFunc()" << endl;
    }

    //注意动态绑定
    virtual ~Base() {
        std::cout << "资源泄露" << std::endl;
    }; // 虚析构函数，防止内存泄漏
};

/*
 * ------------------------------------
 *  派生类：重写虚函数
 * ------------------------------------
 */
class Derived : public Base {
public:
    void virtualFunc() override {
        // override 可选，但更安全
        cout << "[Derived] virtualFunc()" << endl;
    }

    void normalFunc() {
        // 注意：这不是 override，只是隐藏（静态绑定）
        cout << "[Derived] normalFunc()" << endl;
    }
};


int main() {
    Base *p = new Derived(); // 基类指针指向派生类对象

    cout << "=== 动态绑定（virtual function） ===" << endl;
    p->virtualFunc(); // 调用 Derived::virtualFunc()
    // 因为 virtual → 动态绑定 → 查 vtable → 调用真实对象的方法

    cout << "\n=== 静态绑定（non-virtual function） ===" << endl;
    p->normalFunc(); // 调用 Base::normalFunc()
    // 普通函数 → 静态绑定 → 编译期就确定，不看对象真实类型

    delete p;
    return 0;
}
