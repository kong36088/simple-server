//
// Created by wellsjiang on 2020/11/1.
//

#include "Http.h"
#include "utils.h"
#include <iterator>
#include <boost/algorithm/string/join.hpp>

std::shared_ptr<HttpProperty> Http::parse(const std::string &&req) throw(Exception) {
    return doParse(req);
}

std::shared_ptr<HttpProperty> Http::parse(const std::string &req) throw(Exception) {
    return doParse(req);
}

std::shared_ptr<HttpProperty> Http::parse(const char *req) throw(Exception) {
    std::string r(req);
    return doParse(r);
}

std::shared_ptr<HttpProperty> Http::doParse(const std::string &req) throw(Exception) {
    std::vector<std::string> splitReq;
    splitString(req, splitReq, "\r\n");

    std::shared_ptr<HttpProperty> property(new HttpProperty);
    int index;
    if (splitReq.size() < 2) {
        throw Exception("not a valid http request");
    }

    // parse http version
    std::vector<std::string> httpVersion;
    splitString(splitReq[0], httpVersion, " ");
    if (httpVersion.size() != 3) {
        throw Exception("can not parse http version info");
    }
    property->method = httpVersion[0];
    property->url = httpVersion[1];
    property->httpVersion = httpVersion[2];

    // parse headers
    std::vector<std::string> header(2);
    for (index = 1; index < splitReq.size(); ++index) {
        if (splitReq[index].empty()) {
            break;
        }
        splitString(splitReq[index], header, ":");
        if (header.size() > 1) {
            property->headers[header[0]] = header[1];
        }
    }
    // parse body
    if (index + 1 < splitReq.size()) {
        property->body = boost::algorithm::join(splitReq, "\r\n");
    }

    return property;
}

void reset();