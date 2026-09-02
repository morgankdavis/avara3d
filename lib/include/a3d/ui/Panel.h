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
     * @brief Transient immediate-mode overlay panel for application controls and status.
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

        /** @brief Controls panel width and its inset from the upper-right viewport corner. */
        struct Options {
            float          width {260.0f}; ///< Panel width in logical UI units.
            float          margin {12.0f}; ///< Top and right viewport margin in logical UI units.

            /**
             * @brief Returns the Options value currently used as Panel's default argument.
             *
             * The current implementation returns zero width and margin; Panel rejects
             * a zero width, so callers should use Options{} or explicit options instead.
             */
            static Options Default() {
                return {0.0f, 0.0f};
            }
        };

        /** @brief Per-item padding in logical UI units. */
        struct Padding {
            float          top {0.0f}; ///< Padding above the item contents.
            float          bottom {0.0f}; ///< Padding below the item contents.
            float          left {0.0f}; ///< Padding to the left of the item contents.
            float          right {0.0f}; ///< Padding to the right of the item contents.

            /** @brief Returns zero padding on all sides. */
            static Padding Default() {
                return {0.0f, 0.0f, 0.0f, 0.0f};
            }
        };

        /** @brief Controls the optional separator line and capitalization of section headings. */
        struct SectionConfig {
            bool                 line {true}; ///< Draw a separator line beneath the heading when true.
            bool                 uppercase {false}; ///< Convert the displayed heading to uppercase when true.

            /** @brief Returns the default section-heading configuration. */
            static SectionConfig Default() {
                return {true, false};
            }
        };

        // [Public Lifecycle Functions]

        /**
         * @brief Begins an immediate-mode panel for the current frame.
         *
         * @p id is used to preserve UI identity between frames and should remain stable.
         * Panel must be created while an A3D UI frame is active, normally from
         * Application::frameDidBegin().
         *
         * @throws std::invalid_argument if @p id is empty or the supplied width or margin is invalid.
         * @throws std::logic_error if no active UI context or default A3D UI font is available.
         */
        explicit Panel(std::string_view id, const Options& options = Options::Default());

        Panel(const Panel&)            = delete;
        Panel& operator=(const Panel&) = delete;

        Panel(Panel&&)            = delete;
        Panel& operator=(Panel&&) = delete;

        ~Panel();

        // [Public Member Functions]

        /** @brief Draws a section heading with optional separator line, capitalization, and padding. */
        void section(std::string_view text,
                     SectionConfig    config  = SectionConfig::Default(),
                     Padding          padding = {12.0f, 4.0f, 0.0f, 0.0f});

        /** @brief Draws body text, wrapping it to the available item width. */
        void text(std::string_view text, Padding padding = Padding::Default());

        /** @brief Draws a left-aligned label and right-aligned textual value. */
        void value(std::string_view label, std::string_view value, Padding padding = Padding::Default());

        /**
         * @brief Inserts vertical space of @p height logical UI units.
         *
         * @throws std::invalid_argument if @p height is negative.
         */
        void spacer(float height);

        /**
         * @brief Arranges the next @p itemCount items horizontally with equal widths.
         *
         * @throws std::invalid_argument if @p itemCount is zero.
         * @throws std::logic_error if the previous row is incomplete.
         */
        void row(unsigned itemCount);

        /**
         * @brief Arranges the next @p itemCount items as a contiguous horizontal segmented row.
         *
         * @throws std::invalid_argument if @p itemCount is zero.
         * @throws std::logic_error if the previous row is incomplete.
         */
        void segmentedRow(unsigned itemCount);

        /**
         * @brief Draws a button and reports activation once.
         *
         * @throws std::invalid_argument if @p label is empty.
         */
        bool button(std::string_view label, Padding padding = {2.0f, 0.0f, 0.0f, 0.0f});

        /**
         * @brief Draws a selectable option button and reports activation once.
         *
         * @p selected controls the persistent selected appearance for this frame and
         * is not retained by Panel.
         *
         * @throws std::invalid_argument if @p label is empty.
         */
        bool option(std::string_view label, bool selected, Padding padding = {2.0f, 0.0f, 0.0f, 0.0f});

        /**
         * @brief Draws a visually subordinate selectable option button and reports activation once.
         *
         * @p selected controls the persistent selected appearance for this frame and
         * is not retained by Panel.
         *
         * @throws std::invalid_argument if @p label is empty.
         */
        bool subOption(std::string_view label, bool selected, Padding padding = {2.0f, 0.0f, 0.0f, 0.0f});

        /**
         * @brief Draws a floating-point slider and reports whether @p value changed.
         *
         * @throws std::invalid_argument if @p label is empty or the range is not increasing.
         */
        bool slider(std::string_view label,
                    float&           value,
                    float            minimum,
                    float            maximum,
                    std::string_view format  = "%.2f",
                    Padding          padding = {2.0f, 0.0f, 0.0f, 0.0f});

        /**
         * @brief Draws an integer slider and reports whether @p value changed.
         *
         * @throws std::invalid_argument if @p label is empty or @p minimum is not less than @p maximum.
         */
        bool slider(std::string_view label,
                    int&             value,
                    int              minimum,
                    int              maximum,
                    std::string_view format  = "%d",
                    Padding          padding = {2.0f, 0.0f, 0.0f, 0.0f});

        /**
         * @brief Draws a boolean toggle and reports whether @p value changed.
         *
         * @throws std::invalid_argument if @p label is empty.
         */
        bool toggle(std::string_view label, bool& value, Padding padding = Padding::Default());

        /** @brief Returns whether the pointer is hovering the panel during the current frame. */
        bool hovered() const;

    private:
        // [Private Member Functions]

        float       beginItem() const;
        void        endItem();
        bool        drawButton(std::string_view label, bool selected, Padding padding);

        // [Private Member Variables]

        std::string _windowName;
        unsigned    _rowItemsRemaining;
        unsigned    _rowItemCount;
        float       _rowItemWidth;
        float       _rowSpacing;
        bool        _segmentedRow;
        bool        _visible;
        bool        _hovered;
    };

}

#endif // AVARA3D_UI_PANEL_H
