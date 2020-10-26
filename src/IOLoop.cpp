//
// Created by wellsjiang on 2020/10/25.
//

#include "IOLoop.h"

void IOLoop::start() {
    epoll_event events[LISTENQ];
    while (running) {
        int nFds = epoll_wait(epollFd_, events, LISTENQ, -1);
        for (int i = 0; i < nFds; i++) {
            int fd = events[i].data.fd;
            auto handler = handlers_[fd];
            handler->handle(events[i]);
        }
    }
}

void IOLoop::stop() {
    running = false;
}

void IOLoop::add(int fd, std::shared_ptr<Handler> handler, unsigned int events) {
    handlers_[fd] = handler;
    epoll_event e;
    e.events = events;
    e.data.fd = fd;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &e) < 0) {
        LOG_SEV_WITH_LOC("failed to create epoll event, fd: " << fd, error);
    }
}

void IOLoop::modify(int fd, unsigned int events) {
    epoll_event e;
    e.events = events;
    if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &e) < 0) {
        LOG_SEV_WITH_LOC("failed to modify epoll event, fd: " << fd, error);
    }
}

void IOLoop::remove(int fd) {
    handlers_.erase(fd);
    if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr) < 0) {
        LOG_SEV_WITH_LOC("failed to delete epoll event, fd: " << fd, error);
    }
}