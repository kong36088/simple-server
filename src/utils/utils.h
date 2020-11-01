//
// Created by wellsjiang on 2020/10/26.
//

#ifndef SIMPLE_SERVER_UTILS_H
#define SIMPLE_SERVER_UTILS_H

#include "iostream"
#include <vector>

extern void setNonBlocking(int sock);

extern void splitString(const std::string &s, std::vector<std::string> &sv, const char *delimiter);


#endif //SIMPLE_SERVER_UTILS_H
