#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
class Any {
public:
  Any() = default;

  template <typename T>
  Any(T value) : m_ptr(std::make_unique<Derived<T>>(std::move(value))) {}

  Any(const Any &) = delete;
  Any &operator=(const Any &) = delete;
  Any(Any &&) = default;
  Any &operator=(Any &&) = default;

  template <typename T> T cast_() const {
    //把基类转为派生类，从而获得派生类的数据
    auto *derived = dynamic_cast<Derived<T> *>(m_ptr.get());
    if (!derived) {
      throw std::bad_cast();
    }
    return derived->m_data;
  }

private:
  struct Base {
    virtual ~Base() = default;
  };

  template <typename T> struct Derived : Base {
    Derived(T value) : m_data(std::move(value)) {}
    T m_data;
  };

  std::unique_ptr<Base> m_ptr;
};
int main() {
  Any a = 42;
  Any b = std::string("你好");

  int x = a.cast_<int>();
  std::string y = b.cast_<std::string>();

  std::cout << x << std::endl;
  std::cout << y << std::endl;
}
