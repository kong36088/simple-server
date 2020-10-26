#include "logger.h"
#include "ServerHandler.h"
#include "IOLoop.h"

int main() {
    gInitLog(debug, debug);

    ServerHandler server(5555);
    IOLoop::getInstance()->start();
}
