#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
/**
 * 非阻塞 I/O（Non-blocking I/O，NIO）是一种I/O操作模式，核心原理与特性如下：
 * 【核心定义】
 * 将文件描述符（fd）设置为非阻塞模式后，执行读/写等I/O操作时：
 * 1.
 * 若数据尚未就绪（如读操作时内核缓冲区无数据、写操作时内核缓冲区已满），调用进程不会被阻塞挂起，
 *    而是立即返回错误码（多数系统中 EAGAIN 和 EWOULDBLOCK
 * 为同一宏定义，语义一致）；
 * 2. 若数据就绪，则正常执行I/O操作并返回实际处理的字节数，完成操作。
 *
 * 【实现方式】
 * 通过系统调用 fcntl() 修改fd的状态标志，设置 O_NONBLOCK（或
 * O_NDELAY，后者为历史兼容标志）： int flags = fcntl(fd, F_GETFL, 0); fcntl(fd,
 * F_SETFL, flags | O_NONBLOCK);
 * （注：套接字、管道、普通文件等多数fd支持非阻塞模式，部分特殊文件可能不支持）。
 *
 * 【与阻塞I/O的对比】
 * -
 * 阻塞I/O：数据未就绪时，进程会被内核挂起，直到数据就绪后再唤醒执行I/O，期间无法处理其他任务；
 * -
 * 非阻塞I/O：数据未就绪时立即返回错误，进程可继续执行其他任务，具备更高的任务调度灵活性。
 *
 * 【使用方式与局限性】
 * 1.
 * 单独使用时，需通过循环轮询（polling）检测fd是否就绪，会导致CPU空转（忙等），性能损耗大；
 * 2.
 * 实际场景中，通常与IO多路复用（select/poll/epoll）结合使用：由多路复用机制监听fd的就绪事件，
 *    仅在事件就绪时执行非阻塞I/O操作，既避免进程阻塞，又减少CPU空转，是高并发网络编程的常用组合。
 *
 * 【适用场景】
 * 高并发的网络服务（如Web服务器、网关）、需要同时处理多个I/O任务的进程/线程模型。
 */

int main() {
  // 1. 创建 socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    return -1;
  }

  // 2. 设置为非阻塞
  int flags = fcntl(sockfd, F_GETFL, 0);
  fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

  // 3. 服务器地址
  sockaddr_in serv{};
  serv.sin_family = AF_INET;
  serv.sin_port = htons(8888);
  inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);

  // 4. 非阻塞 connect
  int ret = connect(sockfd, (sockaddr *)&serv, sizeof(serv));
  if (ret < 0) {
    if (errno != EINPROGRESS) {
      perror("connect");
      close(sockfd);
      return -1;
    }
    // EINPROGRESS 表示正在连接中（正常）
    std::cout << "connecting...\n";
  }

  // 5. 非阻塞 recv
  char buf[1024];
  while (true) {
    int n = recv(sockfd, buf, sizeof(buf), 0);
    if (n > 0) {
      std::cout << "recv: " << std::string(buf, n) << std::endl;
      break;
    } else if (n == 0) {
      std::cout << "server closed\n";
      break;
    } else {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // 没有数据，继续做别的事
        std::cout << "no data yet...\n";
        usleep(500 * 1000); // 0.5 秒
        continue;
      } else {
        perror("recv");
        break;
      }
    }
  }

  close(sockfd);
  return 0;
}
