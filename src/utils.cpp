//
// Created by wellsjiang on 2020/10/26.
//

#include "utils.h"
#include "logger.h"
#include <fcntl.h>

void setNonBlocking(int sock) {
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0) {
        LOG_SEV_WITH_LOC("fcntl(sock, GETFL)", error);
        return;
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0) {
        LOG_SEV_WITH_LOC("fcntl(sock,SETFL,opts)", error);
        return;
    }
}