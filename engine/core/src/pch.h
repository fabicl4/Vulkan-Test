#pragma once

// Standard Library
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <cmath>

// External libraries with warning suppression
#pragma warning(push, 0)

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vulkan/vulkan.h>

#pragma warning(pop)

