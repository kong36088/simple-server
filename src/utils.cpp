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

void splitString(const std::string &s, std::vector<std::string> &sv, const char *delimiter) {
    splitString(s, sv, delimiter, -1);
}

void splitString(const std::string &s, std::vector<std::string> &sv, const char *delimiter, int limit) {
    sv.clear();
    std::string token;
    int pos = 0, offset = 0, i = 0, deliLen = strlen(delimiter);
    if (limit < 0) {
        limit = INT32_MAX;
    }
    while ((pos = s.find(delimiter, offset)) != std::string::npos && i < limit) {
        token = s.substr(offset, pos - offset);
        sv.emplace_back(token);
        offset = pos + deliLen;
        ++i;
    }
    if (offset < s.size()) {
        token = s.substr(offset);
        sv.emplace_back(token);
    } else if (offset == s.size()) {
        sv.emplace_back("");
    }
    return;
}