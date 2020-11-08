//
// Created by wellsjiang on 2020/11/1.
//

#include "Http.h"
#include "utils.h"
#include <iterator>
#include <boost/algorithm/string/join.hpp>
#include <sstream>

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
    splitString(splitReq[0], httpVersion, " ", 2);
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
        splitString(splitReq[index], header, ":", 1);
        if (header.size() > 1) {
            property->headers[header[0]] = header[1];
        }
    }
    // parse body
    if (index + 1 < splitReq.size()) {
        property->body = boost::algorithm::join(std::move(std::vector<std::string>(splitReq.begin() + index + 1, splitReq.end())), "\r\n");
    }
#ifdef DEBUG
    for (auto it = property->headers.begin(); it != property->headers.end(); it++) {
        std::cout << "header:" << it->first << ":" << it->second << std::endl;
    }
    std::cout << "body:" << property->body << std::endl;
#endif

    return property;
}

void Http::reset() noexcept {}

std::string Http::encode(std::shared_ptr<HttpProperty> property, int code, const std::string& codeMsg) {
    std::stringstream ss;

    ss << property->httpVersion << " " << code << " " << codeMsg << "\r\n";
    if (property->headers.find("Content-Length") == property->headers.end()) {
        property->headers["Content-Length"] = std::to_string(property->body.size());
    }
    for (auto it = property->headers.begin(); it != property->headers.end(); ++it) {
        ss << it->first << ": " << it->second << "\r\n";
    }
    ss << "\r\n" << property->body;

    return ss.str();
}