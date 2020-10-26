//
// Created by wellsjiang on 2020/10/22.
//

#ifndef SIMPLE_SERVER_SERVER_H
#define SIMPLE_SERVER_SERVER_H

#include <netinet/in.h>

class Server {
protected:
    int listenFd = -1;
    bool running = true;

public:
    Server() {};

    void serve();

    int acceptConn() const;

    int readFrom(int fd);

    int writeTo(int fd);

    static void setNonBlocking(int sock);
};


#endif //SIMPLE_SERVER_SERVER_H
