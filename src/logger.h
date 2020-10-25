//
// Created by wellsjiang on 2020/5/20.
//

#ifndef SERVER_LOGGER_H
#define SERVER_LOGGER_H

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/keyword.hpp>

#include <boost/log/attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/attributes/named_scope.hpp>

#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/trivial.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/sources/record_ostream.hpp>

#define LOG_SEV(msg, sev) \
    BOOST_LOG_SEV(vlg, sev) << msg

#define LOG_SEV_WITH_LOC(msg, sev) \
    BOOST_LOG_SEV(vlg, sev)        \
        << logging::add_value("Line", __LINE__) \
        << logging::add_value("File", __FILE__) \
        << logging::add_value("Function", __FUNCTION__) << msg

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

enum SeverityLevel {
    trace = 0,
    debug,
    info,
    warn,
    error,
    fatal
};

template<typename CharT, typename TraitsT>
inline std::basic_ostream<CharT, TraitsT> &operator<<(std::basic_ostream<CharT, TraitsT> &strm, SeverityLevel lvl) {
    static const char *const str[] = {
            "TRACE",
            "DEBUG",
            "INFO",
            "WARN",
            "ERROR",
            "FATAL"};
    if (static_cast<std::size_t>(lvl) < (sizeof(str) / sizeof(*str)))
        strm << str[lvl];
    else
        strm << static_cast<int>(lvl);
    return strm;
}

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(g_logger, src::severity_logger_mt<SeverityLevel>);

BOOST_LOG_ATTRIBUTE_KEYWORD(log_severity, "Severity", SeverityLevel)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_uptime, "Uptime", attrs::timer::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_scope, "Scope", attrs::named_scope::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_threadId, "ThreadID", attrs::current_thread_id::value_type)

typedef src::severity_logger_mt<SeverityLevel> g_logger_mt;

extern void gInitLog(SeverityLevel fileLevel, SeverityLevel consoleLevel);

extern g_logger_mt &vlg;

#endif //SERVER_LOGGER_H
