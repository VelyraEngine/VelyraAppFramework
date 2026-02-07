#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraAppFramework/Settings.hpp>

namespace Velyra::App {

    struct VL_API AppData {
        const ProgramArgs& programArgs;
        Settings settings;

        explicit AppData(const ProgramArgs& args) : programArgs(args) {}
    };

}