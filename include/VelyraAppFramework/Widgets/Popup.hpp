#pragma once

#include <VelyraUtils/Types/Types.hpp>

namespace Velyra::Core {
    class Window;
    class Context;
}

namespace Velyra::App {
    class AppData;
}

namespace Velyra::App::Widgets {

    using PopupID = UID;

    class Popup {
    public:
        Popup(AppData& appData, const std::string& name, PopupID popupID);

        virtual ~Popup() = default;

        void draw(Core::Window& window, Core::Context& context);

        void setOpen(const bool open) { m_Open = open; }

        bool isOpen() const { return m_Open; }

        PopupID getID() const { return m_PopupID; }

    protected:

        virtual void drawContent(Core::Window& window, Core::Context& context) = 0;

        virtual void reset() {}

    protected:
        AppData& m_AppData;

    private:
        const PopupID m_PopupID;
        const std::string m_Name;
        bool m_Open = false;
    };

}