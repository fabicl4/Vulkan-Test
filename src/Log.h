#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)

namespace Log
{
    inline std::shared_ptr<spdlog::logger> s_Logger;

    void Init();

    void SetLevel(spdlog::level::level_enum level);

    void Shutdown();
}


#define LOG_TRACE(...)    ::Log::s_Logger->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Log::s_Logger->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Log::s_Logger->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Log::s_Logger->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Log::s_Logger->critical(__VA_ARGS__)
