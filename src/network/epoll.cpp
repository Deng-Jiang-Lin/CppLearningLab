/**
 * I/O 多路复用（I/O
 * Multiplexing）是高并发I/O编程的核心机制，核心原理与特性如下： 【核心定义】
 * 借助内核提供的系统调用（select/poll/epoll(kqueue)），让单个线程可以同时监听多个文件描述符（fd）的就绪事件（读/写/异常）；
 * 当无任何fd就绪时，线程会被内核阻塞（或非阻塞轮询）；当任意一个/多个fd就绪时，内核会通知线程，线程仅需对就绪的fd执行实际的I/O操作。
 *
 * 【核心优势（对比传统多线程/多进程模型）】
 * 1.
 * 减少资源开销：无需为每个连接（fd）创建独立的线程/进程，避免了大量的线程上下文切换、内存占用等系统开销；
 * 2.
 * 简化并发管理：单个线程即可高效管理成千上万个fd，降低了多线程同步（互斥锁、条件变量）的复杂度。
 *
 * 【实现方式与演进】
 * 1.
 * select：基于fd_set位图实现，存在fd数量限制（默认1024）、线性遍历开销（O(n)）、fd_set需重复初始化的缺陷；
 * 2.
 * poll：改用pollfd数组替代fd_set，突破fd数量限制，但仍保留线性遍历的O(n)开销；
 * 3.
 * epoll（Linux特有）/kqueue（BSD特有）：采用红黑树存储fd、就绪链表存储就绪事件，实现O(1)时间复杂度的事件查询，
 *    支持水平触发（LT）和边缘触发（ET）模式，是高并发场景（如百万级连接）的首选。
 *
 * 【与非阻塞I/O的结合】
 * 实际使用中，I/O多路复用通常与非阻塞I/O配合：将fd设置为非阻塞模式，确保就绪后的读写操作不会因数据未完全处理而阻塞线程，
 * 进一步提升I/O处理的效率。
 *
 * 【适用场景】
 * 高并发的网络服务（如Web服务器、消息中间件、网关）、需要同时处理多个文件/管道I/O的场景。
 */
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <vector>

int setNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
  // 1. 创建监听 socket
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  // 设置为非阻塞
  setNonBlocking(listenfd);
  // 2. 绑定地址
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8888);
  addr.sin_addr.s_addr = INADDR_ANY;
  // 3. 监听，backlog参数提示内核监听队列的最大长度。
  bind(listenfd, (sockaddr *)&addr, sizeof(addr));
  listen(listenfd, 128);
  // 4. 创建 epoll 实例
  int epfd = epoll_create1(0);
  // 5. 将监听 socket 添加到 epoll 实例中
  epoll_event ev{};
  ev.events = EPOLLIN;
  ev.data.fd = listenfd;
  epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
  // 6. 事件循环
  std::vector<epoll_event> events(16);
  // 7. 等待事件
  while (true) {
    int n = epoll_wait(epfd, events.data(), events.size(), -1);

    for (int i = 0; i < n; ++i) {
      int fd = events[i].data.fd;

      if (fd == listenfd) {
        // 新连接
        int connfd = accept(listenfd, nullptr, nullptr);
        setNonBlocking(connfd);

        epoll_event cev{};
        cev.events = EPOLLIN;
        cev.data.fd = connfd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &cev);
      } else {
        // 读数据
        char buf[1024];
        int cnt = read(fd, buf, sizeof(buf));
        if (cnt <= 0) {
          close(fd);
          epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
        } else {
          write(fd, buf, cnt); // echo
        }
      }
    }
  }
}
