//
// Created by wellsjiang on 2020/10/22.
//

#include "server.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include "utils/configs.h"
#include "logger.h"

void Server::setNonBlocking(int sock) {
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0) {
        LOG_SEV_WITH_LOC("fcntl(sock, GETFL)", error);
        return;
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0) {
        LOG_SEV_WITH_LOC("fcntl(sock,SETFL,opts)", error);
        return;
    }
}

void Server::serve() {
    int i, sockfd, epfd, nfds;
    ssize_t n;
    //声明epoll_event结构体的变量, ev用于注册事件, events数组用于回传要处理的事件
    struct epoll_event ev{}, events[20];
    //生成用于处理accept的epoll专用的文件描述符, 指定生成描述符的最大范围为256
    epfd = epoll_create(256);
    struct sockaddr_in serverAddr{};
    listenFd = socket(AF_INET, SOCK_STREAM, 0);

    setNonBlocking(listenFd);       //把用于监听的socket设置为非阻塞方式
    ev.data.fd = listenFd;          //设置与要处理的事件相关的文件描述符
    ev.events = EPOLLIN | EPOLLET;  //设置要处理的事件类型
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenFd, &ev);     //注册epoll事件
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    const char *local_addr = "0.0.0.0";
    inet_aton(local_addr, &(serverAddr.sin_addr));
    serverAddr.sin_port = htons(SERV_PORT);  //或者htons(SERV_PORT);
    serverFd = bind(listenFd, (sockaddr *) &serverAddr, sizeof(serverAddr));
    listen(listenFd, LISTENQ);
    while (running) {
        nfds = epoll_wait(epfd, events, 20, 500); //等待epoll事件的发生
        for (i = 0; i < nfds; ++i)                 //处理所发生的所有事件
        {
            if (events[i].data.fd == listenFd)      //监听事件
            {
                int connFd = acceptConn();
                ev.data.fd = connFd;                //设置用于读操作的文件描述符
                ev.events = EPOLLIN | EPOLLET;      //设置用于注测的读操作事件
                epoll_ctl(epfd, EPOLL_CTL_ADD, connFd, &ev);
                //注册ev事件
            } else if (events[i].events & EPOLLIN)      //读事件
            {
                // TODO event handler?
                if ((sockfd = events[i].data.fd) < 0) {
                    continue;
                }
                int readSize = readFrom(sockfd);
                if (readSize > 0) {
                    ev.data.fd = sockfd;              //设置用于写操作的文件描述符
                    ev.events = EPOLLOUT | EPOLLET;   //设置用于注测的写操作事件
                    //修改sockfd上要处理的事件为EPOLLOUT
                    epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
                } else {
                    LOG_SEV_WITH_LOC("disconnect from: " << sockfd, debug);
                    close(sockfd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, nullptr);
                }
            } else if (events[i].events & EPOLLOUT)//写事件
            {
                sockfd = events[i].data.fd;
                writeTo(sockfd);
                ev.data.fd = sockfd;               //设置用于读操作的文件描述符
                ev.events = EPOLLIN | EPOLLET;     //设置用于注册的读操作事件
                //修改sockfd上要处理的事件为EPOLIN
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
            }
        }
    }
}

int Server::acceptConn() const {
    static struct sockaddr_in clientAddr{};
    static socklen_t clientLen;

    int connFd = accept(listenFd, (sockaddr *) &clientAddr, &clientLen);
    if (connFd < 0) {
        LOG_SEV_WITH_LOC("connfd<0", error);
        exit(1);
    }
    setNonBlocking(connFd);           //把客户端的socket设置为非阻塞方式
    LOG_SEV_WITH_LOC("connect from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << " fd: " << connFd, debug);

    return connFd;
}

int Server::readFrom(int fd) {
    static char buffer[MAX_LINE];
    int received = recv(fd, buffer, sizeof buffer, 0);
    if (received > 0) {
        LOG_SEV_WITH_LOC("get msg: " << buffer, debug);
    }
    return received;
}

int Server::writeTo(int fd) {
    static const char *output = "hello world";
    return write(fd, output, strlen(output));
}
