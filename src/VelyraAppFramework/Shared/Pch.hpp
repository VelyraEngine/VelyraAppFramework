#pragma once

#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <optional>
#include <regex>
#include <utility>
#include <vector>
#include <string_view>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <VelyraUtils/Logging/Logging.hpp>
#include <VelyraUtils/Logging/Formatters.hpp>

#include <VelyraUtils/PlatformDetection.hpp>
#include <VelyraUtils/Conversions/String.hpp>

#include <VelyraCore/VelyraCore.hpp>