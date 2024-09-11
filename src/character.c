#include "minifb/MiniFB.h"
#include "minifb/MiniFB_enums.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

typedef enum {UP, RIGHT, DOWN, LEFT, IDLE} Direction;

typedef struct {
    Vector2I position; // draw position in pixels
    Vector2F position_exact; // draw position exact
    Vector2I grid_position; // grid position in tiles
    Direction direction;
    Direction move;
    int size;
    sprite_atlas *sprite_sheet;
    int status; // 0 = idle, 1 = running
} Player;

/*
                                                ----RENDERING----
*/

// Function to draw a sprite to the screen
void draw_sprite(sprite_atlas *sprite, Vector2I position, Vector2I atlas_location, Vector2I sprite_size) {
    for (int i = atlas_location.y; i < sprite_size.y + atlas_location.y; i++) {
        for (int j = atlas_location.x; j < sprite_size.x + atlas_location.x; j++) { // loop through the sprite in the atlas
            // Check if the pixel is within the bounds of the base buffer
            if ((position.y + i - atlas_location.y) < BASE_HEIGHT && (position.x + j - atlas_location.x) < BASE_WIDTH) {
                // Check if the pixel is within the bounds of the atlas
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


void draw_character_idle_animation(sprite_atlas *buffer, Vector2I position, int size, int start_frame, int frame_count, double time, double speed) {
    int frame = (int)(time * speed) % frame_count;
    draw_frame(buffer, position, frame + start_frame, (Vector2I){size, size});
}

void draw_player_run_animation(Player *player, double time, double speed) {
    draw_character_idle_animation(player->sprite_sheet, player->position, player->size, 0, 4, time, speed);
}

/*
                                                ----PLAYER LOGIC----
*/

Player create_player(Vector2I position, sprite_atlas *sprite_sheet) {
    Player player;
    player.position = (Vector2I){position.x * GRID_SIZE, position.y * GRID_SIZE};
    player.position_exact = (Vector2F){position.x * GRID_SIZE, position.y * GRID_SIZE};
    player.grid_position = position;
    player.direction = 1; // right
    player.move = 4; // idle
    player.size = 21;
    player.sprite_sheet = sprite_sheet;
    player.status = 1; // running
    return player;
}

void move_player(Player *player, double time) {
    player->grid_position.x += 1;
    player->position = (Vector2I){player->grid_position.x * GRID_SIZE, player->grid_position.y * GRID_SIZE};
}

void update_player(Player *player, double delta, double speed) {
    if (player->move != IDLE) {
        switch (player->move) {
            case UP:
                player->position_exact.y -= speed * delta;
                break;
            case RIGHT:
                printf("player exact position: %f, %f\n", player->position_exact.x, player->position_exact.y);
                player->position_exact.x += speed * delta;
                printf("player exact position: %f, %f\n", player->position_exact.x, player->position_exact.y);
                printf("move: %f\n", speed * delta);
                break;
            case DOWN:
                player->position_exact.y += speed * delta;
                break;
            case LEFT:
                player->position_exact.x -= speed * delta;
                break;
        }
        printf("player position: %d, %d\n", player->position.x, player->position.y);
        player->position.x = (int)(round(player->position_exact.x));
        player->position.y = (int)(round(player->position_exact.y));
        printf("player position: %d, %d\n", player->position.x, player->position.y);
    }
}

