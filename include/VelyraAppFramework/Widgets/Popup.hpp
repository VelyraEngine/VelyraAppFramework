#pragma once

#include <VelyraAppFramework/Widgets/IPanel.hpp>

namespace Velyra::App::Widgets {

    using PopupID = UID;

    class Popup: public IPanel {
    public:
        Popup(AppData& appData, const std::string& name, PopupID popupID);

        ~Popup() override = default;

        void draw(Core::Window& window, Core::Context& context);

        void setOpen(bool open);

        /**
         * @brief Draws a green button with default text "Confirm". The valid boolean allows you to control if the
         *        confirm button should be clickable, great when you want to do some input validation.
         * @param window
         * @param context
         * @param confirmButtonLabel
         * @param cancelButtonLabel
         * @param valid
         */
        void drawConfirmationButtons(Core::Window& window, Core::Context& context, const std::string& confirmButtonLabel = "Confirm",
                                     const std::string& cancelButtonLabel = "Cancel", bool valid = true);

        bool isOpen() const { return m_Open; }

        PopupID getID() const { return m_PopupID; }

    protected:

        /**
         * @brief Called when the popup is closed when the user presses on the X-button in the top right corner.
         */
        virtual void reset() {}

        /**
         * @brief Called when setOpen(open = true) is called.
         */
        virtual void onOpen() {}

        /**
         * @brief Called when setOpen(open = false) is called.
         */
        virtual void onClose() {}

        /**
         * @brief Called when the user presses on the confirm button.
         */
        virtual void onConfirm(Core::Window& /*window*/, Core::Context& /*context*/) {}

        /**
         * @brief Called when the user presses on cancel.
         */
        virtual void onCancel(Core::Window& /*window*/, Core::Context& /*context*/) {}

    private:
        const PopupID m_PopupID;
        bool m_Open = false;
    };

}