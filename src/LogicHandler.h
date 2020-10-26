//
// Created by wellsjiang on 2020/10/25.
//

#ifndef SIMPLE_SERVER_LOGICHANDLER_H
#define SIMPLE_SERVER_LOGICHANDLER_H

#include <sys/epoll.h>
#include "Handler.h"
#include "configs.h"
#include <cstring>

class LogicHandler : public Handler {
private:
    char buffer_[MAX_LINE];
    char input_[MAX_LINE];
public:
    LogicHandler() : Handler() {
        memset(input_, 0, sizeof input_);
    };

    int handle(epoll_event) override;
};


#endif //SIMPLE_SERVER_LOGICHANDLER_H
