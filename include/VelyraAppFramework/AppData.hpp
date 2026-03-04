#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraAppFramework/Settings.hpp>

#include <VelyraAppFramework/LayoutEngine/LayoutEngine.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

namespace Velyra::App {

    class Application;

    class VL_API AppData {
    public:
        const ProgramArgs& programArgs;
        Settings settings;
        LayoutEngine layoutEngine;

    public:
        AppData(const ProgramArgs& args, const SettingsDesc& settingsDesc) : programArgs(args), settings(settingsDesc) {
        }

        void addPopup(const SP<Widgets::Popup> &popup) {
            m_Popups.push_back(popup);
        }

    private:
        friend Application;
        std::vector<SP<Widgets::Popup>> m_Popups;
    };

}