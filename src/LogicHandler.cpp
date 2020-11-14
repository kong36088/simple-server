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
        close(fd);
        IOLoop::getInstance()->remove(fd);
        return -1;
    }

    if (events & EPOLLERR) {
        return -1;
    }

    if (events & EPOLLOUT) {
        if (strlen(buffer_) > 0) {
            int written = -1;
            const char *msg;
            try {
                auto property = http.parse(buffer_);
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
            close(fd);
            IOLoop::getInstance()->remove(fd);
        } else {
            IOLoop::getInstance()->modify(fd, EPOLLIN | EPOLLET);
        }
        buffer_[0] = 0;
    }

    if (events & EPOLLIN) {
        int received = recv(fd, buffer_, sizeof buffer_, MSG_WAITALL);
        buffer_[received] = 0;
        if (received > 0) {
            LOG_SEV_WITH_LOC("receive:" << buffer_, debug);
            IOLoop::getInstance()->modify(fd, EPOLLOUT | EPOLLET);
        } else {
            close(fd);
            LOG_SEV_WITH_LOC("disconnect from fd:" << fd, debug);
            IOLoop::getInstance()->remove(fd);
        }
    }
    return 0;
}