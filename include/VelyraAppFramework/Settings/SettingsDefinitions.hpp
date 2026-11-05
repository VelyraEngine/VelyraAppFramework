#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraUtils/IO/IncludeFileSystem.hpp>
#include <VelyraUtils/DevUtils/JsonSerializer.hpp>

/**
 * @brief Git sync configuration. It is not part of SettingsDesc, it is stored in the local settings file under
 *        Settings::GIT_SYNC_KEY, so it can be changed per PC without recompiling. That key is never synced.
 */
struct VL_API GitSyncDesc {
    bool enableGitSync = false; // If true and remoteUrl is not empty, the settings are synced with a git repository
    std::string remoteUrl; // E.g. "git@github.com:Coolrepo/settings.git"
    Velyra::fs::path repoPath; // Where the local clone is stored, defaults to a "SettingsRepo" folder next to the settings file. Do not share it between applications.
    std::string branch = "main";
    std::vector<std::string> localKeys = {"WindowSettings", "ContextSettings"}; // Settings that are specific to this PC and never synced, GIT_SYNC_KEY is always local

    VL_GENERATE_JSON_SERIALIZER_RELAXED(GitSyncDesc, enableGitSync, remoteUrl, repoPath, branch, localKeys)
};

namespace Velyra::App::SettingsKeys {
    constexpr auto GIT_SYNC = "GitSync";
    constexpr auto WINDOW_SETTINGS = "WindowSettings";
    constexpr auto CONTEXT_SETTINGS = "ContextSettings";
    constexpr auto IMGUI_SETTINGS = "ImGuiSettings";

}