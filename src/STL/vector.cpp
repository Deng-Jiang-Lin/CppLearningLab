//
// Created by 11041 on 2025/12/11.
//

#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {};
    //预留空间
    vec.reserve(20); //开辟空间并且初始化
    //resize
    vec.resize(10); //改变大小，默认初始化为0
    std::cout << "capacity: " << vec.capacity() << std::endl; //容量
    std::cout << "size: " << vec.size() << std::endl; //元素个数
    //添加
    vec.push_back(111);
    vec.push_back(112);
    vec.push_back(114);
    vec.push_back(116);
    vec.push_back(117);
    vec.push_back(127);
    vec.push_back(217);
    vec.push_back(3467);

    //尺寸
    const std::size_t size = vec.size();
    //遍历
    for (int i = 0; i < size; ++i) {
        std::cout << vec[i] << std::endl;
    }
    //遍历
    for (const auto i: vec) {
        std::cout << i << std::endl;
    }
    //删除所有的偶数
    for (auto it = vec.begin(); it != vec.end();) {
        if (*it % 2 == 0) {
            it = vec.erase(it); //erase() 会返回指向“被删除元素之后”的迭代器
        } else {
            ++it;
        }
    }
    // //删除
    // vec.erase(
    //     std::remove_if(vec.begin(), vec.end(),
    //                    [](const int x) { return x % 2 == 0; }),
    //     vec.end()
    // );

    // 在每个偶数前插入一个比它小1的数
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        if (*it % 2 == 0) {
            it = vec.insert(it, *it - 1);
            ++it;
        }
    }
}
