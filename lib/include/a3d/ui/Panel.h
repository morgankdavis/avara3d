//
//  Panel.h
//  avara3d
//
//  Created by Morgan Davis on 8/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UI_PANEL_H
#define AVARA3D_UI_PANEL_H

#include <string>
#include <string_view>

namespace a3d::ui {

    struct PanelOptions {
        float width {260.0f};
        float margin {12.0f};
    };

    /**
     * A transient immediate-mode overlay panel.
     *
     * Construct a Panel during Application::frameDidBegin(), emit its contents,
     * and allow it to be destroyed before returning from the callback. The id
     * must remain stable between frames so interactive items retain stable
     * identities.
     *
     * Items are arranged in a vertical stack by default. row() arranges the
     * specified number of subsequent items horizontally with equal widths.
     */
    class Panel {

    public:
        /// Public Lifecycle Functions ///

        explicit Panel(std::string_view id, const PanelOptions& options = {});

        Panel(const Panel&)            = delete;
        Panel& operator=(const Panel&) = delete;

        Panel(Panel&&)            = delete;
        Panel& operator=(Panel&&) = delete;

        ~Panel();

        /// Public Member Functions ///

        void section(std::string_view text);

        void text(std::string_view text);
        void value(std::string_view label, std::string_view value);

        void spacer(float height);

        /**
         * Arranges the next itemCount items horizontally with equal widths.
         *
         * @throws std::invalid_argument if itemCount is zero.
         * @throws std::logic_error if the previous row is incomplete.
         */
        void row(unsigned itemCount);

        /** Draws a button and returns true once when it is activated. */
        bool button(std::string_view label);

        /**
         * Draws a selectable option button and returns true once when activated.
         *
         * selected is supplied by the application each frame and controls only
         * the persistent selected appearance. It is not retained by Panel.
         */
        bool option(std::string_view label, bool selected);

        /**
         * Draws a floating-point slider and returns true when value changed.
         */
        bool slider(std::string_view label,
                    float&           value,
                    float            minimum,
                    float            maximum,
                    std::string_view format = "%.2f");

        /** Draws a boolean toggle and returns true when value changed. */
        bool toggle(std::string_view label, bool& value);

    private:
        /// Private Member Functions ///

        float       beginItem() const;
        void        endItem();
        bool        drawButton(std::string_view label, bool selected);

        /// Private Member Variables ///

        std::string _windowName;
        unsigned    _rowItemsRemaining;
        float       _rowItemWidth;
        float       _rowSpacing;
        bool        _visible;
    };

}

#endif // AVARA3D_UI_PANEL_H
