//
// Created by wellsjiang on 2020/10/25.
//

#ifndef SIMPLE_SERVER_LOGICHANDLER_H
#define SIMPLE_SERVER_LOGICHANDLER_H

#include <sys/epoll.h>
#include "Handler.h"
#include "configs.h"
#include <cstring>
#include "logger.h"
#include "Http.h"

class LogicHandler : public Handler {
private:
    char buffer_[MAX_LINE] = {0};
    char input[MAX_LINE] = {0};
    Http http;
    bool doResponse = false;
public:
    LogicHandler() : Handler() {};

    int handle(epoll_event e) override;

    ~LogicHandler() {
        LOG_SEV_WITH_LOC("LogicHandler destroyed", debug);
    }
};


#endif //SIMPLE_SERVER_LOGICHANDLER_H
