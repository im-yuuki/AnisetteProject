//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include <thread>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

namespace anisette::core {
    /**
     * @brief Start the game core
     *
     * This function initializes the game core and starts the main game loop.
     * It processes command-line arguments and sets up necessary resources.
     *
     * @return Exit code, 0 for success, otherwise errors
     */
    int run();

    class ThreadModule {
    public:
        virtual ~ThreadModule() = default;
        void start_thread() {
            if (thread_instance != nullptr) return;
            stop_flag = false;
            thread_instance = new std::thread([this] { thread(); });
        }
        void stop_thread() {
            stop_flag = true;
        }
        void wait_for_thread_stop() const {
            if (thread_instance == nullptr) return;
            thread_instance->join();
            delete thread_instance;
        }
    protected:
        std::thread* thread_instance = nullptr;
        virtual void thread() = 0;
        std::atomic<bool> stop_flag{false};
    };

    class VideoModule final : public ThreadModule {
    public:
        VideoModule();
        ~VideoModule() override;
    private:
        SDL_Window *window;
        SDL_Renderer *renderer;

        void thread() override;
        void configure_window() const;
        void configure_renderer();
    };

    class AudioModule final {
    public:
        AudioModule();
        ~AudioModule();
    };

    class EventModule final : public ThreadModule {
    public:
        EventModule();
        ~EventModule() override;
    private:
        void thread() override;
    };
}