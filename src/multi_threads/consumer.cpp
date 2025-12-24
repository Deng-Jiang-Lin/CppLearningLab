#include <condition_variable>
#include <mutex>
#include <queue>
#include <type_traits>
std::queue<int> queue;
std::mutex mu;
std::condition_variable cv;

// 生产者
void producer() {
  for (int i = 0; i < 10; ++i) {
    std::unique_lock<std::mutex> lock(mu); //加锁
    queue.push(i);
    cv.notify_one();
  }
}

void consumer() {
  while (true) {
    std::unique_lock<std::mutex> lock(mu); //加锁
    cv.wait(lock, []() { return !queue.empty(); });
    auto res = queue.front();
    queue.pop();
  }
}

int main() { return 0; }