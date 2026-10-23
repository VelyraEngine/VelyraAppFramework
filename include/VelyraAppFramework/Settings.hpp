#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <nlohmann/json.hpp>
#include <VelyraUtils/Conversions/Json.hpp>
#include <VelyraUtils/Logging/LoggingFwd.hpp>


namespace Velyra::App {

    class GitSync;

    struct VL_API GitSyncDesc {
        std::string remoteUrl; // E.g. "git@github.com:Coolrepo/settings.git". If empty, settings are not synced with git
        fs::path repoPath; // Where the local clone is stored, defaults to a "SettingsRepo" folder next to the settings file. Do not share it between applications.
        std::string branch = "main";
        std::vector<std::string> localKeys = {"WindowSettings", "ContextSettings"}; // Settings that are specific to this PC and never synced
    };

    struct VL_API SettingsDesc {
        bool enableSave = true; // If true, settings will be saved to disk on destruction of the Settings object. The file path is determined by settingsFilePath. If false, settings will not be saved to disk.
        fs::path settingsFile;
        GitSyncDesc gitSync{};
    };

    /**
     * @brief Key value store for settings, saved as json. Optionally synced with a git repository, in which case git
     *        is the source of truth: the local file follows git, but changes made while git was not reachable are
     *        merged and pushed on the next sync.
     */
    class VL_API Settings {
    public:
        explicit Settings(SettingsDesc desc);

        ~Settings();

        void load();

        void save();

        /**
         * @brief Merges the settings with the git repository and pushes local changes. Called by load() and save().
         * @return false if git sync is disabled or the remote could not be reached
         */
        bool sync();

        /**
         * @return The setting with the given key, the default value of T if the setting does not exist or does not
         *         match T (e.g. a field was added to T since the setting was saved)
         */
        template<typename T>
        T getSetting(const std::string& key) const {
            if (m_AppSettings.contains(key)) {
                try {
                    return Utils::fromJson<T>(m_AppSettings.at(key));
                }
                catch (const std::exception& e) {
                    logInvalidSetting(key, e);
                }
            }
            return T{};
        }

        template<typename T>
        void setSetting(const std::string& key, const T& value) {
            m_AppSettings[key] = Utils::toJson(value);
        }

        void setSaveEnabled(const bool enabled) { m_Desc.enableSave = enabled; }

        void setSettingsFilePath(const fs::path& path);

        void setGitSync(const GitSyncDesc& gitSync);

    private:
        void logInvalidSetting(const std::string& key, const std::exception& error) const;

        void loadSettings();

        void saveSettings() const;

        fs::path getSyncStatePath() const;

    private:
        Utils::LogPtr m_Logger;

        SettingsDesc m_Desc;
        nlohmann::json m_AppSettings = nlohmann::json::object();
        UP<GitSync> m_GitSync;
    };

}
