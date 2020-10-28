//
// Created by wellsjiang on 2020/10/25.
//

#ifndef SIMPLE_SERVER_HANDLER_H
#define SIMPLE_SERVER_HANDLER_H

#include <sys/epoll.h>

class Handler{
public:
    Handler() = default;

    virtual int handle(epoll_event event) {};
};

#endif //SIMPLE_SERVER_HANDLER_H
