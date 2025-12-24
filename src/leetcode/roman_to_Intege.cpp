// 1. 补充必要的头文件（缺一不可）
#include <iostream> // 用于 std::cout、std::endl
#include <sstream>  // 用于 std::istringstream
#include <string>   // 用于 std::string
#include <unordered_map>
#include <vector> // 用于 std::vector

int main() {
  std::string pattern = "abba";
  std::string s = "dog cat dog cat";
  std::unordered_map<std::string, int> smap = {
      {"Alice", 95}, {"Bob", 88}, {"Charlie", 76}};

  for (const auto &[ch, val] : smap) {
    std::cout << ch << ":" << val << std::endl;
  };

  return 0;
}