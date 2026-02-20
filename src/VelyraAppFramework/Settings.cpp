#include <Pch.hpp>

#include <VelyraAppFramework/Settings.hpp>
#include "LoggerNames.hpp"

namespace Velyra::App {

    Settings::Settings():
    m_Logger(Utils::getLogger(VL_APP_SETTINGS_LOGGER)){

    }

    Settings::Settings(const SettingsDesc& desc):
    settingsFilePath(desc.settingsFilePath),
    enableSave(desc.enableSave),
    m_Logger(Utils::getLogger(VL_APP_SETTINGS_LOGGER)){
        if (fs::is_regular_file(desc.settingsFilePath)) {
            std::ifstream file(desc.settingsFilePath);

            nlohmann::json j = nlohmann::json::parse(file, nullptr, false);
            SPDLOG_LOGGER_INFO(m_Logger, "Loaded settings from file '{}'", desc.settingsFilePath.string());

            if (j.contains("WindowSettings")) {
                windowSettings = Utils::fromJson<WindowSettings>(j.at("WindowSettings"));
            }
            if (j.contains("ContextSettings")) {
                contextSettings = Utils::fromJson<ContextSettings>(j.at("ContextSettings"));
            }
            if (j.contains("ImGuiSettings")) {
                imGuiSettings = Utils::fromJson<ImGuiSettings>(j.at("ImGuiSettings"));
            }
            if (j.contains("AppSettings")) {
                m_SettingsJson = j.at("AppSettings");
            }
        }
    }

    Settings::~Settings() {
        if (enableSave) {
            save();
        }
    }

    void Settings::save() const {
        if (settingsFilePath.empty()) {
            SPDLOG_LOGGER_WARN(m_Logger, "Settings file path is empty, cannot save settings");
            return;
        }
        nlohmann::json j;
        j["WindowSettings"] = Utils::toJson(windowSettings);
        j["ContextSettings"] = Utils::toJson(contextSettings);
        j["ImGuiSettings"] = Utils::toJson(imGuiSettings);
        j["AppSettings"] = m_SettingsJson;
        std::ofstream file(settingsFilePath);
        file << j.dump(4);
        file.close();

        SPDLOG_LOGGER_INFO(m_Logger, "Saved settings to file '{}'", settingsFilePath.string());
    }
}
