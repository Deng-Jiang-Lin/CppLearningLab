#include <iostream>

// 普通比较器函数模板
template<typename T>
bool mgreater(T a, T b) {
    return a > b;
}

template<typename T>
bool mless(T a, T b) {
    return a < b;
}

// 高阶函数：接收比较策略
template<typename T, typename Compare>
bool compare(T a, T b, Compare comp) {
    return comp(a, b);  // 根据传入策略比较
}

int main() {
    constexpr int x = 10;
    constexpr int y = 20;

    // 使用 mgreater 比较
    if (compare(x, y, mgreater<int>)) {
        std::cout << "x > y\n";
    } else {
        std::cout << "x <= y\n";
    }

    // 使用 mless 比较
    if (compare(x, y, mless<int>)) {
        std::cout << "x < y\n";
    } else {
        std::cout << "x >= y\n";
    }

    return 0;
}
