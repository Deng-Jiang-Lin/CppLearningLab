#include <cstddef>
#include <cstring>
#include <iostream>
#include <ostream>

/*
 *声明是指告诉编译器某个符号的存在，在程序变量表中记录类型和名字
 *定义则是指为该符号分配内存空间或实现其代码逻辑。
 */

/*
 *extern用于指示变量或函数的定义在另一个源文件中，并在当前源文件中声明。
 *说明该符号具有外部链接(external linkage)属性。
 *编译和链接期
 *在编译期：extern
 *用于告诉编译器某个变量或函数的定义在其他源文件中，
 *编译器会为它生成一个符号表项，并在当前源文件中建立一个对该符号的引用。

 *在链接期：链接器将多个目标文件合并成一个可执行文件，
 *并且在当前源文件中声明的符号，会在其它源文件中找到对应的定义，并将它们链接起来。
 */

/*
 *mutable
 *如果需要在const函数里面修改一些跟类状态无关的数据成员，
 *那么这个函数就应该被mutable来修饰，并且放在函数后后面关键字位置。
 */
extern int global_var;

class MyInt {
public:
  /*
   * 当将一个参数传递给构造函数时，如果构造函数声明中使用了 explicit
   * 关键字，则只能使用显式转换进行转换，而不能进行隐式转换。
   */
  explicit MyInt(int n) : num(n) {}

private:
  int num;
};

// static修饰全局变量可以将变量的作用域限定在当前文件中，使得其他文件无法访问该变量。
static int static_int = 10;

void foo() {
  static int count = 0; // static 修饰局部变量,运行完毕变量依旧存在
  count++;              // count ++
  std::cout << count << std::endl;
}

// static修饰函数可以将函数的作用域限定在当前文件中，使得其他文件无法访问该函数。
static void static_func() {}
void func(char (&str)[10]) {
  std::cout << "sizeof(str): " << sizeof(str) << std::endl;
}

struct AlignedStruct {
  char a;  // 本来1字节，padding 3 字节
  int b;   //  4 字节
  short c; // 本来 short 2字节，但是整体需要按照 4
           // 字节对齐(成员对齐边界最大的是int 4) ，
           //所以需要padding 2，总共: 4 + 4 + 4
  static int
      d; // static修饰类成员变量和函数,修饰类成员变量和函数可以使得它们在所有类对象中共享，且不需要创建对象就可以直接访问。
};
//当一个变量被声明为volatile时，编译器会禁止对该变量进行优化，以确保每次访问变量时都会从内存中读取其值，而不是从寄存器或缓存中读取。
volatile int volatile_int = -10;

int main() {
  // 前提: 64 位电脑上
  char str[] = "Hello World";
  char *p = str;
  double *dp;
  int n = 10;

  std::cout << "sizeof(char): " << sizeof(char)
            << std::endl; // 1 byte 语言层面的固定大小
  std::cout << "sizeof(str): " << sizeof(str)
            << std::endl; // 12 bytes 包含字符串结束符'\0'
  std::cout << "sizeof(p): " << sizeof(p) << std::endl; // 8 bytes 指针大小
  std::cout << "sizeof(double*): " << sizeof(dp)
            << std::endl; // 8 bytes 指针大小

  void *vp = malloc(100);
  std::cout << "sizeof(vp): " << sizeof(vp) << std::endl; // 8 bytes 指针大小

  /*
  sizeof  是一个 C++编译期间计算的操作符，用于计算数据类型或对象所占用的字节数。
  */
  int a = 42;
  std::cout << "Size of int: " << sizeof(int)
            << std::endl; // 输出 int 类型的大小
  std::cout << "Size of a: " << sizeof(a) << std::endl; // 输出变量 a 的大小
  std::cout << "Size of double: " << sizeof(double)
            << std::endl; // 输出 double 类型的大小

  /*strlen 是一个 C 标准库中的函数，用于计算 C
      风格字符串（以空字符 '\0'
     结尾的字符数组）的长度，即不包括结尾的空字符的字符个数。*/
  const char *cstr = "Hello, World!";
  std::cout << "Length of cstr: " << strlen(cstr)
            << std::endl; // 输出字符串的长度
  std::cout << "退化" << std::endl;
  char array[] = "Hello World";
  printf("sizeof=%lu\n", sizeof(array)); // 8bytes
  printf("strlen=%zu\n", strlen(array)); // 11

  // 指向只读变量的指针
  const int *ptr_1;
  int value = 10;
  const int const_value = 20; //常量
  ptr_1 = &value;
  // *ptr_1 = &const_value; 非法，可以改变指针的指向，不能改变指针指向的值

  //只读指针，指针本身是常量,可以修改指针指向的值，但是不能修改指针本身
  int *const ptr_2 = &value;
  *ptr_2 = 100;
  // ptr_2 = &value; 非法

  //只读指针指向只读变量
  const int *const ptr_3 = &a; //指针指向的数值和指针本身都不能修改
  // ptr_3 = &value;
  // *ptr_3 = 10;

  //常量引用：常量引用是指引用一个只读变量的引用，因此不能通过常量引用修改变量的值。
  const int &cvalve = const_value;
  // cvalve = 10;错误

  foo();
  foo();

  /* MyInt myInt = 10; // 注意，这段代码有两个步骤：
   1.int 类型的10先隐式类型转换为 MyInt 的一个临时对象
   2.隐式类型转换后的临时对象再通过复制构造函数生成 a
  */
}