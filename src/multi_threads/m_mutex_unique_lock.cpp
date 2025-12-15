#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

/*
unique_lock用法大全
一、std::unique_lock<std::mutex> lock(mtx);
自动释放锁（RAII机制），即使出现异常也会自动解锁 std::mutex mtx
示例：
  std::unique_lock<std::mutex> lock(mtx); //构造即上锁
  lock.unlock(); //手动释放锁
  lock.lock(); //手动上锁

二、 std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
defer_lock：构造时不立即加锁，使用的时候需要手动添加锁，自动解锁
示例：
  lock.lock(); //手动上锁
  lock.unlock(); //手动释放锁

三、std::unique_lock<std::mutex> lock(mtx, std::defer_lock);//尝试锁定
示例：
    //try_lock：尝试加锁，立即返回（非阻塞）
    if (lock.try_lock()) {
        // 成功获取锁
        process_data();
        return true;
    } else {
        // 锁被其他线程持有，执行替代操作
        fallback_operation();
        return false;
    }
    // 如果try_lock成功，离开作用域时会自动解锁
四、std::unique_lock<std::mutex> lock(mtx, std::defer_lock);//超时锁
示例：
    // try_lock_for：尝试在指定时间内获取锁
    if (lock.try_lock_for(std::chrono::milliseconds(100))) {
        // 在100ms内成功获取锁
        critical_operation();
    } else {
        // 超时未获取锁
        handle_timeout();
    }

五、所有权转移
示例：
  std::unique_lock<std::mutex> create_lock() {
      std::mutex mtx;
      std::unique_lock<std::mutex> lock(mtx);

      // 做一些初始化...

      return lock;  // unique_lock支持移动语义
  }

  void use_lock() {
      // 接收转移的所有权
      std::unique_lock<std::mutex> lock = create_lock();

      // 现在lock拥有锁的所有权
      // ...
  } // 自动解锁
}


*/
class BankAccount {
private:
  std::mutex mtx; //锁
  double balance; // 余额

public:
  BankAccount(double initial) : balance(initial) {}

  void deposit(double amount) {
    //防止多个线程同时修改余额造成数据乱序或错误
    std::unique_lock<std::mutex> lock(mtx);
    balance += amount;
    std::cout << "存入 " << amount << ", 余额: " << balance << std::endl;
  }

  void withdraw(double amount) {
    std::unique_lock<std::mutex> lock(mtx);
    if (balance >= amount) {
      balance -= amount;
      std::cout << "取出 " << amount << ", 余额: " << balance << std::endl;
    } else {
      std::cout << "余额不足! 尝试取出: " << amount << ", 余额: " << balance
                << std::endl;
    }
  }

  void transfer(BankAccount &to, double amount) {
    // 锁定两个账户，避免死锁
    std::unique_lock<std::mutex> lock1(mtx, std::defer_lock); //拿到自己的锁
    std::unique_lock<std::mutex> lock2(to.mtx, std::defer_lock); //拿到别人的锁
    std::lock(lock1, lock2); // 原子性地锁定两个

    if (balance >= amount) {
      balance -= amount;
      to.balance += amount;
      std::cout << "转账 " << amount << " 成功" << std::endl;
    }
  }

  double getBalance() {
    std::unique_lock<std::mutex> lock(mtx);
    return balance;
  }
};

int main() {
  BankAccount account1(1000);
  BankAccount account2(500);

  std::vector<std::thread> threads;

  // 多线程操作账户
  for (int i = 0; i < 5; ++i) {
    threads.emplace_back([&account1, i]() {
      account1.deposit(i * 100);
      account1.withdraw(i * 50);
    });

    threads.emplace_back(
        [&account1, &account2, i]() { account1.transfer(account2, i * 30); });
  }

  for (auto &t : threads)
    t.join();

  std::cout << "账户1余额: " << account1.getBalance() << std::endl;
  std::cout << "账户2余额: " << account2.getBalance() << std::endl;

  return 0;
}