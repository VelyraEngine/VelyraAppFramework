#pragma once

#include <VelyraUtils/Conversions/Json.hpp>
#include <VelyraUtils/Logging/LoggingFwd.hpp>
#include <VelyraAppFramework/Settings/SettingsDefinitions.hpp>
#include <nlohmann/json.hpp>

namespace Velyra::App {

    class GitSync;

    struct VL_API SettingsDesc {
        bool enableSave = true; // If true, settings will be saved on destruction of the Settings object, to settingsFile and to git if git sync is enabled
        fs::path settingsFile;
    };

    /**
     * @brief Key value store for settings, saved as json. The local settings file is always read first, it contains
     *        the GitSyncDesc under GIT_SYNC_KEY. If git sync is enabled there, the settings are synced with the git
     *        repository, in which case git is the source of truth: the local file follows git, but changes made while
     *        git was not reachable are merged and pushed on the next sync. Saving writes to both git and the local file.
     */
    class VL_API Settings {
    public:
        explicit Settings(SettingsDesc desc);

        ~Settings();

        /**
         * @brief Reads the local settings file, applies the GitSyncDesc stored in it and syncs with git if enabled.
         *        If the local file could be read, unsaved changes are discarded.
         */
        void load();

        /**
         * @brief Syncs with git if enabled and writes the local settings file. Does nothing if saving is disabled.
         */
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

        /**
         * @brief Stores the git sync configuration in the settings (saved to the local file) and applies it. Call
         *        sync() afterwards to pull the settings from the new repository right away.
         */
        void setGitSync(const GitSyncDesc& gitSync);

        GitSyncDesc getGitSync() const { return getSetting<GitSyncDesc>(SettingsKeys::GIT_SYNC); }

    private:
        void logInvalidSetting(const std::string& key, const std::exception& error) const;

        void loadSettings();

        void saveSettings() const;

        fs::path getSyncStatePath() const;

        /**
         * @brief (Re)creates m_GitSync from the GitSyncDesc stored in the settings
         */
        void applyGitSync();

    private:
        Utils::LogPtr m_Logger;

        SettingsDesc m_Desc;
        GitSyncDesc m_GitSyncDesc; // The applied configuration, with defaults resolved
        nlohmann::json m_AppSettings = nlohmann::json::object();
        UP<GitSync> m_GitSync;
    };

}
