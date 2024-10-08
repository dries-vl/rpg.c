#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "map.c"

static void
keyboard(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool isPressed) { 
    uint8_t *key_buffer = mfb_get_key_buffer(window);
    const char *window_title = "BIIIIK";
    if(window) {
        window_title = (const char *) mfb_get_user_data(window);
    }
    if (key == KB_KEY_ESCAPE) {
        mfb_close(window);
    }
    
    if (isPressed) {
        switch (key) {
            case KB_KEY_W: case KB_KEY_E:
                move_input(UP, window);
                break;
            case KB_KEY_S: case KB_KEY_N:
                move_input(DOWN, window);
                break;
            case KB_KEY_A: case KB_KEY_M:
                move_input(LEFT, window);
                break;
            case KB_KEY_D: case KB_KEY_I:
                move_input(RIGHT, window);
                break;
        }
    }
    else {move_input(IDLE, window);}
}

void set_windows_icon() {
    #ifdef _WIN32
    HICON hIcon = (HICON)LoadImage(NULL, "assets/icon.ico", IMAGE_ICON, 256, 256, LR_LOADFROMFILE);
    if (!hIcon) {
        MessageBox(NULL, "Could not load icon!", "Error", MB_ICONERROR);
        return;
    }

    HWND hwnd = GetActiveWindow();
    if (!hwnd) {
        MessageBox(NULL, "Could not find window!", "Error", MB_ICONERROR);
        return;
    }

    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    #endif
}

// Function to fill a quarter of the screen with a single color
void fill_square(int start_x, int start_y, int end_x, int end_y, uint32_t color) {
    for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
            base_buffer[y * BASE_WIDTH + x] = color;
        }
    }
}

void draw_background() {
    int half_width = BASE_WIDTH / 2;
    int half_height = BASE_HEIGHT / 2;

    // Fill each quarter with a different color
    fill_square(0, 0, half_width, half_height, MFB_RGB(255, 0, 0));  // Top-left, Red
    fill_square(half_width, 0, BASE_WIDTH, half_height, MFB_RGB(0, 255, 0));  // Top-right, Green
    fill_square(0, half_height, half_width, BASE_HEIGHT, MFB_RGB(0, 0, 255));  // Bottom-left, Blue
    fill_square(half_width, half_height, BASE_WIDTH, BASE_HEIGHT, MFB_RGB(255, 255, 0));  // Bottom-right, Yellow
}

__declspec(dllimport) void *cuda_add(int *a, int *b, int *c);

void run_cuda_code() {
    // load the cuda code dll
    HMODULE hCudaDll = LoadLibrary("kernel.dll");
    if (!hCudaDll) {
        printf("Failed to load CUDA DLL.\n");
        return;
    }
    // load the function from the dll
    void (*cuda_add)(int*, int*, int*) = (void (*)(int*, int*, int*))GetProcAddress(hCudaDll, "cuda_add");
    if (!cuda_add) {
        printf("Failed to get function address from CUDA DLL.\n");
        FreeLibrary(hCudaDll);
        return;
    }

    int size = 10;
    int a[10] = {1, 1, 1, 1, 1, 4, 4, 4, 4, 4};
    int b[10] = {1, 1, 1, 1, 1, 4, 4, 4, 4, 4};
    int c[10];

    cuda_add(a, b, c);

    // print the array
    for (int i = 0; i < size; i++) {
        printf("%d ", c[i]);  // Print each element followed by a space
    }
    printf("\n");  // New line after printing array
    FreeLibrary(hCudaDll);
}

int main() {
    struct mfb_window *window = mfb_open_ex("rpg.c", BASE_WIDTH * 4, BASE_HEIGHT * 4, WF_BORDERLESS);
    if (!window)
        return 0;
    set_windows_icon();
    mfb_set_viewport_best_fit(window, BASE_WIDTH, BASE_HEIGHT);

    sprite_atlas font_atlas = load_image_bmp("assets/font_atlas.bmp");
    sprite_atlas ui_atlas = load_image_bmp("assets/ui_atlas.bmp");
    sprite_atlas dude = load_image_bmp("assets/TEST_DUDE_CR.bmp");
    sprite_atlas tile_atlas = load_image_bmp("assets/tile_atlas.bmp");
    sprite_atlas map = load_image_bmp("assets/map.bmp");

    // cuda gpu call
    // run_cuda_code();

    struct mfb_timer *timer = mfb_timer_create();
    double delta = 0;
    double time = 0;
    game_state = init_game_state(create_player((Vector2I){1, 1}, &dude, (Vector2I){16, 21})); // size map is random!!
    printf("player: %d\n", game_state.player.move);
    Player extra_player = create_player((Vector2I){7, 4}, &dude, (Vector2I){16, 21});
    Player extra_player2 = create_player((Vector2I){6, 4}, &dude, (Vector2I){16, 21});
    mfb_set_keyboard_callback(window, keyboard);
    load_map(&map, &game_state);
    move_player(&extra_player, UP);

    mfb_set_target_fps(30);
    // llm_request_async("Show me some shitty ascii art. Don't say anything at all, immediately output an ascii art cow. Max. 3x3 rows high");

    while (mfb_wait_sync(window)) {
        time = mfb_timer_now(timer);
        delta = mfb_timer_delta(timer);
        char fps_string[100]; snprintf(fps_string, sizeof(fps_string), "FPS: %.0f", 1.0 / delta);
        draw_background();
        draw_map(&map, &tile_atlas);
        draw_string_8px(&font_atlas, (Vector2I){1, 1}, fps_string);
        // draw_string_8px(&font_atlas, (Vector2I){1, 100}, "This is a sentence supposed to look normal. Does it?");
        if (extra_player.grid_position.x == 13 && extra_player.grid_position.y == 1) {
            extra_player.stop = TRUE;
            if (!move_player(&extra_player, DOWN)) {
            }
        }
        else if (extra_player.grid_position.x == 7 && extra_player.grid_position.y == 6) {
            extra_player.stop = TRUE;
            if (!move_player(&extra_player, UP)) {
            }
        }
        else if (extra_player.grid_position.y == 1) {
            extra_player.stop = TRUE;
            if (!move_player(&extra_player, RIGHT)) {
            }
        }
        else if (extra_player.grid_position.y == 6) {
            extra_player.stop = TRUE;
            if (!move_player(&extra_player, LEFT)) {
            }
        }
        update_player(&game_state.player, delta, time, 8);
        update_player(&extra_player, delta, time, 8);
        update_player(&extra_player2, delta, time, 4);
        // draw_frame(&dude, (Vector2I){0, 0}, 0, (Vector2I){21, 21});
        // draw_image(&dude, (Vector2I){0, 0});
        // TODO: stop the text from rendering until button is pressed -> show next chunk of text that fits the box
        draw_sprite(&ui_atlas, ivec2(2,114), ivec2(2,114), ivec2(236, 44));
        draw_string_8px(&font_atlas, (Vector2I){8, 120}, "Pretend response from LLM: this is some art:\\n####\\n#   #\\n####.");
        mfb_update_ex(window, base_buffer, BASE_WIDTH, BASE_HEIGHT);
        // printf("collision map: %s\n", game_state.collision_map[0]);
        // printf("collision map: %s\n", game_state.collision_map[1]);
        // printf("collision map: %s\n", game_state.collision_map[2]);
        // printf("collision map: %s\n", game_state.collision_map[3]);
        // printf("collision map: %s\n", game_state.collision_map[4]);
        // printf("collision map: %s\n", game_state.collision_map[5]);
        // printf("FPS: %f\n", 1.0 / delta);
    }

    return 0;
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // Parse command-line arguments using GetCommandLineA (ANSI version)
    LPSTR cmdLine = GetCommandLineA(); // Get the full command-line as a single string
    int argc = 0;
    char **argv = NULL;

    // Use a basic approach to split the command line into argc and argv
    // For simplicity, just handle it as a single argument case
    argc = 1;
    argv = (char **)malloc(sizeof(char *));
    argv[0] = cmdLine; // Treat the whole command line as a single argument
    
    // Open a console to show the debug output
    // AllocConsole();
    // FILE* fp;
    // freopen_s(&fp, "CONOUT$", "w", stdout);
    // freopen_s(&fp, "CONOUT$", "w", stderr);

    // Call main with the parsed arguments
    int result = main();

    // Clean up (no dynamic memory allocation here, so no need for free())
    return result;
}
#endif
