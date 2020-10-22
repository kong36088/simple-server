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
#include "utils/logger.h"
#include "utils/configs.h"

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
    int i, listenfd, connfd, sockfd, epfd, nfds;
    ssize_t n;
    char line[MAXLINE];

    socklen_t clilen;

    //声明epoll_event结构体的变量, ev用于注册事件, events数组用于回传要处理的事件
    struct epoll_event ev{}, events[20];
    //生成用于处理accept的epoll专用的文件描述符, 指定生成描述符的最大范围为256
    epfd = epoll_create(256);
    struct sockaddr_in clientAddr{};
    struct sockaddr_in serverAddr{};
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    setNonBlocking(listenfd);       //把用于监听的socket设置为非阻塞方式
    ev.data.fd = listenfd;          //设置与要处理的事件相关的文件描述符
    ev.events = EPOLLIN | EPOLLET;  //设置要处理的事件类型
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);     //注册epoll事件
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    const char *local_addr = "0.0.0.0";
    inet_aton(local_addr, &(serverAddr.sin_addr));
    serverAddr.sin_port = htons(SERV_PORT);  //或者htons(SERV_PORT);
    auto serverFd = bind(listenfd, (sockaddr *) &serverAddr, sizeof(serverAddr));
    listen(listenfd, LISTENQ);
    while (true) {
        nfds = epoll_wait(epfd, events, 20, 500); //等待epoll事件的发生
        for (i = 0; i < nfds; ++i)                 //处理所发生的所有事件
        {
            if (events[i].data.fd == listenfd)      //监听事件
            {
                connfd = accept(listenfd, (sockaddr *) &clientAddr, &clilen);
                if (connfd < 0) {
                    perror("connfd<0");
                    exit(1);
                }
                setNonBlocking(connfd);           //把客户端的socket设置为非阻塞方式
                char *str = inet_ntoa(clientAddr.sin_addr);
                std::cout << "connect from " << str << std::endl;
                ev.data.fd = connfd;                //设置用于读操作的文件描述符
                ev.events = EPOLLIN | EPOLLET;      //设置用于注测的读操作事件
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
                //注册ev事件
            } else if (events[i].events & EPOLLIN)      //读事件
            {
                if ((sockfd = events[i].data.fd) < 0) {
                    continue;
                }
                if ((n = read(sockfd, line, MAXLINE)) < 0) // 这里和IOCP不同
                {
                    if (errno == ECONNRESET) {
                        close(sockfd);
                        events[i].data.fd = -1;
                    } else {
                        std::cout << "readline error" << std::endl;
                    }
                } else if (n == 0) {
                    close(sockfd);
                    events[i].data.fd = -1;
                }
                ev.data.fd = sockfd;              //设置用于写操作的文件描述符
                ev.events = EPOLLOUT | EPOLLET;   //设置用于注测的写操作事件
                //修改sockfd上要处理的事件为EPOLLOUT
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
            } else if (events[i].events & EPOLLOUT)//写事件
            {
                sockfd = events[i].data.fd;
                write(sockfd, line, n);
                ev.data.fd = sockfd;               //设置用于读操作的文件描述符
                ev.events = EPOLLIN | EPOLLET;     //设置用于注册的读操作事件
                //修改sockfd上要处理的事件为EPOLIN
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
            }
        }
    }
}