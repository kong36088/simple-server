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
#include <errno.h>


// todo handler 池化
// todo 设置一个 fd buffer map
// todo handler 非线程安全，改为只用一个线程操作读写
int LogicHandler::handle(epoll_event event) {
    int fd = event.data.fd;
    uint32_t events = event.events;

    if (events & EPOLLHUP) {
        LOG_SEV_WITH_LOC("get HUP from fd:" << fd, debug);
        IOLoop::getInstance()->remove(fd);
        close(fd);
        return -1;
    }

    if (events & EPOLLERR) {
        LOG_SEV_WITH_LOC("get HUP from fd:" << fd, debug);
        IOLoop::getInstance()->remove(fd);
        close(fd);
        return -1;
    }

    if (events & EPOLLOUT) {
        if (strlen(buffer_) > 0) {
            int written = -1;
            const char *msg;
            LOG_SEV_WITH_LOC("input:" << input, debug);
            try {
                auto property = http.parse(input);
                std::shared_ptr<HttpProperty> rspProperty(new HttpProperty());
                rspProperty->httpVersion = property->httpVersion;
                rspProperty->body = property->body;
                msg = http.encode(rspProperty, 200, "OK").c_str();
            } catch (Exception e) {
                msg = e.what();
            }
            written = write(fd, msg, strlen(msg));
            LOG_SEV_WITH_LOC("send:" << msg, debug);
            if (written < 0) {
                LOG_SEV_WITH_LOC("write failed, fd:" << fd, error);
            }
            IOLoop::getInstance()->remove(fd);
            close(fd);
        } else {
            IOLoop::getInstance()->modify(fd, EPOLLIN | EPOLLET);
        }
        buffer_[0] = 0;
    }

    if (events & EPOLLIN) {
        int received = recv(fd, buffer_, sizeof buffer_, MSG_WAITALL);
        LOG_SEV_WITH_LOC("received length:" << std::to_string(received), debug);
        if (received > 0) {
            buffer_[received] = 0;
            memcpy(&input[strlen(input)], buffer_, (size_t)received);
            LOG_SEV_WITH_LOC("receive:" << buffer_, debug);
            IOLoop::getInstance()->modify(fd, EPOLLOUT | EPOLLET);
        } else if (received == 0){
            buffer_[0] = 0;
            LOG_SEV_WITH_LOC("disconnect from fd:" << fd, debug);
            IOLoop::getInstance()->remove(fd);
            close(fd);
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                buffer_[0] = 0;
                LOG_SEV_WITH_LOC("disconnect from fd:" << fd, debug);
                IOLoop::getInstance()->remove(fd);
                close(fd);
            } else {
                LOG_SEV_WITH_LOC("get EAGAIN | EWOULDBLOCK from fd:" << fd, debug);
            }
        }
    }
    return 0;
}