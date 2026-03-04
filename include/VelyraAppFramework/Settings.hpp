#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <nlohmann/json_fwd.hpp>
#include <VelyraUtils/Conversions/Json.hpp>
#include <VelyraUtils/DevUtils/JsonSerializer.hpp>

namespace Velyra::App {

    struct VL_API WindowSettings {
        U32 width = 1280;
        U32 height = 720;

        VL_GENERATE_JSON_SERIALIZER(WindowSettings, width, height)
    };

    struct VL_API ImGuiSettings {
        bool useDocking = false;
        bool useViewports = true;
        bool useImPlot = false;

        VL_GENERATE_JSON_SERIALIZER(ImGuiSettings, useDocking, useViewports, useImPlot)
    };

    struct VL_API ContextSettings {
        VL_GRAPHICS_API graphicsAPI = VL_API_OPENGL;
        bool enableVSync = true;

        VL_GENERATE_JSON_SERIALIZER(ContextSettings, graphicsAPI, enableVSync)
    };

    struct VL_API GitInfo {
        bool saveToGit = false; // If true, the settings will also be saved to a git repository
        fs::path gitRepoPath; // The path to the git repository where settings will be saved if saveToGit is true
        std::string ssh_string; // The SSH string to use for pushing to the git repository, e.g. "git@github:Coolrepo/settings.git"
    };

    struct VL_API SettingsDesc {
        bool enableSave = true; // If true, settings will be saved to disk on destruction of the Settings object. The file path is determined by settingsFilePath. If false, settings will not be saved to disk.
        fs::path settingsFilePath;
        GitInfo gitInfo;
    };

    class VL_API Settings {
    public:
        Settings();

        explicit Settings(const SettingsDesc& desc);

        ~Settings();

        void save() const;

        template<typename T>
        T getSetting(const std::string& key) const {
            if (m_SettingsJson.contains(key)) {
                return Utils::fromJson<T>(m_SettingsJson.at(key));
            }
            SPDLOG_LOGGER_WARN(m_Logger, "Requested setting '{}' not found, returning default value", key);
            static const T default_value{};
            return default_value;
        }

        template<typename T>
        void setSetting(const std::string& key, const T& value) {
            m_SettingsJson[key] = Utils::toJson(value);
        }

    public:
        bool enableSave = false;

        WindowSettings windowSettings;
        ImGuiSettings imGuiSettings;
        ContextSettings contextSettings;

    private:

        bool tryLoadFromGit();

        void loadSettings(const fs::path& filePath);

        void saveSettings(const fs::path& filePath) const;

        void trySaveToGit() const;

    private:
        nlohmann::json m_SettingsJson = nlohmann::json::object();
        Utils::LogPtr m_Logger;
        GitInfo m_GitInfo;
        fs::path m_SettingsFilePath;
    };

}
