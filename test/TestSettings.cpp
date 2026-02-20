#include "TestPch.hpp"

#include <VelyraAppFramework/Settings.hpp>

using namespace Velyra;
using namespace Velyra::App;

class TestSettings: public ::testing::Test {
};

TEST_F(TestSettings, CreateSettingsFromNonExistingFile) {
    const std::filesystem::path tempFilePath = std::filesystem::temp_directory_path() / "test_settings.json";
    {
        SettingsDesc desc;
        desc.settingsFilePath = tempFilePath;
        desc.enableSave = true;
        Settings settings(desc);

        constexpr WindowSettings defaultWindowSettings;
        EXPECT_EQ(settings.windowSettings.width, defaultWindowSettings.width);
        EXPECT_EQ(settings.windowSettings.height, defaultWindowSettings.height);

        constexpr ImGuiSettings defaultImGuiSettings;
        EXPECT_EQ(settings.imGuiSettings.useImPlot, defaultImGuiSettings.useImPlot);
        EXPECT_EQ(settings.imGuiSettings.useViewports, defaultImGuiSettings.useViewports);
        EXPECT_EQ(settings.imGuiSettings.useDocking, defaultImGuiSettings.useDocking);

        constexpr ContextSettings defaultContextSettings;
        EXPECT_EQ(settings.contextSettings.graphicsAPI, defaultContextSettings.graphicsAPI);
        EXPECT_EQ(settings.contextSettings.enableVSync, defaultContextSettings.enableVSync);
    }

    // When going out of scope, the settings should be saved to the file
    EXPECT_TRUE(std::filesystem::exists(tempFilePath));
}

TEST_F(TestSettings, EnableAutoSave) {
    const std::filesystem::path tempFilePath = std::filesystem::temp_directory_path() / "test_settings1.json";
    {
        SettingsDesc desc;
        desc.settingsFilePath = tempFilePath;
        desc.enableSave = true;
        Settings settings(desc);

        settings.windowSettings.width = 800;
        settings.windowSettings.height = 600;
    }
    // Now the settings should be saved to the file
    {
        SettingsDesc desc;
        desc.settingsFilePath = tempFilePath;
        desc.enableSave = true;
        Settings settings(desc);
        EXPECT_EQ(settings.windowSettings.width, 800);
        EXPECT_EQ(settings.windowSettings.height, 600);
    }
}

struct CustomSettings {
    std::string name;
    int value = 0;

    VL_GENERATE_JSON_SERIALIZER(CustomSettings, name, value)
};

TEST_F(TestSettings, CustomSettings) {
    const std::filesystem::path tempFilePath = std::filesystem::temp_directory_path() / "test_settings2.json";
    {
        SettingsDesc desc;
        desc.settingsFilePath = tempFilePath;
        desc.enableSave = true;
        Settings settings(desc);

        CustomSettings customSettings;
        customSettings.name = "Test";
        customSettings.value = 42;

        settings.setSetting("custom", customSettings);
    }
    // Now the settings should be saved to the file
    {
        SettingsDesc desc;
        desc.settingsFilePath = tempFilePath;
        desc.enableSave = true;
        Settings settings(desc);

        auto [name, value] = settings.getSetting<CustomSettings>("custom");
        EXPECT_EQ(name, "Test");
        EXPECT_EQ(value, 42);
    }
}