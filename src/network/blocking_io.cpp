#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

/**
 * 阻塞 I/O（Blocking
 * I/O）是最基础的I/O操作模式，也是文件描述符（fd）的默认模式，核心原理与特性如下：
 * 【核心定义】
 * 文件描述符默认处于阻塞模式（无需主动设置标志），当进程执行读/写等I/O操作时：
 * 1.
 * 若数据尚未就绪（读操作：内核缓冲区无数据；写操作：内核缓冲区已满；网络I/O：连接未建立/数据未到达），
 *    调用进程会立即从运行态转为休眠态，被内核挂起（不占用CPU资源），直到数据就绪或发生错误（如连接中断、超时）；
 * 2.
 * 若数据就绪，则内核完成数据在内核态与用户态之间的拷贝，操作完成后进程被唤醒，I/O调用返回实际处理的字节数。
 *
 * 【阻塞的两个阶段（以网络读I/O为例）】
 * 1.
 * 等待数据阶段：内核等待网络数据到达并写入内核缓冲区，此阶段进程处于阻塞状态；
 * 2.
 * 数据拷贝阶段：内核将数据从内核缓冲区拷贝到用户进程的缓冲区，此阶段进程仍处于阻塞状态。
 * （注：传统阻塞I/O的两个阶段均为阻塞，区别于后续的I/O模型如信号驱动I/O、异步I/O）。
 *
 * 【与非阻塞I/O的对比】
 * - 阻塞I/O：数据未就绪时进程挂起，无需轮询，CPU利用率低但编程模型简单；
 * -
 * 非阻塞I/O：数据未就绪时进程立即返回错误码（EAGAIN/EWOULDBLOCK），需结合轮询或多路复用，编程复杂但灵活性高。
 *
 * 【适用场景与局限性】
 * 1.
 * 适用场景：低并发、逻辑简单的I/O场景（如本地文件读写、简单的客户端网络请求），编程实现成本低；
 * 2.
 * 局限性：在高并发场景下（如百万级网络连接），若为每个阻塞I/O创建独立线程，会导致线程数量暴增，
 *    引发线程上下文切换开销剧增、内存资源耗尽等问题，因此高并发场景通常采用非阻塞I/O+多路复用的组合。
 */

int main() {
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    perror("socket");
    return -1;
  }

  // 2. 绑定地址
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8888);
  addr.sin_addr.s_addr = INADDR_ANY;
  //把地址和端口绑定到socket上
  if (bind(listenfd, (sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    close(listenfd);
    return -1;
  }

  // 3. 监听，backlog参数提示内核监听队列的最大长度。
  if (listen(listenfd, 5) < 0) {
    perror("listen");
    close(listenfd);
    return -1;
  }

  std::cout << "Server listening on port 8888...\n";

  while (true) {
    // 4. 阻塞等待客户端连接
    sockaddr_in client{};
    socklen_t len = sizeof(client);
    int connfd = accept(listenfd, (sockaddr *)&client, &len);

    if (connfd < 0) {
      perror("accept");
      continue;
    }

    std::cout << "Client connected\n";

    char buf[1024];

    while (true) {
      // 5. 阻塞等待数据
      memset(buf, 0, sizeof(buf));
      ssize_t n = recv(connfd, buf, sizeof(buf), 0);

      if (n > 0) {
        std::cout << "recv: " << buf << std::endl;
        // 6. 阻塞发送
        send(connfd, buf, n, 0);
      } else if (n == 0) {
        std::cout << "Client closed\n";
        break;
      } else {
        perror("recv");
        break;
      }
    }

    close(connfd);
  }

  close(listenfd);
  return 0;
}
