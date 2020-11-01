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

void splitString(const std::string &s, std::vector<std::string> &sv, const char *delimiter = " ") {
    sv.clear();
    char *buffer = new char[s.size() + 1];
    buffer[s.size()] = '\0';
    std::copy(s.begin(), s.end(), buffer);
    char *p = std::strtok(buffer, delimiter);
    do {
        sv.emplace_back(p);
    } while ((p = std::strtok(NULL, delimiter)));
    delete[] buffer;
    return;
}