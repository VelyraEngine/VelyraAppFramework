#include <Pch.hpp>

#include <VelyraAppFramework/AppInclude.hpp>

namespace Velyra::App {

#if defined(VL_PLATFORM_WINDOWS)

    fs::path get_appdata_path(const std::string& app_name) {
        const char* appdata = std::getenv("APPDATA");
        if (appdata) {
            fs::path path = appdata;
            path /= app_name;
            fs::create_directories(path);
            return path;
        } else {
            // Fallback to current directory if APPDATA is not set
            return fs::current_path();
        }
    }

#elif defined(VL_PLATFORM_LINUX)

    fs::path get_appdata_path(const std::string& app_name) {
        const char* xdg = std::getenv("XDG_CONFIG_HOME");
        fs::path base;
        if (xdg) {
            base = xdg;
        }
        else {
            const char* home = std::getenv("HOME");
            if (home) {
                base = home;
                base /= ".config";
            }
            else {
                // Fallback to current directory if HOME is not set
                base = ".";
            }
        }
        base /= app_name;
        fs::create_directories(base);
        return base;
    }

#endif


}