#include "game.hpp"

#include <cstddef>

#include "SDL3/SDL_clipboard.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "SDL3_image/SDL_image.h"
#include "cpp_utility.hpp"
#include "logging.hpp"

#define NK_IMPLEMENTATION
#include "nuklear.h"
#define NK_SDL3_RENDERER_IMPLEMENTATION
#include "sdl3_renderer/nuklear_sdl3_renderer.h"

Game::Game(SDL_Window* window, SDL_Renderer* renderer)
    : window(window),
      renderer(renderer),
      ctx(nk_sdl_init(window, renderer, nk_sdl_allocator())),
      main_bg_path({EXE_DIR.string(), {"/"}, main_bg_src}),
      AA(NK_ANTI_ALIASING_ON) {
    SDL_GetWindowSize(window, &win_w, &win_h);

    struct nk_font_atlas* atlas;
    struct nk_font_config config     = nk_font_config(0);
    float                 font_scale = 1;
    /* set up the font atlas and add desired font; note that font
     * sizes are multiplied by font_scale to produce better results at
     * higher DPIs */
    atlas = nk_sdl_font_stash_begin(ctx);
    font = nk_font_atlas_add_default(atlas, 22 * font_scale, &config);
    /*font = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/DroidSans.ttf", 14 * font_scale,
     * &config);*/
    nk_sdl_font_stash_end(ctx);

    /* this hack makes the font appear to be scaled down to the
     * desired size and is only necessary when font_scale > 1 */
    font->handle.height /= font_scale;
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    nk_style_set_font(ctx, &font->handle);

    nk_input_begin(ctx);

    InfoLog("Main texture : {}", main_bg_path.Cstr());
    main_menu_bg_tex = IMG_LoadTexture(renderer, main_bg_path.Cstr());
    if (main_menu_bg_tex == NULL) {
        ErrorLog("Failed to load MainMenu texture", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }
    deletors.push([this] {
        DebugLog("Release main menu texture");
        SDL_DestroyTexture(main_menu_bg_tex);
    });
}

void Game::OnEvent(SDL_Event* e) {
    if (e->type == SDL_EVENT_WINDOW_RESIZED) {
        SDL_GetWindowSize(window, &win_w, &win_h);
    }
}

[[nodiscard]] Game::GameState Game::Run(u64 delta_ns) {
    SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r,
                           BACKGROUND_COLOR.g, BACKGROUND_COLOR.b,
                           BACKGROUND_COLOR.a);
    SDL_RenderClear(renderer);
    nk_input_end(ctx);
    switch (ui_scene) {
        case UIScene::MAIN_MENU:
            MainMenu();
            break;
        case UIScene::MULTIPLAYERS:
            MultiplierMenu();
            break;
        case UIScene::SETTING_MENU:
            SettingMenu();
            break;
    }

    nk_sdl_render(ctx, AA);
    nk_sdl_update_TextInput(ctx);

    SDL_RenderPresent(renderer);

    nk_input_begin(ctx);
    return game_state;
}

void Game::MainMenu() {
    SDL_RenderTexture(renderer, main_menu_bg_tex, NULL, NULL);

    float width =
        font->handle.width(font->handle.userdata, font->handle.height,
                           "Single Player", strlen("Single Player"));
    int button_count = 4;
    float row_height = 30;
    float title_h    = 30;
    float pad_y      = ctx->style.window.padding.y * 2;
    float border_h   = ctx->style.window.border * 2;

    float h = (row_height + pad_y + border_h) * button_count + title_h;

    if (nk_begin(ctx, "Menu", nk_rect(50.f, 50.f, width + 50, h),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(ctx, 0, 1);
        if (nk_button_label(ctx, "Single Player")) {
            InfoLog("Single player");
        }
        if (nk_button_label(ctx, "Multi Players")) {
            InfoLog("Multi players");
            ui_scene = UIScene::MULTIPLAYERS;
        }
        if (nk_button_label(ctx, "Settings")) {
            InfoLog("Setting");
            ui_scene = UIScene::SETTING_MENU;
        }
        if (nk_button_label(ctx, "Quit")) {
            InfoLog("Quit");
            game_state = GameState::QUIT;
        }
    }
    nk_end(ctx);
}

void Game::SettingMenu() {
    SDL_RenderTexture(renderer, main_menu_bg_tex, NULL, NULL);
    if (nk_begin(ctx, "Settttting", nk_rect(50.f, 50.f, 200.f, 300.f),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
    }
    nk_end(ctx);
}

void Game::MultiplierMenu() {
    SDL_RenderTexture(renderer, main_menu_bg_tex, NULL, NULL);
    if (nk_begin(ctx, "Multi Players",
                 nk_rect(50.f, 50.f, 200.f, 300.f),
                 NK_WINDOW_BORDER | NK_WINDOW_CLOSABLE |
                     NK_WINDOW_MOVABLE)) {
        nk_menubar_begin(ctx);
        nk_layout_row_begin(ctx, NK_STATIC, 25, 2);
        nk_layout_row_push(ctx, 45);
        if (nk_menu_begin_label(ctx, "FILE", NK_TEXT_LEFT,
                                nk_vec2(120, 200))) {
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_menu_item_label(ctx, "OPEN", NK_TEXT_LEFT);
            nk_menu_item_label(ctx, "CLOSE", NK_TEXT_LEFT);
            nk_menu_end(ctx);
        }
        nk_layout_row_push(ctx, 45);
        if (nk_menu_begin_label(ctx, "EDIT", NK_TEXT_LEFT,
                                nk_vec2(120, 200))) {
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_menu_item_label(ctx, "COPY", NK_TEXT_LEFT);
            nk_menu_item_label(ctx, "CUT", NK_TEXT_LEFT);
            nk_menu_item_label(ctx, "PASTE", NK_TEXT_LEFT);
            nk_menu_end(ctx);
        }
        nk_layout_row_end(ctx);
        nk_menubar_end(ctx);
    }
    else {
        ui_scene = UIScene::MAIN_MENU;
    }
    nk_end(ctx);
}
