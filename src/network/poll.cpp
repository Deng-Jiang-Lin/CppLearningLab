#include <arpa/inet.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>

/**
 * poll 是 POSIX 标准中对 select 系统调用的改进版本，核心优化与局限性如下：
 * 【相对于 select 的改进】
 * 1. 突破文件描述符数量限制：select 依赖 fd_set
 * 位图（通常由内核编译参数限制为1024）， poll 改用 struct pollfd
 * 数组存储文件描述符及关注的事件，数组大小仅受系统内存限制，
 *    理论上支持无限数量的文件描述符（实际受限于进程的文件描述符上限 ulimit
 * -n）。
 * 2. 避免 fd 重复拷贝与重置：select 的 fd_set
 * 会被内核修改，每次调用前需重新初始化； poll 的 pollfd
 * 数组中，输入事件（events）和输出事件（revents）分离，无需重复重置。
 *
 * 【核心局限性（高并发场景性能不足）】
 * 1. 线性遍历机制：poll 内核态仍需遍历整个 pollfd 数组，检查每个 fd
 * 的就绪状态， 时间复杂度为 O(n)（n 为监听的 fd 总数）。当高并发场景下 fd
 * 数量达到数千/万级时， 遍历开销会急剧增加，导致性能显著下降。
 * 2. 无边缘触发（ET）模式：poll 仅支持水平触发（LT），即只要 fd 处于就绪状态，
 *    每次调用都会返回该事件，可能导致重复处理（epoll 支持 ET
 * 模式，减少不必要的系统调用）。
 *
 * 【与 epoll 的对比】
 * epoll（Linux 特有）通过三大机制解决 poll 的性能瓶颈：
 * 1. 事件驱动模型：内核维护红黑树存储监听的 fd，就绪 fd 会被加入就绪链表，
 *    用户态仅需遍历就绪链表（时间复杂度 O(1)），无需遍历所有 fd。
 * 2. 内存映射（mmap）：减少内核态与用户态之间的事件拷贝开销。
 * 3. 支持 LT/ET 两种触发模式，适配不同场景的性能优化需求。
 *
 * 【适用场景】
 * - poll：跨平台场景（如 POSIX 兼容系统）、fd 数量较少的中低并发场景。
 * - epoll：Linux 平台、高并发高吞吐场景（如百万级连接的网络服务器）。
 */

int main() {
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8888);
  addr.sin_addr.s_addr = INADDR_ANY;

  bind(listenfd, (sockaddr *)&addr, sizeof(addr));
  listen(listenfd, 10);

  std::vector<pollfd> fds;
  fds.push_back({listenfd, POLLIN, 0});

  while (true) {
    int nready = poll(fds.data(), fds.size(), -1);
    if (nready < 0) {
      perror("poll");
      break;
    }

    // 新连接
    if (fds[0].revents & POLLIN) {
      int connfd = accept(listenfd, nullptr, nullptr);
      fds.push_back({connfd, POLLIN, 0});
    }

    // 已有连接
    for (size_t i = 1; i < fds.size(); ++i) {
      if (fds[i].revents & POLLIN) {
        char buf[1024];
        int n = read(fds[i].fd, buf, sizeof(buf));
        if (n <= 0) {
          close(fds[i].fd);
          fds.erase(fds.begin() + i);
          --i;
        } else {
          write(fds[i].fd, buf, n);
        }
      }
    }
  }
}
