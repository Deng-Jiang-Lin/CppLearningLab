///赋值运算符
#include <cstring>
#include <iostream>
class MyString {
public:
  MyString(const char *str = nullptr) {
    if (str == nullptr) {
      m_string = new char[1];
      m_string[0] = '\0';
    } else {
      m_string = new char[strlen(str) + 1];
      strcpy(m_string, str);
    }
  }

  ~MyString() {
    delete[] m_string;
    m_string = nullptr;
  };

  MyString &operator=(const MyString &other) {
    if (this == &other) {
      return *this;
    }
    delete[] m_string;
    m_string = new char[strlen(other.m_string) + 1];
    strcpy(m_string, other.m_string);
    return *this;
  };

private:
  char *m_string;
};

int main() {
  char str1[] = "Hello World";
  MyString str2 = str1; // 调用拷贝构造函数（深拷贝）

  // 测试赋值运算符
  MyString str3("C++ is great");
  str2 = str3; // 调用赋值运算符，覆写str2的内容
  return 0;
}