//
// Created by Yuuki on 02/04/2025.
//
#pragma once
#include <string>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>

#define BTN_ROUNDED_RECTANGLE_RADIUS 10

namespace anisette::components {
    class Item {
    public:
        virtual ~Item() {
            SDL_DestroyTexture(texture);
        };
        virtual void draw(SDL_Renderer *renderer, SDL_Rect area, bool hovered, uint8_t opacity) = 0;
        bool reload_requested = false;
        SDL_Rect last_area {0, 0, 0, 0};

    protected:
        SDL_Texture *texture = nullptr;
        bool last_hover_state = false;
    };

    class TextButton final : public Item {
    public:
        TextButton(const std::string &text, const int size, const SDL_Color foreground, const SDL_Color background,
                   const SDL_Color hover) : text(text), foreground(foreground), background(background), hover_background(hover), font_size(size) {}

        void draw(SDL_Renderer *renderer, const SDL_Rect area, const bool hovered, const uint8_t opacity) override {
            if (!texture || last_hover_state != hovered || reload_requested) {
                last_hover_state = hovered;
                texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, area.w, area.h);
                if (!texture)
                    return;
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                SDL_SetRenderTarget(renderer, texture);
                // draw background
                int r = background.r, g = background.g, b = background.b, a = background.a;
                if (hovered) {
                    r = hover_background.r;
                    g = hover_background.g;
                    b = hover_background.b;
                    a = hover_background.a;
                }
                // roundedRectangleRGBA(renderer, area.x, area.y, area.x + area.w, area.y + area.h,
                //                      BTN_ROUNDED_RECTANGLE_RADIUS, r, g, b, a);
                SDL_SetRenderDrawColor(renderer, r, g, b, a);
                SDL_RenderClear(renderer);
                // draw text
                TTF_SetFontSize(core::video::primary_font, font_size);
                SDL_Surface *surface = TTF_RenderText_Blended(core::video::primary_font, text.c_str(), foreground);
                if (!surface)
                    return;
                // center text
                const SDL_Rect text_rect{(area.w - surface->w) / 2, (area.h - surface->h) / 2, surface->w, surface->h};
                const auto text_texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
                if (!text_texture) return;
                SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
                SDL_SetRenderTarget(renderer, texture);
                SDL_RenderCopy(renderer, text_texture, nullptr, &text_rect);
                SDL_DestroyTexture(text_texture);
            }
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_SetTextureAlphaMod(texture, opacity);
            SDL_RenderCopy(renderer, texture, nullptr, &area);
            last_area = area;
        }

        ~TextButton() override {
            SDL_DestroyTexture(texture);
        }

        std::string text;
    private:
        const SDL_Color foreground;
        const SDL_Color background;
        const SDL_Color hover_background;
        const int font_size;
    };

    class IconButton final : public Item {
    public:
        IconButton(const std::string &icon_path, const SDL_Color background, const SDL_Color hover) : background(background), hover_background(hover) {
            // load icon
            icon = IMG_Load(icon_path.c_str());
        }

        void draw(SDL_Renderer *renderer, const SDL_Rect area, const bool hovered, const uint8_t opacity) override {
            if (!texture || last_hover_state != hovered || reload_requested) {
                last_hover_state = hovered;
                if (!icon) return;
                texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, area.w, area.h);
                if (!texture) return;
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                SDL_SetRenderTarget(renderer, texture);
                // draw background
                int r = background.r, g = background.g, b = background.b, a = background.a;
                if (hovered) {
                    r = hover_background.r;
                    g = hover_background.g;
                    b = hover_background.b;
                    a = hover_background.a;
                }
                // roundedRectangleRGBA(renderer,
                //     area.x, area.y, area.x + area.w, area.y + area.h,
                //     BTN_ROUNDED_RECTANGLE_RADIUS, r, g, b, a
                //     );
                SDL_SetRenderDrawColor(renderer, r, g, b, a);
                SDL_RenderClear(renderer);
                // draw icon
                const double area_ratio = static_cast<double>(area.w) / area.h;
                const double icon_ratio = static_cast<double>(icon->w) / icon->h;
                SDL_Rect icon_rect{};
                if (icon_ratio > area_ratio) {
                    icon_rect.w = area.w;
                    icon_rect.h = icon->h * area.w / icon->w;
                    icon_rect.x = area.x;
                    icon_rect.y = area.y + (area.h - icon_rect.h) / 2;
                } else {
                    icon_rect.h = area.h;
                    icon_rect.w = icon->w * area.h / icon->h;
                    icon_rect.x = area.x + (area.w - icon_rect.w) / 2;
                    icon_rect.y = area.y;
                }
                // draw icon
                SDL_Texture *icon_texture = SDL_CreateTextureFromSurface(renderer, icon);
                if (!icon_texture) return;
                SDL_SetTextureBlendMode(icon_texture, SDL_BLENDMODE_BLEND);
                SDL_SetRenderTarget(renderer, texture);
                SDL_RenderCopy(renderer, icon_texture, nullptr, &icon_rect);
                SDL_DestroyTexture(icon_texture);
            }
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_SetTextureAlphaMod(texture, opacity);
            SDL_RenderCopy(renderer, texture, nullptr, &area);
            last_area = area;
        }

        ~IconButton() override {
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(icon);
        }

    private:
        SDL_Surface *icon = nullptr;
        SDL_Texture *texture = nullptr;
        const SDL_Color background;
        const SDL_Color hover_background;
    };

    class Text final : public Item {
    public:
        Text(const std::string &text, const int size, const SDL_Color foreground) : text(text), foreground(foreground), font_size(size) {}

        void draw(SDL_Renderer *renderer, SDL_Rect area, const bool hovered, const uint8_t opacity) override {
            if (!texture || reload_requested) {
                const auto surface = TTF_RenderText_Blended(core::video::secondary_font, text.c_str(), foreground);
                if (!surface) return;
                texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
                if (!texture) return;
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                SDL_QueryTexture(texture, nullptr, nullptr, nullptr, &src_h);
            }
            SDL_Rect src_rect{0, 0, src_h * area.w / area.h, src_h};
            SDL_SetTextureAlphaMod(texture, opacity);
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_RenderCopy(renderer, texture, &src_rect, &area);
            last_area = area;
        }

        ~Text() override {
            SDL_DestroyTexture(texture);
        }

        std::string text;
    private:
        int src_h = 0;
        SDL_Color foreground;
        const int font_size;
    };

    class Image final : public Item {
    public:
        explicit Image(const std::string &path) : path(path) {}

        void draw(SDL_Renderer *renderer, SDL_Rect area, const bool hovered, const uint8_t opacity) override {
            if (!texture || reload_requested) {
                texture = IMG_LoadTexture(renderer, path.c_str());
                if (!texture) return;
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
            }
            // calculate rect
            const double area_ratio = static_cast<double>(area.w) / area.h;
            const double icon_ratio = static_cast<double>(width) / height;
            SDL_Rect icon_rect{};
            if (icon_ratio > area_ratio) {
                icon_rect.w = area.w;
                icon_rect.h = height * area.w / width;
                icon_rect.x = area.x;
                icon_rect.y = area.y + (area.h - icon_rect.h) / 2;
            } else {
                icon_rect.h = area.h;
                icon_rect.w = width * area.h / height;
                icon_rect.x = area.x + (area.w - icon_rect.w) / 2;
                icon_rect.y = area.y;
            }
            SDL_SetTextureAlphaMod(texture, opacity);
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_RenderCopy(renderer, texture, nullptr, &icon_rect);
            last_area = area;
        }
        std::string path;
    private:
        int width = 0, height = 0;
    };
}