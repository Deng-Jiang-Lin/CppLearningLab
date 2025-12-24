#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
/**
 * select 是POSIX标准的IO多路复用机制，核心原理与局限性如下：
 * 【核心功能】
 * 单线程通过阻塞等待内核返回多个文件描述符（fd）的就绪状态（读/写/异常），
 * 实现对多路I/O的统一管理，避免为每个fd创建独立线程带来的资源开销。
 *
 * 【关键缺陷（导致高并发场景下性能不足）】
 * 1.
 * fd数量硬性限制：select依赖fd_set位图存储关注的fd，位图的位数由内核编译参数（如FD_SETSIZE）固定（通常为1024），
 *    无法突破该限制监听更多fd，直接制约高并发场景的扩展性。
 * 2.
 * 线性遍历开销大：每次调用select时，内核需遍历整个fd_set（无论fd是否就绪），检查每个fd的状态，
 *    时间复杂度为O(n)（n为fd_set的最大容量），fd数量越多，遍历耗时越长。
 * 3.
 * 重复初始化与拷贝：fd_set会被内核修改，每次调用select前需重新初始化fd_set；同时，每次调用都会将整个fd_set
 *    在用户态与内核态之间拷贝，增加额外的内存开销。
 *
 * 【替代方案】
 * 由于上述缺陷，select在高并发（如万级以上连接）场景下逐渐被poll和epoll取代：
 * -
 * poll：改用pollfd数组替代fd_set，突破fd数量限制，但仍保留线性遍历的O(n)时间复杂度。
 * -
 * epoll（Linux特有）：采用红黑树存储fd、就绪链表存储就绪事件，实现O(1)时间复杂度的事件查询，
 *   并通过mmap减少内存拷贝，支持水平触发（LT）和边缘触发（ET）模式，成为高并发场景的首选。
 */

int main() {
  // 1. 创建监听 socket
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);

  // 2. 绑定地址
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8888);
  addr.sin_addr.s_addr = INADDR_ANY;
  //把地址和端口绑定到socket上
  bind(listenfd, (sockaddr *)&addr, sizeof(addr));
  // 3. 监听
  listen(listenfd, 10);
  // 4. 初始化 fd_set
  fd_set allset, readset;
  FD_ZERO(&allset);
  FD_SET(listenfd, &allset);
  // 最大文件描述符值
  int maxfd = listenfd;
  // 5. 事件循环
  while (true) {
    //  必须每次拷贝一份，因为select会修改传入的 fd_set
    readset = allset;
    // 6. 等待事件
    int nready = select(maxfd + 1, &readset, NULL, NULL, NULL);
    if (nready < 0) {
      perror("select");
      break;
    }

    // 新连接到来
    if (FD_ISSET(listenfd, &readset)) {
      int connfd = accept(listenfd, NULL, NULL);
      FD_SET(connfd, &allset);
      maxfd = std::max(maxfd, connfd);
      if (--nready <= 0) {
        continue;
      }
    }

    //  已有连接数据
    for (int fd = listenfd + 1; fd <= maxfd; fd++) {
      if (FD_ISSET(fd, &readset)) {
        char buf[1024];
        int n = read(fd, buf, sizeof(buf));
        if (n <= 0) {
          close(fd);
          FD_CLR(fd, &allset);
        } else {
          write(fd, buf, n); // echo
        }
        if (--nready <= 0) {
          break;
        }
      }
    }
  }
}
