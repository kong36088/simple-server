//
// Created by wellsjiang on 2020/10/25.
//

#ifndef SIMPLE_SERVER_IOLOOP_H
#define SIMPLE_SERVER_IOLOOP_H

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
#include <unordered_map>
#include "Handler.h"

class IOLoop {
private:
    int epollFd_;
    std::unordered_map<int, std::shared_ptr<Handler>> handlers_;
    bool running = false;

    IOLoop() {
        epollFd_ = epoll_create1(0);
        if (epollFd_ < 0) {
            LOG_SEV_WITH_LOC("create epoll failed", fatal);
            exit(-1);
        }
    }

public:
    static IOLoop *getInstance() {
        static IOLoop instance;
        return &instance;
    }

    void start();

    void stop();

    void add(int fd, std::shared_ptr<Handler> handler, unsigned int events);

    void modify(int fd, unsigned int events);

    void remove(int fd);
};


#endif //SIMPLE_SERVER_IOLOOP_H
