#include <Pch.hpp>

#include "GitSync.hpp"

namespace Velyra::App {

    inline std::string quote(const std::string& argument) {
        return "\"" + argument + "\"";
    }

    GitSync::GitSync(const GitSyncDesc& desc, std::string fileName, Utils::LogPtr logger):
    m_Desc(desc),
    m_FileName(std::move(fileName)),
    m_Logger(std::move(logger)){

    }

    std::optional<nlohmann::json> GitSync::pull() {
        if (!initRepository()) {
            return std::nullopt;
        }
        if (runGit("fetch -q origin") != 0) {
            SPDLOG_LOGGER_WARN(m_Logger, "Failed to fetch settings from '{}', the remote is not reachable", m_Desc.remoteUrl);
            return std::nullopt;
        }
        const std::string remoteBranch = "refs/remotes/origin/" + m_Desc.branch;
        if (runGit("rev-parse -q --verify " + quote(remoteBranch)) != 0) {
            SPDLOG_LOGGER_INFO(m_Logger, "Branch '{}' does not exist on '{}' yet, it will be created", m_Desc.branch, m_Desc.remoteUrl);
            return std::make_optional(nlohmann::json::object());
        }
        // Throws away everything that is not on the remote, including commits of earlier pushes that failed
        if (runGit("checkout -q -f -B " + quote(m_Desc.branch) + " " + quote(remoteBranch)) != 0) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to check out branch '{}' in '{}'", m_Desc.branch, m_Desc.repoPath);
            return std::nullopt;
        }

        const fs::path filePath = m_Desc.repoPath / m_FileName;
        if (!fs::is_regular_file(filePath)) {
            return std::make_optional(nlohmann::json::object());
        }
        std::ifstream file(filePath);
        nlohmann::json settings = nlohmann::json::parse(file, nullptr, false);
        if (settings.is_discarded() || !settings.is_object()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Settings file '{}' in the git repository is not valid json, ignoring it", filePath);
            return std::nullopt;
        }
        return std::make_optional(std::move(settings));
    }

    bool GitSync::push(const nlohmann::json& settings) {
        const fs::path filePath = m_Desc.repoPath / m_FileName;
        {
            std::ofstream file(filePath);
            if (!file.is_open()) {
                SPDLOG_LOGGER_ERROR(m_Logger, "Failed to write settings file '{}'", filePath);
                return false;
            }
            file << settings.dump(4) << "\n";
        }
        if (runGit("add -- " + quote(m_FileName)) != 0) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to add '{}' to git", filePath);
            return false;
        }
        if (runGit("diff --cached --quiet -- " + quote(m_FileName)) == 0) {
            return true; // Nothing changed
        }
        if (runGit("commit -q -m " + quote("Update " + m_FileName)) != 0) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to commit '{}', is git user.name and user.email configured?", filePath);
            return false;
        }
        if (runGit("push -q origin " + quote("HEAD:refs/heads/" + m_Desc.branch)) != 0) {
            SPDLOG_LOGGER_WARN(m_Logger, "Failed to push settings to '{}', they will be pushed on the next sync", m_Desc.remoteUrl);
            return false;
        }
        SPDLOG_LOGGER_INFO(m_Logger, "Pushed settings to '{}'", m_Desc.remoteUrl);
        return true;
    }

    bool GitSync::initRepository() {
        // No clone, so creating the repository also works without internet
        if (!fs::exists(m_Desc.repoPath / ".git")) {
            SPDLOG_LOGGER_INFO(m_Logger, "Creating settings git repository at '{}'", m_Desc.repoPath);
            fs::create_directories(m_Desc.repoPath);
            if (runGit("init -q") != 0 ||
                runGit("symbolic-ref HEAD " + quote("refs/heads/" + m_Desc.branch)) != 0 ||
                runGit("remote add origin " + quote(m_Desc.remoteUrl)) != 0) {
                SPDLOG_LOGGER_ERROR(m_Logger, "Failed to create git repository at '{}', is git installed?", m_Desc.repoPath);
                fs::remove_all(m_Desc.repoPath);
                return false;
            }
            return true;
        }
        // The remote URL could have changed since the repository was created
        if (runGit("remote set-url origin " + quote(m_Desc.remoteUrl)) != 0) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to set the remote of git repository '{}'", m_Desc.repoPath);
            return false;
        }
        return true;
    }

    int GitSync::runGit(const std::string& arguments) const {
        // Never ask for passwords and do not wait long without internet, this blocks the application
        const std::string command = "git -c " + quote("core.sshCommand=ssh -o BatchMode=yes -o ConnectTimeout=5")
            + " -C " + quote(m_Desc.repoPath.string()) + " " + arguments;
        SPDLOG_LOGGER_DEBUG(m_Logger, "Running '{}'", command);
        return std::system(command.c_str());
    }

}
