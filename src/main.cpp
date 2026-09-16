#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

#include "SDL3/SDL_timer.h"
#include "game.hpp"
#include "logging.hpp"

int main() {
    // no need to receive cmd line args
    bool succ = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS |
                         SDL_INIT_AUDIO | SDL_INIT_GAMEPAD);
    if (!succ) {
        ErrorLog("Failed to Init SDL3 WTF: {}", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    auto window   = SDL_CreateWindow("Hello", 800, 800, 0);
    auto renderer = SDL_CreateRenderer(window, nullptr);

    Game game{window, renderer};
    auto game_state = Game::GameState::RUNNING;

    bool is_running = true;
    auto last       = SDL_GetTicksNS();
    while (is_running) {
        // Get timing and input evets
        SDL_Event e;
        bool      quitting = false;
        while (SDL_PollEvent(&e)) {
            quitting = (e.type == SDL_EVENT_QUIT);
            game.OnEvent(&e);
            SDL_ConvertEventToRenderCoordinates(renderer, &e);
            nk_sdl_handle_event(game.NkContext(), &e);
        }
        auto delta_ns = SDL_GetTicksNS() - last;
        last          = SDL_GetTicksNS();

        game_state = game.Run(delta_ns);
        is_running = (game_state != Game::GameState::QUIT) && !quitting;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
