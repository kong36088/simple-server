#include "logger.h"
#include "server.h"

int main() {
    gInitLog(debug, debug);

    Server s;

    s.serve();
}
