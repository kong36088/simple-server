//
// Created by wellsjiang on 2020/10/25.
//

#include "ServerHandler.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include "IOLoop.h"
#include "utils/utils.h"
#include "LogicHandler.h"

ServerHandler::ServerHandler(int port) {
    sockaddr_in serverAddr;

    serverFd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    const char *localAddr = "0.0.0.0";
    inet_aton(localAddr, &(serverAddr.sin_addr));
    serverAddr.sin_port = htons(port);
    if (bind(serverFd, (sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        LOG_SEV_WITH_LOC("bind address failed", fatal);
        exit(-1);
    }
    listen(serverFd, LISTENQ);
    std::shared_ptr<Handler> handler = std::shared_ptr<Handler>(new ServerHandler);
    IOLoop::getInstance()->add(serverFd, handler, EPOLLIN | EPOLLET);

    LOG_SEV_WITH_LOC("start listening " << localAddr << ":" << port, debug);
}

int ServerHandler::handle(epoll_event event) {
    sockaddr_in clientAddr;
    int fd = event.data.fd;

    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(fd, (sockaddr *) &clientAddr, &clientLen);
    if (clientFd < 0) {
        LOG_SEV_WITH_LOC("accept client failed", error);
        return -1;
    }

    LOG_SEV_WITH_LOC("accept new client, client:" << inet_ntoa(clientAddr.sin_addr) << " port: " << ntohs(clientAddr.sin_port) << " fd:" << clientFd, debug);
    std::shared_ptr<Handler> handler(new LogicHandler());
    IOLoop::getInstance()->add(clientFd, handler, EPOLLIN | EPOLLOUT | EPOLLET);

    return 0;
}