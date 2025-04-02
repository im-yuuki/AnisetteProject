//
// Created by Yuuki on 02/04/2025.
//
#include "item.h"

namespace anisette::components {
    TextButton::TextButton(const std::string &text, const int size, SDL_Color foreground, SDL_Color background, SDL_Color hover)
                : foreground(foreground), background(background), hover(hover), text(text), font_size(size) {}


    void TextButton::draw(SDL_Renderer *renderer, SDL_Rect area, const int hovered) {
    }

}