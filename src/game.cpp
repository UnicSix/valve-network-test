#include "game.hpp"

#include <cstddef>
#include <cstring>

#include "SDL3/SDL_clipboard.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "SDL3_image/SDL_image.h"
#include "cpp_alias.hpp"
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
        // std::exit(EXIT_FAILURE);
    }
    deletors.push([this] {
        DebugLog("Release main menu texture");
        SDL_DestroyTexture(main_menu_bg_tex);
    });
    ui_scene_flags.set(U32(UIScene::MAIN_MENU));
    std::memset(txt_edit_buf, 0, sizeof(txt_edit_buf));
    nk_textedit_init_fixed(txt_edit, txt_edit_buf,
                           sizeof(txt_edit_buf));
    deletors.push([this] {
        DebugLog("Release text edit buffer");
        nk_textedit_free(txt_edit);
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
    if (ui_scene_flags.test(U32(UIScene::MAIN_MENU))) {
        MainMenu();
    }
    if (ui_scene_flags.test(U32(UIScene::MULTIPLAYERS))) {
        MultiplierMenu();
    }
    if (ui_scene_flags.test(U32(UIScene::SETTING_MENU))) {
        SettingMenu();
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
    int   button_count = 4;
    float row_height   = 30;
    float title_h      = 30;
    float pad_y        = ctx->style.window.padding.y * 2;
    float border_h     = ctx->style.window.border * 2;

    float h =
        (row_height + pad_y + border_h) * button_count + title_h;

    ctx->style.window.background       = nk_color{0, 0, 0, 0};
    ctx->style.window.fixed_background = nk_style_item_color({0});
    ctx->style.window.border           = 0.f;
    if (nk_begin(ctx, "Menu", nk_rect(50.f, 50.f, 250, 250),
                 NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(ctx, 0, 1);
        if (nk_button_label(ctx, "Single Player")) {
            InfoLog("Single player");
            ui_scene_flags.flip(U32(UIScene::SINGLEPLAYER));
        }
        if (nk_button_label(ctx, "Multi Players")) {
            InfoLog("Multi players");
            ui_scene_flags.flip(U32(UIScene::MULTIPLAYERS));
        }
        if (nk_button_label(ctx, "Settings")) {
            InfoLog("Setting");
            ui_scene_flags.flip(U32(UIScene::SETTING_MENU));
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
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE |
                     NK_WINDOW_CLOSABLE)) {
    }
    else {
        ui_scene_flags.flip(U32(UIScene::SETTING_MENU));
    }
    nk_end(ctx);
}

void Game::MultiplierMenu() {
    if (nk_begin(ctx, "Multi Players",
                 nk_rect(300.f, 50.f, 200.f, 300.f), 0)) {
        nk_layout_row_static(ctx, 40, 80, 2);
        if (nk_button_label(ctx, "JOIN")) {
            DebugLog("Join");
        }
        if (nk_button_label(ctx, "HOST")) {
            DebugLog("Host");
        }
    }
    else {
        ui_scene_flags.flip(U32(UIScene::MULTIPLAYERS));
    }
    nk_end(ctx);
}
