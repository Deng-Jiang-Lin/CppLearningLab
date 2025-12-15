#include <iostream>
#include <new>      // std::bad_alloc, std::nothrow_t, ::operator new
#include <cstdlib>  // malloc, free
#include <cstring>  // memcpy
using namespace std;

// 一个用于演示的类（有构造与析构）
// 我们通过输出观察构造/析构何时发生
class Demo {
public:
    Demo(int v = 0) : value(v) {
        cout << "Demo ctor: " << value << " at " << this << endl;
    }
    ~Demo() {
        cout << "Demo dtor: " << value << " at " << this << endl;
    }

    void show() const { cout << "Demo::show value=" << value << " at " << this << endl; }
private:
    int value;
};

// ======= 1) new 的等价流程（高层演示） =======
// new T(args) 大致等价于两步：
//  A. operator new(sizeof(T)) 分配足够的原始内存（如果失败抛 std::bad_alloc）
//  B. 在返回的内存上调用构造函数（placement new）
//
// 下面是用代码手动执行这两步得到的效果。
void demo_new_delete_equiv() {
    cout << "\n--- demo_new_delete_equiv ---\n";

    // 1. 分配原始内存（类似 operator new）
    void* raw = ::operator new(sizeof(Demo)); // 若分配失败，会抛 std::bad_alloc

    // 2. 在 raw 指向的位置构造对象（placement new）
    Demo* p = new (raw) Demo(42); // 注意：这不是分配内存的 new，而是 placement new（仅调用 ctor）

    // 使用对象
    p->show();

    // 3. 手动析构（因为我们用了 placement new）
    p->~Demo(); // 调用析构函数（必须手动调用）

    // 4. 释放内存（类似 operator delete）
    ::operator delete(raw);
    // 如果用的是 malloc/free，这里要用 free(raw)；但标准 operator delete 通常与 operator new 配对
}

// ======= 2) new 抛出与 nothrow 版本 =======
void demo_new_throw_vs_nothrow() {
    cout << "\n--- demo_new_throw_vs_nothrow ---\n";

    try {
        // 普通 new：分配失败会抛出 std::bad_alloc
        Demo* p1 = new Demo(1); // 分配+构造，通常成功
        delete p1;               // delete 自动调用析构并释放内存
    } catch (const bad_alloc& e) {
        cout << "alloc failed: " << e.what() << endl;
    }

    // 使用 nothrow：失败时返回 nullptr，而不是抛异常
    Demo* p2 = new (nothrow) Demo(2); // 如果内存不足，p2 == nullptr
    if (p2) {
        p2->show();
        delete p2;
    } else {
        cout << "nothrow new returned nullptr\n";
    }
}

// ======= 3) new[] / delete[]（数组 new） =======
// new[] 会对每个元素调用构造，delete[] 会对每个元素调用析构。
// 编译器通常在分配的内存前面保存元素个数以便 delete[] 时知道要调用多少次析构。
// 你不要把 new 与 delete[] 混用，务必成对使用。
void demo_new_array() {
    cout << "\n--- demo_new_array ---\n";

    // 使用 new[]：会为 3 个 Demo 分配空间并逐个构造
    Demo* arr = new Demo[3]{10, 20, 30}; // aggregate 初始化每个元素的参数
    arr[0].show();
    arr[1].show();
    arr[2].show();

    // 必须使用 delete[] 来析构并释放
    delete[] arr;
}

// ======= 4) placement new 的典型用途（内存池 / vector 内部） =======
// 先分配一块大内存，然后逐个 placement-new 对象，最后逐个析构并释放内存
void demo_placement_new_pool() {
    cout << "\n--- demo_placement_new_pool ---\n";

    // 模拟分配一块原始字节缓冲区（用于存放 5 个 Demo）
    size_t N = 5;
    void* buffer = std::malloc(sizeof(Demo) * N); // malloc 只分配字节，不构造对象

    Demo* base = static_cast<Demo*>(buffer);

    // 在 buffer 上逐个构造 Demo（placement new）
    for (size_t i = 0; i < N; ++i) {
        new (base + i) Demo(static_cast<int>(i + 100));
    }

    // 使用它们
    for (size_t i = 0; i < N; ++i) {
        base[i].show();
    }

    // 必须逐个调用析构
    for (size_t i = 0; i < N; ++i) {
        base[i].~Demo();
    }

    // 释放原始内存
    std::free(buffer);
}

// ======= 5) operator new / operator delete 的自定义实现示例 =======
// 你可以重载全局 operator new/delete（不推荐在库中乱改），常见用于记录、替换分配器等。
// 下面演示一个简单的“包装”实现（仅示范，不用于生产）。
void* my_operator_new(size_t size) {
    cout << "[my_operator_new] requesting " << size << " bytes\n";
    void* p = std::malloc(size);
    if (!p) throw bad_alloc(); // 与全局 operator new 的行为一致
    return p;
}
void my_operator_delete(void* p) noexcept {
    cout << "[my_operator_delete] free " << p << "\n";
    std::free(p);
}

// 使用自定义 operator new/delete 的演示（局部作用）
// 注意：真正重载全局 operator new/delete 需要 careful 配置，这里仅演示调用 ::operator new 的替代写法
void demo_custom_operator_new() {
    cout << "\n--- demo_custom_operator_new ---\n";
    // 手动调用我们的分配器函数（模拟 operator new）
    void* mem = my_operator_new(sizeof(Demo));
    Demo* p = new (mem) Demo(999); // placement new 在我们分配的内存上构造
    p->show();
    p->~Demo(); // 析构
    my_operator_delete(mem); // 释放内存
}

// ======= 6) 常见错误与注意事项（代码里演示安全与错误对比） =======
void demo_common_errors() {
    cout << "\n--- demo_common_errors ---\n";

    // 错误示范：malloc 分配但直接当作已构造对象使用（未调用构造）
    void* mem = std::malloc(sizeof(Demo));
    // Demo* p_bad = static_cast<Demo*>(mem);
    // p_bad->show(); // UB：对象未构造，不能访问
    // 正确做法应当先 placement new：
    Demo* p_good = new (mem) Demo(7);
    p_good->show();
    p_good->~Demo();
    std::free(mem);

    // 错误示范：new[] 与 delete 混用
    Demo* arr = new Demo[2]{1,2};
    // delete arr; // ❌ 错误：必须使用 delete[]
    delete[] arr; // ✅ 正确

    // 错误示范：delete 空指针是安全的（标准保证），但重复 delete 是严重 BUG
    Demo* p = new Demo(5);
    delete p;
    // delete p; // ❌ 重复 delete -> UB
}

// ======= main：运行所有演示 =======
int main() {
    demo_new_delete_equiv();
    demo_new_throw_vs_nothrow();
    demo_new_array();
    demo_placement_new_pool();
    demo_custom_operator_new();
    demo_common_errors();

    cout << "\nAll demos finished.\n";
    return 0;
}
