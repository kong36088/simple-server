//
// Created by wellsjiang on 2020/10/25.
//

#include "LogicHandler.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include "IOLoop.h"
#include <cstring>

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

    if (events & EPOLLOUT) {
        int written = write(fd, input_, strlen(input_));
        LOG_SEV_WITH_LOC("send:" << input_, debug);
        if (written < 0) {
            LOG_SEV_WITH_LOC("write failed, fd:" << fd, error);
        }

        IOLoop::getInstance()->modify(fd, EPOLLIN | EPOLLET);
    }

    if (events & EPOLLIN) {
        int received = recv(fd, buffer_, sizeof buffer_, MSG_WAITALL);
        if (received > 0) {
            LOG_SEV_WITH_LOC("receive:" << buffer_, debug);
            memcpy(input_, buffer_, received);
            IOLoop::getInstance()->modify(fd, EPOLLOUT | EPOLLET);
        } else {
            input_[0] = 0;
            close(fd);
            LOG_SEV_WITH_LOC("disconnect from fd:" << fd, debug);
            IOLoop::getInstance()->remove(fd);
        }
    }
    return 0;
}