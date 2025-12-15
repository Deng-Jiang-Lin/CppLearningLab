#include <iostream>

class A {
public:
    virtual ~A() = default;
    virtual void print() =0; // 纯虚函数
};


class B : public A {
public:
    void print() override {
        std::cout << "我是B" << std::endl;
    };
};

class C : public A {
public:
    void print() override {
        std::cout << "我是C" << std::endl;
    };

    void printt() {
        std::cout << "我是C2" << std::endl;
    }
};

void print(A &a) {
    if (auto *c = dynamic_cast<C *>(&a)) {
        c->printt();
    }
    a.print();
}

int main() {
    B b;
    C c;
    print(b);
    print(c);
    return 0;
}
