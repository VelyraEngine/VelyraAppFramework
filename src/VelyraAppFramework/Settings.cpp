#include <Pch.hpp>
#include <LoggerNames.hpp>

#include <VelyraAppFramework/Settings.hpp>
#include "Settings/GitSync.hpp"
#include "Settings/JsonMerge.hpp"

namespace Velyra::App {

    inline std::optional<nlohmann::json> readJsonFile(const fs::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return std::nullopt;
        }
        nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
        if (json.is_discarded() || !json.is_object()) {
            return std::nullopt;
        }
        return std::make_optional(std::move(json));
    }

    /**
     * @brief Writes to a temporary file first, so a crash while writing never leaves a half written file behind
     */
    inline bool writeJsonFile(const fs::path& path, const nlohmann::json& json) {
        if (path.has_parent_path()) {
            fs::create_directories(path.parent_path());
        }
        fs::path tempPath = path;
        tempPath += ".tmp";
        {
            std::ofstream file(tempPath);
            if (!file.is_open()) {
                return false;
            }
            file << json.dump(4);
            if (!file.good()) {
                return false;
            }
        }
        std::error_code error;
        fs::rename(tempPath, path, error);
        return !error;
    }

    inline fs::path siblingPath(const fs::path& settingsFile, const std::string& suffix) {
        return settingsFile.parent_path() / (settingsFile.stem().string() + suffix);
    }

    Settings::Settings(SettingsDesc desc):
    m_Logger(Utils::getLogger(VL_APP_SETTINGS_LOGGER)),
    m_Desc(std::move(desc)) {
        setGitSync(m_Desc.gitSync);
        load();
    }

    Settings::~Settings() {
        // Never throw from a destructor, losing the last changes is better than crashing
        try {
            save();
        }
        catch (const std::exception& e) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to save settings: {}", e.what());
        }
    }

    void Settings::load() {
        if (fs::is_regular_file(m_Desc.settingsFile)) {
            loadSettings();
        }
        else {
            SPDLOG_LOGGER_WARN(m_Logger, "Settings file '{}' does not exist or is not a regular file, starting with default settings", m_Desc.settingsFile);
        }
        sync();
    }

    void Settings::save() {
        if (!m_Desc.enableSave) {
            SPDLOG_LOGGER_INFO(m_Logger, "Settings save is disabled. Not saving settings");
            return;
        }
        // A successful sync already saved the settings
        if (!sync()) {
            saveSettings();
        }
    }

    bool Settings::sync() {
        if (m_GitSync == nullptr) {
            return false;
        }
        std::optional<nlohmann::json> remote = m_GitSync->pull();
        if (!remote.has_value()) {
            SPDLOG_LOGGER_WARN(m_Logger, "Could not sync settings with git, using the local settings. Local changes are pushed on the next sync");
            return false;
        }

        // Settings that are specific to this PC are left out of the sync
        nlohmann::json local = m_AppSettings;
        nlohmann::json localOnly = nlohmann::json::object();
        for (const auto& key : m_Desc.gitSync.localKeys) {
            if (local.contains(key)) {
                localOnly[key] = local.at(key);
                local.erase(key);
            }
            remote->erase(key);
        }

        // The base is the version of the last successful sync, it tells which side changed what
        const std::optional<nlohmann::json> syncState = readJsonFile(getSyncStatePath());
        const nlohmann::json* base = nullptr;
        if (syncState.has_value() && syncState->value("remoteUrl", "") == m_Desc.gitSync.remoteUrl && syncState->contains("base")) {
            base = &syncState->at("base");
        }

        std::vector<std::string> conflicts;
        nlohmann::json merged = mergeJson(base, local, *remote, conflicts);
        if (!conflicts.empty()) {
            const fs::path backupPath = siblingPath(m_Desc.settingsFile, ".conflict.json");
            std::string conflictList;
            for (const auto& conflict : conflicts) {
                conflictList += (conflictList.empty() ? "" : ", ") + conflict;
            }
            SPDLOG_LOGGER_WARN(m_Logger, "Settings '{}' were changed both locally and in git, using the git version. The local version is backed up in '{}'",
                conflictList, backupPath);
            if (m_Desc.enableSave) {
                writeJsonFile(backupPath, m_AppSettings);
            }
        }

        nlohmann::json newBase = *remote;
        if (m_Desc.enableSave && merged != *remote && m_GitSync->push(merged)) {
            newBase = merged;
        }

        merged.update(localOnly);
        m_AppSettings = std::move(merged);

        if (m_Desc.enableSave) {
            // Save the settings before the base, if we crash in between the local changes are pushed again next time
            saveSettings();
            if (!writeJsonFile(getSyncStatePath(), {{"remoteUrl", m_Desc.gitSync.remoteUrl}, {"base", newBase}})) {
                SPDLOG_LOGGER_ERROR(m_Logger, "Failed to write sync state '{}'", getSyncStatePath());
            }
        }
        SPDLOG_LOGGER_INFO(m_Logger, "Synced settings with '{}'", m_Desc.gitSync.remoteUrl);
        return true;
    }

    void Settings::setSettingsFilePath(const fs::path& path) {
        m_Desc.settingsFile = path;
        setGitSync(m_Desc.gitSync);
    }

    void Settings::setGitSync(const GitSyncDesc& gitSync) {
        m_Desc.gitSync = gitSync;
        m_GitSync.reset();
        if (gitSync.remoteUrl.empty()) {
            return;
        }
        if (m_Desc.settingsFile.empty()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Cannot sync settings with git without a settings file");
            return;
        }
        if (m_Desc.gitSync.repoPath.empty()) {
            m_Desc.gitSync.repoPath = m_Desc.settingsFile.parent_path() / "SettingsRepo";
        }
        m_GitSync = createUP<GitSync>(m_Desc.gitSync, m_Desc.settingsFile.filename().string(), m_Logger);
    }

    void Settings::loadSettings() {
        const std::optional<nlohmann::json> settings = readJsonFile(m_Desc.settingsFile);
        if (!settings.has_value()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to read settings file '{}', starting with default settings", m_Desc.settingsFile);
            return;
        }
        m_AppSettings = *settings;
        SPDLOG_LOGGER_INFO(m_Logger, "Loaded settings from file '{}'", m_Desc.settingsFile);
    }

    void Settings::saveSettings() const {
        if (m_Desc.settingsFile.empty()) {
            SPDLOG_LOGGER_WARN(m_Logger, "Settings file path is empty, cannot save settings");
            return;
        }
        if (!writeJsonFile(m_Desc.settingsFile, m_AppSettings)) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to write settings file '{}'", m_Desc.settingsFile);
            return;
        }
        SPDLOG_LOGGER_INFO(m_Logger, "Saved settings to file '{}'", m_Desc.settingsFile);
    }

    fs::path Settings::getSyncStatePath() const {
        return siblingPath(m_Desc.settingsFile, ".sync.json");
    }
}
