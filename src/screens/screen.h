//
// Created by Yuuki on 18/03/2025.
//
#pragma once

class Screen {
public:
    virtual void focus_enter() = 0;
    virtual void focus_leave() = 0;
};