//
// Created by wellsjiang on 2020/10/25.
//
#include "logger.h"

g_logger_mt &vlg = g_logger::get();

void gInitLog(SeverityLevel fileLevel, SeverityLevel consoleLevel) {
    logging::add_common_attributes();
    boost::shared_ptr<logging::core> core = logging::core::get();

    logging::formatter formatter_file =
            expr::stream
                    << "[" << expr::format_date_time(log_timestamp, "%Y-%m-%d %H:%M:%S")
                    << "]" << expr::if_(expr::has_attr(log_uptime))[expr::stream << " [" << format_date_time(log_uptime, "%O:%M:%S") << "]"]
                    << "[" << log_threadId << "][" << log_severity << "]"
                    << "[" << boost::log::expressions::attr<std::string>("File") <<
                    ":" << boost::log::expressions::attr<int>("Line") <<
                    ":" << boost::log::expressions::attr<std::string>("Function") << "]"
                    << expr::message;

    boost::shared_ptr<sinks::text_file_backend> backend =
            boost::make_shared<sinks::text_file_backend>(
                    keywords::file_name = "logs/server_%Y-%m-%d.log",
                    keywords::rotation_size = 200 * 1024 * 1024,
                    keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0),
                    keywords::open_mode = std::ios_base::app);

    // // Wrap it into the frontend and register in the core.
    // // The backend requires synchronization in the frontend.
    typedef sinks::synchronous_sink<sinks::text_file_backend> sink_t;
    boost::shared_ptr<sink_t> sink(new sink_t(backend));
    sink->set_formatter(formatter_file);
    sink->set_filter(log_severity >= fileLevel);

    core->add_global_attribute("Scope", attrs::named_scope());
    core->add_global_attribute("ThreadID", attrs::current_thread_id());
    core->add_sink(sink);

#ifdef DEBUG
    auto console_sink = logging::add_console_log();
    logging::formatter formatter_console =
            expr::stream
                    << "[" << expr::format_date_time(log_timestamp, "%H:%M:%S")
                    << "]" << expr::if_(expr::has_attr(log_uptime))[expr::stream << " [" << format_date_time(log_uptime, "%O:%M:%S") << "]"]
                    << "[" << log_severity << "]"
                    << "[" << boost::log::expressions::attr<std::string>("File") <<
                    ":" << boost::log::expressions::attr<int>("Line") <<
                    ":" << boost::log::expressions::attr<std::string>("Function") << "]"
                    << expr::message;
    console_sink->set_formatter(formatter_console);
    console_sink->set_filter(log_severity >= consoleLevel);
    core->add_sink(console_sink);
#endif // DEBUG
}