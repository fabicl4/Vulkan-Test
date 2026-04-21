#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)

#include <memory>

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

// GLM types support

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}