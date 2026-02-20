#pragma once

#include <vector>
#include <string_view>

#include <VelyraCore/VelyraCore.hpp>

namespace Velyra::App {

    using ProgramArgs = std::vector<std::string_view>;

    namespace fs = std::filesystem;

    fs::path get_appdata_path(const std::string& app_name);

}