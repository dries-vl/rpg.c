#ifdef _WIN32
#include <windows.h>
#endif

#include <minifb/MiniFB.h>
#include <minifb/MiniFB_enums.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "text.c"


static void
keyboard(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool isPressed) {
    const char *window_title = "BIIIIK";
    if(window) {
        window_title = (const char *) mfb_get_user_data(window);
    }
    fprintf(stdout, "%s > keyboard: key: %s (pressed: %d) [key_mod: %x]\n", window_title, mfb_get_key_name(key), isPressed, mod);
    if (key == KB_KEY_ESCAPE) {
        mfb_close(window);
    }
    switch (key) {
        case KB_KEY_UP:
            if (isPressed) {
                move_input(UP);
            }
            break;
        case KB_KEY_DOWN:
            if (isPressed) {
                move_input(DOWN);
            }
            break;
        case KB_KEY_LEFT:
            if (isPressed) {
                move_input(LEFT);
            }
            break;
        case KB_KEY_RIGHT: 
            if (isPressed) {
                move_input(RIGHT);
            }
            break;
    }
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

void update_graphics() {
    int half_width = BASE_WIDTH / 2;
    int half_height = BASE_HEIGHT / 2;

    // Fill each quarter with a different color
    fill_square(0, 0, half_width, half_height, MFB_RGB(255, 0, 0));  // Top-left, Red
    fill_square(half_width, 0, BASE_WIDTH, half_height, MFB_RGB(0, 255, 0));  // Top-right, Green
    fill_square(0, half_height, half_width, BASE_HEIGHT, MFB_RGB(0, 0, 255));  // Bottom-left, Blue
    fill_square(half_width, half_height, BASE_WIDTH, BASE_HEIGHT, MFB_RGB(255, 255, 0));  // Bottom-right, Yellow
}

int main() {
    struct mfb_window *window = mfb_open_ex("rpg.c", BASE_WIDTH * 4, BASE_HEIGHT * 4, 0);
    if (!window)
        return 0;
    set_windows_icon();
    mfb_set_viewport_best_fit(window, BASE_WIDTH, BASE_HEIGHT);

    sprite_atlas font_atlas = load_image_bmp("assets/font_atlas.bmp");
    sprite_atlas dude = load_image_bmp("assets/TEST_DUDE_CR.bmp");

    struct mfb_timer *timer = mfb_timer_create();
    double delta = 0;
    double time = 0;
    Player player = create_player((Vector2I){1, 1}, &dude);
    mfb_set_keyboard_callback(window, keyboard);

    while (mfb_wait_sync(window)) {
        time = mfb_timer_now(timer);
        delta = mfb_timer_delta(timer);
        update_graphics();
        char buffer[100]; snprintf(buffer, sizeof(buffer), "time: %.2f", time);
        draw_string_8px(&font_atlas, (Vector2I){1, 1}, buffer);
        update_player(&player, delta, time, 10);
        // draw_frame(&dude, (Vector2I){0, 0}, 0, (Vector2I){21, 21});
        // draw_image(&dude, (Vector2I){0, 0});
        mfb_update_ex(window, base_buffer, BASE_WIDTH, BASE_HEIGHT);
        // printf("FPS: %f\n", 1.0 / delta);
    }

    free(scaled_buffer);
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
    int result = main(argc, argv);

    // Clean up (no dynamic memory allocation here, so no need for free())
    return result;
}
#endif
