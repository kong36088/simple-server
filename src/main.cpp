#include "logger.h"
#include "ServerHandler.h"
#include "IOLoop.h"

int main() {
    gInitLog(debug, debug);

    ServerHandler server(SERV_PORT);
    IOLoop::getInstance()->start();
}
