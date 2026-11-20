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

    /**
     * @brief Only options the app works fine without belong here, they depend on the user and the platform. Options
     *        the app itself depends on, like ImPlot and the dockspace, are part of the ApplicationDesc.
     */
    struct VL_API ImGuiSettings {
        bool useViewports = false;
        bool saveImGuiWindowData = false; // Stores the window positions and dock layout in the appdata folder

        VL_GENERATE_JSON_SERIALIZER_RELAXED(ImGuiSettings, useViewports, saveImGuiWindowData)
    };

}