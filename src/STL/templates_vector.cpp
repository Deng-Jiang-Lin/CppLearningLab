#include <iostream>
#include <cstdlib>
#include <stdexcept>
using namespace std;

// ======================================================
// 自定义空间配置器（简化版）
// 负责：开辟内存 / 释放内存 / 构造对象 / 析构对象
// ======================================================
template<typename T>
struct Allocator {
    // 分配内存（仅分配，不构造）
    static T *allocate(const size_t n) {
        return static_cast<T *>(malloc(sizeof(T) * n));
    }

    // 释放内存（不析构）
    static void deallocate(T *ptr) {
        free(ptr);
    }

    // 构造对象（placement new）,在指定的内存位置（ptr）上构造一个类型为 T 的对象，使用 value 作为构造参数。
    static void construct(T *ptr, const T &value) {
        new(ptr) T(value);
    }

    // 调用对象的析构函数
    static void destroy(T *ptr) {
        ptr->~T();
    }
};


// ======================================================
// 自定义 Vector 容器
// 模拟 std::vector（简化版）
// ======================================================
template<typename T, typename Alloc = Allocator<T> >
class Vector {
public:
    // ----------------------------
    // 构造函数：仅开辟空间，不构造对象
    // ----------------------------
    explicit Vector(size_t cap = 10)
        : first_(nullptr), last_(nullptr), end_(nullptr), allocator_() {
        first_ = allocator_.allocate(cap);
        last_ = first_; // 当前没有元素
        end_ = first_ + cap; // 容量终点
    }

    // ----------------------------
    // 析构函数：先析构对象，再释放内存
    // ----------------------------
    ~Vector() {
        for (T *p = first_; p != last_; ++p)
            allocator_.destroy(p);

        allocator_.deallocate(first_);
    }

    // ----------------------------
    // 拷贝构造（深拷贝）
    // ----------------------------
    Vector(const Vector &other) : allocator_() {
        size_t cap = other.capacity();
        size_t len = other.size();

        first_ = allocator_.allocate(cap);

        // 构造每个有效元素
        for (size_t i = 0; i < len; ++i)
            allocator_.construct(first_ + i, other.first_[i]);

        last_ = first_ + len;
        end_ = first_ + cap;
    }

    // ----------------------------
    // 赋值运算符（深拷贝 + 自赋值检查）
    // ----------------------------
    Vector &operator=(const Vector &other) {
        if (this == &other)
            return *this;

        // 先析构旧元素
        for (T *p = first_; p != last_; ++p)
            allocator_.destroy(p);

        // 释放旧内存
        allocator_.deallocate(first_);

        // 重新分配
        size_t cap = other.capacity();
        size_t len = other.size();
        first_ = allocator_.allocate(cap);

        // 拷贝对象
        for (size_t i = 0; i < len; ++i)
            allocator_.construct(first_ + i, other.first_[i]);

        last_ = first_ + len;
        end_ = first_ + cap;

        return *this;
    }

    // ----------------------------
    // push_back：构造新对象
    // ----------------------------
    void push_back(const T &value) {
        if (full())
            expand();

        allocator_.construct(last_, value);
        ++last_;
    }

    // ----------------------------
    // pop_back：先析构对象
    // ----------------------------
    void pop_back() {
        if (empty())
            throw out_of_range("vector empty");

        --last_;
        allocator_.destroy(last_);
    }

    // ----------------------------
    // back：访问最后一个元素
    // ----------------------------
    const T &back() const {
        if (empty())
            throw std::runtime_error("vector empty");
        return *(last_ - 1);
    }

    // 状态查询函数
    bool empty() const { return first_ == last_; }
    bool full() const { return last_ == end_; }
    [[nodiscard]] size_t size() const { return last_ - first_; }
    [[nodiscard]] size_t capacity() const { return end_ - first_; }
    //下标运算符
    T &operator[](int index) {
        if (index < 0 || index >= size()) {
            throw out_of_range("index out of range");
        }
        return first_[index];
    }

    class Iterator {
    public:
        explicit Iterator(T *ptr) : ptr_(ptr) {
        }

        // !=
        bool operator!=(const Iterator &it) const {
            return it.ptr_ != ptr_;
        }

        //前置++
        void operator++() {
            ++ptr_;
        }

        // 解引用
        const T &operator*() const {
            return *ptr_;
        }

    private:
        T *ptr_;
    };

    //begin
    Iterator begin() { return Iterator(first_); }
    //end
    Iterator end() { return Iterator(last_); }

private:
    T *first_; // 起始地址
    T *last_; // 已使用区域的下一个位置
    T *end_; // 容量终点
    Alloc allocator_;

    // ----------------------------
    // 扩容：2 倍扩容
    // ----------------------------
    void expand() {
        const size_t oldCap = capacity();
        size_t newCap = oldCap * 2;

        T *newData = allocator_.allocate(newCap);

        size_t len = size();

        // 把旧元素拷贝到新内存
        for (size_t i = 0; i < len; ++i)
            allocator_.construct(newData + i, first_[i]);

        // 析构旧数据
        for (T *p = first_; p != last_; ++p)
            allocator_.destroy(p);

        allocator_.deallocate(first_);

        // 更新指针
        first_ = newData;
        last_ = newData + len;
        end_ = newData + newCap;
    }
};


// ======================================================
// 测试类（用于验证构造/析构顺序）
// ======================================================
class Test {
public:
    Test() { cout << "Test ctor\n"; }
    ~Test() { cout << "Test dtor\n"; }
};


// ======================================================
// main 函数测试
// ======================================================
int main() {
    Vector<int> v;

    // Test t1, t2;
    // v.push_back(t1);
    // v.push_back(t2);
    //
    // v.pop_back();
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    //测试迭代器
    for (Vector<int>::Iterator it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << std::endl;
    }

    return 0;
}
