//
// Created by wellsjiang on 2020/10/25.
//

#include "IOLoop.h"
#include <future>
#include "configs.h"
#include <chrono>
#include <memory>

SafeQueue<std::shared_ptr<LoopEvent_t>> queue;

void IOLoop::start() {
    epoll_event events[LISTENQ];

    std::future<void> fu;
    for (int i = 0; i < MAX_REACTOR; ++i) {
        LOG_SEV_WITH_LOC("start reactor" << i, debug);
        reactorFus[i] = std::async(std::launch::async, reactor);
    }

    LOG_SEV_WITH_LOC("simple-server: start serving", debug);
    while (isRunning()) {
        int nFds = epoll_wait(epollFd_, events, LISTENQ, -1);
        LOG_SEV_WITH_LOC("get nFds" << nFds, debug);
        for (int i = 0; i < nFds; i++) {
            int fd = events[i].data.fd;
            auto handler = handlers_[fd];
            if (handler) {
                LOG_SEV_WITH_LOC("do handle, fd:" << fd, debug);
                queue.enqueue(std::shared_ptr<LoopEvent_t>(new LoopEvent_t(handler, events[i])));
            } else {
                LOG_SEV_WITH_LOC("get empty handler, give up, fd:" << fd, error);
            }
        }
    }
    for (int i = 0; i < MAX_REACTOR; ++i) {
        reactorFus[i].wait_for(std::chrono::seconds(10));
    }
    return;
}

void IOLoop::reactor() {
    while (IOLoop::getInstance()->isRunning()) {
        auto le = queue.dequeue();
        if (le != nullptr) {
            le->handler->handle(le->event);
        }
    }
    return;
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
    e.data.fd = fd;
    if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &e) < 0) {
        LOG_SEV_WITH_LOC("failed to modify epoll event, fd: " << fd, error);
    }
}

void IOLoop::remove(int fd) {
    handlers_.erase(fd);
    epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr);
}