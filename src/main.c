#ifdef _WIN32
#include <windows.h>
#endif
#include "minifb/MiniFB.h"
#include "minifb/MiniFB_enums.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BASE_WIDTH  240
#define BASE_HEIGHT 160

uint32_t base_buffer[BASE_WIDTH * BASE_HEIGHT];
uint32_t *scaled_buffer = NULL;

void set_windows_icon() {
    #ifdef _WIN32
    HICON hIcon = (HICON)LoadImage(NULL, "assets/icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
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
    struct mfb_window *window = mfb_open_ex("rpg.c", BASE_WIDTH * 4, BASE_HEIGHT * 4, WF_BORDERLESS);
    if (!window)
        return 0;
    set_windows_icon();
    mfb_set_viewport_best_fit(window, BASE_WIDTH, BASE_HEIGHT);

    while (mfb_wait_sync(window)) {
        update_graphics();
        mfb_update_ex(window, base_buffer, BASE_WIDTH, BASE_HEIGHT);
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
