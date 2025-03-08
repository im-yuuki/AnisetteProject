//
// Created by Yuuki on 19/02/2025.
//

#pragma once
#include <string>
#include <vector>
#include <utility>

namespace game::core {
    class PreferencesLoader {
    public:
        PreferencesLoader() = default;
        virtual ~PreferencesLoader() = default;
        bool save();
    protected:
        virtual bool _import(std::pair<std::string, std::string> pair) = 0;
        virtual std::vector<std::pair<std::string, std::string>> _export() = 0;
        [[nodiscard]] bool read(const char* file_path);
        [[nodiscard]] bool write(const char* file_path);
    };

    class RenderConfigLoader final : protected PreferencesLoader {
    public:
        unsigned int render_width = 1280;
        unsigned int render_height = 720;
        bool full_screen = true;

        RenderConfigLoader();
        bool _import(std::pair<std::string, std::string> pair) override;
        std::vector<std::pair<std::string, std::string>> _export() override;
        [[nodiscard]] bool is_using_fallback_mode() const;
    private:
        bool fallback_mode = true;
    };
}
