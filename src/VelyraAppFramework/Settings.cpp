#include <Pch.hpp>

#include <VelyraAppFramework/Settings.hpp>
#include "LoggerNames.hpp"

namespace Velyra::App {

    Settings::Settings():
    m_Logger(Utils::getLogger(VL_APP_SETTINGS_LOGGER)){

    }

    Settings::Settings(const SettingsDesc& desc):
    enableSave(desc.enableSave),
    m_Logger(Utils::getLogger(VL_APP_SETTINGS_LOGGER)),
    m_SettingsFilePath(desc.settingsFilePath),
    m_ApplicationName(desc.applicationName) {
        if (fs::is_regular_file(desc.settingsFilePath)) {
            loadSettings(desc.settingsFilePath);
        }
        else {
            SPDLOG_LOGGER_WARN(m_Logger, "Settings file '{}' not found, starting with default settings", desc.settingsFilePath.string());
        }
    }

    Settings::~Settings() {
        if (enableSave) {
            save();
        }
    }

    void Settings::save() const {
        if (m_SettingsFilePath.empty()) {
            SPDLOG_LOGGER_WARN(m_Logger, "Settings file path is empty, cannot save settings");
            return;
        }
       saveSettings(m_SettingsFilePath);
    }

    void Settings::tryLoadFromGit() {
        if (gitInfo.gitRepoPath.empty()) {
            SPDLOG_LOGGER_WARN(m_Logger, "Git repository path is empty, cannot load settings from git");
            return;
        }
        // First, check if the repo exists locally, if not, try to clone it. If cloning fails, return false and fall back to local backup if available
        // In that case, some invalid config (like the ssh string) was passed
        if (!fs::exists(gitInfo.gitRepoPath)) {
            SPDLOG_LOGGER_INFO(m_Logger, "Git repository not found at '{}', cloning from git, this can take a while...", gitInfo.gitRepoPath.string());
            int success = std::system(("git clone \"" + gitInfo.sshString + "\" \"" + gitInfo.gitRepoPath.string() + "\"").c_str());
            if (success != 0) {
                SPDLOG_LOGGER_ERROR(m_Logger, "Failed to clone git repository, returned error code {}, falling back to local backup", success);
                return;
            }
        }
        // Else, pull the latest changes
        else {
            SPDLOG_LOGGER_INFO(m_Logger, "Git repository found at '{}', pulling latest changes...", gitInfo.gitRepoPath.string());
            int success = std::system(("git -C " + gitInfo.gitRepoPath.string() + " pull").c_str());
            if (success != 0) {
                SPDLOG_LOGGER_ERROR(m_Logger, "Failed to pull git repository, returned error code {}, falling back to local backup", success);
                return;
            }
        }
        // Now, fetch our settings file from the repo
        const std::string gitSettingsFilePath = (gitInfo.gitRepoPath / m_SettingsFilePath.filename()).string();
        if (!fs::is_regular_file(gitSettingsFilePath)) {
            SPDLOG_LOGGER_WARN(m_Logger, "Settings file not found in git repository at '{}'", gitSettingsFilePath);
            return;
        }

        std::ifstream file(gitSettingsFilePath);
        if (!file.is_open()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to open settings file from git repository at '{}'", gitSettingsFilePath);
            return;
        }
        nlohmann::json appSettingsFromGit = nlohmann::json::parse(file, nullptr, false);
        auto lastGitUpdated = Utils::fromJson<TimePoint>(appSettingsFromGit.at("lastUpdated"));
        if (lastGitUpdated < localSettings.lastUpdated) {
            SPDLOG_LOGGER_WARN(m_Logger, "Settings file in git repository is older than local backup, skipping loading from git to prevent data loss");
            return;
        }
        m_AppSettings = appSettingsFromGit.at("AppSettings");
        SPDLOG_LOGGER_INFO(m_Logger, "Loaded settings from git repository at '{}'", gitSettingsFilePath);
    }

    void Settings::loadSettings(const fs::path &filePath) {
        std::ifstream file(filePath);

        nlohmann::json j = nlohmann::json::parse(file, nullptr, false);
        SPDLOG_LOGGER_INFO(m_Logger, "Loaded settings from file '{}'", filePath.string());

        if (j.contains("WindowSettings")) {
            windowSettings = Utils::fromJson<WindowSettings>(j.at("WindowSettings"));
        }
        if (j.contains("ContextSettings")) {
            contextSettings = Utils::fromJson<ContextSettings>(j.at("ContextSettings"));
        }
        if (j.contains("LocalSettings")) {
            localSettings = Utils::fromJson<LocalSettings>(j.at("LocalSettings"));
        }
        if (j.contains("GitInfo")) {
            gitInfo = Utils::fromJson<GitInfo>(j.at("GitInfo"));
        }
        if (j.contains("AppSettings")) {
            m_AppSettings = j.at("AppSettings");
            tryLoadFromGit();
        }
    }

    void Settings::saveSettings(const fs::path &filePath) const {
        localSettings.lastUpdated = getSystemTime();
        nlohmann::json j;
        j["WindowSettings"] = Utils::toJson(windowSettings);
        j["ContextSettings"] = Utils::toJson(contextSettings);
        j["LocalSettings"] = Utils::toJson(localSettings);
        j["GitInfo"] = Utils::toJson(gitInfo);
        j["AppSettings"] = m_AppSettings;
        std::ofstream file(filePath);
        file << j.dump(4);
        file.close();

        SPDLOG_LOGGER_INFO(m_Logger, "Saved settings to file '{}'", filePath.string());

        trySaveToGit();
    }

    void Settings::trySaveToGit() const {
        if (gitInfo.gitRepoPath.empty()) {
            SPDLOG_LOGGER_WARN(m_Logger, "Git repository path is empty, cannot save settings to git");
            return;
        }
        const std::string gitSettingsFilePath = (gitInfo.gitRepoPath / m_SettingsFilePath.filename()).string();

        nlohmann::json j;
        j["lastUpdated"] = Utils::toJson(localSettings.lastUpdated);
        j["AppSettings"] = m_AppSettings;
        std::ofstream file(gitSettingsFilePath);
        file << j.dump(4);
        file.close();

        int success = std::system(("git -C " + gitInfo.gitRepoPath.string() + " add " + gitSettingsFilePath).c_str());
        if (success != 0) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to add settings file to git, returned error code {}", success);
            return;
        }
        success = std::system(("git -C " + gitInfo.gitRepoPath.string() + " commit -m \"Update settings " + m_SettingsFilePath.filename().string() + "\"").c_str());
        if (success != 0) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to commit settings file to git, returned error code {}", success);
            return;
        }
        success = std::system(("git -C " + gitInfo.gitRepoPath.string() + " push").c_str());
        if (success != 0) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to push settings file to git, returned error code {}", success);
            return;
        }
        SPDLOG_LOGGER_INFO(m_Logger, "Saved settings to git repository at '{}'", gitInfo.gitRepoPath.string());
    }
}
