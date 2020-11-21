//
// Created by wellsjiang on 2020/11/14.
//

#ifndef SIMPLE_SERVER_LOOPEVENT_H
#define SIMPLE_SERVER_LOOPEVENT_H

#include <memory>
#include <iostream>
#include <utility>
#include "Handler.h"

typedef struct LoopEvent {
public:
    std::shared_ptr<Handler> handler;
    epoll_event event;

    LoopEvent(std::shared_ptr<Handler> handler, epoll_event event) : handler(std::move(handler)), event(event) {}
} LoopEvent_t;

#endif //SIMPLE_SERVER_LOOPEVENT_H
