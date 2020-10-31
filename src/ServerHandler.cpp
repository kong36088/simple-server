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

    // create socket
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    setNonBlocking(serverFd); // non-blocking
    LOG_SEV_WITH_LOC("server listen fd: " << serverFd, debug);

    // start epoll
    std::shared_ptr<Handler> handler = std::shared_ptr<Handler>(new ServerHandler);
    IOLoop::getInstance()->add(serverFd, handler, EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLET);

    // bind address
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

    LOG_SEV_WITH_LOC("start listening " << localAddr << ":" << port, debug);
}

int ServerHandler::handle(epoll_event event) {
    sockaddr_in clientAddr;
    int fd = event.data.fd;

    LOG_SEV_WITH_LOC("server get fd: " << fd, debug);

    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(fd, (sockaddr *) &clientAddr, &clientLen);
    if (clientFd < 0) {
        LOG_SEV_WITH_LOC("accept client failed", error);
        return -1;
    }
    setNonBlocking(clientFd); // non-blocking

    LOG_SEV_WITH_LOC("accept new client, client:" << inet_ntoa(clientAddr.sin_addr) << " port: " << ntohs(clientAddr.sin_port) << " fd:" << clientFd, debug);
    std::shared_ptr<Handler> handler(new LogicHandler());
    IOLoop::getInstance()->add(clientFd, handler, EPOLLIN | EPOLLOUT | EPOLLET);

    return 0;
}