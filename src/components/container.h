//
// Created by Yuuki on 02/04/2025.
//
#include "item.h"

namespace anisette::components {

    class Container {};

    class ItemWrapper : public Container {};
    class Dropdown : public ItemWrapper {};

    class VerticalBox : public Container {};
    class HorizontalBox : public Container {};

    class VerticalScrollable : public Container {};
}