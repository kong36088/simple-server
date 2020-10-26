//
// Created by wellsjiang on 2020/10/25.
//

#include "LogicHandler.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include "IOLoop.h"

int LogicHandler::handle(epoll_event event) {
    int fd = event.data.fd;
    uint32_t events = event.events;

    if (events & EPOLLHUP) {
        LOG_SEV_WITH_LOC("get HUP from fd:" << fd, debug);
        IOLoop::getInstance()->remove(fd);
        return -1;
    }

    if (events & EPOLLERR) {
        return -1;
    }

    if (events & EPOLLIN) {
        int received = recv(fd, buffer_, sizeof buffer_, MSG_WAITALL);
        if (received >= 0) {
            memcpy(input_, buffer_, received);
            IOLoop::getInstance()->modify(fd, EPOLLOUT | EPOLLET);
        } else {
            close(fd);
            LOG_SEV_WITH_LOC("disconnect from fd:" << fd, debug);
            IOLoop::getInstance()->remove(fd);
        }
    } else if (events & EPOLLOUT) {
        int written = send(fd, input_, sizeof input_, 0);
        if (written < 0) {
            LOG_SEV_WITH_LOC("write failed, fd:" << fd, error);
        }

        IOLoop::getInstance()->modify(fd, EPOLLIN | EPOLLET);
    }
    return 0;
}