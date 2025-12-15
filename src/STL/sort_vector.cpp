#include <cstdlib>
#include <iostream>
#include <map>
#include <unordered_set>
//
// Created by 11041 on 2025/12/11.
//
int main() {
    //1.顺序容器
    std::unordered_set<int> set1;
    for (int i = 0; i < 10; i++) {
        set1.insert(rand() % 20 + 1);
    }
    for (const auto &item: set1) {
        std::cout << item << " ";
    }
    std::cout << set1.size() << std::endl; // 输出集合大小
    std::cout << set1.count(15) << std::endl; // 统计元素15的个数
    set1.clear();
    set1.erase(5); // 删除元素5
    //2.无序容器
    std::map<int, std::string> m;
    m.insert(std::make_pair(5, "hello"));
    m[10] = "world";
    for (const auto &[fst, snd]: m) {
        std::cout << fst << " " << snd << std::endl;

        //3.关联容器
        return 0;
    };
}
