//
//  Panel.h
//  avara3d
//
//  Created by Morgan Davis on 8/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UI_PANEL_H
#define AVARA3D_UI_PANEL_H

#include <optional>
#include <string>
#include <string_view>

namespace a3d::ui {

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
        // [Public Types]

        struct Options {
            float          width {260.0f};
            float          margin {12.0f};

            static Options Default() {
                return {0.0f, 0.0f};
            }
        };

        struct Padding {
            float          top {0.0f};
            float          bottom {0.0f};
            float          left {0.0f};
            float          right {0.0f};

            static Padding Default() {
                return {0.0f, 0.0f, 0.0f, 0.0f};
            }
        };

        struct SectionConfig {
            bool                 line {true};
            bool                 uppercase {false};

            static SectionConfig Default() {
                return {true, false};
            }
        };

        // [Public Lifecycle Functions]

        explicit Panel(std::string_view id, const Options& options = Options::Default());

        Panel(const Panel&)            = delete;
        Panel& operator=(const Panel&) = delete;

        Panel(Panel&&)            = delete;
        Panel& operator=(Panel&&) = delete;

        ~Panel();

        // [Public Member Functions]

        void section(std::string_view text,
                     SectionConfig    config  = SectionConfig::Default(),
                     Padding          padding = {12.0f, 4.0f, 0.0f, 0.0f});

        void text(std::string_view text, Padding padding = Padding::Default());
        void value(std::string_view label, std::string_view value, Padding padding = Padding::Default());

        void spacer(float height);

        /**
         * Arranges the next itemCount items horizontally with equal widths.
         *
         * @throws std::invalid_argument if itemCount is zero.
         * @throws std::logic_error if the previous row is incomplete.
         */
        void row(unsigned itemCount);

        /** Draws a button and returns true once when it is activated. */
        bool button(std::string_view label, Padding padding = {2.0f, 0.0f, 0.0f, 0.0f});

        /**
         * Draws a selectable option button and returns true once when activated.
         *
         * selected is supplied by the application each frame and controls only
         * the persistent selected appearance. It is not retained by Panel.
         */
        bool option(std::string_view label, bool selected, Padding padding = {2.0f, 0.0f, 0.0f, 0.0f});

        /**
         * Draws a floating-point slider and returns true when value changed.
         */
        bool slider(std::string_view label,
                    float&           value,
                    float            minimum,
                    float            maximum,
                    std::string_view format  = "%.2f",
                    Padding          padding = {2.0f, 0.0f, 0.0f, 0.0f});

        /**
     * Draws aa integer slider and returns true when value changed.
     */
        bool slider(std::string_view label,
                    int&             value,
                    int              minimum,
                    int              maximum,
                    std::string_view format  = "%d",
                    Padding          padding = {2.0f, 0.0f, 0.0f, 0.0f});

        /** Draws a boolean toggle and returns true when value changed. */
        bool toggle(std::string_view label, bool& value, Padding padding = Padding::Default());

        bool hovered() const;

    private:
        // [Private Member Functions]

        float       beginItem() const;
        void        endItem();
        bool        drawButton(std::string_view label, bool selected, Padding padding);

        // [Private Member Variables]

        std::string _windowName;
        unsigned    _rowItemsRemaining;
        float       _rowItemWidth;
        float       _rowSpacing;
        bool        _visible;
        bool        _hovered;
    };

}

#endif // AVARA3D_UI_PANEL_H
