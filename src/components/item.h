//
// Created by Yuuki on 02/04/2025.
//
#pragma once
#include "core.h"
#include <string>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define ROUNDED_RECTANGLE_RADIUS 5

namespace anisette::components {
    class Item {
    public:
        virtual ~Item() {
            SDL_DestroyTexture(texture);
        };

        virtual void draw(SDL_Renderer *renderer, SDL_Rect area, bool hovered) = 0;

        uint8_t alpha = 255;
        SDL_Rect last_area {-1, -1, 0, 0};

    protected:
        SDL_Texture *texture = nullptr;
        bool last_hover_state = false;
        bool init_finished = false;
    };

    class TextButton final : public Item {
    public:
        TextButton(const std::string &text, const int size, const SDL_Color foreground, const SDL_Color background, const SDL_Color hover)
            : foreground(foreground), background(background), hover_background(hover), font_size(size), text(text) {}


        void draw(SDL_Renderer *renderer, const SDL_Rect area, const bool hovered) override {
            if (!init_finished) {
                // load text texture
                TTF_SetFontSize(core::video::primary_font, font_size);
                const auto surface = TTF_RenderText_Blended(core::video::primary_font, text.c_str(), foreground);
                if (!surface) return;
                text_texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
                if (!text_texture) return;
                SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
                SDL_QueryTexture(text_texture, nullptr, nullptr, &text_w, &text_h);
                init_finished = true;
            }

            if (last_area.w != area.w || last_area.h != area.h) {
                SDL_DestroyTexture(texture);
                texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, area.w, area.h);
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                // force trigger draw
                last_hover_state = !hovered;
            }

            if (!texture) return;

            if (last_hover_state != hovered) {
                last_hover_state = hovered;
                SDL_SetRenderTarget(renderer, texture);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                SDL_RenderClear(renderer);
                // draw background
                int r = background.r, g = background.g, b = background.b, a = background.a;
                if (hovered) {
                    r = hover_background.r;
                    g = hover_background.g;
                    b = hover_background.b;
                    a = hover_background.a;
                }
                // width and height -2 to have border inside
                roundedBoxRGBA(renderer, 0, 0, area.w - 2, area.h - 2, ROUNDED_RECTANGLE_RADIUS, r, g, b, a);
                // draw text
                if (!text_texture) return;
                const SDL_Rect text_rect {(area.w - text_w) / 2, (area.h - text_h) / 2, text_w, text_h};
                SDL_SetRenderTarget(renderer, texture);
                SDL_RenderCopy(renderer, text_texture, nullptr, &text_rect);
            }
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_SetTextureAlphaMod(texture, alpha);
            SDL_RenderCopy(renderer, texture, nullptr, &area);
            last_area = area;
        }

        ~TextButton() override {
            SDL_DestroyTexture(texture);
            SDL_DestroyTexture(text_texture);
        }

        SDL_Color foreground;
        SDL_Color background;
        SDL_Color hover_background;
    private:
        const int font_size;
        const std::string text;
        int text_w = 0, text_h = 0;
        SDL_Texture *text_texture = nullptr;
        SDL_Texture *texture = nullptr;
    };

    class IconButton final : public Item {
    public:
        IconButton(const std::string &icon_path, const SDL_Color background, const SDL_Color hover)
            : background(background), hover_background(hover), icon_path(icon_path) {}

        void draw(SDL_Renderer *renderer, const SDL_Rect area, const bool hovered) override {
            if (!init_finished) {
                // load text texture
                icon = IMG_LoadTexture(renderer, icon_path.c_str());
                if (icon) {
                    SDL_SetTextureBlendMode(icon, SDL_BLENDMODE_BLEND);
                    SDL_QueryTexture(icon, nullptr, nullptr, &icon_w, &icon_h);
                }
                init_finished = true;
            }

            if (last_area.w != area.w || last_area.h != area.h) {
                SDL_DestroyTexture(texture);
                texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, area.w, area.h);
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                // force trigger draw
                last_hover_state = !hovered;
            }

            if (!texture) return;

            if (last_hover_state != hovered) {
                last_hover_state = hovered;
                SDL_SetRenderTarget(renderer, texture);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                SDL_RenderClear(renderer);
                // draw background
                int r = background.r, g = background.g, b = background.b, a = background.a;
                if (hovered) {
                    r = hover_background.r;
                    g = hover_background.g;
                    b = hover_background.b;
                    a = hover_background.a;
                }
                roundedBoxRGBA(renderer, 0, 0,  area.w - 2, area.h - 2, ROUNDED_RECTANGLE_RADIUS, r, g, b, a);
                // draw icon
                if (!icon) return;
                const double area_ratio = static_cast<double>(area.w) / area.h;
                const double icon_ratio = static_cast<double>(icon_w) / icon_h;
                SDL_Rect icon_rect{0, 0, 0, 0};
                if (icon_ratio > area_ratio) {
                    icon_rect.w = area.w;
                    icon_rect.h = icon_h * area.w / icon_w;
                    icon_rect.y = (area.h - icon_rect.h) / 2;
                } else {
                    icon_rect.h = area.h;
                    icon_rect.w = icon_w * area.h / icon_h;
                    icon_rect.x = (area.w - icon_rect.w) / 2;
                }
                SDL_RenderCopy(renderer, icon, nullptr, &icon_rect);
            }
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_SetTextureAlphaMod(texture, alpha);
            SDL_RenderCopy(renderer, texture, nullptr, &area);
            last_area = area;
        }

        ~IconButton() override {
            SDL_DestroyTexture(texture);
            SDL_DestroyTexture(icon);
        }

        SDL_Color background;
        SDL_Color hover_background;
    private:
        const std::string icon_path;
        int icon_w = 0, icon_h = 0;
        SDL_Texture *icon = nullptr;
        SDL_Texture *texture = nullptr;
    };

    class Text final : public Item {
    public:
        Text(const std::string &text, const int size, const SDL_Color foreground) : text(text), foreground(foreground), font_size(size) {}

        void draw(SDL_Renderer *renderer, SDL_Rect area, const bool hovered) override {
            if (!init_finished) {
                SDL_DestroyTexture(texture);
                TTF_SetFontSize(core::video::secondary_font, font_size);
                const auto surface = TTF_RenderText_Blended(core::video::secondary_font, text.c_str(), foreground);
                if (!surface) return;
                texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
                if (!texture) return;
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                SDL_QueryTexture(texture, nullptr, nullptr, &text_w, &text_h);
                init_finished = true;
            }
            if (!texture) return;
            if (text_w < area.w) {
                area.w = text_w;
            }
            if (text_h < area.h) {
                area.y += (area.h - text_h) / 2;
                area.h = text_h;
            }
            SDL_SetTextureAlphaMod(texture, alpha);
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_RenderCopy(renderer, texture, nullptr, &area);
        }

        ~Text() override {
            SDL_DestroyTexture(texture);
        }

        void change_text(const std::string &new_text) {
            if (new_text == text) return;
            text = new_text;
            init_finished = false;
        }

    private:
        std::string text;
        SDL_Color foreground;
        const int font_size;
        int text_w = 0, text_h = 0;
    };

    class Image final : public Item {
    public:
        explicit Image(const std::string &path) : path(path) {}

        void draw(SDL_Renderer *renderer, SDL_Rect area, const bool hovered) override {
            if (!init_finished) {
                texture = IMG_LoadTexture(renderer, path.c_str());
                if (!texture) return;
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                SDL_QueryTexture(texture, nullptr, nullptr, &img_w, &img_h);
                init_finished = true;
            }
            if (!texture) return;
            const double area_ratio = static_cast<double>(area.w) / area.h;
            const double img_ratio = static_cast<double>(img_w) / img_h;
            if (img_ratio > area_ratio) {
                area.y += (area.y - img_h * area.w / img_w) / 2;
                area.h = img_h * area.w / img_w;
            } else {
                area.x += (area.w - img_w * area.h / img_h) / 2;
                area.w = img_w * area.h / img_h;
            }
            SDL_SetTextureAlphaMod(texture, alpha);
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_RenderCopy(renderer, texture, nullptr, &area);
        }
    private:
        const std::string path;
        int img_w = 0, img_h = 0;
    };

    class ProgressBar final : public Item {
    public:
        ProgressBar(const int base, const SDL_Color foreground, const SDL_Color background)
            : base(base), foreground(foreground), background(background) {}
        const int base;
        int value = 0;
        SDL_Color foreground;
        SDL_Color background;

        void draw(SDL_Renderer *renderer, const SDL_Rect area, const bool hovered) override {
            // draw background
            roundedBoxRGBA(renderer, area.x, area.y, area.x + area.w - 2, area.y + area.h - 2, ROUNDED_RECTANGLE_RADIUS, background.r, background.g, background.b, background.a);
            // draw progress
            const int progress_w = area.w * value / base;
            roundedBoxRGBA(renderer, area.x, area.y, area.x + progress_w - 2, area.y + area.h - 2, ROUNDED_RECTANGLE_RADIUS, foreground.r, foreground.g, foreground.b, foreground.a);
        }
    };
}