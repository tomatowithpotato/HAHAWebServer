#ifndef __HAHA_SOCKET_H__
#define __HAHA_SOCKET_H__

#include <fcntl.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <memory>
#include "InetAddress.h"
#include "base/Buffer.h"

namespace haha{

namespace sockops{

/*创建非阻塞socket*/
int createNBSocket();
/*创建默认（阻塞）socket*/
int createSocket();
void setNonBlocking(int fd);
/*禁用Nagle算法*/
void setNoDelay(int fd);
/*获取socket对端地址*/
InetAddress getPeerName(int fd);
/*获取socket本端地址*/
InetAddress getSockName(int fd);

}

class Socket{
public:
    typedef std::shared_ptr<Socket> ptr;

    enum FDTYPE{
        BLOCK,
        NONBLOCK,
    };

    // 根据已有描述符创建一个socket
    explicit Socket(int fd):fd_(fd) {}
    // 新生成一个socket
    explicit Socket(FDTYPE fdtype = FDTYPE::BLOCK);
    ~Socket();
    int getFd() const noexcept { return fd_; }
    bool isBlocked() { return isBlocked_; }
    // 获取本地地址
    InetAddress getLocalAddress();
    // 获取远端地址
    InetAddress getRemoteAddress();

    // 设置非阻塞
    void setNonBlocking();
    
    void bind(const InetAddress &address);
    void listen();
    // 接受连接
    Socket::ptr accept();
    // 开启地址复用
    void enableReuseAddr(bool on);
    // 开启端口复用
    void enableReusePort(bool on);
    // 开启保活
    void enableKeepAlive(bool on);
    // 忽略SIGPIPE信号
    void enableIgnoreSIGPIPE(bool on);

    // 把buff中的数据发到对端
    virtual int send(Buffer::ptr buff, int *lastLen = nullptr);
    // 从对端读取数据存到buff中
    virtual int recv(Buffer::ptr buff, int *lastLen = nullptr);

private:
    int fd_;
    bool isBlocked_;
};


class SslSocket : Socket{

};

}

#endif