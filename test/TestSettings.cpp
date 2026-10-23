#include "TestPch.hpp"

#include <VelyraAppFramework/Settings.hpp>
#include <VelyraUtils/DevUtils/JsonSerializer.hpp>

#include <cstdlib>
#include <fstream>

using namespace Velyra;
using namespace Velyra::App;

struct CustomSettings {
    std::string name;
    int value = 0;

    VL_GENERATE_JSON_SERIALIZER(CustomSettings, name, value)
};

class TestSettings: public ::testing::Test {
protected:
    void SetUp() override {
        m_TestDir = fs::temp_directory_path() / ("VelyraTestSettings_" + std::string(::testing::UnitTest::GetInstance()->current_test_info()->name()));
        fs::remove_all(m_TestDir);
        fs::create_directories(m_TestDir);
    }

    void TearDown() override {
        fs::remove_all(m_TestDir);
    }

    fs::path m_TestDir;
};

TEST_F(TestSettings, CreateSettingsFromNonExistingFile) {
    const fs::path settingsFile = m_TestDir / "settings.json";
    {
        Settings settings({.enableSave = true, .settingsFile = settingsFile});

        const auto custom = settings.getSetting<CustomSettings>("custom");
        EXPECT_EQ(custom.name, "");
        EXPECT_EQ(custom.value, 0);
    }
    // When going out of scope, the settings should be saved to the file
    EXPECT_TRUE(fs::exists(settingsFile));
}

TEST_F(TestSettings, DisableSave) {
    const fs::path settingsFile = m_TestDir / "settings.json";
    {
        Settings settings({.enableSave = false, .settingsFile = settingsFile});
        settings.setSetting("custom", CustomSettings{"Test", 42});
    }
    EXPECT_FALSE(fs::exists(settingsFile));
}

TEST_F(TestSettings, CustomSettings) {
    const fs::path settingsFile = m_TestDir / "settings.json";
    {
        Settings settings({.enableSave = true, .settingsFile = settingsFile});
        settings.setSetting("custom", CustomSettings{"Test", 42});
    }
    {
        Settings settings({.enableSave = true, .settingsFile = settingsFile});
        auto [name, value] = settings.getSetting<CustomSettings>("custom");
        EXPECT_EQ(name, "Test");
        EXPECT_EQ(value, 42);
    }
}

TEST_F(TestSettings, CorruptFileStartsWithDefaults) {
    const fs::path settingsFile = m_TestDir / "settings.json";
    {
        std::ofstream file(settingsFile);
        file << "{ this is not json";
    }
    Settings settings({.enableSave = false, .settingsFile = settingsFile});
    EXPECT_EQ(settings.getSetting<CustomSettings>("custom").value, 0);
}

/**
 * @brief Simulates multiple PCs that sync their settings with the same remote, which is a bare git repository in a
 *        temporary folder. Taking the remote offline is done by renaming it.
 */
class TestSettingsGitSync: public TestSettings {
protected:
    void SetUp() override {
        if (std::system("git --version > /dev/null 2>&1") != 0) {
            GTEST_SKIP() << "git is not installed";
        }
        TestSettings::SetUp();
        // Commits need an identity, do not depend on the git config of the machine running the tests
        setenv("GIT_AUTHOR_NAME", "Velyra Test", 0);
        setenv("GIT_AUTHOR_EMAIL", "test@velyra.test", 0);
        setenv("GIT_COMMITTER_NAME", "Velyra Test", 0);
        setenv("GIT_COMMITTER_EMAIL", "test@velyra.test", 0);

        m_RemotePath = m_TestDir / "remote.git";
        ASSERT_EQ(std::system(("git init -q --bare \"" + m_RemotePath.string() + "\"").c_str()), 0);
    }

    SettingsDesc pcDesc(const std::string& pcName) const {
        SettingsDesc desc;
        desc.settingsFile = m_TestDir / pcName / "settings_App.json";
        desc.gitSync.remoteUrl = m_RemotePath.string();
        return desc;
    }

    void setOnline(const bool online) const {
        const fs::path offlinePath = m_TestDir / "remote_offline.git";
        if (online) {
            fs::rename(offlinePath, m_RemotePath);
        }
        else {
            fs::rename(m_RemotePath, offlinePath);
        }
    }

    fs::path m_RemotePath;
};

TEST_F(TestSettingsGitSync, NewPCGetsSettingsFromGit) {
    {
        Settings laptop(pcDesc("Laptop"));
        laptop.setSetting("custom", CustomSettings{"Laptop", 1});
    }
    // This used to be rejected because the new local file was considered newer
    Settings desktop(pcDesc("Desktop"));
    EXPECT_EQ(desktop.getSetting<CustomSettings>("custom").value, 1);
    EXPECT_EQ(desktop.getSetting<CustomSettings>("custom").name, "Laptop");
}

TEST_F(TestSettingsGitSync, RemoteChangesAreFollowed) {
    { Settings laptop(pcDesc("Laptop")); laptop.setSetting("value", 1); }
    { Settings desktop(pcDesc("Desktop")); EXPECT_EQ(desktop.getSetting<int>("value"), 1); desktop.setSetting("value", 2); }
    // The laptop did not change anything since its last sync, so it follows git
    Settings laptop(pcDesc("Laptop"));
    EXPECT_EQ(laptop.getSetting<int>("value"), 2);
}

TEST_F(TestSettingsGitSync, OfflineChangesArePushedLater) {
    { Settings laptop(pcDesc("Laptop")); laptop.setSetting("a", 1); }
    { Settings desktop(pcDesc("Desktop")); EXPECT_EQ(desktop.getSetting<int>("a"), 1); }

    // The laptop changes something without internet
    setOnline(false);
    {
        Settings laptop(pcDesc("Laptop"));
        EXPECT_EQ(laptop.getSetting<int>("a"), 1); // The local settings are used
        laptop.setSetting("b", 2);
    }
    setOnline(true);

    // Meanwhile the desktop changes something else
    { Settings desktop(pcDesc("Desktop")); desktop.setSetting("c", 3); }

    // The laptop is back online, both changes are merged and pushed
    {
        Settings laptop(pcDesc("Laptop"));
        EXPECT_EQ(laptop.getSetting<int>("a"), 1);
        EXPECT_EQ(laptop.getSetting<int>("b"), 2);
        EXPECT_EQ(laptop.getSetting<int>("c"), 3);
    }
    Settings desktop(pcDesc("Desktop"));
    EXPECT_EQ(desktop.getSetting<int>("b"), 2);
    EXPECT_EQ(desktop.getSetting<int>("c"), 3);
}

TEST_F(TestSettingsGitSync, ConflictGitWinsAndLocalIsBackedUp) {
    { Settings laptop(pcDesc("Laptop")); laptop.setSetting("value", 1); }
    { Settings desktop(pcDesc("Desktop")); }

    setOnline(false);
    { Settings laptop(pcDesc("Laptop")); laptop.setSetting("value", 5); }
    setOnline(true);
    { Settings desktop(pcDesc("Desktop")); desktop.setSetting("value", 7); }

    Settings laptop(pcDesc("Laptop"));
    EXPECT_EQ(laptop.getSetting<int>("value"), 7);
    EXPECT_TRUE(fs::exists(m_TestDir / "Laptop" / "settings_App.conflict.json"));
}

TEST_F(TestSettingsGitSync, NewPCWithoutInternet) {
    { Settings laptop(pcDesc("Laptop")); laptop.setSetting("a", 1); }

    setOnline(false);
    { Settings desktop(pcDesc("Desktop")); desktop.setSetting("b", 2); }
    setOnline(true);

    // Without a base, local only settings are kept and git wins for the rest
    Settings desktop(pcDesc("Desktop"));
    EXPECT_EQ(desktop.getSetting<int>("a"), 1);
    EXPECT_EQ(desktop.getSetting<int>("b"), 2);
}

TEST_F(TestSettingsGitSync, LocalKeysAreNotSynced) {
    {
        Settings laptop(pcDesc("Laptop"));
        laptop.setSetting("WindowSettings", 800);
        laptop.setSetting("shared", 1);
    }
    {
        Settings desktop(pcDesc("Desktop"));
        EXPECT_EQ(desktop.getSetting<int>("WindowSettings"), 0);
        EXPECT_EQ(desktop.getSetting<int>("shared"), 1);
        desktop.setSetting("WindowSettings", 1920);
    }
    Settings laptop(pcDesc("Laptop"));
    EXPECT_EQ(laptop.getSetting<int>("WindowSettings"), 800);
}

TEST_F(TestSettingsGitSync, NoCommitWhenNothingChanged) {
    { Settings laptop(pcDesc("Laptop")); laptop.setSetting("a", 1); }
    { Settings laptop(pcDesc("Laptop")); }
    { Settings laptop(pcDesc("Laptop")); }

    const std::string countCommand = "test \"$(git --git-dir=\"" + m_RemotePath.string() + "\" rev-list --count main)\" = 1";
    EXPECT_EQ(std::system(countCommand.c_str()), 0);
}
