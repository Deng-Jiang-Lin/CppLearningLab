#include <functional>
#include <iostream>
#include <vector>
//不合理版本
// std::string
// printUpper(const std::function<std::string(const std::string &)> &upper,
//            std::string mstring) {
//   return upper(mstring);
// }

//字符串需要常量引用
std::string transformString(
    const std::function<std::string(const std::string &)> &transformer,
    const std::string &input) { // 改为常量引用，避免拷贝
  return transformer(input);
}

int main() {
  int x = 1;
  int y = 2;

  //一、参数传递
  auto add =
      ([](int ix, int iy) { std::cout << "sum is :" << ix + iy << std::endl; });
  add(4, 3);

  //二、捕获外部值变量
  auto add2 = ([x, y](int ix, int iy) {
    std::cout << "sum is :" << ix + iy + x + y << std::endl;
  });
  add2(3, 5);
  //三、捕获外部引用变量
  auto add3 = ([&x](int iy) { x += iy; });
  add3(3);
  std::cout << "new x is :" << x << std::endl;

  auto add4 = ([&]() {
    x += 1;
    y += 1;
  });
  add3(300);
  std::cout << "new x is :" << x << std::endl;
  std::cout << "new y is :" << y << std::endl;

  //四、使用 mutable 可以修改值捕获的变量
  auto add5 = ([x]() mutable {
    x = 20; // 可以修改，但只是修改副本
    std::cout << "内部: x = " << x << std::endl;
  });
  add5();
  std::cout << "new x is :" << x << std::endl; // new x is :304

  // 5、lamba作为函数参数
  // 把string全部转为大写
  auto upperTrans = [](std::string &mstring) {
    for (auto &c : mstring) {
      c = std::toupper(static_cast<unsigned char>(c));
    }
  };
  std::string text = "hello";
  upperTrans(text);
  std::cout << "new text is :" << text << std::endl;

  // 6、使用模板 main 内部使用template
  // template <typename Func>

  // void for_each(const std::vector<int> &vec, Func func) {
  //   for (int val : vec) {
  //     func(val);
  //   }
  // }

  // std::vector<int> vec = {1, 2, 3, 4, 5, 6, 10};
  // int sum2 = 0;
  // for_each(vec, [&sum2](int x) { sum2 += x * x; });
  // std::cout << "平方和: " << sum2 << std::endl;
  return 0;
}