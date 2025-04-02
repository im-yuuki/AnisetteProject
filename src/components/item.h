//
// Created by Yuuki on 02/04/2025.
//
#include <string>
#include <functional>
#include <SDL2/SDL_render.h>

namespace anisette::components {
    class Item {
    public:
        virtual void draw(SDL_Renderer *renderer, SDL_Rect area, const bool hovered, const uint8_t opacity) = 0;
        virtual void on_click() = 0;
    };

    class Button : public Item {
    public:

    protected:

    };

    class TextButton final : public Button {
    public:
        TextButton(const std::string &text, const int size, SDL_Color foreground, SDL_Color background, SDL_Color hover);
        void draw(SDL_Renderer *renderer, SDL_Rect area, const bool hovered, const uint8_t opacity) override;

        void on_click() override {
            if (!callback) return;
            callback();
        };

        void set_callback(const std::function<void()> &fn) {
            this->callback = fn;
        }
    private:
        const SDL_Color foreground;
        const SDL_Color background;
        const SDL_Color hover;
        const std::string text;
        const int font_size;
        std::function<void()> callback = nullptr;
    };

    class IconButton final : public Button {
    public:
        IconButton(const std::string &icon_path, const int size, SDL_Color background, SDL_Color hover);
        void draw(SDL_Renderer *renderer, SDL_Rect area, const bool hovered, const uint8_t opacity) override;

        void on_click() override {
            if (!callback) return;
            callback();
        };

        void set_callback(const std::function<void()> &fn) {
            this->callback = fn;
        }
    private:
        const SDL_Color background;
        const SDL_Color hover;
        const std::string icon_path;
        std::function<void()> callback = nullptr;
    };

    class RadioButton final : public Button {
    public:
        RadioButton(const std::string &icon_path, const int size, SDL_Color color, SDL_Color hover);
        void draw(SDL_Renderer *renderer, SDL_Rect area, const bool hovered, const uint8_t opacity) override;
        void on_click() override;
    private:
        const SDL_Color color;
        const SDL_Color hover;
        const std::string icon_path;
        bool selected = false;
    };

    class Text : public Item {};
    class Icon : public Item {};
    class Image : public Item {};
}