#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraUtils/DevUtils/JsonSerializer.hpp>

namespace Velyra::App {

    struct VL_API WindowSettings {
        U32 width = 1280;
        U32 height = 720;

        VL_GENERATE_JSON_SERIALIZER_RELAXED(WindowSettings, width, height)
    };

    struct VL_API ContextSettings {
        VL_GRAPHICS_API graphicsAPI = VL_API_OPENGL;
        bool enableVSync = true;

        VL_GENERATE_JSON_SERIALIZER_RELAXED(ContextSettings, graphicsAPI, enableVSync)
    };

    struct VL_API ImGuiSettings {
        bool useDocking = false;
        bool useViewports = false;
        bool saveImGuiWindowData = false; // Creates imgui.ini to store window settings

        VL_GENERATE_JSON_SERIALIZER_RELAXED(ImGuiSettings, useDocking, useViewports, saveImGuiWindowData)
    };

}