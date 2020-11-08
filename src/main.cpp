#include "logger.h"
#include "ServerHandler.h"
#include "IOLoop.h"
#include <vector>
#include "utils.h"

int main() {
    gInitLog(debug, debug);

    ServerHandler server(SERV_PORT);
    IOLoop::getInstance()->start();
}

//int main() {
//    std::cout << "1\n2";
//}
