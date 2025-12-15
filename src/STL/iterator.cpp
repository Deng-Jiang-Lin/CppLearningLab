#include <iostream>
#include <vector>

/*
 * 演示四种迭代器类型：
 * 1. 普通迭代器 (iterator)
 * 2. 常量迭代器 (const_iterator)
 * 3. 反向迭代器 (reverse_iterator)
 * 4. 常量反向迭代器 (const_reverse_iterator)
 *
 * 同时展示：vector 在中间插入元素会导致迭代器失效
 */

template <typename T> bool contains(const std::vector<T> &vec, const T &value) {
  for (auto it = vec.cbegin(); it != vec.cend(); ++it) {
    if (*it == value) {
      return true;
    }
  }
  return false;
}
// 通过函数指针调用函数，无法内联，效率较低，因为具有调用开销
int main() {
  std::vector<int> vec = {1, 2, 3, 4, 5};

  std::cout << "=== 普通迭代器遍历，并在访问到 3 时插入 ===\n";

  for (auto it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it << std::endl;

    if (*it == 3) {
      // 在当前位置插入 10
      // 注意：insert 会导致迭代器 it 失效
      it = vec.insert(it, 10);

      // 修改插入后的元素
      *it = 100;

      // 跳出循环避免使用失效的迭代器
      break;
    }
  }

  std::cout << "\n=== 插入后整个 vector 的内容 ===\n";
  for (auto x : vec)
    std::cout << x << " ";
  std::cout << std::endl;

  return 0;
}
