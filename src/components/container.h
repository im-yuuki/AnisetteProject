//
// Created by Yuuki on 02/04/2025.
//
#pragma once
#include "item.h"
#include "common.h"

#include <SDL2/SDL_render.h>
#include <ranges>
#include <vector>

namespace anisette::components {

    class Container {
    public:
        virtual ~Container() = default;
        virtual void draw(SDL_Renderer *renderer, SDL_Rect draw_rect, uint8_t alpha = 255) = 0;
        virtual void set_hidden(const bool state) = 0;

        // DO NOT MODIFY THIS VALUE DIRECTLY
        bool hidden = false;
        SDL_Color fill_color = {0, 0, 0, 0};
        virtual void hide_draw_rect() {};
    };

    class BlankContainer final : public Container {
    public:
        BlankContainer() {};
        // a container with... nothing, to use as flexbox dynamic space
        void draw(SDL_Renderer *renderer, SDL_Rect draw_rect, uint8_t alpha = 255) override {}
        void set_hidden(const bool state) override { hidden = state; }
    };

    class ItemWrapper final : public Container {
    public:
        explicit ItemWrapper(Item *item) : Container(), item(item) {}

        void draw(SDL_Renderer *renderer, const SDL_Rect draw_rect, const uint8_t alpha = 255) override {
            if (hidden) return;
            if (fill_color.r || fill_color.g || fill_color.b || fill_color.a) {
                SDL_SetRenderTarget(renderer, nullptr);
                SDL_SetRenderDrawColor(renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(renderer, &draw_rect);
            }
            int x, y;
            SDL_GetMouseState(&x, &y);
            item->alpha = alpha;
            item->draw(renderer, draw_rect, utils::check_point_in_rect(x, y, draw_rect));
        }

        void set_hidden(const bool state) override {
            item->last_area = {-1, -1, 0, 0};
            hidden = state;
        }

        ~ItemWrapper() override {
            delete item;
        }

        void hide_draw_rect() override {
            item->last_area = {-1, -1, 0, 0};
            hidden = true;
        }
    private:
        Item *item;
    };

    struct GridChildProperties {
        enum PositionX { LEFT, CENTER, RIGHT };
        enum PositionY { TOP, MIDDLE, BOTTOM };
        PositionX x = CENTER;
        PositionY y = MIDDLE;
        uint8_t width_perc = 0, height_perc = 0;

        GridChildProperties() = default;
        GridChildProperties(const PositionX position_x, const PositionY position_y, const int width, const int height) {
            x = position_x;
            y = position_y;
            width_perc = width;
            height_perc = height;
        }
    };

    class Grid final : public Container {
    public:
        explicit Grid(const uint8_t padding = 0, const SDL_Color fill_color = {0, 0, 0, 0}) : padding(padding) {
            this->fill_color = fill_color;
        }

        Grid* add_child(Container *child, const GridChildProperties::PositionX x, const GridChildProperties::PositionY y, const uint8_t width_perc = 0, const uint8_t height_perc = 0) {
            GridChildProperties child_properties {x, y, width_perc, height_perc};
            children.emplace_back(child, child_properties);
            return this;
        }

        void draw(SDL_Renderer *renderer, SDL_Rect draw_rect, const uint8_t alpha) override {
            if (hidden) return;
            // draw background color
            if (fill_color.r || fill_color.g || fill_color.b || fill_color.a) {
                SDL_SetRenderTarget(renderer, nullptr);
                SDL_SetRenderDrawColor(renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(renderer, &draw_rect);
            }

            // recalculate the parent rect for padding
            int base_size = draw_rect.w < draw_rect.h ? draw_rect.w : draw_rect.h;
            if (padding > 0) {
                draw_rect.x += base_size * padding / 200;
                draw_rect.y += base_size * padding / 200;
                draw_rect.w -= base_size * padding / 100;
                draw_rect.h -= base_size * padding / 100;
                base_size = draw_rect.w < draw_rect.h ? draw_rect.w : draw_rect.h;
            }
            // check for invalid rect
            if (draw_rect.w <= 0 || draw_rect.h <= 0) return;

            for (const auto [item, properties]: children) {
                if (item->hidden) continue;
                SDL_Rect item_rect{};
                // calculate size
                item_rect.w = properties.width_perc == 255 ? draw_rect.w : properties.width_perc * base_size / 100;
                item_rect.h = properties.height_perc == 255 ? draw_rect.h : properties.height_perc * base_size / 100;

                // calculate position
                switch (properties.x) {
                    case GridChildProperties::LEFT:
                        item_rect.x = draw_rect.x;
                        break;
                    case GridChildProperties::RIGHT:
                        item_rect.x = draw_rect.x + draw_rect.w - item_rect.w;
                        break;

                    default:
                    case GridChildProperties::CENTER:
                        item_rect.x = draw_rect.x + (draw_rect.w - item_rect.w) / 2;
                }
                switch (properties.y) {
                    case GridChildProperties::TOP:
                        item_rect.y = draw_rect.y;
                        break;
                    case GridChildProperties::BOTTOM:
                        item_rect.y = draw_rect.y + draw_rect.h - item_rect.h;
                        break;

                    default:
                    case GridChildProperties::MIDDLE:
                        item_rect.y = draw_rect.y + (draw_rect.h - item_rect.h) / 2;
                }
                // ensure item rect is inside the parent rect
                if (item_rect.x < draw_rect.x) item_rect.x = draw_rect.x;
                if (item_rect.y < draw_rect.y) item_rect.y = draw_rect.y;
                if (item_rect.w > draw_rect.w) item_rect.w = draw_rect.w;
                if (item_rect.h > draw_rect.h) item_rect.h = draw_rect.h;
                // draw the item
                item->draw(renderer, item_rect, alpha);
            }
        }

        void set_hidden(const bool state) override {
            hidden = state;
        }

        void hide_draw_rect() override {
            for (const auto &item: children | std::views::keys) {
                item->hide_draw_rect();
            }
        }

        ~Grid() override {
            for (const auto &item: children | std::views::keys) {
                delete item;
            }
        }

        std::vector<std::pair<Container*, GridChildProperties>> children;
    private:
        const uint8_t padding;

    };

    class FlexContainer : public Container {
    public:
        // padding and spacing are percentages of the smallest dimension of the parent rect
        explicit FlexContainer(const int padding = 0, const int spacing = 1) : padding(padding), spacing(spacing) {}

        FlexContainer* add_item(Container *item, uint8_t fixed_size_perc = 0) {
            children.emplace_back(item, fixed_size_perc);
            return this;
        }

        void set_hidden(const bool state) override {
            hidden = state;
        }

        ~FlexContainer() override {
            for (const auto &item: children | std::views::keys) delete item;
        }

        std::vector<std::pair<Container*, uint8_t>> children;
    protected:
        const int padding, spacing;
        void hide_draw_rect() override {
            for (const auto &item: children | std::views::keys) {
                item->hide_draw_rect();
            }
        }
    };

    class VerticalBox final : public FlexContainer {
    public:
        explicit VerticalBox(const int padding = 0, const int spacing = 1) : FlexContainer(padding, spacing) {}

        void draw(SDL_Renderer *renderer, SDL_Rect draw_rect, const uint8_t alpha = 255) override {
            if (hidden) return;
            // draw background color
            if (fill_color.r || fill_color.g || fill_color.b || fill_color.a) {
                SDL_SetRenderTarget(renderer, nullptr);
                SDL_SetRenderDrawColor(renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(renderer, &draw_rect);
            }

            // resize parent rect for padding
            if (padding > 0) {
                const int padding_px = (draw_rect.w < draw_rect.h ? draw_rect.w : draw_rect.h) * padding / 200;
                draw_rect.x += padding_px;
                draw_rect.y += padding_px;
                draw_rect.w -= padding_px * 2;
                draw_rect.h -= padding_px * 2;
            }
            // check for invalid rect
            if (draw_rect.w <= 0 || draw_rect.h <= 0) return;

            // count total size of fixed height items
            const int base_size = draw_rect.h;
            int item_count = 0, fixed_height_perc = 0, dynamic_item_count = 0;
            for (const auto &[item, h_percent] : children) {
                if (item->hidden) continue;
                item_count++;
                if (h_percent != 0) fixed_height_perc += h_percent;
                else dynamic_item_count++;
                fixed_height_perc += spacing;
            }
            if (item_count > 0) fixed_height_perc -= spacing;
            // calculate dynamic item size
            int dynamic_item_px = 0, y = draw_rect.y;
            if (dynamic_item_count == 0) {
                y += (100 - fixed_height_perc) * base_size / 200;
            } else {
                dynamic_item_px = (100 - fixed_height_perc) * base_size / 100 / dynamic_item_count;
                // show no dynamic item if its size is negative
                if (dynamic_item_px < 0) dynamic_item_px = 0;
            }
            for (const auto &[item, h_percent]: children) {
                if (item->hidden) continue;
                SDL_Rect item_rect {draw_rect.x, y, draw_rect.w, 0};
                // child height calculation
                if (h_percent != 0) item_rect.h = h_percent * base_size / 100;
                else if (dynamic_item_px == 0) continue;
                else item_rect.h = dynamic_item_px;
                // y for next item
                y = y + item_rect.h + spacing * base_size / 100;
                // ensure item rect is inside the parent rect
                if (item_rect.x < draw_rect.x) item_rect.x = draw_rect.x;
                if (item_rect.y < draw_rect.y) item_rect.y = draw_rect.y;
                if (item_rect.w > draw_rect.w) item_rect.w = draw_rect.w;
                if (item_rect.h > draw_rect.h) item_rect.h = draw_rect.h;
                // draw item
                item->draw(renderer, item_rect, alpha);
            }
        }
    };

    class HorizontalBox final : public FlexContainer {
    public:
        explicit HorizontalBox(const int padding = 0, const int spacing = 1) : FlexContainer(padding, spacing) {}

        void draw(SDL_Renderer *renderer, SDL_Rect draw_rect, const uint8_t alpha = 255) override {
            if (hidden) return;
            // draw background color
            if (fill_color.r || fill_color.g || fill_color.b || fill_color.a) {
                SDL_SetRenderTarget(renderer, nullptr);
                SDL_SetRenderDrawColor(renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(renderer, &draw_rect);
            }

            // resize parent rect for padding
            if (padding > 0) {
                const int padding_px = (draw_rect.w < draw_rect.h ? draw_rect.w : draw_rect.h) * padding / 200;
                draw_rect.x += padding_px;
                draw_rect.y += padding_px;
                draw_rect.w -= padding_px * 2;
                draw_rect.h -= padding_px * 2;
            }
            // check for invalid rect
            if (draw_rect.w <= 0 || draw_rect.h <= 0) return;

            // count total size of fixed width items
            const int base_size = draw_rect.w;
            int item_count = 0, fixed_width_perc = 0, dynamic_item_count = 0;
            for (const auto &[item, w_percent] : children) {
                if (item->hidden) continue;
                item_count ++;
                if (w_percent != 0) fixed_width_perc += w_percent;
                else dynamic_item_count++;
                fixed_width_perc += spacing;
            }
            if (item_count > 0) fixed_width_perc -= spacing;
            // calculate dynamic item size
            int dynamic_item_px = 0, x = draw_rect.x;
            if (dynamic_item_count == 0) {
                x += (100 - fixed_width_perc) * base_size / 200;
            } else {
                dynamic_item_px = (100 - fixed_width_perc) * base_size / 100 / dynamic_item_count;
                // show no dynamic item if its size is negative
                if (dynamic_item_px < 0) dynamic_item_px = 0;
            }
            for (const auto &[item, w_percent]: children) {
                if (item->hidden) continue;
                SDL_Rect item_rect {x, draw_rect.y, 0, draw_rect.h};
                // child width calculation
                if (w_percent != 0) item_rect.w = w_percent * base_size / 100;
                else if (dynamic_item_px == 0) continue;
                else item_rect.w = dynamic_item_px;
                // x for next item
                x = x + item_rect.w + spacing * base_size / 100;
                // ensure item rect is inside the parent rect
                if (item_rect.x < draw_rect.x) item_rect.x = draw_rect.x;
                if (item_rect.y < draw_rect.y) item_rect.y = draw_rect.y;
                if (item_rect.w > draw_rect.w) item_rect.w = draw_rect.w;
                if (item_rect.h > draw_rect.h) item_rect.h = draw_rect.h;
                // draw item
                item->draw(renderer, item_rect, alpha);
            }
        }
    };
}