//
// Created by wellsjiang on 2020/11/1.
//

#ifndef SIMPLE_SERVER_HTTP_H
#define SIMPLE_SERVER_HTTP_H

#include <iostream>
#include <map>
#include "Exception.h"
#include "HttpProperty.h"
#include <memory>

class Http {
public:
    Http() {};

    std::shared_ptr<HttpProperty> parse(const std::string &&req) throw(Exception);

    std::shared_ptr<HttpProperty> parse(const std::string &req) throw(Exception);

    std::shared_ptr<HttpProperty> parse(const char *req) throw(Exception);

    void reset() noexcept;

protected:
    std::shared_ptr<HttpProperty> doParse(const std::string &req) throw(Exception);
};


#endif //SIMPLE_SERVER_HTTP_H
