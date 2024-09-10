#include "minifb/MiniFB.h"
#include "minifb/MiniFB_enums.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BASE_WIDTH  240
#define BASE_HEIGHT 160

uint32_t base_buffer[BASE_WIDTH * BASE_HEIGHT];
uint32_t *scaled_buffer = NULL;

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

void scale_buffer(int window_width, int window_height) {
    if (scaled_buffer) {
        free(scaled_buffer);
    }
    scaled_buffer = (uint32_t *)malloc(window_width * window_height * sizeof(uint32_t));

    for (int y = 0; y < window_height; ++y) {
        for (int x = 0; x < window_width; ++x) {
            int base_x = x * BASE_WIDTH / window_width;
            int base_y = y * BASE_HEIGHT / window_height;
            scaled_buffer[y * window_width + x] = base_buffer[base_y * BASE_WIDTH + base_x];
        }
    }
}

int main() {
    struct mfb_window *window = mfb_open_ex("Test Window", BASE_WIDTH, BASE_HEIGHT, WF_BORDERLESS);
    if (!window)
        return 0;

    // mfb_set_viewport_best_fit(window, BASE_WIDTH, BASE_HEIGHT);

    while (mfb_wait_sync(window)) {
        update_graphics();
        // scale_buffer(window_width, window_height);  // Scale every frame in case of window resize
        mfb_update(window, base_buffer);
    }

    free(scaled_buffer);
    return 0;
}
