//
// Created by wellsjiang on 2020/10/22.
//

#ifndef SIMPLE_SERVER_SERVER_H
#define SIMPLE_SERVER_SERVER_H


class Server {
public:
    Server();

    void serve();

    static static void setNonBlocking(int sock);
};


#endif //SIMPLE_SERVER_SERVER_H
