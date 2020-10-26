//
// Created by wellsjiang on 2020/10/25.
//

#ifndef SIMPLE_SERVER_SERVERHANDLER_H
#define SIMPLE_SERVER_SERVERHANDLER_H

#include <sys/epoll.h>
#include "Handler.h"
#include "configs.h"
#include <cstring>

class ServerHandler : public Handler {
private:
    int serverFd = -1;
public:
    ServerHandler()=delete;

    explicit ServerHandler(int port);

    int handle(epoll_event) override;
};


#endif //SIMPLE_SERVER_SERVERHANDLER_H
