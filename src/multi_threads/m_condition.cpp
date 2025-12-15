#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

/*
条件变量
定义：条件变量用来让线程“等待某个条件成立”，并由其他线程唤醒。条件变量不保存数据，它只是一个“通知机制”。

notify_one() 唤醒一个等待线程 典型队列模型（生产者–消费者）
notify_all() 唤醒全部等待线程 广播事件（如“退出程序”）

*/

std::queue<int> q;
std::mutex mtx;
std::condition_variable cv;

void producer() {
  for (int i = 0; i < 5; ++i) {
    {
      std::lock_guard<std::mutex> lock(mtx);
      q.push(i);
      std::cout << "生产: " << i << std::endl;
    }
    cv.notify_one();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void consumer() {
  while (true) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return !q.empty(); });

    int value = q.front();
    q.pop();
    lock.unlock();

    std::cout << "消费: " << value << std::endl;
    if (value == 4)
      break;
  }
}

int main() {
  std::thread t1(producer);
  std::thread t2(consumer);

  t1.join();
  t2.join();
}
