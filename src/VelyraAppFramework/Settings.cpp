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
    m_GitInfo(desc.gitInfo),
    m_SettingsFilePath(desc.settingsFilePath){
        if (desc.gitInfo.saveToGit) {
            const bool loadedFromGit = tryLoadFromGit();
            if (loadedFromGit) {
                return; // If we successfully loaded from git, we can return early and skip loading from local file, which could be outdated
            }
        }
        // else just load from local file if it exists,
        if (fs::is_regular_file(desc.settingsFilePath)) {
            loadSettings(desc.settingsFilePath);
        }
    }

    Settings::~Settings() {
        if (enableSave) {
            save();
        }
    }

    void Settings::save() const {
        trySaveToGit();
        // ALWAYS save to local file, even if saving to git fails, to ensure we don't lose settings. The git saving is just a backup, not the primary storage
        if (m_SettingsFilePath.empty()) {
            SPDLOG_LOGGER_WARN(m_Logger, "Settings file path is empty, cannot save settings");
            return;
        }
       saveSettings(m_SettingsFilePath);
    }

    bool Settings::tryLoadFromGit() {
        // First, check if the repo exists locally, if not, try to clone it. If cloning fails, return false and fall back to local backup if available
        // In that case, some invalid config (like the ssh string) was passed
        if (!fs::exists(m_GitInfo.gitRepoPath)) {
            SPDLOG_LOGGER_INFO(m_Logger, "Git repository not found at '{}', cloning from git, this can take a while...", m_GitInfo.gitRepoPath.string());
            int success = std::system(("git clone \"" + m_GitInfo.ssh_string + "\" \"" + m_GitInfo.gitRepoPath.string() + "\"").c_str());
            if (success != 0) {
                SPDLOG_LOGGER_ERROR(m_Logger, "Failed to clone git repository, returned error code {}, falling back to local backup", success);
                return false;
            }
        }
        // Else, pull the latest changes
        else {
            SPDLOG_LOGGER_INFO(m_Logger, "Git repository found at '{}', pulling latest changes...", m_GitInfo.gitRepoPath.string());
            int success = std::system(("git -C " + m_GitInfo.gitRepoPath.string() + " pull").c_str());
            if (success != 0) {
                SPDLOG_LOGGER_ERROR(m_Logger, "Failed to pull git repository, returned error code {}, falling back to local backup", success);
                return false;
            }
        }
        // Now, fetch our settings file from the repo
        const std::string gitSettingsFilePath = (m_GitInfo.gitRepoPath / m_SettingsFilePath.filename()).string();
        if (!fs::is_regular_file(gitSettingsFilePath)) {
            SPDLOG_LOGGER_WARN(m_Logger, "Settings file not found in git repository at '{}'", gitSettingsFilePath);
            return false;
        }
        loadSettings(gitSettingsFilePath);
        return true;
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
        if (j.contains("ImGuiSettings")) {
            imGuiSettings = Utils::fromJson<ImGuiSettings>(j.at("ImGuiSettings"));
        }
        if (j.contains("AppSettings")) {
            m_SettingsJson = j.at("AppSettings");
        }
    }

    void Settings::saveSettings(const fs::path &filePath) const {
        nlohmann::json j;
        j["WindowSettings"] = Utils::toJson(windowSettings);
        j["ContextSettings"] = Utils::toJson(contextSettings);
        j["ImGuiSettings"] = Utils::toJson(imGuiSettings);
        j["AppSettings"] = m_SettingsJson;
        std::ofstream file(filePath);
        file << j.dump(4);
        file.close();

        SPDLOG_LOGGER_INFO(m_Logger, "Saved settings to file '{}'", filePath.string());
    }

    void Settings::trySaveToGit() const {
        if (m_GitInfo.gitRepoPath.empty()) {
            SPDLOG_LOGGER_WARN(m_Logger, "Git repository path is empty, cannot save settings to git");
            return;
        }
        const std::string gitSettingsFilePath = (m_GitInfo.gitRepoPath / m_SettingsFilePath.filename()).string();
        saveSettings(gitSettingsFilePath);
        int success = std::system(("git -C " + m_GitInfo.gitRepoPath.string() + " add " + gitSettingsFilePath).c_str());
        if (success != 0) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to add settings file to git, returned error code {}", success);
            return;
        }
        success = std::system(("git -C " + m_GitInfo.gitRepoPath.string() + " commit -m \"Update settings " + m_SettingsFilePath.filename().string() + "\"").c_str());
        if (success != 0) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to commit settings file to git, returned error code {}", success);
            return;
        }
        success = std::system(("git -C " + m_GitInfo.gitRepoPath.string() + " push").c_str());
        if (success != 0) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to push settings file to git, returned error code {}", success);
            return;
        }
        SPDLOG_LOGGER_INFO(m_Logger, "Saved settings to git repository at '{}'", m_GitInfo.gitRepoPath.string());
    }
}
