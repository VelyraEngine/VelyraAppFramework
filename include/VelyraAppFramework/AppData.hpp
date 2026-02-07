#pragma once

#include <VelyraAppFramework/AppInclude.hpp>

namespace Velyra::App {

    struct VL_API AppData {
        const ProgramArgs& programArgs;

        explicit AppData(const ProgramArgs& args) : programArgs(args) {}
    };

}