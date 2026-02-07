#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <nlohmann/json_fwd.hpp>
#include <VelyraUtils/Conversions/Json.hpp>

namespace Velyra::App {

    struct VL_API SettingsDesc {
        fs::path settingsFilePath; // If empty, settings will not be loaded/saved to a file
    };

    class VL_API Settings {
    public:
        Settings();

        explicit Settings(const SettingsDesc& desc);

        ~Settings();

        void save() const;

        template<typename T>
        const T& get_setting(const std::string& key) const {
            if (m_SettingsJson.contains(key)) {
                return Utils::fromJson<T>(m_SettingsJson.at(key));
            }
            SPDLOG_LOGGER_WARN(m_Logger, "Requested setting '{}' not found, returning default value", key);
            static const T default_value{};
            return default_value;
        }

        template<typename T>
        void set_setting(const std::string& key, const T& value) {
            m_SettingsJson[key] = Utils::toJson(value);
        }

    public:
        fs::path settingsFilePath;
        bool enableSave = false;


    private:
        nlohmann::json m_SettingsJson = nlohmann::json::object();
        Utils::LogPtr m_Logger;
    };

}