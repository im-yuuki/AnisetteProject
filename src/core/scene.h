//
// Created by Yuuki on 21/03/2025.
//

#pragma once
#include <functional>
#include <memory>
#include <string>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_events.h>

namespace anisette::core {
    class Scene {
    public:
        explicit Scene(SDL_Renderer *renderer) : renderer(renderer) {}
        virtual ~Scene() = default;

        virtual void on_focus() = 0;
        virtual void on_leave() = 0;
        virtual bool handle_event(const SDL_Event &event) = 0;
        virtual void handle_frame(const uint64_t &start_frame) = 0;
    protected:
        SDL_Renderer *renderer = nullptr;
    };

    class SceneFactory {
    public:
        static SceneFactory *instance() {
            static SceneFactory factory;
            return &factory;
        }
        using Creator = std::function<Scene*(SDL_Renderer*)>;

        void register_scene(const std::string& scene_name, Creator creator) {
            scene_registry.emplace(scene_name, std::move(creator));
        }

        Scene* create_scene(const std::string& scene_name, SDL_Renderer* renderer) {
            if (const auto it = scene_registry.find(scene_name); it != scene_registry.end()) {
                return it->second(renderer);
            }
            return nullptr;
        }

    private:
        SceneFactory() = default;
        std::unordered_map<std::string, Creator> scene_registry;
    };
}
