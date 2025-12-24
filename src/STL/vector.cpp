//
// Created by 11041 on 2025/12/11.
//

#include <algorithm>
#include <iostream>
#include <vector>
int mySqrt(int x) {
  //判断0
  if (x == 0) {
    return 0;
  }
  int left = 1;
  int right = x;
  int ans = 0; // 存储结果;
  while (left < right) {
    int mid = left + (right - left) / 2;
    if (mid <= x / mid) {
      ans = mid;
      left = mid + 1;
    } else {
      right = mid - 1;
    } 
  }
  return ans;
}
int main() {
  std::vector<int> vec = {};
  //预留空间
  vec.reserve(20); //开辟空间并且初始化
  // resize
  vec.resize(10); //改变大小，默认初始化为0
  std::cout << "capacity: " << vec.capacity() << std::endl; //容量
  std::cout << "size: " << vec.size() << std::endl;         //元素个数
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
  for (const auto i : vec) {
    std::cout << i << std::endl;
  }
  //删除所有的偶数
  for (auto it = vec.begin(); it != vec.end();) {
    if (*it % 2 == 0) {
      it = vec.erase(it); // erase() 会返回指向“被删除元素之后”的迭代器
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
  /*
  reserve 只分配内存，不创建对象；
  resize 会改变元素个数，并构造或销毁元素。
  */
  std::vector<int> v = {1, 2, 3};
  v.push_back(4);
  v.push_back(5);
  v.push_back(4);
  v.push_back(7); //自动扩容x2，重新分配 + 拷贝
                  //   v.resize(3);    //缩小size，不改变capacity
  v.reserve(3);
  std::cout << "vector v size: " << v.size() << std::endl;
  std::cout << "vector v capacity: " << v.capacity() << std::endl;
  int x = 8;
  int k = x / 2;

}
