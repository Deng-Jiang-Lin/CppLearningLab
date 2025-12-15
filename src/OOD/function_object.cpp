//
// Created by 11041 on 2025/12/11.
//

#include <iostream>


template<typename T>
class MyGrater {
public:
    bool operator()(T a, T b) {
        return a > b;
    }

private:
    int _count = 0; // 成员变量，记录比较次数
};

template<typename T>
class MyLess {
public:
    bool operator()(T a, T b) {
        return a < b;
    }
};

/*高阶函数：接收比较策略

*/
template<typename T, typename Compare>
bool compare(T a, T b, Compare comp) {
    return comp(a, b);
}

int main() {
    std::cout << compare(10, 20, MyLess<int>()) << std::endl;
    std::cout << compare(10, 20, MyGrater<int>()) << std::endl;
    return 0;
}
