#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <nlohmann/json_fwd.hpp>
#include <VelyraUtils/Conversions/Json.hpp>
#include <VelyraUtils/DevUtils/JsonSerializer.hpp>

#include <optional>

namespace Velyra::App {

    struct VL_API WindowSettings {
        U32 width = 1280;
        U32 height = 720;

        VL_GENERATE_JSON_SERIALIZER(WindowSettings, width, height)
    };

    struct VL_API ContextSettings {
        VL_GRAPHICS_API graphicsAPI = VL_API_OPENGL;
        bool enableVSync = true;
        bool useDocking = false;
        bool useViewports = false;
        bool useImPlot = false;

        VL_GENERATE_JSON_SERIALIZER(ContextSettings, graphicsAPI, enableVSync, useDocking, useViewports, useImPlot)
    };

    struct VL_API GitInfo {
        bool saveToGit = false; // If true, the settings will also be saved to a git repository
        fs::path gitRepoPath; // The path to the git repository where settings will be saved if saveToGit is true
        std::string sshString; // The SSH string to use for pushing to the git repository, e.g. "git@github:Coolrepo/settings.git"

        VL_GENERATE_JSON_SERIALIZER(GitInfo, saveToGit, gitRepoPath, sshString)
    };

    struct LocalSettings {
        TimePoint lastUpdated = getSystemTime();

        VL_GENERATE_JSON_SERIALIZER(LocalSettings, lastUpdated)
    };

    struct VL_API SettingsDesc {
        bool enableSave = true; // If true, settings will be saved to disk on destruction of the Settings object. The file path is determined by settingsFilePath. If false, settings will not be saved to disk.
        fs::path settingsFilePath;
        std::string_view applicationName;
    };

    class VL_API Settings {
    public:
        Settings();

        explicit Settings(const SettingsDesc& desc);

        ~Settings();

        void save() const;

        template<typename T>
        T getSetting(const std::string& key) const {
            if (m_AppSettings.contains(key)) {
                return Utils::fromJson<T>(m_AppSettings.at(key));
            }
            SPDLOG_LOGGER_WARN(m_Logger, "Requested setting '{}' not found, returning default value", key);
            static const T default_value{};
            return default_value;
        }

        template<typename T>
        void setSetting(const std::string& key, const T& value) {
            m_AppSettings[key] = Utils::toJson(value);
        }

    public:
        bool enableSave = false;

        WindowSettings windowSettings;
        ContextSettings contextSettings;
        /*
         * Mutable since we do not want to bother the user with manually updating the last updated time every time they
         * change a setting, we can just update it automatically when saving the settings
         */
        mutable LocalSettings localSettings;
        GitInfo gitInfo;

    private:

        void tryLoadFromGit();

        void loadSettings(const fs::path& filePath);

        void saveSettings(const fs::path& filePath) const;

        void trySaveToGit() const;

    private:
        nlohmann::json m_AppSettings = nlohmann::json::object();
        Utils::LogPtr m_Logger;
        fs::path m_SettingsFilePath;
        std::string_view m_ApplicationName;
    };

}
