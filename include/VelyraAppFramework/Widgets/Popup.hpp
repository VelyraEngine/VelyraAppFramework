#pragma once

#include <VelyraAppFramework/AppInclude.hpp>

namespace Velyra::App::Widgets {

    class Popup {
    public:
        explicit Popup(const std::string& name);

        virtual ~Popup() = default;

        void draw();

        void setOpen(const bool open) { m_Open = open; }

        bool isOpen() const { return m_Open; }

    protected:

        virtual void drawContent() = 0;

        virtual void reset() {}

    private:
        const std::string m_Name;
        bool m_Open = false;
    };

}