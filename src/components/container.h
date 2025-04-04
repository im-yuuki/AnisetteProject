//
// Created by Yuuki on 02/04/2025.
//
#pragma once
#include "item.h"

#include <SDL2/SDL_render.h>
#include <ranges>
#include <vector>

namespace anisette::components {

    class Container {
    public:
        virtual ~Container() = default;
        virtual void draw(SDL_Renderer *renderer, SDL_Rect draw_rect) = 0;
        bool hidden = false;
    };

    class ItemWrapper final : public Container {
    public:
        explicit ItemWrapper(Item *item) : item(item) {}

        void draw(SDL_Renderer *renderer, const SDL_Rect draw_rect) override {
            if (hidden) return;
            int x, y;
            bool hovered = false;
            SDL_GetMouseState(&x, &y);
            // check if mouse is inside the item rect
            if (x >= draw_rect.x && x <= draw_rect.x + draw_rect.w && y >= draw_rect.y && y <= draw_rect.y + draw_rect.h) {
                hovered = true;
            }
            item->draw(renderer, draw_rect, hovered, 255);
        }

        ~ItemWrapper() override {
            delete item;
        }
    private:
        Item *item;
        const int current_zoom = 0;
    };

    struct GridChildProperties {
        typedef uint8_t Flag;
        static constexpr Flag POSITION_FREEFORM = 0;
        // 2 first bit store x position
        static constexpr Flag POSITION_X_LEFT   = 0b00000001;
        static constexpr Flag POSITION_X_RIGHT  = 0b00000010;
        static constexpr Flag POSITION_X_CENTER = 0b00000011;
        // 2 next bit store y position
        static constexpr Flag POSITION_Y_TOP    = 0b00000100;
        static constexpr Flag POSITION_Y_BOTTOM = 0b00001000;
        static constexpr Flag POSITION_Y_MIDDLE = 0b00001100;
        //
        static constexpr Flag CHILD_W_DYNAMIC   = 0b00010000;
        static constexpr Flag CHILD_H_DYNAMIC   = 0b00100000;
        // bit 6 make width of child draw rect relative to parent size
        // if set, children's target size will be overrided
        static constexpr Flag CHILD_W_RELATIVE  = 0b01000000;
        // the same for height on bit 7
        static constexpr Flag CHILD_H_RELATIVE  = 0b10000000;

        Flag flags = POSITION_FREEFORM;
        int x = 0, y = 0, width = 0, height = 0;
    };

    class Grid final : public Container {
    public:
        explicit Grid(const int padding = 0) : padding(padding) {}

        /**
         * @brief Add a child container to the grid
         *
         * Note that if you set CHILD_W_RELATIVE or CHILD_H_RELATIVE in flags, you must set width and height parameters
         * with values between 0 and 100 as percentages (%) of the parent size.
         *
         * @param child
         * @param flags
         * @param width
         * @param height
         * @param place_x
         * @param place_y
         */

        void add_child(Container *child, const GridChildProperties::Flag flags = GridChildProperties::POSITION_FREEFORM,
            const int width = 0, const int height = 0, const int place_x = 0, const int place_y = 0) {
            GridChildProperties child_properties;
            child_properties.flags = flags;
            child_properties.x = place_x;
            child_properties.y = place_y;
            child_properties.width = width;
            child_properties.height = height;
            children.emplace_back(child, child_properties);
        }

        void draw(SDL_Renderer *renderer, SDL_Rect draw_rect) override {
            // recalculate the parent rect for padding
            draw_rect.x += padding;
            draw_rect.y += padding;
            draw_rect.w -= padding;
            draw_rect.h -= padding;
            if (draw_rect.x < 0 || draw_rect.y < 0 || draw_rect.w <= 0 || draw_rect.h <= 0)
                return;
            for (const auto [item, properties]: children) {
                if (item->hidden) continue;
                SDL_Rect item_rect{};
                // calculate width
                if (properties.flags & GridChildProperties::POSITION_FREEFORM) {}
                if (properties.flags & GridChildProperties::CHILD_W_RELATIVE) {
                    item_rect.w = properties.width * draw_rect.w / 100;
                    if (item_rect.w < 0)
                        item_rect.w = 0;
                } else {
                    item_rect.w = properties.width;
                }
                // calculate height
                if (properties.flags & GridChildProperties::CHILD_H_RELATIVE) {
                    item_rect.h = properties.height * draw_rect.h / 100;
                    if (item_rect.h < 0)
                        item_rect.h = 0;
                } else {
                    item_rect.h = properties.height;
                }

                // calculate position
                switch (properties.flags & GridChildProperties::POSITION_X_CENTER) {
                    case GridChildProperties::POSITION_X_LEFT:
                        item_rect.x = draw_rect.x;
                        break;
                    case GridChildProperties::POSITION_X_RIGHT:
                        item_rect.x = draw_rect.x + draw_rect.w - item_rect.w;
                        break;
                    case GridChildProperties::POSITION_X_CENTER:
                        item_rect.x = draw_rect.x + (draw_rect.w - item_rect.w) / 2;
                        break;
                    default:
                        item_rect.x = properties.x;
                }
                switch (properties.flags & GridChildProperties::POSITION_Y_MIDDLE) {
                    case GridChildProperties::POSITION_Y_TOP:
                        item_rect.y = draw_rect.y;
                        break;
                    case GridChildProperties::POSITION_Y_BOTTOM:
                        item_rect.y = draw_rect.y + draw_rect.h - item_rect.h;
                        break;
                    case GridChildProperties::POSITION_Y_MIDDLE:
                        item_rect.y = draw_rect.y + (draw_rect.h - item_rect.h) / 2;
                        break;
                    default:
                        item_rect.y = properties.y;
                }
                // ensure item rect is inside the parent rect
                // if (item_rect.x < draw_rect.x) item_rect.x = draw_rect.x;
                // if (item_rect.y < draw_rect.y) item_rect.y = draw_rect.y;
                // if (item_rect.w < draw_rect.w) item_rect.w = draw_rect.w;
                // if (item_rect.h < draw_rect.h) item_rect.h = draw_rect.h;
                // draw the item
                item->draw(renderer, item_rect);
            }
        }

        std::vector<std::pair<Container*, GridChildProperties>> children;
    private:
        const int padding;
    };

    class FlexContainer : public Container {
    public:
        explicit FlexContainer(const int padding = 0, const int spacing = 10) : padding(padding), spacing(spacing) {}

        void add_item(Container *item, int fixed_size_percent = -1) {
            children.emplace_back(item, fixed_size_percent);
        }

        std::vector<std::pair<Container*, int>> children;
    protected:
        const int padding, spacing; // pixels
    };

    class VerticalBox final : public FlexContainer {
        void draw(SDL_Renderer *renderer, SDL_Rect draw_rect) override {
            draw_rect.x += padding;
            draw_rect.y += padding;
            draw_rect.w -= padding;
            draw_rect.h -= padding;
            if (draw_rect.x < 0 || draw_rect.y < 0 || draw_rect.w <= 0 || draw_rect.h <= 0) return;
            int fixed_height = 0, dynamic_item_count = 0;
            for (const auto &[item, fixed_size_percent]: children) {
                if (item->hidden) continue;
                if (fixed_size_percent > 0) fixed_height += fixed_size_percent * draw_rect.h / 100;
                else dynamic_item_count++;
                fixed_height += spacing;
            }
            fixed_height -= spacing;
            int dynamic_item_height = 0, y = 0;
            if (dynamic_item_count == 0) {
                y = (draw_rect.y - fixed_height) / 2;
                dynamic_item_height = 0;
            } else {
                dynamic_item_height = (draw_rect.h - fixed_height) / dynamic_item_count;
                y = draw_rect.y;
            }
            for (const auto &[item, fixed_size_percent]: children) {
                if (item->hidden) continue;
                SDL_Rect item_rect {draw_rect.x, y, draw_rect.w, 0};
                if (fixed_size_percent > 0) item_rect.h = fixed_size_percent * draw_rect.h / 100;
                else item_rect.h = dynamic_item_height;
                y = y + item_rect.h + spacing;
                // ensure item rect is inside the parent rect
                // if (item_rect.x < draw_rect.x) item_rect.x = draw_rect.x;
                // if (item_rect.y < draw_rect.y) item_rect.y = draw_rect.y;
                // if (item_rect.w < draw_rect.w) item_rect.w = draw_rect.w;
                // if (item_rect.h < draw_rect.h) item_rect.h = draw_rect.h;
                // draw the item
                item->draw(renderer, item_rect);
            }
        }
    };

    class HorizontalBox final : public FlexContainer {
    public:
        void draw(SDL_Renderer *renderer, SDL_Rect draw_rect) override {
            draw_rect.x += padding;
            draw_rect.y += padding;
            draw_rect.w -= padding;
            draw_rect.h -= padding;
            if (draw_rect.x < 0 || draw_rect.y < 0 || draw_rect.w <= 0 || draw_rect.h <= 0) return;
            int fixed_width = 0, dynamic_item_count = 0;
            for (const auto &[item, fixed_size_percent]: children) {
                if (item->hidden) continue;
                if (fixed_size_percent > 0) fixed_width += fixed_size_percent * draw_rect.w / 100;
                else dynamic_item_count++;
                fixed_width += spacing;
            }
            fixed_width -= spacing;
            int dynamic_item_width = 0, x = 0;
            if (dynamic_item_count == 0) {
                x = (draw_rect.x - fixed_width) / 2;
                dynamic_item_width = 0;
            } else {
                dynamic_item_width = (draw_rect.w - fixed_width) / dynamic_item_count;
                x = draw_rect.x;
            }
            for (const auto &[item, fixed_size_percent]: children) {
                if (item->hidden) continue;
                SDL_Rect item_rect {x, draw_rect.y, 0, draw_rect.h};
                if (fixed_size_percent > 0) item_rect.w = fixed_size_percent * draw_rect.w / 100;
                else item_rect.w = dynamic_item_width;
                x = x + item_rect.w + spacing;
                // ensure item rect is inside the parent rect
                // if (item_rect.x < draw_rect.x) item_rect.x = draw_rect.x;
                // if (item_rect.y < draw_rect.y) item_rect.y = draw_rect.y;
                // if (item_rect.w < draw_rect.w) item_rect.w = draw_rect.w;
                // if (item_rect.h < draw_rect.h) item_rect.h = draw_rect.h;
                // draw the item
                item->draw(renderer, item_rect);
            }
        }
    };
}