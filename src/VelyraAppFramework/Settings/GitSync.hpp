#pragma once

#include <VelyraAppFramework/Settings.hpp>

namespace Velyra::App {

    /**
     * @brief Reads and writes one settings file in a git repository. The local clone is only a mirror of the remote,
     *        every pull resets it to the remote branch. Changes that could not be pushed are not kept in the clone,
     *        Settings keeps them in the local settings file and pushes them on the next sync.
     */
    class GitSync {
    public:
        GitSync(const GitSyncDesc& desc, std::string fileName, Utils::LogPtr logger);

        /**
         * @brief Fetches the latest version of the settings file from the remote.
         * @return The remote settings, an empty object if the remote does not have the file yet, std::nullopt if the
         *         remote could not be reached (e.g. no internet)
         */
        std::optional<nlohmann::json> pull();

        /**
         * @brief Commits the given settings and pushes them to the remote. Does nothing if they did not change.
         * @return false if the push failed, e.g. no internet or the remote changed since the last pull
         */
        bool push(const nlohmann::json& settings);

    private:
        bool initRepository();

        int runGit(const std::string& arguments) const;

    private:
        GitSyncDesc m_Desc;
        std::string m_FileName;
        Utils::LogPtr m_Logger;
    };

}
