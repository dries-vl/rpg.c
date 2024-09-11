#include <stdint.h>
#include "load.c"

typedef struct {
    int x;
    int y;
} Vector2I;

// Function to draw a sprite to the screen
void draw_sprite(sprite_atlas *sprite, Vector2I position, Vector2I atlas_location, Vector2I sprite_size) {
    for (int i = atlas_location.y; i < sprite_size.y + atlas_location.y; i++) {
        for (int j = atlas_location.x; j < sprite_size.x + atlas_location.x; j++) { // loop through the sprite in the atlas
            // Check if the pixel is within the bounds of the base buffer
            if ((position.y + i - atlas_location.y) < BASE_HEIGHT && (position.x + j - atlas_location.x) < BASE_WIDTH) {
                // Check if the pixel is within the bounds of the sprite
                if (i < sprite->height && j < sprite->width) {
                    // add transparency
                    uint32_t pixel = sprite->buffer[i * sprite->width + j];
                    uint32_t pixel_old = base_buffer[(position.y + i - atlas_location.y) * BASE_WIDTH + (position.x + j - atlas_location.x)];
                    double alpha = (pixel & 0xFF000000) / 0xFF000000;
                    //printf("%f\n", alpha);
                    //pixel = base_buffer[(position.y + i - atlas_location.y) * BASE_WIDTH + (position.x + j - atlas_location.x)] * (1-alpha) + pixel * alpha;
                    //pixel = base_buffer[(position.y + i - atlas_location.y) * BASE_WIDTH + (position.x + j - atlas_location.x)] * (1-alpha);
                    uint32_t R = (pixel & 0x00FF0000) * alpha;
                    uint32_t G = (pixel & 0x0000FF00) * alpha;
                    uint32_t B = (pixel & 0x000000FF) * alpha;
                    R += (pixel_old & 0x00FF0000) * (1-alpha);
                    G += (pixel_old & 0x0000FF00) * (1-alpha);
                    B += (pixel_old & 0x000000FF) * (1-alpha);
                    base_buffer[(position.y + i - atlas_location.y) * BASE_WIDTH + (position.x + j - atlas_location.x)] = R + G + B;
                }
            }
        }
    }
}
// Same but a whole image
void draw_image(sprite_atlas *sprite, Vector2I position) {
    draw_sprite(sprite, position, (Vector2I){0, 0}, (Vector2I){sprite->width, sprite->height});
}

// Draw a frame
void draw_frame(sprite_atlas *sprite, Vector2I position, int frame, Vector2I sprite_size) {
    draw_sprite(sprite, position, (Vector2I){frame * sprite_size.x, 0}, sprite_size); // !!! add modulo for extra rows
}


void draw_character_idle_animation(sprite_atlas *buffer, int size, int start_frame, int frame_count, double time, double speed) {
    int frame = (int)(time * speed) % frame_count;
    draw_frame(buffer, (Vector2I){80 , 80}, frame + start_frame, (Vector2I){size, size});
}
