#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraAppFramework/Settings.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutEngine.hpp>

namespace Velyra::App {

    struct VL_API AppData {
        const ProgramArgs& programArgs;
        Settings settings;
        LayoutEngine layoutEngine;

        explicit AppData(const ProgramArgs& args) : programArgs(args) {}
    };

}