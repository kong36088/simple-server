//
// Created by wellsjiang on 2020/11/1.
//

#ifndef SIMPLE_SERVER_HTTPPROPERTY_H
#define SIMPLE_SERVER_HTTPPROPERTY_H

#include <iostream>
#include <unordered_map>

class HttpProperty {
public:
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    std::string host;
    std::string url;
    std::string method;
    std::string httpVersion;

    HttpProperty(): httpVersion("HTTP/1.1") {};
};


#endif //SIMPLE_SERVER_HTTPPROPERTY_H
