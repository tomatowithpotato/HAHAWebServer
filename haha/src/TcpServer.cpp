#include "TcpServer.h"
#include <iostream>

namespace haha{

TcpServer::TcpServer()
    :timeoutInterval_(5)
    ,eventLoop_(std::make_shared<EventLoop>())
    ,threadPool_(&ThreadPool::getInstance())
    ,servSock_(Socket::FDTYPE::NONBLOCK)
{
    servSock_.enableReuseAddr(true);
    servSock_.enableReusePort(true);
}

void TcpServer::start(const InetAddress &address){
    servSock_.bind(address);
    servSock_.listen();
    listenChannel_ = std::make_shared<Channel>(eventLoop_.get(), servSock_.getFd(), false);
    listenChannel_->setReadCallback(std::bind(&TcpServer::handleServerAccept, this));
    listenChannel_->setEvents(EPOLLIN | kServerEvent);

    threadPool_->start();

    eventLoop_->addChannel(listenChannel_.get());
    eventLoop_->loop(timeoutInterval_);
}

void TcpServer::handleServerAccept(){
    Socket::ptr sock = servSock_.accept();
    if(sock == nullptr){
        return;
    }
    handleConnected(sock);
}

void TcpServer::handleConnected(Socket::ptr sock){
    sock->setNonBlocking();
    int connfd = sock->getFd();
    TcpConnection::ptr connection;
    {
        ReadWriteLock::RAIIWriteLock lock(connMtx_);
        connects_[connfd] = std::make_shared<TcpConnection>(sock);
        connection = connects_[connfd];
    }
    TcpConnection::weak_ptr weak_conn(connection);
    connection->setChannel(std::make_shared<Channel>(eventLoop_.get(), connfd, false));
    connection->setEvents(EPOLLIN | kConnectionEvent);
    connection->getChannel()->setReadCallback(std::bind(&TcpServer::handleConnectionRead, this, weak_conn));
    connection->getChannel()->setWriteCallback(std::bind(&TcpServer::handleConnectionWrite, this, weak_conn));
    connection->getChannel()->setCloseCallback(std::bind(&TcpServer::handleConnectionClose, this, weak_conn));

    onNewConntection(connection);
    
    // 单loop模式下，就是eventLoop_
    // 未来要扩展为one loop per thread，预留
    auto loop = connection->getChannel()->getEventLoop();

    loop->addChannel(connection->getChannel().get());
    loop->addTimer(Timer(
        connfd,
        TimeStamp::nowSecond(TcpConnection::TimeOut),
        std::bind(&TcpServer::handleConnectionClose, this, weak_conn)
    ));
}

void TcpServer::handleConnectionRead(TcpConnection::weak_ptr weak_conn) {
    // HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "handleConnectionRead";
    TcpConnection::ptr conn = weak_conn.lock();
    // 检查连接是否被析构
    if(!conn){
        HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "conn already destroy";
        return;
    }
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;
    auto loop = conn->getChannel()->getEventLoop();
    loop->adjustTimer(Timer(
        conn->getFd(),
        TimeStamp::nowSecond(TcpConnection::TimeOut),
        nullptr
    ));
    threadPool_->addTask(std::bind(&TcpServer::onRecv, this, weak_conn));
    // HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << (std::string("ptr count: ") + std::to_string(conn.use_count()));
}

void TcpServer::handleConnectionWrite(TcpConnection::weak_ptr weak_conn) {
    // HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "handleConnectionWrite";
    TcpConnection::ptr conn = weak_conn.lock();
    // 检查连接是否被析构
    if(!conn){
        HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "conn already destroy";
        return;
    }
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;
    auto loop = conn->getChannel()->getEventLoop();
    loop->adjustTimer(Timer(
        conn->getFd(),
        TimeStamp::nowSecond(TcpConnection::TimeOut),
        nullptr
    ));
    threadPool_->addTask(std::bind(&TcpServer::onSend, this, weak_conn));
}

void TcpServer::handleConnectionClose(TcpConnection::weak_ptr weak_conn) {
    // HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "handleConnectionClose";
    TcpConnection::ptr conn = weak_conn.lock();
    // 检查连接是否被析构
    if(!conn){
        HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "conn already destroy";
        return;
    }
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;

    onCloseConntection(conn);
    
    auto loop = conn->getChannel()->getEventLoop();
    loop->delChannel(conn->getChannel().get());
    conn->setDisconnected(true);

    // 从连接表中删除连接，因为可能有多个线程同时删，因此必须加锁保护
    {
        ReadWriteLock::RAIIWriteLock lock(connMtx_);
        auto it = connects_.find(conn->getFd());
        if(it != connects_.end()){
            connects_.erase(it);
            // HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << (std::string("ptr count: ") + std::to_string(conn.use_count()));
        }
    }
    
}

void TcpServer::onRecv(TcpConnection::weak_ptr weak_conn){
    TcpConnection::ptr conn = weak_conn.lock();
    // 检查连接是否被析构
    if(!conn){
        HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "conn already destroy";
        return;
    }
    TcpConnection::status status = conn->recv();
    if(status.type == status.CLOSED || status.type == status.ERROR){
        HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "onRecv: CLOSE or ERROR";
        /* 关闭连接 */
        auto loop = conn->getChannel()->getEventLoop();
        loop->delTimer(Timer(
            conn->getFd(),
            TimeStamp::nowSecond(0),
            nullptr
        ));
        handleConnectionClose(conn);
        return;
    }
    /* 处理接收的数据 */
    if(onMessage(conn) == MESSAGE_STATUS::AGAIN){
        /* 数据不完整，接着读 */
        conn->setEvents(EPOLLIN | kConnectionEvent);
    }
    else{
        /* 全部处理完就准备写 */
        conn->setEvents(EPOLLOUT | kConnectionEvent);
    }

    auto loop = conn->getChannel()->getEventLoop();
    loop->modChannel(conn->getChannel().get());
    // HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << (std::string("ptr count: ") + std::to_string(conn.use_count()));
}

void TcpServer::onSend(TcpConnection::weak_ptr weak_conn){
    TcpConnection::ptr conn = weak_conn.lock();
    // 检查连接是否被析构
    if(!conn){
        HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "conn already destroy";
        return;
    }
    TcpConnection::status status = conn->send();
    EventLoop *loop = nullptr;
    switch (status.type)
    {
    case status.COMPLETED:
        if(conn->isKeepAlive()){
            /* 准备接受新的连接 */
            conn->setEvents(EPOLLIN | kConnectionEvent);
            loop = conn->getChannel()->getEventLoop();
            loop->modChannel(conn->getChannel().get());
        }
        else{
            // HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "onSend: COMPLETED, has write " << status.n;
            /* 关闭连接 */
            loop = conn->getChannel()->getEventLoop();
            loop->delTimer(Timer(
                conn->getFd(),
                TimeStamp::nowSecond(0),
                nullptr
            ));
            handleConnectionClose(conn);
            // HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << (std::string("ptr count: ") + std::to_string(conn.use_count()));
        }
        break;
    case status.AGAIN:
        // HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "onSend: AGAIN ";
        /* 没写完，接着写 */
        conn->setEvents(EPOLLOUT | kConnectionEvent);
        loop = conn->getChannel()->getEventLoop();
        loop->modChannel(conn->getChannel().get());
        break;
    default:
        // HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "onSend: CLOSE or ERROR";
        /* 关闭连接 */
        loop = conn->getChannel()->getEventLoop();
        loop->delTimer(Timer(
            conn->getFd(),
            TimeStamp::nowSecond(0),
            nullptr
        ));
        handleConnectionClose(conn);
        break;
    }
}

TcpServer::MESSAGE_STATUS TcpServer::onMessage(TcpConnection::ptr conn){
    HAHA_LOG_INFO(HAHA_LOG_ROOT()) << "onMessage";
    return MESSAGE_STATUS::OK;
}

bool TcpServer::onNewConntection(TcpConnection::ptr conn){
    HAHA_LOG_INFO(HAHA_LOG_ROOT()) << "onNewConntection";
    return true;
}

bool TcpServer::onCloseConntection(TcpConnection::ptr conn){
    HAHA_LOG_INFO(HAHA_LOG_ROOT()) << "onCloseConntection";
    return true;
}

}