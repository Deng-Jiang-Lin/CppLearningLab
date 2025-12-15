#include <condition_variable> // 条件变量
#include <future>
#include <iostream>
#include <mutex> // 基本互斥量
#include <queue>
#include <shared_mutex> // C++14 共享互斥量
#include <thread>
#include <vector>
/*
https://chatgpt.com/s/t_693bc8130a448191952f3e89c97aa4d9
*/
//加法函数
int sum(const int a, const int b) {
  int result = 0;
  for (int i = a; i < b; ++i) {
    result += i;
  }
  return result;
}
//线程运行成员
class Worker {
public:
  void run(int x) { std::cout << x << std::endl; }
};

void func(int id) { std::cout << "Thread " << id << " running\n"; }

int main() {
  // 一、声明一个线程
  std::thread t1([]() {
    //休眠3秒
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << std::this_thread::get_id() << "\nt2:Hello, World from thread!"
              << std::endl;
  });
  std::thread t3([]() {
    //休眠3秒
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << std::this_thread::get_id() << "\nt3:Hello, World from thread!"
              << std::endl;
  });
  t3.detach(); // 分离线程，主线程不等待t3完成
  t1.join();   // 主线程执行到这里会等待子线程完成后执行
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "Hello, World from main!" << std::endl;

  //二、 线程传参示例,并且返回
  std::future<int> result1 = std::async(std::launch::async, sum, 1, 500);
  std::future<int> result2 = std::async(std::launch::async, sum, 501, 1000);
  std::cout << "sum of 1-1000:" << result1.get() + result2.get() << std::endl;

  //三、线程运行成员函数
  Worker w;
  std::thread t4(&Worker::run, &w, 10);
  t4.join();

  //四、开启多个线程
  std::vector<std::thread> v;
  for (int i = 0; i < 5; ++i)
    v.emplace_back(func, i);
  //加入线程
  for (auto &t : v)
    t.join();

  //五、lock
  std::unique_lock<std::mutex> lock; // 不关联任何互斥量
  std::mutex mtx;
  std::unique_lock<std::mutex> lock1(mtx); // 立即上锁
  return 0;
}
